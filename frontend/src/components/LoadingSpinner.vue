<template>
    <span class="spinner" v-bind="attrs">
    </span>
</template>

<script setup>
import { useAttrs } from 'vue';
defineOptions({
    inheritAttrs: false
});
const attrs = useAttrs();
</script>

<style scoped>
.spinner {
    contain: content;

    display: inline-grid;
    padding: var(--shadow-dist-l);
    width:   min-content;

    font-size:   2.3rem;
    line-height: 0;

    &::before,
    &::after {
        content:   '...' / '';
        grid-area: 1 / 1;
    }

    &::before {
        opacity: 0;
    }

    &::after {
        animation:  
            cycle 500ms infinite steps(1, end);

        text-align: left;
        translate:  0 -0.22em;
    }
}

@keyframes cycle {
    16.6% { text-align: right; }
    66.6% { text-align: left; }

    0%           { content: '...' / ''; }
    16.6%, 83.3% { content: '..'  / ''; }
    33.3%, 66.6% { content: '.'   / ''; }
    50%          { content: ''    / ''; }
}
</style>