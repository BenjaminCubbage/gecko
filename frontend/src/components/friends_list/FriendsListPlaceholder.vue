<template>
    <div class="friends-list-placeholder-message">
        <LoadingSpinner v-if="variant === 'loading'" style="font-size: 2.4rem; padding: 12px 0;" />
        <div v-else-if="variant === 'info'" class="info">
            {{ message }}
        </div>
        <div v-else-if="variant === 'error'" class="error">
            {{ message }}
        </div>
    </div>
</template>

<script setup>
import LoadingSpinner from '@/components/loading_spinner/LoadingSpinner.vue';

const props = defineProps({
    // 'loading' | 'info' | 'error'
    variant: { type: String, default: 'info' },

    // message optional for loading variant
    message: { type: String, required: false }
});
</script>

<style scoped>
.friends-list-placeholder-message {
    height: 68px;

    text-align: center;
    font-family: var(--font-heading);
    font-size: 2.1rem;
    color: black;

    display: grid;
    place-content: center;
}

.info {
    color: black;
}

.error {
    color: crimson;
}

.placeholder-transition-enter-active,
.placeholder-transition-leave-active {
    transition: transform 100ms ease;
}

.placeholder-transition-enter-from,
.placeholder-transition-leave-to {
    transform: scale(0.95);
}
</style>