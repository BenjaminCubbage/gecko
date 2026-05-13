<template>
    <div class="friends-list">
        <FriendsListSearch />
        <FriendsListFriends />
    </div>
</template>

<script setup>
import FriendsListFriends from './FriendsListFriendsOld.vue';
import FriendsListSearch  from './FriendsListSearchOld.vue';
</script>

<style scoped>
.friends-list {
    display:        flex;
    flex-direction: column;
}
</style>