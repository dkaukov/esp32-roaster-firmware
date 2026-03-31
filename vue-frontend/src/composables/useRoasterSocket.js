import { onBeforeUnmount, onMounted } from "vue";

function createSocketUrl() {
  if (!import.meta.env.PROD) {
    return "ws://rst-544b84.sphere.home/dashws";
  }

  return `ws://${document.location.host}/dashws`;
}

export function useRoasterSocket(roasterState) {
  const { state, applyLogMessage, applyConfigMessage, applyStatusMessage } = roasterState;
  const chartListeners = new Map();
  let socket = null;
  let pingInterval = null;

  function notifyChart(id, chart) {
    const listeners = chartListeners.get(id);

    if (!listeners) {
      return;
    }

    const timestamps = [...(chart.x_axis || [])];
    const values = [...(chart.y_axis || [])];

    listeners.forEach((callback) => callback({ id, timestamps, values }));
  }

  function send(payload) {
    if (socket && socket.readyState === WebSocket.OPEN) {
      socket.send(JSON.stringify(payload));
    }
  }

  function connect() {
    state.ws.url = createSocketUrl();
    socket = new WebSocket(state.ws.url);

    socket.onopen = () => {
      state.ws.connected = true;
    };

    socket.onclose = () => {
      state.ws.connected = false;
    };

    socket.onerror = () => {
      state.ws.connected = false;
    };

    socket.onmessage = (event) => {
      const json = JSON.parse(event.data);
      state.ws.connected = true;

      if (json.topic === "log") {
        applyLogMessage(json.message);
      }

      if (json.topic === "config") {
        applyConfigMessage(json);
      }

      if (json.topic === "status") {
        applyStatusMessage(json);
        notifyChart("Bt", state.control.chartBt);
        notifyChart("Et", state.control.chartEt);
      }
    };

    pingInterval = window.setInterval(() => {
      if (!socket) {
        return;
      }

      if (socket.readyState === WebSocket.CONNECTING) {
        state.ws.connected = false;
      } else if (socket.readyState === WebSocket.CLOSED) {
        state.ws.connected = false;
        if (import.meta.env.PROD) {
          window.location.reload();
        }
      } else if (socket.readyState === WebSocket.OPEN) {
        socket.send('{"command":"ping"}');
      }
    }, 5000);

    if (!import.meta.env.PROD) {
      clearInterval(pingInterval);
      pingInterval = null;
    }
  }

  function disconnect() {
    if (pingInterval) {
      clearInterval(pingInterval);
      pingInterval = null;
    }

    if (socket) {
      socket.onopen = null;
      socket.onclose = null;
      socket.onmessage = null;
      socket.onerror = null;
      socket.close();
      socket = null;
    }
  }

  function subscribeChart(id, callback) {
    if (!chartListeners.has(id)) {
      chartListeners.set(id, new Set());
    }

    const listeners = chartListeners.get(id);
    listeners.add(callback);

    return () => {
      listeners.delete(callback);
      if (listeners.size === 0) {
        chartListeners.delete(id);
      }
    };
  }

  const actions = {
    actuate(actuator) {
      send({
        command: "actuate",
        actuator,
      });
    },
    tune(config) {
      send({
        command: "tune",
        config,
      });
    },
    buttonClicked(id, value) {
      send({
        command: "buttonClicked",
        id,
        value,
      });
    },
    tare() {
      send({
        command: "tare",
      });
    },
    sliderChanged(id, value) {
      send({
        command: "sliderChanged",
        id,
        value,
      });
    },
    requestStats() {
      send({
        command: "getStats",
      });
    },
  };

  onMounted(connect);
  onBeforeUnmount(disconnect);

  return {
    actions,
    subscribeChart,
  };
}
