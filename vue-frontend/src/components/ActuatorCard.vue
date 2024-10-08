<template>
  <div class="column box is-4">
    <h4 class="title is-4 is-pulled-left">{{ actuator.title }}</h4>
    <div class="is-pulled-right">
      <v-sevenseg :value="displayValue" color-back="transparent" color-on="rgb(100, 100, 100)" color-off="rgb(245, 245, 245)" height="45" digits="6" slant="10" width="200"></v-sevenseg>
      {{ actuator.units }}
    </div>
    <div class="block" />
    <div>
      <input class="slider is-fullwidth s-circle is-large" :min="0" :max="1000" v-model="pct" @change="sendValue" type="range" style="opacity: 0.7" />
      <progress class="progress is-small is-fullwidth" :min="0" :max="actuator.maxValue - actuator.minValue" :value="actuator.value - actuator.minValue" style="margin-top: -22px; height: 11px" />
    </div>
    <div class="block" />
    <div class="buttons are-small is-pulled-right">
      <button class="button is-white" style="cursor: pointer" @click="sendCalibrate" :disabled="!actuator.isReady">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
          <path fill="none" d="M0 0h24v24H0z" />
          <path d="M21 20a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V9.49a1 1 0 0 1 .386-.79l8-6.222a1 1 0 0 1 1.228 0l8 6.222a1 1 0 0 1 .386.79V20zm-2-1V9.978l-7-5.444-7 5.444V19h14z" />
        </svg>
      </button>
      <button class="button is-white" style="cursor: pointer" @click="sendStepDown" :disabled="!actuator.isReady">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
          <path fill="none" d="M0 0h24v24H0z" />
          <path d="M12 10.667l9.223-6.149a.5.5 0 0 1 .777.416v14.132a.5.5 0 0 1-.777.416L12 13.333v5.733a.5.5 0 0 1-.777.416L.624 12.416a.5.5 0 0 1 0-.832l10.599-7.066a.5.5 0 0 1 .777.416v5.733zm-2 5.596V7.737L3.606 12 10 16.263zm10 0V7.737L13.606 12 20 16.263z" />
        </svg>
      </button>
      <button class="button is-white" style="cursor: pointer" @click="sendStepUp" :disabled="!actuator.isReady">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24">
          <path fill="none" d="M0 0h24v24H0z" />
          <path d="M12 13.333l-9.223 6.149A.5.5 0 0 1 2 19.066V4.934a.5.5 0 0 1 .777-.416L12 10.667V4.934a.5.5 0 0 1 .777-.416l10.599 7.066a.5.5 0 0 1 0 .832l-10.599 7.066a.5.5 0 0 1-.777-.416v-5.733zM10.394 12L4 7.737v8.526L10.394 12zM14 7.737v8.526L20.394 12 14 7.737z" />
        </svg>
      </button>
    </div>
  </div>
</template>

<script>
import EventBus from "@/event-bus.js";
import VSevenseg from "v-sevenseg/src/components/VSevenseg.vue";

export default {
  props: ["actuator"],

  components: {
    VSevenseg,
  },

  data() {
    return {
      activity: true,
      displayValue: this.round(this.actuator.value),
      pct: ((this.actuator.value - this.actuator.minValue) / (this.actuator.maxValue - this.actuator.minValue)) * 1000,
    };
  },

  methods: {
    round(val) {
      return val.toFixed(2);
    },
    sendValue() {
      this.msg = {
        actuator: {},
      };
      // eslint-disable-next-line
      this.actuator.isReady = false;
      this.msg.actuator[this.actuator.id] = {};
      this.msg.actuator[this.actuator.id].pct = this.pct / 1000;
      EventBus.$emit("actuate", this.msg);
    },
    sendCalibrate() {
      this.msg = {
        actuator: {},
      };
      // eslint-disable-next-line
      this.actuator.isReady = false;
      this.msg.actuator[this.actuator.id] = {};
      this.msg.actuator[this.actuator.id].calibrate = true;
      EventBus.$emit("actuate", this.msg);
    },
    sendStepUp() {
      this.msg = {
        actuator: {},
      };
      // eslint-disable-next-line
      this.actuator.isReady = false;
      this.msg.actuator[this.actuator.id] = {};
      this.msg.actuator[this.actuator.id].value = this.actuator.rawValue + this.actuator.step;
      EventBus.$emit("actuate", this.msg);
    },
    sendStepDown() {
      this.msg = {
        actuator: {},
      };
      // eslint-disable-next-line vue/no-mutating-props
      this.actuator.isReady = false;
      this.msg.actuator[this.actuator.id] = {};
      this.msg.actuator[this.actuator.id].value = this.actuator.rawValue - this.actuator.step;
      EventBus.$emit("actuate", this.msg);
    },
  },

  watch: {
    "actuator.value": function () {
      this.activity = true;
      this.displayValue = this.round(this.actuator.value);
      setTimeout(() => {
        this.activity = false;
      }, 100);
    },
  },

  mounted() {
    setTimeout(() => {
      this.activity = false;
    }, 500);
  },
};
</script>
