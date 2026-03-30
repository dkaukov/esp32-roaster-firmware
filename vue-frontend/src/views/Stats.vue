<template>
  <div>
    <div v-if="stats.enabled" class="mx-auto max-w-[1220px]">
      <div class="rounded-[1.6rem] bg-white shadow-[0_0_50px_rgba(232,232,232,0.9)]">
        <div class="stats-content">
          <div class="stats-row"><span class="stats-label">Hardware:</span><span>{{ compactValue(stats.hardware) }}</span></div>
          <div class="stats-row"><span class="stats-label">Uptime:</span><span>{{ compactValue(stats.upTime) }}</span></div>
          <div class="stats-row"><span class="stats-label">SDK Version:</span><span>{{ compactValue(stats.sdk) }}</span></div>
          <div class="stats-row"><span class="stats-label">Chip ID:</span><span>{{ compactValue(stats.chipId) }}</span></div>
          <div class="stats-row"><span class="stats-label">Free Heap:</span><span>{{ compactValue(stats.freeHeap) }} bytes</span></div>
          <div class="stats-row"><span class="stats-label">Min. Free Heap:</span><span>{{ compactValue(stats.minFreeHeap) }} bytes</span></div>
          <div class="stats-row"><span class="stats-label">Heap Max alloc block:</span><span>{{ compactValue(stats.maxAllocHeap) }} bytes</span></div>
          <div class="stats-row"><span class="stats-label">Stack size:</span><span>{{ compactValue(stats.stackHighWaterMark) }} bytes</span></div>
          <div class="stats-row"><span class="stats-label">Chip Temperature:</span><span>{{ formatChipTemp(stats.chipTempC) }}</span></div>
          <div class="stats-row"><span class="stats-label">Sketch Hash:</span><span>{{ compactValue(stats.sketchHash) }}</span></div>
          <div class="stats-row"><span class="stats-label">MAC Address:</span><span>{{ compactValue(stats.macAddress) }}</span></div>
          <div class="stats-row"><span class="stats-label">Host name:</span><span>{{ compactValue(stats.hostname) }}</span></div>
          <div class="stats-row"><span class="stats-label">Signal Level (RSSI):</span><span>{{ compactValue(stats.wifiSignal) }}</span></div>
        </div>
      </div>
    </div>
    <div v-else class="mx-auto max-w-[1220px] py-10 text-center text-lg font-normal text-slate-700">
      <div>
        <svg class="mx-auto h-16 w-16 align-middle text-slate-500" viewBox="0 0 24 24" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
          <g id="Stockholm-icons-/-Code-/-Lock-circle" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
            <rect id="bound" x="0" y="0" width="24" height="24"></rect>
            <circle id="Oval-5" fill="currentColor" opacity="0.3" cx="12" cy="12" r="10"></circle>
            <path d="M14.5,11 C15.0522847,11 15.5,11.4477153 15.5,12 L15.5,15 C15.5,15.5522847 15.0522847,16 14.5,16 L9.5,16 C8.94771525,16 8.5,15.5522847 8.5,15 L8.5,12 C8.5,11.4477153 8.94771525,11 9.5,11 L9.5,10.5 C9.5,9.11928813 10.6192881,8 12,8 C13.3807119,8 14.5,9.11928813 14.5,10.5 L14.5,11 Z M12,9 C11.1715729,9 10.5,9.67157288 10.5,10.5 L10.5,11 L13.5,11 L13.5,10.5 C13.5,9.67157288 12.8284271,9 12,9 Z" id="Combined-Shape" fill="currentColor"></path>
          </g>
        </svg>
        <div class="mt-4">Stats are disabled</div>
      </div>
    </div>
  </div>
</template>

<script>
import Socket from "../socket.js";

export default {
  props: ["stats"],

  computed: {
    getWifiMode() {
      switch (this.stats.wifiMode) {
        case 0:
          return "WIFI_AP";
        case 1:
          return "WIFI_STA";
        case 2:
          return "WIFI_AP_STA";
        case 3:
          return "WIFI_OFF";
        default:
          return "NaN";
      }
    },
  },

  methods: {
    compactValue(value) {
      return String(value ?? "")
        .replace(/\r\n/g, " ")
        .replace(/\s+/g, " ")
        .trim();
    },
    formatChipTemp(value) {
      if (value === null || value === undefined || Number.isNaN(value)) {
        return "Unavailable";
      }
      return `${Number(value).toFixed(1)} °C`;
    },
  },

  mounted() {
    Socket.send(
      JSON.stringify({
        command: "getStats",
      })
    );
  },
};
</script>

<style>
.stats-content {
  padding: 1.25rem 1.5rem;
}

.stats-row {
  display: flex;
  gap: 0.9rem;
  align-items: baseline;
  padding: 0.8rem 0;
  border-bottom: 1px solid rgba(43, 53, 79, 0.08);
  line-height: 1.25;
  font-size: 0.95rem;
}

.stats-row:last-child {
  border-bottom: 0;
  padding-bottom: 0;
}

.stats-label {
  min-width: 11rem;
  font-weight: 700;
  color: #2b354f;
}
</style>
