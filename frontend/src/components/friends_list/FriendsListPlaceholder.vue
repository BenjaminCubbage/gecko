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

defineProps({
    // 'loading' | 'info' | 'error'
    variant: { type: String, default: 'info' },

    // message optional for loading variant
    message: { type: String, required: false }
});
</script>

<style scoped>
.friends-list-placeholder-message {
    display:        grid;
    height:         68px;
    place-content:  center;

    color:          black;
    font-family:    var(--font-heading);
    font-size:      2.1rem;
    text-align:     center;
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