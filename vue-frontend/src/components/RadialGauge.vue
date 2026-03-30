<template>
  <canvas ref="canvas" :width="canvasWidth" :height="canvasHeight"></canvas>
</template>

<script>
import CanvasGauges from "canvas-gauges";

export default {
  name: "RadialGauge",

  props: {
    options: {
      type: Object,
      required: true,
    },
    value: {
      type: Number,
      default: 0,
    },
  },

  data() {
    return {
      gauge: null,
    };
  },

  computed: {
    canvasWidth() {
      return this.options.width || 300;
    },
    canvasHeight() {
      return this.options.height || 300;
    },
    gaugeOptions() {
      return {
        ...this.options,
        renderTo: this.$refs.canvas,
        value: this.value,
      };
    },
  },

  methods: {
    initGauge() {
      const { RadialGauge } = CanvasGauges;
      this.gauge = new RadialGauge(this.gaugeOptions);
      this.gauge.draw();
    },
    updateGauge() {
      if (!this.gauge) {
        return;
      }

      this.gauge.update(this.gaugeOptions);
    },
  },

  watch: {
    value() {
      if (this.gauge) {
        this.gauge.value = this.value;
      }
    },
    options: {
      deep: true,
      handler() {
        this.updateGauge();
      },
    },
  },

  mounted() {
    this.initGauge();
  },

  beforeUnmount() {
    if (this.gauge && typeof this.gauge.destroy === "function") {
      this.gauge.destroy();
    }
    this.gauge = null;
  },
};
</script>
