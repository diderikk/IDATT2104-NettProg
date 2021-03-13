<template>
    <form @submit.prevent="onSubmit">
        <label for='message'>{{ label }}</label>
        <input id='message' type="text" v-model="input">
        <button id="submit" type="submit">Submit message</button>
    </form>
</template>


<script>
export default {
    name: "Form",
    
    data() {
        return {
            input: "",
            label: "Write your message: "
        }
    },
    methods: {
        onSubmit() {
            console.log(this.input);
            this.$emit("post", this.input);
            this.input = "";
        }
    }

}

</script>

<style scoped>
    form{
        width: 100%;
        display: flex;
        flex-direction: column;
        align-items: center;
      }
      #message{
        margin-top: 5px;
      }
      #submit{
        margin-top: 10px;
        margin-bottom: 10px;
        border-radius: 5px;
        width: 12%;
        height: 30px;
        border: 1px solid transparent;
        cursor: pointer;
        background: lightblue;
      }
</style>