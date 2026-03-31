import { reactive } from "vue";

function getTicks(min, max, count) {
  const ticks = [];

  for (let i = 0; i <= count; i += 1) {
    ticks.push(Number(min + ((max - min) / count) * i).toFixed(2));
  }

  return ticks;
}

function createLineSeries(id) {
  return {
    id,
    type: "line",
    name: id,
    x_axis: [],
    y_axis: [],
  };
}

function createBaseGauge({ id, title, units, minValue, maxValue, width = 300, height = 300, tickCount = 8 }) {
  return {
    id,
    value: minValue,
    rawValue: 255,
    width,
    height,
    title,
    fontTitleWeight: "bold",
    units,
    minValue,
    maxValue,
    majorTicks: getTicks(minValue, maxValue, tickCount),
    highlights: [],
    minorTicks: 20,
    strokeTicks: true,
    animation: true,
    animationRule: "linear",
    animationDuration: 250,
    animatedValue: true,
    visible: false,
    valueDec: 1,
  };
}

function appendChartPoint(chart, timestamp, value, windowMs = 10 * 60 * 1000) {
  chart.x_axis.push(timestamp);
  chart.y_axis.push(value);

  const cutoff = timestamp - windowMs;
  const firstVisibleIndex = chart.x_axis.findIndex((pointTs) => pointTs >= cutoff);

  if (firstVisibleIndex > 0) {
    chart.x_axis = chart.x_axis.slice(firstVisibleIndex);
    chart.y_axis = chart.y_axis.slice(firstVisibleIndex);
  }
}

function flagsEqual(currentFlags, nextFlags) {
  if (currentFlags === nextFlags) {
    return true;
  }

  if (!Array.isArray(currentFlags) || !Array.isArray(nextFlags) || currentFlags.length !== nextFlags.length) {
    return false;
  }

  return currentFlags.every((flag, index) => {
    const nextFlag = nextFlags[index];
    return flag.label === nextFlag.label && flag.tone === nextFlag.tone;
  });
}

