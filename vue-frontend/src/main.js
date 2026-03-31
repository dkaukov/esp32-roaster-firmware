import { createApp } from "vue";
import App from "./App.vue";
import router from "./router.js";
import "./tailwind.css";

router.afterEach((to) => {
  document.title = `${to.name} - ESP32-Roaster`;
});

createApp(App).use(router).mount("#app");
