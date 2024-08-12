<template>
  <terminal name="my-terminal" 
  :drag-conf="dragConf"
  :show-header="true"
  :line-space="1"
  :context=null
  :context-suffix=null
  :cursor-style="none"
  :enable-cursor-blink="false" 
  />
</template>

<script>
import Terminal from 'vue-web-terminal'
import { TerminalApi } from 'vue-web-terminal';
import 'vue-web-terminal/lib/theme/dark.css'
import EventBus from "../event-bus";

export default {
  name: "log-view",

  components: {
    Terminal,
  },

  data(){
    return {
      dragConf: {
        width: "78%",
        height: "200%",
        zIndex: 100,
        init: {
          x: 200,
          y: 200
        },
        pinned: false
      }
    }
  },

  methods: {
    pushMessage(s) {
      Terminal.pushMessage('my-terminal', s);
    }
  },

  props: ["log"],

  mounted() {
    EventBus.$on("logLine", (msg) => {
      TerminalApi.pushMessage('my-terminal', msg);
    });
  },

};
</script>

<style scoped>
</style>
