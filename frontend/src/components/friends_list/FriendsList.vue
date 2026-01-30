<template>
    <div class="friends-list">
        <FriendsListBorder title="ADD FRIEND">
            <FriendsListSearchBar
                v-model="searchText"
                :mode="searchMode"
                :disabled="!session.activeUser().value"
                @submit="search" />

            <FriendsListPlaceholderMessage
                v-if="searchResultPlaceholderVariant"
                :variant="searchResultPlaceholderVariant"
                :message="searchResultPlaceholderMessage" />

            <FriendsListEntry
                v-else
                :entryType="searchResult.type"
                :user="searchResult.user"
                @sendRequest="sendRequest"
                @acceptRequest="acceptRequest"
                isSearchResult>
                <template #username="{ user }">
                    {{ user.username }}
                </template>
            </FriendsListEntry>

            <FriendsListDivider v-if="friends.pendingIncoming().length">
                Incoming Requests
            </FriendsListDivider>

            <FriendsListEntry
                v-for="friend in friends.pendingIncoming()"
                :user="friend.user"
                entryType="pendingin"
                @delete="deleteFriendOrRequest"
                @acceptRequest="acceptRequest"
                :key="friend.user.userID">
                <template #username="{ user }">
                    {{ user.username }}
                </template>
            </FriendsListEntry>
        </FriendsListBorder>

        <FriendsListBorder title="FRIENDS">
            <FriendsListPlaceholderMessage
                v-if="friendsPlaceholderVariant"
                :variant="friendsPlaceholderVariant"
                :message="friendsPlaceholderMessage" />

            <FriendsListEntry
                v-for="friend in friends.pendingOutgoing()"
                :user="friend.user"
                entryType="pendingout"
                @delete="deleteFriendOrRequest"
                :key="friend.user.userID">
                <template #username="{ user }">
                    {{ user.username }}
                </template>
            </FriendsListEntry>

            <FriendsListEntry
                v-for="friend in friends.activeFriends()"
                :user="friend.user"
                :acceptedOn="friend.acceptedOn"
                entryType="active"
                @delete="deleteFriendOrRequest"
                :key="friend.user.userID">
                <template #username="{ user }">
                    {{ user.username }}
                </template>
            </FriendsListEntry>
        </FriendsListBorder>
    </div>
</template>

<script setup>
import { ref, inject, watch } from 'vue';
import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { equalsIgnoreCase } from '@/core/string/EqualsIgnoreCase.js';
import { Keys } from '@/core/store/Keys.js';
import { User } from '@/core/models/User.js';

import FriendsListBorder from './FriendsListBorder.vue';
import FriendsListEntry from './FriendsListEntry.vue';
import FriendsListSearchBar from './FriendsListSearchBar.vue';
import FriendsListPlaceholderMessage from './FriendsListPlaceholder.vue';
import FriendsListDivider from './FriendsListDivider.vue';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);

const searchText = ref('');
const searchMode = ref('normal');

// { user: <number>, type: <string>, acceptedOn: <string> | null } | null
const searchResult = ref(null);

// note(ben): Placeholder only shown when this is not falsy (like null)
const searchResultPlaceholderVariant = ref('loading');
const searchResultPlaceholderMessage = ref('Loading Results');

const friendsPlaceholderVariant = ref('loading');
const friendsPlaceholderMessage = ref('Loading Friends');

watch(session.state(), state => {
    if (state === 'ready' && session.activeUser().value && !searchResult.value) {
        searchResultPlaceholderVariant.value = null;
        searchResult.value = {
            user: session.activeUser().value,
            type: 'me'
        };
    }
}, { immediate: true });

