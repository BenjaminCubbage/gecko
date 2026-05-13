<template>
    <div class="
        friends-list
        shdw shdw--inst-lt-gray shdw--elevated-l">
        <FriendsListItem />
    </div>
</template>

<script setup>
import FriendsListItem from './FriendsListItem.vue';
</script>

<style scoped>
.friends-list {
    display:   flex;
    flex-flow: column;
    gap:       20px;
    padding:   10px;
    
    background: var(--col-lt-gray-4);

    border:        var(--border-s);
    border-radius: var(--radius-s);
}
</style>