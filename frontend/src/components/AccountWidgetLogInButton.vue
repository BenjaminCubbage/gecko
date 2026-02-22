<template>
    <a 
        class="login-button txtr-diag txtr-diag--lt-gray"
        :class="{ 'login-button--pressed': isPressed }"
        href="/auth/login" 
        draggable="false"
        @click="press">
        <StrokedText>
            <i class="google-icon hn hn-google"></i>
        </StrokedText>
    </a>
</template>

<script setup>
import { ref, onUnmounted } from 'vue';

import StrokedText from './StrokedText.vue';

const isPressed    = ref(false);
let unpressTimeout = null;

function press() {
    if (!isPressed.value) {
        isPressed.value = true;

        unpressTimeout = setTimeout(() => {
            isPressed.value = false;
            unpressTimeout  = null;
        }, 4000);
    }
}

onUnmounted(() => {
    if (unpressTimeout)
        clearTimeout(unpressTimeout);
});
</script>

<style scoped>
.login-button {
    display:       flex;
    height:        46px;
    width:         48px;
    place-content: center;
    place-items:   center;

    color:           black;
    text-decoration: none;

    --login-button-aura:   0 0;
    --login-button-offset: 0px;

    box-shadow:    
        var(--login-button-aura),
        0 calc(var(--shadow-dist-s) - var(--login-button-offset))
        0 black,
        inset  3px  3px var(--col-lt-gray-0),
        inset -3px -3px var(--col-lt-gray-6);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transition:
        box-shadow 100ms ease,
        translate  100ms ease;

    translate: 0 var(--login-button-offset);

    corner-shape: notch;
}

.login-button:hover {
    --login-button-offset: calc(var(--shadow-dist-s) / 2);
}

.login-button:active,
.login-button--pressed,
.login-button--pressed:hover {
    --login-button-aura:   var(--shadow-aura);
    --login-button-offset: var(--shadow-dist-s);
}

.google-icon {
    color:       transparent;
    font-size:   26px;
    display: inline-block;

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

    translate: 0.5px 1px;
}
</style>