<template>
    <div class="friends-list-friends">
        <FriendsListDetailsDialog
            ref="detailsDialog"
            :user="detailedFriend?.user"
            v-model:is-open="isDetailsOpen" />

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
                <FriendsListFriendsCard
                    v-for="friend in friends.pendingIncoming.value"
                    role="listitem"
                    class="list-item"
                    :user="friend.user"
                    :key="friend.user.userID"
                    v-roving-item />

                <FriendsListFriendsCard
                    v-for="friend in friends.pendingOutgoing.value"
                    role="listitem"
                    class="list-item"
                    :user="friend.user"
                    :key="friend.user.userID"
                    v-roving-item
                    @show-friend-details="showFriendDetails" />

                <FriendsListFriendsCard
                    v-for="friend in friends.activeFriends.value"
                    role="listitem"
                    class="list-item"
                    :user="friend.user"
                    :key="friend.user.userID"
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
    max-height:     max(50vh, 100px);
    overflow:       auto;

    padding:        var(--shadow-dist-m);
    scroll-padding: 10px;

    scrollbar-color: var(--col-gray-3) transparent;

    background:    var(--col-lt-gray-1);
    border-radius: var(--radius-s);
    border:        var(--border-s);

    & > .list-item {
        margin: 3px;

        border: var(--border-thickness-s) solid var(--col-gray-4);
        border-radius: var(--radius-s);

        box-shadow: 
            0 var(--shadow-dist-s) var(--col-gray-4),
            3px 6px var(--col-gray-1);

        background: 
            linear-gradient(
                var(--col-gray-0) 50%,
                var(--col-gray-1) 50%);
    }
}
</style>