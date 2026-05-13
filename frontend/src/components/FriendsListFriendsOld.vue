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
            <!--
                For some reason FF is allowing focus on this ul if I
                don't set tabindex to -1
            -->
            <ul class="
                list
                shdw shdw--recessed shdw--otst-orange shdw--inst-lt-gray"
                tabindex="-1"
                v-roving-container>
                <!--
                    I am keying by index on purpose. 
                    
                    For performance reasons, each slot should salvage as much 
                    state as possible when flipping between pages.
                -->
                <FriendsListFriendsCard
                    v-for="(friend, index) in pageFriends"
                    role="listitem"
                    class="list-item"
                    :user="friend?.user"
                    :is-expanded="friend != null && isDetailsOpen && detailedFriend.user.userID == friend.user.userID"
                    :key="index"
                    v-roving-item
                    @show-friend-details="showFriendDetails" />
            </ul>
        </div>

        <FriendsListNavigationArrows
            class="friends-list-navigation-arrows"
            :max-value="pageMax"
            v-model:current-value="currentPage" />
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    ref
} from 'vue';

import FriendsListDetailsDialog    from './FriendsListDetailsDialogOld.vue';
import FriendsListFriendsCard      from './FriendsListFriendsCardOld.vue';
import FriendsListNavigationArrows from './FriendsListNavigationArrowsOld.vue';
import { Keys }                    from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);

const isDetailsOpen  = ref(false);
const detailedFriend = ref(null);

const cardsPerPage = 5;
const currentPage  = ref(0);

const pageMax = computed(() => {
    let maxPage = 0 | friends.allFriends.length / cardsPerPage;

    if (friends.allFriends.length && 
        friends.allFriends.length % 5 === 0)
        --maxPage;

    return maxPage;
});

const pageFriends = computed(() => {
    const start  = currentPage.value * cardsPerPage;
    const result = friends.allFriends.slice(start, start + cardsPerPage);
    result.length = cardsPerPage;
    return result;
});

function showFriendDetails(friend) {
    detailedFriend.value = friend;
    isDetailsOpen.value  = true;
}
</script>

<style scoped>
.friends-list-friends {
    display:        flex;
    flex-direction: column;
    gap: 8px;
}

.list-border {
    padding: 12px;

    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.list {
    contain: content;

    display:        flex;
    flex-direction: column;
    gap:            8px;
    overflow:       auto;

    padding:        calc(6px + var(--shadow-dist-m));
    scroll-padding: 10px;

    scrollbar-color: var(--col-gray-3) transparent;
    scrollbar-width: thin;

    background:    var(--col-lt-gray-1);
    border-radius: var(--radius-s);
    border:        var(--border-s);
}

.friends-list-navigation-arrows {
    align-self: end;
}
</style>