export function useRoasterState() {
  const state = reactive({
    ws: {
      url: "",
      connected: false,
    },
    stats: {
      enabled: false,
      releaseTag: null,
      sdk: null,
      chipId: null,
      sketchHash: null,
      macAddress: null,
      freeHeap: null,
      minFreeHeap: null,
      maxAllocHeap: null,
      stackHighWaterMark: null,
      chipTempC: null,
      wifiMode: null,
      hostname: "",
      wifiSignal: null,
      upTime: null,
      hardware: null,
    },
    log: {
      lines: "",
    },
    home: {
      BT: createBaseGauge({ id: "Bt", title: "Bt", units: "\u2103", minValue: 10.0, maxValue: 250.0 }),
      ET: createBaseGauge({ id: "Et", title: "Et", units: "\u2103", minValue: 10.0, maxValue: 250.0 }),
      W: {
        ...createBaseGauge({ id: "W", title: "W", units: "g", minValue: 0.0, maxValue: 400.0 }),
        animationRule: "dequad",
        animationDuration: 120,
      },
      status: {
        symbol: "danger",
        icon: "danger",
        name: "Roaster status",
        value: "Booting up",
        flags: [],
        progress: null,
      },
    },
    control: {
      chartBt: createLineSeries("Bt"),
      chartEt: createLineSeries("Et"),
      status: {
        symbol: "danger",
        icon: "danger",
        name: "ATU status",
        value: "Booting up",
        progress: null,
      },
    },
  });

  function applyLogMessage(message) {
    const normalizedLogLine = String(message || "")
      .replace(/\r\n/g, "\n")
      .replace(/\n+$/g, "");

    if (normalizedLogLine) {
      state.log.lines = state.log.lines ? `${state.log.lines}\n${normalizedLogLine}` : normalizedLogLine;
    }
  }

  function applyConfigMessage(json) {
    state.stats.enabled = true;
    state.stats.sdk = json.device.sdkVersion;
    state.stats.releaseTag = json.device.sketchMD5;
    state.stats.chipId = json.device.chipModel;
    state.stats.sketchHash = json.device.sketchMD5;
    state.stats.macAddress = json.device.wifi.macAddress;
    state.stats.wifiMode = 1;
    state.stats.hostname = json.device.wifi.hostname;
    state.stats.hardware = json.device.hardware;
  }

  function applyStatusMessage(json) {
    state.stats.freeHeap = json.system.freeHeap;
    state.stats.minFreeHeap = json.system.minFreeHeap;
    state.stats.maxAllocHeap = json.system.maxAllocHeap;
    state.stats.stackHighWaterMark = json.system.stackHighWaterMark;
    const chipTempSensor = ((json.sensor || {}).chipTemp || {}).T;
    state.stats.chipTempC =
      chipTempSensor !== undefined && chipTempSensor !== null ? chipTempSensor : json.system.chipTempC;
    state.stats.wifiSignal = json.system.rssi;

    state.home.BT.value = (json.sensor.BT || {}).Tlut;
    state.home.ET.value = (json.sensor.ET || {}).Tlut;
    state.home.W.value = (json.sensor.scale || {}).W;

    const now = Date.now();
    appendChartPoint(state.control.chartBt, now, (json.sensor.BT || {}).Tlut);
    appendChartPoint(state.control.chartEt, now, (json.sensor.ET || {}).Tlut);

    const baseStatus = json.atu.state;

    const upTime = new Date(0);
    upTime.setSeconds(json.system.upTime || 0);
    state.stats.upTime = `${((json.system.upTime || 0) / 60 / 60 / 24).toFixed(0)} days ${upTime
      .toISOString()
      .substr(11, 8)}`;

    const heaterOn = Boolean(((json.actuator || {}).heater || {}).isOn);
    const heaterRawValue = Number(((json.actuator || {}).heater || {}).value);
    const mcuOverheat = Number(state.stats.chipTempC) >= 105;
    const heaterPercent = Number.isFinite(heaterRawValue) ? Math.round((heaterRawValue / 255) * 100) : 0;

    const nextStatusValue = heaterOn ? "heater is on" : baseStatus;
    let nextStatusSymbol = heaterOn ? "danger" : baseStatus === "ready" ? "success" : "warning";
    const nextStatusIcon = heaterOn ? "heater" : nextStatusSymbol;
    const nextFlags = [
      mcuOverheat ? { label: "MCU overheat", tone: "danger" } : null,
    ].filter(Boolean);

    if (mcuOverheat) {
      nextStatusSymbol = "danger";
    }

    if (state.home.status.value !== nextStatusValue) {
      state.home.status.value = nextStatusValue;
    }

    if (state.home.status.symbol !== nextStatusSymbol) {
      state.home.status.symbol = nextStatusSymbol;
    }

    if (state.home.status.icon !== nextStatusIcon) {
      state.home.status.icon = nextStatusIcon;
    }

    if (!flagsEqual(state.home.status.flags, nextFlags)) {
      state.home.status.flags = nextFlags;
    }

    const nextProgress = heaterOn
      ? {
          label: "Heater PWM",
          value: Math.max(0, Math.min(100, heaterPercent)),
          tone: "danger",
        }
      : null;

    const currentProgress = state.home.status.progress;
    const progressChanged =
      currentProgress?.label !== nextProgress?.label ||
      currentProgress?.value !== nextProgress?.value ||
      currentProgress?.tone !== nextProgress?.tone;

    if (progressChanged) {
      state.home.status.progress = nextProgress;
    }

    state.control.status.value = state.home.status.value;
    state.control.status.symbol = state.home.status.symbol;
    state.control.status.icon = state.home.status.icon;
    state.control.status.progress = state.home.status.progress;
  }

  return {
    state,
    applyLogMessage,
    applyConfigMessage,
    applyStatusMessage,
  };
}
