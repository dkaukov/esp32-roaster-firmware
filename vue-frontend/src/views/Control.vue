<template>
  <div>
    <div v-if="true" class="mx-auto max-w-[1220px]">
      <div class="grid gap-6 xl:grid-cols-2">
        <div>
          <line-chart ref="chartBt" :key="'chartBt'" :title="control.chartBt.name" :options="noAnimationOptions"></line-chart>
        </div>
        <div>
          <line-chart ref="chartEt" :key="'chartEt'" :title="control.chartEt.name" :options="noAnimationOptions"></line-chart>
        </div>
      </div>
    </div>
    <div v-else class="mx-auto max-w-[1220px]">
      <div class="py-10 text-center text-lg font-normal text-slate-700">
        <div>
          Loading
          <svg width="24px" height="24px" class="spinner" style="vertical-align: middle" viewBox="0 0 24 24" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
            <g id="Stockholm-icons-/-Code-/-Loading" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
              <g id="Group">
                <polygon id="Shape" points="0 0 24 0 24 24 0 24"></polygon>
              </g>
              <path d="M12,4 L12,6 C8.6862915,6 6,8.6862915 6,12 C6,15.3137085 8.6862915,18 12,18 C15.3137085,18 18,15.3137085 18,12 C18,10.9603196 17.7360885,9.96126435 17.2402578,9.07513926 L18.9856052,8.09853149 C19.6473536,9.28117708 20,10.6161442 20,12 C20,16.418278 16.418278,20 12,20 C7.581722,20 4,16.418278 4,12 C4,7.581722 7.581722,4 12,4 Z" id="Oval-3" fill="currentColor" opacity="0.3" transform="translate(12.000000, 12.000000) scale(-1, 1) translate(-12.000000, -12.000000) "></path>
            </g>
          </svg>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import LineChart from '@/components/LineChart.vue';

export default {
  name: "control",

  props: ["control"],
  inject: ["chartUpdates"],

  components: {
    LineChart,
  },

  data() {
    return {
      noAnimationOptions: {
        animation: false,
      },
    };
  },

  methods: {
    syncChart(id, chart) {
      const refName = id === "Bt" ? "chartBt" : "chartEt";
      const chartRef = this.$refs[refName];

      if (chartRef) {
        chartRef.setSeries(chart.x_axis || [], chart.y_axis || []);
      }
    },
  },

  mounted() {
    this.syncChart("Bt", this.control.chartBt);
    this.syncChart("Et", this.control.chartEt);
    this.handleChartUpdate = ({ id, timestamps, values }) => {
      const refName = id === "Bt" ? "chartBt" : id === "Et" ? "chartEt" : null;
      const chartRef = refName ? this.$refs[refName] : null;

      if (chartRef) {
        chartRef.setSeries(timestamps, values);
      }
    };
    this.unsubscribeBt = this.chartUpdates?.subscribe("Bt", this.handleChartUpdate);
    this.unsubscribeEt = this.chartUpdates?.subscribe("Et", this.handleChartUpdate);
  },

  beforeUnmount() {
    this.unsubscribeBt?.();
    this.unsubscribeEt?.();
    this.unsubscribeBt = null;
    this.unsubscribeEt = null;
    this.handleChartUpdate = null;
  },
};
</script>
