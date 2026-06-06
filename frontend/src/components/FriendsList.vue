<template>
    <div class="friends-list
                txtr-vert txtr-vert--orange
                shdw shdw--inst-orange shdw--elevated-l">
        <FriendsListModeTabs
            class="list-tabs"
            v-model:mode="selectedTab" />

        <FriendsListDetails
            class="list-details"
            :user="detailsUser"
            :state="detailsState"
            :slide-direction="detailsSlideDirection"
            :isLoading="showIsLoading"
            @send-request="sendRequest"
            @reject="reject"
            @unsend="unsend"
            @unfriend="unfriend"
            @accept="accept" />

        <template v-if="selectedTab === 'list'">
            <FriendsListView
                class="list-list-view"
                :friends="friendsPage"
                v-model:selected-friend="selectedFriend" />

            <div class="list-foot">
                <FriendsListRequestsToggle />
                <FriendsListPageSelect
                    :pageCount="pageCount"
                    v-model:selectedPage="selectedPage" />
            </div>
        </template>

        <template v-else>
            <FriendsListSearchView
                v-model:search-input="searchInput"
                :isLoading="showIsLoadingSearchResult"
                @search-submitted="searchSubmitted" />
        </template>
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    ref,
    toRaw,
    watch
} from 'vue';

import FriendsListDetails        from './FriendsListDetails.vue';
import FriendsListModeTabs       from './FriendsListModeTabs.vue';
import FriendsListPageSelect     from './FriendsListPageSelect.vue';
import FriendsListView           from './FriendsListView.vue';
import FriendsListRequestsToggle from './FriendsListRequestsToggle.vue';
import FriendsListSearchView     from './FriendsListSearchView.vue';

import { useThrottledRef } from '@/composables/useThrottledRef.js';

import { Keys }     from '@/core/di/keys.js';
import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { User }     from '@/models/user.js';

import {
    Friend,
    FriendStatus
} from '@/models/friend.js';

const friends = inject(Keys.FriendsStore);
const session = inject(Keys.SessionStore);

const selectedTab = ref('list');

const selectedFriend        = ref(null);
const detailsSlideDirection = ref('forwards');

/*
    Pagination / indices
*/

const pageSize     = 5;
const selectedPage = ref(0);

const pageCount = computed(() => {
    return 1 + (0 | ((friends.allFriends.length - 1) / pageSize));
});

const friendsPage = computed(oldValue => {
    let page = friends.allFriends
        .slice(
            pageSize * selectedPage.value,
            pageSize * selectedPage.value + pageSize);

    if (page.length < pageSize)
        page = page.concat(Array(pageSize - page.length).fill(null));

    if (page.length === oldValue?.length &&
        oldValue.every((v, i) => v === page[i])) {
        return oldValue;
    }

    return page;
});

const selectedIndex = computed(() =>
    friendsPage.value?.indexOf(selectedFriend.value) ?? -1);

const absoluteSelectedIndex = computed(() => 
    friends.allFriends.indexOf(selectedFriend.value) ?? -1);

/*
    If user hasn't searched yet we want to show the default
    search title. Otherwise we can show no results.
*/
const hasSearchedYet         = ref(false);
const searchResult           = ref(null);
const searchInput            = ref('');

/*
    Loading anything?
*/
const isLoadingSearchResult = ref(false);
const isLoadingFriendAction = ref(false);
const isLoadingFriends = computed(() => friends.state.value === 'loading');
const isLoading        = computed(() =>
    isLoadingSearchResult.value ||
    isLoadingFriendAction.value ||
    isLoadingFriends.value);

/*
    Throttled loading indicators
*/
const loadingThrottleMS         = 200;
const showIsLoading             = useThrottledRef(isLoading,             loadingThrottleMS);
const showIsLoadingSearchResult = useThrottledRef(isLoadingSearchResult, loadingThrottleMS);

/* Don't throttle entrance to loading state */
watch(isLoading,             v => v ? (showIsLoading.value = v)             : {});
watch(isLoadingSearchResult, v => v ? (showIsLoadingSearchResult.value = v) : {});

/*
    Connection failed?
*/
const didSearchConnFail  = ref(false);
const didFriendsConnFail = computed(() => friends.state.value === 'error');

