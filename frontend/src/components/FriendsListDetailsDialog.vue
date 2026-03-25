<template>
    <BaseDialog
        aria-label="User details"
        color="orange"
        v-model:is-open="isOpen">
        <FriendsListFriendsCard 
            v-if="user != null" 
            class="friends-list-friends-card" 
            variant="details"
            :user="user" />
    </BaseDialog>
</template>

<script setup>
import BaseDialog             from './BaseDialog.vue';
import FriendsListFriendsCard from './FriendsListFriendsCard.vue';
import { User }               from '@/models/user.js';

const props = defineProps({
    user: {
        type:     null,
        required: true,
        validator(f) {
            return f instanceof User || f == null;
        }
    }
});

const isOpen = defineModel('isOpen');
</script>

<style scoped>
.friends-list-friends-card {
    margin: 6px;
}
</style>