<template>
    <div class="friends-list-friends">
        <FriendsListDetailsDialog 
            ref="detailsDialog"
            :friend="detailedFriend"
            v-model:is-open="isDetailsOpen" />

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

        <div class="
            list-border 
            txtr-diag txtr-diag--orange 
            shdw shdw--inst-orange shdw--elevated-l">
            <!-- For some reason FF is allowing focus on this ul if I 
                 don't set tabindex to -1 -->
            <ul class="
                list 
                shdw shdw--recessed shdw--otst-orange shdw--inst-lt-gray"
                tabindex="-1"
                v-roving-container>
                <li
                    v-for="friend in friends.pendingOutgoing"
                    :key="friend.user.userID">
                    <FriendsListFriendsCard
                        :friend="friend"
                        friend-type="outgoing"
                        v-roving-item />
                </li>

                <FriendsListFriendsCard
                    v-for="friend in friends.activeFriends"
                    :key="friend.user.userID"
                    class="list-item"
                    :friend="friend"
                    friend-type="active"
                    v-roving-item
                    @show-friend-details="showFriendDetails" />
            </ul>
        </div>
    </div>
</template>

<script setup>
import { 
    inject,
    ref
} from 'vue';

import FriendsListDetailsDialog from './FriendsListDetailsDialog.vue';
import FriendsListFriendsCard   from './FriendsListFriendsCard.vue';
import { Keys }                 from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);

const isDetailsOpen  = ref(false);
const detailedFriend = ref(null);

function showFriendDetails(friend) {
    detailedFriend.value = friend;
    isDetailsOpen.value  = true;
}
</script>

<style scoped>
.friends-list-friends {
    display:        flex;
    flex-direction: column;
}

.details-dialog {
    width: 100px;
    height: 100px;
    
}

.list-border {
    padding: 9px;

    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.list {
    contain: content;

    display:        flex;
    flex-direction: column;
    gap:            3px;
    max-height:     max(30vh, 100px);
    overflow:       auto;
    padding:        6px 0;
    
    scrollbar-color:  black transparent;
    scrollbar-gutter: stable both-edges;

    background:    var(--col-lt-gray-1);
    border-radius: var(--radius-s);
    border:        var(--border-s);
}
</style>