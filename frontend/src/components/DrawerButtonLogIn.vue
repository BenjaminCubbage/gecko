<template>
    <DrawerButton
        ref="drawerButtonEl"
        color="lt-gray"
        variant="toggle"
        aria-label="Log in with Google"
        v-model:is-toggled="isLoggingIn"
        :temporarily-disabled="isLoggingIn">
        <i class="icon hn hn-google"></i>
    </DrawerButton>
</template>

<script setup>
    import DrawerButton from './DrawerButton.vue';

    const isLoggingIn = defineModel('isLoggingIn', {
        required: true,
        type:     Boolean
    });
</script>

<style scoped>
.icon {
    width:     30px;
    translate: 0 0px;

    color: transparent;

    --google-blue:   #3179eb;
    --google-green:  #259643;
    --google-red:    #de3122;
    --google-yellow: #fb9905;

    background-image: conic-gradient(at 50% 40%,
        var(--google-red)    25%,
        var(--google-blue)   25%,
        var(--google-blue)   50%,
        var(--google-green)  50%,
        var(--google-green)  65%,
        var(--google-yellow) 65%,
        var(--google-yellow) 90%,
        var(--google-red)    90%);

    background-clip: text;
}
</style>