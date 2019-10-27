import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import VueSocketIO from 'vue-socket.io'
import ElementUI from 'element-ui';
import 'element-ui/lib/theme-chalk/index.css';

Vue.config.productionTip = false
Vue.use(ElementUI);
Vue.use(new VueSocketIO({
  debug: true,
  connection: 'http://localhost:8001',
  vuex: {
    store,
    actionPrefix: 'SOCKET_',
    mutationPrefix: 'SOCKET_'
  }
}))

new Vue({
  router,
  store,
  render: h => h(App),
  sockets: {
    connect: async() => {
      await store.dispatch('setConnected', true)
    },
    disconnect: async () => {
      await store.dispatch('setConnected', false)
    }
  }
}).$mount('#app')
