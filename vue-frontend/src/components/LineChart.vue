<template>
  <div class="line-chart">
    <div class="line-chart__header">
      <span class="line-chart__dot" :class="{ 'line-chart__dot--active': activity }"></span>
      <p class="line-chart__title">{{ title }}</p>
    </div>
    <div class="line-chart__canvas-wrap">
      <canvas ref="canvas"></canvas>
    </div>
  </div>
</template>

<script>
import { Chart } from "chart.js/auto";
import { markRaw } from "vue";

export default {
  name: "LineChart",

  props: {
    title: {
      type: String,
      default: "",
    },
    options: {
      type: Object,
      default: () => ({}),
    },
  },

  data() {
    return {
      activity: false,
      activityTimeout: null,
      refreshFrame: null,
      pendingLabels: [],
      pendingValues: [],
    };
  },

  mounted() {
    this.initChart();
  },

  beforeUnmount() {
    if (this.refreshFrame !== null) {
      cancelAnimationFrame(this.refreshFrame);
      this.refreshFrame = null;
    }

    if (this.activityTimeout !== null) {
      clearTimeout(this.activityTimeout);
      this.activityTimeout = null;
    }

    if (this.chartInstance) {
      this.chartInstance.destroy();
      this.chartInstance = null;
    }
  },

  methods: {
    formatTimeLabel(timestamp) {
      const date = new Date(Number(timestamp) || 0);

      if (Number.isNaN(date.getTime())) {
        return "";
      }

      return date.toLocaleTimeString([], {
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
      });
    },
    buildOptions() {
      return {
        responsive: true,
        maintainAspectRatio: false,
        animation: this.options?.animation ?? false,
        normalized: true,
        interaction: {
          mode: "index",
          intersect: false,
        },
        plugins: {
          legend: {
            display: false,
          },
        },
        scales: {
          x: {
            ticks: {
              maxTicksLimit: 6,
              autoSkip: true,
            },
            grid: {
              display: true,
            },
          },
          y: {
            beginAtZero: false,
          },
        },
      };
    },
    initChart() {
      const canvas = this.$refs.canvas;

      if (!canvas) {
        return;
      }

      this.chartInstance = markRaw(
        new Chart(canvas, {
          type: "line",
          data: {
            labels: [],
            datasets: [
              {
                data: [],
                borderColor: "#3273dc",
                backgroundColor: "rgba(50, 115, 220, 0.12)",
                borderWidth: 2,
                pointRadius: 0,
                pointHoverRadius: 3,
              tension: 0.2,
              spanGaps: true,
              fill: false,
            },
          ],
          },
          options: this.buildOptions(),
        }),
      );
    },
    scheduleRefresh() {
      if (this.refreshFrame !== null) {
        return;
      }

      this.refreshFrame = requestAnimationFrame(() => {
        this.refreshFrame = null;
        this.refreshChart();
      });
    },
    pulseActivity() {
      this.activity = true;

      if (this.activityTimeout !== null) {
        clearTimeout(this.activityTimeout);
      }

      this.activityTimeout = setTimeout(() => {
        this.activity = false;
        this.activityTimeout = null;
      }, 160);
    },
    setSeries(timestamps = [], values = []) {
      const safeTimestamps = Array.isArray(timestamps) ? timestamps : [];
      const safeValues = Array.isArray(values) ? values : [];
      const length = Math.min(safeTimestamps.length, safeValues.length);
      const nextLabels = [];
      const nextValues = [];

      for (let index = 0; index < length; index += 1) {
        const numericValue = Number(safeValues[index]);

        if (!Number.isFinite(numericValue)) {
          continue;
        }

        nextLabels.push(this.formatTimeLabel(safeTimestamps[index]));
        nextValues.push(numericValue);
      }

      this.pendingLabels = nextLabels;
      this.pendingValues = nextValues;

      this.scheduleRefresh();
    },
    refreshChart() {
      if (!this.chartInstance) {
        this.initChart();
      }

      if (!this.chartInstance) {
        return;
      }

      this.chartInstance.data.labels = this.pendingLabels.slice();
      this.chartInstance.data.datasets[0].data = this.pendingValues.slice();
      this.pulseActivity();
      this.chartInstance.update("none");
    },
  },
};
</script>

<style scoped>
.line-chart {
  min-height: 320px;
}

.line-chart__header {
  display: flex;
  align-items: center;
  gap: 0.45rem;
  margin-bottom: 0.5rem;
}

.line-chart__title {
  margin: 0;
  font-size: 0.95rem;
  font-weight: 600;
}

.line-chart__dot {
  width: 0.65rem;
  height: 0.65rem;
  border-radius: 999px;
  background: rgba(50, 115, 220, 0.24);
  transition:
    transform 120ms ease,
    background-color 120ms ease,
    box-shadow 120ms ease;
}

.line-chart__dot--active {
  background: #3273dc;
  transform: scale(1.15);
  box-shadow: 0 0 0 0.24rem rgba(50, 115, 220, 0.16);
}

.line-chart__canvas-wrap {
  position: relative;
  height: 300px;
}
</style>
