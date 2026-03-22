<template>
    <BaseDialog 
        ref="baseDialog" 
        color="orange"
        v-model:is-open="isOpen">
        <template v-if="friend != null">
            <FriendsListFriendsCard class="friends-list-friends-card" :friend="friend" variant="details" />
        </template>
    </BaseDialog>
</template>

<script setup>
import BaseDialog             from './BaseDialog.vue';
import FriendsListFriendsCard from './FriendsListFriendsCard.vue';
import { Friend }             from '@/models/friend.js';

const props = defineProps({
    friend: {
        type:     null,
        required: true,
        validator(f) {
            return f instanceof Friend || f == null;
        }
    }
});

const isOpen = defineModel('isOpen');
</script>

<style scoped>
.friends-list-friends-card {
}
</style>