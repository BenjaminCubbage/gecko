<template>
    <BaseDialog 
        ref="baseDialog" 
        color="orange"
        v-model:is-open="isOpen">
        <template v-if="user != null">
            <FriendsListFriendsCard :user="user" variant="details" />
        </template>
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