const detailsState = computed(() => {
    switch (selectedTab.value) {
        case 'list':
            return didFriendsConnFail.value
                ? 'loadfailed'
                : friends.state.value !== 'ready'
                    ? 'empty'
                    : selectedFriend.value == null
                        ? 'nofriends'
                        : 'userresult';

        case 'search':
            return didFriendsConnFail.value || didSearchConnFail.value
                ? 'loadfailed'
                : !hasSearchedYet.value
                    ? 'search'
                    : searchResult.value == null
                        ? 'searchnotfound'
                        : 'userresult';
    }
});

const detailsUser = computed(() => {
    return selectedTab.value === 'list'
        ? selectedFriend.value?.user
        : searchResult.value;
});

watch(absoluteSelectedIndex, (newValue, oldValue) => {
    if (newValue === -1) {
        selectedFriend.value = friendsPage.value
            .filter(f => f != null)
            .findLast(f => friends.allFriends.indexOf(toRaw(f)) <= oldValue);
    }
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

watch([selectedIndex, selectedPage], (
    [newIndex, newPage], 
    [oldIndex, oldPage]) => {
        /* Normalize -1 -> 0 */
        oldIndex = Math.max(oldIndex, 0);

        if (newPage != oldPage) 
            detailsSlideDirection.value = newPage < oldPage 
                ? 'forwards' : 'backwards';
        else if (newIndex != oldIndex)
            detailsSlideDirection.value = newIndex < oldIndex
                ? 'forwards' : 'backwards';
    });

function searchSubmitted() {
    /*
        If we have a friend with this username or we searched
        for ourself then we can return immediately.
    */
    const cachedResult = friends.getFriendByUsername(searchInput.value)?.user
        ?? (searchInput.value.toLowerCase() === session.activeUser.value.username.toLowerCase() ? session.activeUser.value : null)
        ?? (searchInput.value.toLowerCase() === searchResult.value?.username.toLowerCase()      ? searchResult.value       : null);

    if (cachedResult != null) {
        hasSearchedYet.value = true;
        searchResult.value   = cachedResult;
        return;
    }

    isLoadingSearchResult.value = true;
    Dispatch.Get_UserByUsername(searchInput.value)
        .onSuccess(body => {
            didSearchConnFail.value     = false;
            hasSearchedYet.value        = true;
            isLoadingSearchResult.value = false;
            searchResult.value          = User.fromJSON(body.user);
        })
        .onHttpError((body, status) => {
            didSearchConnFail.value = false;

            if (status === 404) {
                hasSearchedYet.value = true;
                searchResult.value   = null;
            }
        })
        .onNetworkError(() => didSearchConnFail.value     = true)
        .onError       (() => isLoadingSearchResult.value = false);
}

/*
    Friendship actions
*/

async function sendRequest(user, resolve) {
    try {
        isLoadingFriendAction.value = true;
        await friends.publishCreateFriendRequest(session, user);
    } finally {
        isLoadingFriendAction.value = false;
        resolve();
    }
}

async function unfriend(user, resolve) {
    try {
        isLoadingFriendAction.value = true;
        await friends.publishDeleteFriendOrRequest(session, user.userID);
    } finally {
        isLoadingFriendAction.value = false;
        resolve();
    }
}

async function accept(user, resolve) {
    try {
        isLoadingFriendAction.value = true;
        await friends.publishAcceptFriendRequest(session, user.userID);
    } finally {
        isLoadingFriendAction.value = false;
        resolve();
    }
}

/* These are the same for now */
const unsend = unfriend;
const reject = unfriend;
</script>

<style scoped>
.friends-list {
    --overhang-x-tabs: -18px;
    --overhang-y-tabs: -24px;

    display:   flex;
    flex-flow: column;

    padding:    27px 12px 9px;
    margin-top: calc(-1 * var(--overhang-y-tabs));

    display:   flex;
    flex-flow: column;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    & > .list-tabs      { z-index: 1; }
    & > .list-details   { z-index: 0; align-self: stretch; }
    & > .list-list-view { z-index: 0; align-self: stretch; margin-top: 18px; }
    & > .list-foot      { z-index: 0; align-self: stretch; margin-top: 9px; }

    @media (width < 600px) {
        --overhang-x-tabs: -10px;
    }
}

.list-tabs {
    position: absolute;
    inset: var(--overhang-y-tabs) var(--overhang-x-tabs) auto;
}

.list-foot {
    display:         flex;
    justify-content: space-between;
    align-items:     center;
}
</style>