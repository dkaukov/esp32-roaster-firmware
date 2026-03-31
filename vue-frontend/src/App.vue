<template>
  <div class="min-h-screen bg-white px-4 pb-8 pt-4 md:px-8 md:pt-6">
    <div class="mx-auto max-w-[1280px]">
      <navbar />

      <main class="pt-6 md:pt-8">
        <router-view v-slot="{ Component }">
          <transition name="fade" mode="out-in">
            <component :is="Component" :stats="stats" :log="log" :home="home" :control="control" />
          </transition>
        </router-view>
      </main>
    </div>
  </div>
</template>

<script>
import { provide, toRefs } from "vue";
import Navbar from "./components/Navbar.vue";
import { useRoasterSocket } from "./composables/useRoasterSocket.js";
import { useRoasterState } from "./composables/useRoasterState.js";

export default {
  components: {
    Navbar,
  },
  setup() {
    const roasterState = useRoasterState();
    const { actions, subscribeChart } = useRoasterSocket(roasterState);

    provide("roasterActions", actions);
    provide("chartUpdates", {
      subscribe: subscribeChart,
    });

    return {
      ...toRefs(roasterState.state),
    };
  },
};
</script>

<style lang="scss">
@use "./assets/scss/main.scss";
</style>
