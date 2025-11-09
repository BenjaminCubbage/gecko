<template>
    <button class="button" @click="$emit('click')">
        <slot />
    </button>
</template>

<script setup>
    defineEmits(['click']);
</script>

<style scoped>
    .button {
        font-family: var(--font-heading);
        font-size: 1.6rem;
        padding: 8px 22px;

        background: var(--bg);

        border: var(--border-small);
        border-radius: var(--border-radius-small);
        box-shadow: var(--border-shadow-large);
        transition: transform 100ms ease;
    }

    .button:hover {
        transform: scale(1.03) rotate(-1deg);
    }

    .button:active {
        transform: scale(0.97) rotate(0);
        box-shadow: var(--border-shadow-small);
    }
</style>