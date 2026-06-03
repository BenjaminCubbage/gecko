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
            :isLoading="isLoading"
            @send-request="sendRequest"
            @reject="reject"
            @unsend="unsend" />

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
                v-model:search-input="searchInput"
                :isLoading="isLoadingSearchResult"
                @search-submitted="searchSubmitted" />
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
    If user hasn't searched yet we want to show
    the default search title. Otherwise we can show
    no results.
*/
const hasSearchedYet = ref(false);
const searchResult   = ref(null);
const searchInput    = ref('');

/*
    Loading anything?
*/
const isLoadingSearchResult = ref(false);
const isLoadingFriendAction = ref(false);
const isLoadingFriends = computed(() => friends.state.value === 'loading');
const isLoading = computed(() =>
    isLoadingSearchResult.value || 
    isLoadingFriendAction.value ||
    isLoadingFriends.value);
    
/*
    Connection failed?
*/
const didSearchConnFail  = ref(false);
const didFriendsConnFail = computed(() => friends.state.value === 'error');

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
                : didFriendsConnFail.value
                    ? 'loadfailed'
                    : selectedFriend.value == null
                        ? 'nofriends'
                        : 'userresult';

        case 'search':
            return !hasSearchedYet.value
                ? 'search'
                : didFriendsConnFail.value || didSearchConnFail.value
                    ? 'loadfailed'
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
            searchResult.value          = User.fromJSON(body.user);
            isLoadingSearchResult.value = false;
        })
        .onHttpError((body, status) => {
            didSearchConnFail.value = false;
            
            if (status === 404) {
                hasSearchedYet.value = true;
                searchResult.value   = null;
            } 
        })
        .onNetworkError(() => {
            didSearchConnFail.value = true;
        })
        .onError(() => { 
            isLoadingSearchResult.value = false 
        });
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