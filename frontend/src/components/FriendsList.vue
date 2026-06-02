<template>
    <div class="friends-list
                txtr-vert txtr-vert--orange
                shdw shdw--inst-orange shdw--elevated-l">
        <FriendsListModeTabs
            class="list-tabs"
            v-model:mode="mode" />

        <FriendsListDetails
            class="list-details"
            :friend="selectedFriend"
            :transitionDirection="detailsTransitionDirection" />

        <template v-if="mode === 'list'">
            <FriendsListView
                class="list-list-view"
                :friends="friends"
                v-model:selected-friend="selectedFriend" />

            <div class="list-foot">
                <FriendsListRequestsToggle />
                <FriendsListPageSelect />
            </div>
        </template>

        <template v-else>
            <FriendsListSearchView />
        </template>
    </div>
</template>

<script setup>
import {
    computed,
    ref,
    watch
} from 'vue';

import FriendsListDetails        from './FriendsListDetails.vue';
import FriendsListModeTabs       from './FriendsListModeTabs.vue';
import FriendsListPageSelect     from './FriendsListPageSelect.vue';
import FriendsListView           from './FriendsListView.vue';
import FriendsListRequestsToggle from './FriendsListRequestsToggle.vue';
import FriendsListSearchView     from './FriendsListSearchView.vue';

import {
    Friend,
    FriendStatus
} from '@/models/friend.js';

const friends = ref([
    Friend.fromJSON({
        'user': {
            'username': 'Bob',
            'user_id':  1
        },
        'accepted_on': '12/21/2002'
    }, FriendStatus.PendingIncoming),
    Friend.fromJSON({
        'user': {
            'username': 'Jim',
            'user_id':  2
        },
        'accepted_on': '12/21/2002'
    }, FriendStatus.PendingOutgoing),
    Friend.fromJSON({
        'user': {
            'username': 'Larry',
            'user_id':  3
        },
        'accepted_on': '12/21/2002'
    }, FriendStatus.Active),
    null,
    null
]);

const selectedFriend = ref(friends.value[0]);
const previousIndex  = ref(0);

const currentIndex = computed(() =>
    friends.value?.indexOf(selectedFriend.value));

const detailsTransitionDirection = ref('normal');

watch(currentIndex, (value, previousValue) => {
    detailsTransitionDirection.value =
        previousValue > value ? 'forwards' : 'backwards';
});

/* List view or search view. */
const mode = ref('list');
</script>

<style scoped>
.friends-list {
    --overhang-x-tabs: -18px;
    --overhang-y-tabs: -24px;

    display:   flex;
    flex-flow: column;

    padding: 27px 12px 9px;
    margin-top: calc(-1 * var(--overhang-y-tabs));

    display:   flex;
    flex-flow: column;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    & > .list-tabs      { z-index: 1; }
    & > .list-details   { z-index: 0; align-self: stretch; }
    & > .list-list-view { z-index: 0; align-self: stretch; margin-top: 18px; }
    & > .list-foot      { z-index: 0; align-self: stretch; margin-top: 9px; }
}

.list-tabs {
    position: absolute;
    left: var(--overhang-x-tabs);
    top:  var(--overhang-y-tabs);
}

.list-foot {
    display:         flex;
    justify-content: space-between;
    align-items:     center;
}
</style>