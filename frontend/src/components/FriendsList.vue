<template>
    <div class="friends-list">
        <FriendsListStage class="list-stage" />

        <div
            class="
                list-border
                txtr-vert txtr-vert--orange
                shdw shdw--inst-orange shdw--elevated-l">
            <FriendsListModeTabs
                class="border-tabs"
                v-model:mode="mode" />

            <FriendsListDetails
                class="border-details"
                :friend="selectedFriend"
                :transitionDirection="detailsTransitionDirection" />

            <template v-if="mode === 'list'">
                <FriendsListView
                    class="border-list-view"
                    :friends="friends"
                    v-model:selected-friend="selectedFriend" />

                <div class="border-foot">
                    <FriendsListRequestsToggle />
                    <FriendsListPageSelect />
                </div>
            </template>

            <template v-else>
                <FriendsListSearchView />
            </template>
        </div>
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
import FriendsListStage          from './FriendsListStage.vue';
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
    display:   flex;
    flex-flow: column;

    width: 520px;

    & > .list-stage       { z-index: 1; place-self: center  stretch; }
    & > .list-border      { z-index: 0; place-self: stretch stretch; }
    & > .list-page-select { z-index: 1; place-self: center; }
}

.list-stage {
    margin-bottom: -20px;
}

.list-border {
    display:   flex;
    flex-flow: column;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    padding: 30px 12px 9px;

    & > .border-tabs      { z-index: 1; }
    & > .border-details   { z-index: 0; align-self: stretch; }
    & > .border-list-view { z-index: 0; align-self: stretch; margin-top: 18px; }
    & > .border-foot      { z-index: 0; align-self: stretch; margin-top: 9px; }
}

.border-tabs {
    position: absolute;
    top:  -18px;
    left: -18px;
}

.border-foot {
    display:         flex;
    justify-content: space-between;
    align-items:     center;
}

.stage-view {
    margin: 15px;
}
</style>