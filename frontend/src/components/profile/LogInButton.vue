<template>
    <a class="log-in-button" :href="href" draggable="false">
        <i class="google-icon hn hn-google"></i>

        <div class="button-text">
            Log in
        </div>
    </a>
</template>

<script setup>
import { defineProps } from "vue";

defineProps({
    href: { type: String, required: true }
});
</script>

<style scoped>
    .log-in-button {
        color: var(--text);
        justify-self: start;

        display: flex;
        align-items: center;
        gap: 12px;

        padding: 5px 22px 5px 12px;
        background: var(--bg);
        border-radius: 60px;
        border: var(--border-small);
        box-shadow: var(--border-shadow-large);
        text-decoration: none;

        transition: transform 100ms ease;
        pointer-events: all;
    }

    .log-in-button:hover {
        transform: scale(1.02) rotate(1deg);
    }

    .log-in-button:active {
        transform: scale(0.97) rotate(0);
        box-shadow: var(--border-shadow-small);
    }

    .google-icon {
        --google-red: #EA4335;
        --google-blue: #4285F4;
        --google-green: #34A853;
        --google-yellow: #FBBC05;

        background-image: conic-gradient(at 50% 40%, 
            var(--google-red)    25%, 
            var(--google-blue)   25%, 
            var(--google-blue)   50%, 
            var(--google-green)  50%,
            var(--google-green)  65%,
            var(--google-yellow) 65%,
            var(--google-yellow) 90%,
            var(--google-red)    90%);
        color: transparent;
        background-clip: text;
        font-size: 26px;
        line-height: 1;
    }

    .button-text {
        user-select: none;
        font-family: var(--font-heading);
        letter-spacing: 0.04em;
        font-size: 2.4rem;
    }
</style>