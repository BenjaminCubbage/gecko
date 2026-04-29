<template>
    <div class="icon-sd-card-info">
        <IconSDCard class="icon-sd-card" />
        <IconInfo class="icon-info" />
    </div>
</template>

<script setup>
import IconSDCard from './IconSDCard.vue';
import IconInfo   from './IconInfo.vue';
</script>

<style scoped>
.icon-sd-card-info {
    display:     grid;
    place-items: center;
}

.icon-sd-card {
    grid-area: 1 / 1;
    height: 100%;
}

.icon-info {
    grid-area: 1 / 1;
    height:    50%;
    translate: -15% -19%;
}
</style>