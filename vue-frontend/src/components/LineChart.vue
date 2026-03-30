<template>
  <div class="column">
    <div class="card">
      <span class="dot" :class="{ active: activity }"></span>
      <div class="card-content">
        <div class="columns is-mobile is-vcentered">
          <div class="column is-narrow has-text-primary">
            <div class="card-icon pb-1 pt-2 px-2 has-background-link-light has-text-link">
              <svg width="24" height="24" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
                <path d="M5 3v16h16v2H3V3h2zm15.293 3.293l1.414 1.414L16 13.414l-3-2.999-4.293 4.292-1.414-1.414L13 7.586l3 2.999 4.293-4.292z" />
              </svg>
            </div>
          </div>
          <div class="column">
            <h6 class="is-size-6 has-text-muted">{{ chart.name }}</h6>
          </div>
        </div>
        <div class="columns">
          <div class="column is-12">
            <canvas ref="canvas" height="200"></canvas>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import Chart from "chart.js";

export default {
  props: {
    chart: {
      type: Object,
      required: true,
    },
    options: {
      type: Object,
      default: () => ({}),
    },
  },

  data() {
    return {
      activity: false,
      chartInstance: null,
      baseOptions: {
        animation: false,
        responsive: true,
        aspectRatio: 1,
        height: 200,
        legend: {
          display: false,
        },
        scales: {
          xAxes: [
            {
              type: "time",
              time: {
                unit: "minute",
              },
              gridLines: {
                display: true,
              },
            },
          ],
        },
      },
    };
  },

  watch: {
    "chart.x_axis": {
      deep: true,
      handler() {
        this.refreshChart();
      },
    },
    "chart.y_axis": {
      deep: true,
      handler() {
        this.refreshChart();
      },
    },
  },

  computed: {
    mergedOptions() {
      return {
        ...this.baseOptions,
        ...this.options,
        legend: {
          ...this.baseOptions.legend,
          ...(this.options.legend || {}),
        },
        scales: {
          ...this.baseOptions.scales,
          ...(this.options.scales || {}),
        },
      };
    },
  },

  methods: {
    setActivityPulse() {
      if (this.activity === false) {
        this.activity = true;
        setTimeout(() => {
          this.activity = false;
        }, 100);
      }
    },
    initChart() {
      this.chartInstance = new Chart(this.$refs.canvas.getContext("2d"), {
        type: "line",
        data: {
          labels: [...this.chart.x_axis],
          datasets: [
            {
              label: "",
              backgroundColor: "#4c73f5",
              borderColor: "#4c73f5",
              data: [...this.chart.y_axis],
              fill: false,
              lineTension: 0,
              pointRadius: 0,
            },
          ],
        },
        options: this.mergedOptions,
      });
    },
    refreshChart() {
      if (!this.chartInstance) {
        return;
      }

      this.setActivityPulse();
      this.chartInstance.data.labels = [...this.chart.x_axis];
      this.chartInstance.data.datasets[0].data = [...this.chart.y_axis];
      this.chartInstance.options = this.mergedOptions;
      this.chartInstance.update(0);
    },
  },

  mounted() {
    this.initChart();
    setTimeout(() => {
      this.activity = false;
    }, 500);
  },

  beforeUnmount() {
    if (this.chartInstance) {
      this.chartInstance.destroy();
      this.chartInstance = null;
    }
  },
};
</script>
