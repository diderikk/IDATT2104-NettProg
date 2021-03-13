<template>
  <h1>{{ title }}</h1>
  <Form @post="postMessage"/>
  <textarea id="chat" readonly=true v-model="allMessages"></textarea>
</template>

<script>
import Form from "./components/Form";

export default {
  name: "App",
  mounted(){
    this.ws.addEventListener('message',(event) => {
      this.messages.push(event.data);
    })
  },
  components: { Form },
  data() {
    return {
      title: "Websocket Client",
      messages: ['Hello'],
      ws: new WebSocket('ws://localhost:3001')
    };
  },
  methods: {
    postMessage(message){
      console.log(message);
      this.ws.send(message);
    }
  },
  computed: {
    allMessages() {
      return this.messages.join('\n');
    }
  }
};
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}
#chat {
  width: 40%;
  border: 3px sold lightcoral;
  outline: none;
  border-radius: 5px;
  padding: 10px;
  resize: none;
  font-size: 16px;
  height: 30vh;
}
</style>
