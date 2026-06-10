<template>
    <span 
        class="loading-spinner"
        inert>
        <span class="spinner-dots"></span>
    </span>
</template>

<script setup>
defineProps({
    color: {
        type:    String,
        default: 'black'
    }
});
</script>

<style scoped>
.loading-spinner {
    --_col-dots-1: v-bind('color === "black" || color === "white" ? color : `var(--col-${color}-3)`');
    --_col-dots-2: v-bind('color === "black" || color === "white" ? color : `var(--col-${color}-4)`');
    --_col-dots-3: v-bind('color === "black" || color === "white" ? color : `var(--col-${color}-5)`');

    width:  var(--_sz-spinner);
    height: var(--_sz-spinner);

    display:         flex;
    flex-flow:       column;
    align-items:     center;
    justify-content: center;

    font: 33px/0.18 var(--font-main);

    --_wd-spinner: 16px;
    --_ht-spinner: 16px;

    &::before,
    > .spinner-dots::before,
    &::after {
        animation: loading-spinner 600ms calc(100ms * var(--i)) steps(2, end) infinite;
    }

    &::before               { --i: 0; color: var(--_col-dots-1); }
    > .spinner-dots::before { --i: 1; color: var(--_col-dots-2); }
    &::after                { --i: 2; color: var(--_col-dots-3); }
    
    &::before,
    &::after,
    > .spinner-dots::before {
        content: '\f510';
    }
}

@keyframes loading-spinner {
    0%  { content: '\f510'; }
    33% { content: '\f511'; }
    66% { content: '\f512'; }
}
</style>