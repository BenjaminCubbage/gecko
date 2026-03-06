<template>
    <div class="friends-list">
        <FriendsListSearch />
        <FriendsListFriends />
    </div>
</template>

<script setup>
import FriendsListFriends from './FriendsListFriends.vue';
import FriendsListSearch  from './FriendsListSearch.vue';
</script>

<style scoped>
.friends-list {
    display:        flex;
    flex-direction: column;
}
</style>