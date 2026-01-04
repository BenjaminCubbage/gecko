<template>
    <div class="friends-list-divider">
        <label class="label">
            <slot />
        </label>
    </div>
</template>

<script setup>
</script>

<style scoped>
    .friends-list-divider {
        margin: 8px 4px;
        background: black;
        height: 3px;
        text-align: center;

        display: flex;
        place-items: center;
        place-content: center;
    }

    .label {
        padding: 0 4px;
        color: black;
        background: white;
        font-size: 1.9rem;
        line-height: 0.9;
    }
</style>