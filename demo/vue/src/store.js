import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    connected: false
  },
  mutations: {
    setConnected (state, connected) {
      state.connected = connected
    }
  },
  getters: {
    isConnected: state => {
      return state.connected
    }
  },
  actions: {
    setConnected ({commit}, connected) {
      commit('setConnected', connected)
    }
  }
})
