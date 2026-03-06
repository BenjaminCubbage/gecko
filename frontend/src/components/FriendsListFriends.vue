<template>
    <div class="friends-list-friends">
        <BaseLabel class="label" color="orange">
            INCOMING REQUESTS
        </BaseLabel>

        <ul v-roving-container>
            <li 
                v-for="friend in friends.pendingIncoming"
                :key="friend.user.userID"
                v-roving-item>
                <FriendsListFriendsCard
                    :friend="friend"
                    friend-type="incoming" />
            </li>
        </ul>

        <BaseLabel class="label" color="orange">
            FRIENDS
        </BaseLabel>

        <ul v-roving-container>
            <li 
                v-for="friend in friends.pendingOutgoing" 
                :key="friend.user.userID"
                v-roving-item>
                <FriendsListFriendsCard
                    :friend="friend"
                    friend-type="outgoing" />
            </li>
        </ul>

        <ul v-roving-container>
            <li 
                v-for="friend in friends.activeFriends" 
                :key="friend.user.userID"
                v-roving-item>
                <FriendsListFriendsCard
                    :friend="friend"
                    friend-type="active" />
            </li>
        </ul>
    </div>
</template>

<script setup>
import { inject, ref } from 'vue';

import FriendsListFriendsCard from './FriendsListFriendsCard.vue';
import BaseLabel              from './BaseLabel.vue';

import { Keys } from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);
</script>

<style scoped>
.friends-list-friends {
    display:        flex;
    flex-direction: column;
    font-size: 3rem;
}

.label {
    align-self: center;
}
</style>