import { createApp } from 'vue'
import App from './App.vue'
import router from './router'



//Vue.config.productionTip = false

router.beforeEach((to, from, next) => {
  let title = to.name;
  document.title = title + " - ESP-ATU";
  next();
});

createApp(App).use(router).mount('#app')