watch([ 
    friends.state(),
    friends.pendingIncoming(),
    friends.pendingOutgoing(),
    friends.activeFriends()
], () => {
    switch (friends.state().value) {
    case 'loading':
    case 'uninitialized':
        friendsPlaceholderVariant.value = 'loading';
        friendsPlaceholderMessage.value = 'Loading Friends';
        break;

    case 'loggedout':
        friendsPlaceholderVariant.value = 'info';
        friendsPlaceholderMessage.value = `Log in to have friends`;

        searchResultPlaceholderVariant.value = 'info';
        searchResultPlaceholderMessage.value = `Log in to have friends`;
        break;

    case 'error':
        friendsPlaceholderVariant.value = 'error';
        friendsPlaceholderMessage.value = `Couldn't Load Friends`;

        if (searchResult.value == null) {
            searchResultPlaceholderVariant.value = 'error';
            searchResultPlaceholderMessage.value = `Couldn't Load Friends`;
        }
        break;

    case 'ready':
        if (friends.pendingOutgoing() != null &&
            friends.activeFriends() != null) {
            if (!friends.activeFriends().length &&
                !friends.pendingOutgoing().length) {
                friendsPlaceholderVariant.value = 'info';
                friendsPlaceholderMessage.value = 'No Friends (how sad)';
            } else {
                friendsPlaceholderVariant.value = null;
            }
        }
        break;
    }
}, { immediate: true, deep: true });

async function sendRequest(user) {
    await friends.createFriendRequest(session, user);

    if (searchResult.value.user.userID === user.userID)
        searchResult.value.type = 'pendingout';
}

async function acceptRequest(user) {
    await friends.acceptFriendRequest(session, user.userID);

    if (searchResult.value.user.userID === user.userID)
        searchResult.value.type = 'active';
}

async function deleteFriendOrRequest(user) {
    await friends.deleteFriendOrRequest(session, user.userID);

    if (searchResult.value.user.userID === user.userID)
        searchResult.value.type = 'notfriends';
}

function search() {
    if (!session.activeUser().value)
        return;

    if (tryFillSearchResultFromCache(searchText.value))
        return;

    searchMode.value = 'loading';
    Dispatch.Get_UserByUsername(searchText.value)
        .onSuccess(body => {
            searchMode.value = 'normal';

            const user      = User.fromJSON(body['user']);
            const duplicate = friends.updateFriendInCacheIfExists(user.userID, {
                user: user
            });

            if (!duplicate) {
                searchResultPlaceholderVariant.value = null;
                searchResult.value = {
                    user: duplicate ?? user,
                    type: 'notfriends'
                };
            } else {
                // We already know this person after all--use the
                // cache instead
                tryFillSearchResultFromCache(searchText.value);
            }
        })
        .onHttpError((body, status) => {
            if (status == 404) {
                searchResultPlaceholderVariant.value = 'info';
                searchResultPlaceholderMessage.value = 'User Not Found';
            } else {
                console.warn(`[FriendsList] Server responded with ${status}: `, body);

                searchResultPlaceholderVariant.value = 'error';
                searchResultPlaceholderMessage.value = `Oh, darn. The server responded with an `
                                                     + `error: ${status}: ${JSON.stringify(body)}`;
            }

            searchResult.value = null;
            searchMode.value = 'normal';
        })
        .onNetworkError(() => {
            searchResultPlaceholderVariant.value = 'error';
            searchResultPlaceholderMessage.value = `Couldn't Connect to the Server!`;
            searchMode.value = 'normal';
        });
}

function tryFillSearchResultFromCache(username) {
    if (friends.state().value !== 'ready')
        return false;

    if (session.activeUser().value && equalsIgnoreCase(session.activeUser().value.username, username)) {
        searchResultPlaceholderVariant.value = null;
        searchResult.value = {
            user: session.activeUser().value,
            type: 'me'
        };

        return true;
    }

    const [friend, type] = friends.getFriendInCacheByUsername(username);

    if (friend) {
        searchResult.value.user = friend.user;
        searchResultPlaceholderVariant.value = null;

        switch (type) {
        case 'active':     searchResult.value.type = 'active';     return true;
        case 'pendingin':  searchResult.value.type = 'pendingin';  return true;
        case 'pendingout': searchResult.value.type = 'pendingout'; return true;
        default:
            searchResultPlaceholderVariant.value = 'error';
            searchResultPlaceholderMessage.value = `Uhhh... that shouldn't happen`;
            throw new Error('[FriendsList]: Unknown friendship type when getting friend from cache');
        }
    }

    return false;
}
</script>

<style scoped>
.friends-list {
    width: 600px;
}
</style>