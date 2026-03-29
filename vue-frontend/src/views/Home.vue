<template>
  <div>
    <div class="container" v-if="true">
      <div class="section">

        <div class="columns is-vcentered">
          <div class="column">
            <div class="ml-4">
              <status-card :key="'cardid'" :card="home.status"></status-card>
            </div>
          </div>
        </div>


        <div class="columns is-gapless">
          <div class="column has-text-centered">
              <radial-gauge :options="home.BT" :value="home.BT.value" ref="bt"></radial-gauge>
            </div>
            <div class="column has-text-centered">
              <radial-gauge :options="home.ET" :value="home.ET.value" ref="et"></radial-gauge>
            </div>
            <div class="column has-text-centered">  
              <radial-gauge :options="home.W" :value="home.W.value" ref="w"></radial-gauge>
              <button class="button is-light is-small mt-3" @click="sendTare">Tare</button>
          </div>
        </div>

      </div>
    
    </div>

    <div class="container" v-else>
      <div class="section">
        <div class="row">
          <div class="columns is-centered has-text-centered" style="font-weight: 400; font-size: 18px">
            <div class="column">
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
    </div>
  </div>
</template>

<script>
//import GenericCard from '@/components/GenericCard.vue';
//import TemperatureCard from '@/components/TemperatureCard.vue';
//import HumdidityCard from '@/components/HumidityCard.vue';
import StatusCard from "@/components/StatusCard.vue";
import EventBus from "@/event-bus.js";
//import ProgressCard from '@/components/ProgressCard.vue';
//import SliderCard from '@/components/SliderCard.vue';
//import ButtonCard from '@/components/ButtonCard.vue';

//import BarChart from '@/components/BarChart.vue';
//import LineChart from '@/components/LineChart.vue';

//import LinearGauge from "vue-canvas-gauges/src/LinearGauge";
import RadialGauge from "vue-canvas-gauges/src/RadialGauge";

export default {
  name: "home",

  props: ["cards", "charts", "home"],

  components: {
    //GenericCard,
    //TemperatureCard,
    //HumdidityCard,
    StatusCard,
    //ProgressCard,
    //ButtonCard,
    //SliderCard,
    //BarChart,
    //LinearGauge,
    RadialGauge,
    //LineChart
  },

  methods: {
    sendTare() {
      EventBus.$emit("tare");
    },
  },

  mounted() {
  },
};
</script>
