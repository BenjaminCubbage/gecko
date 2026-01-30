<template>
    <button
        class="logout-button"
        @click="emit('click')">
        Log Out
    </button>
</template>

<script setup>
const emit = defineEmits(['click']);
</script>

<style>
.logout-button {
    align-items:     center;
    display:         flex;
    justify-content: center;
    margin:          4px 0;
    padding:         0 12px;

    color:       black;
    font-family: var(--font-heading);
    font-size:   2.2rem;

    background:    hsl(0, 100%, 71%);
    border:        var(--border-s);
    border-radius: 999px;
    box-shadow:    2px 2px 0 0 black;

    transform:  scale(1) rotate(0deg);
    transition: transform 200ms ease;
}

.logout-button:hover {
    transform: scale(1.02) rotate(-1deg);
}

.logout-button:active {
    background: hsl(0, 86%, 67%);
    box-shadow: 1px 1px 0 0 black;

    transform: scale(0.98) rotate(0deg);
}
</style>