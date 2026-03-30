import { createApp } from "vue";
import App from "./App.vue";
import router from "./router.js";

router.beforeEach((to, from, next) => {
  let title = to.name;
  document.title = title + " - ESP32-Roaster";
  next();
});

createApp(App).use(router).mount("#app");
