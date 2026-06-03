<template>
    <div class="friends-list
                txtr-vert txtr-vert--orange
                shdw shdw--inst-orange shdw--elevated-l">
        <FriendsListModeTabs
            class="list-tabs"
            v-model:mode="selectedTab" />

        <FriendsListDetails
            class="list-details"
            :user="selectedFriend?.user"
            :state="detailsState"
            :slide-direction="detailsSlideDirection"
            :isLoading="isLoadingAnything" />

        <template v-if="selectedTab === 'list'">
            <FriendsListView
                class="list-list-view"
                :friends="friendsPage"
                v-model:selected-friend="selectedFriend" />

            <div class="list-foot">
                <FriendsListRequestsToggle />
                <FriendsListPageSelect />
            </div>
        </template>

        <template v-else>
            <FriendsListSearchView 
                :isLoading="isLoadingSearchResult" />
        </template>
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    ref,
    watch
} from 'vue';

import FriendsListDetails        from './FriendsListDetails.vue';
import FriendsListModeTabs       from './FriendsListModeTabs.vue';
import FriendsListPageSelect     from './FriendsListPageSelect.vue';
import FriendsListView           from './FriendsListView.vue';
import FriendsListRequestsToggle from './FriendsListRequestsToggle.vue';
import FriendsListSearchView     from './FriendsListSearchView.vue';

import { Keys } from '@/core/di/keys.js';

import {
    Friend,
    FriendStatus
} from '@/models/friend.js';

const friends = inject(Keys.FriendsStore);

const selectedTab = ref('list');

const selectedFriend        = ref(null);
const detailsSlideDirection = ref('forwards');

/*
    If user hasn't searched yet we want to show
    the default search title. Otherwise we can show
    no results.
*/
const hasSearchedYet  = ref(false);
const searchResult    = ref(null);

/*
    Loading anything?
*/
const isLoadingSearchResult = ref(false);
const isLoadingFriends = computed(() =>
    friends.state.value === 'loading');

const isLoadingAnything = computed(() =>
    isLoadingSearchResult.value || 
    isLoadingFriends.value);

const friendsPage = computed(() => {
    return friends.allFriends
        .concat(Array(5))
        .slice(0, 5);
});

const selectedIndex = computed(() =>
    friendsPage.value?.indexOf(selectedFriend.value) ?? -1);

const detailsState = computed(() => {
    switch (selectedTab.value) {
        case 'list':
            return friends.state.value !== 'ready'
                ? 'empty'
                : selectedFriend.value == null
                    ? 'nofriends'
                    : 'userresult';

        case 'search':
            return !hasSearchedYet.value
                ? 'search'
                : searchResult == null
                    ? 'searchnotfound'
                    : 'userresult';
    }
});

const detailsUser = computed(() => {
    return selectedTab.value === 'list'
        ? selectedFriend.value?.user
        : searchResult.value;
});

watch(friendsPage, () => {
    if (friendsPage.value.length > 0 && selectedFriend.value == null)
        selectedFriend.value = friendsPage.value[0];
}, { immediate: true });

watch(selectedTab, () => {
    detailsSlideDirection.value = 
        selectedTab.value === 'list'
            ? detailsSlideDirection.value = 'forwards'
            : detailsSlideDirection.value = 'backwards';
});

watch(selectedIndex, (value, previousValue) => {
    detailsSlideDirection.value =
        previousValue > value ? 'forwards' : 'backwards';
});
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