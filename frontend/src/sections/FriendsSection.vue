<template>
    <section class="friends-section">
        <FriendsList />
    </section>
</template>

<script setup>
import FriendsList from '@/components/FriendsList.vue';
</script>

<style scoped>
.friends-section {
    margin-top:    12px;
    display:       grid;
    justify-items: center;
    gap: 12px;
}
</style>