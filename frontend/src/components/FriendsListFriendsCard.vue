<template>
    <div 
        ref="friendsListFriendsCardEl" 
        class="friends-list-friends-card"
        v-roving-container>
        {{ friend.user.username }}

        <button ref="button1El" v-roving-item v-if="computedFriendType === 'active'"      @click="deleteFriend">delete friend</button>
        <button ref="button1El" v-roving-item v-if="computedFriendType === 'incoming'"    @click="acceptFriend">accept request</button>
        <button ref="button2El" v-roving-item v-if="computedFriendType === 'incoming'"    @click="deleteFriend">delete request</button>
        <button ref="button1El" v-roving-item v-if="computedFriendType === 'outgoing'"    @click="deleteFriend">cancel request</button>
        <button ref="button1El" v-roving-item v-if="computedFriendType === 'not-friends'" @click="requestFriend">send request</button>
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    ref,
    useTemplateRef
} from 'vue';

import { Friend } from '@/models/friend.js';
import { Keys }   from '@/core/di/keys.js';

const props = defineProps({
    friend: {
        type:     Friend,
        required: true
    },

    friendType: {
        type:    String,
        default: 'auto-detect',
        validator(value) {
            return [
                'auto-detect',
                'active',
                'incoming',
                'outgoing',
                'not-friends'
            ].includes(value);
        }
    }
});

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);

const computedFriendType = computed(() => {
    if (props.friendType !== 'auto-detect')
        return props.friendType;

    if (friends.pendingIncoming.some(f => f.user.userID === props.friend.user.userID)) return 'incoming';
    if (friends.pendingOutgoing.some(f => f.user.userID === props.friend.user.userID)) return 'outgoing';
    if (friends.activeFriends  .some(f => f.user.userID === props.friend.user.userID)) return 'active';

    return 'not-friends';
});

async function acceptFriend() {
    await friends.publishAcceptFriendRequest(session, props.friend.user.userID);
}

async function deleteFriend() {
    await friends.publishDeleteFriendOrRequest(session, props.friend.user.userID);
}

async function requestFriend() {
}
</script>

<style scoped>
.friends-list-friends-card {

}
</style>