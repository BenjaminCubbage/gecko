<template>
    <div class="friends-list">
        <FriendsListBorder title="ADD FRIEND">
            <FriendsListSearchBar
                v-model="searchText"
                :mode="searchMode"
                @submit="search" />

            <FriendsListPlaceholderMessage
                v-if="searchResultPlaceholderVariant"
                :variant="searchResultPlaceholderVariant"
                :message="searchResultPlaceholderMessage" />

            <FriendsListEntry
                v-else
                :entryType="searchResultType"
                :user="searchResultUser"
                @sendRequest="sendRequest"
                @acceptRequest="acceptRequest"
                isSearchResult>
                <template #username="{ user }">
                    {{ user['username'] }}
                </template>
            </FriendsListEntry>

            <FriendsListDivider v-if="friends.pendingIncoming().value.length">
                Incoming Requests
            </FriendsListDivider>
            
            <FriendsListEntry
                v-for="user in friends.pendingIncoming().value"
                :user="user"
                entryType="pendingin"
                @delete="deleteFriendOrRequest"
                @acceptRequest="acceptRequest"
                :key="user['user_id']">
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
                v-for="user in friends.pendingOutgoing().value"
                :user="user"
                entryType="pendingout"
                @delete="deleteFriendOrRequest"
                :key="user['user_id']">
                <template #username="{ user }">
                    {{ user['username'] }}
                </template>
            </FriendsListEntry>

            <FriendsListEntry
                v-for="user in friends.activeFriends().value"
                :user="user"
                entryType="active"
                @delete="deleteFriendOrRequest"
                :key="user['user_id']">
                <template #username="{ user }">
                    {{ user.username }}
                </template>
            </FriendsListEntry>
        </FriendsListBorder>
    </div>
</template>

<script setup>
import { shallowRef, toRaw, ref, inject, watch, nextTick, triggerRef } from 'vue';
import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { equalsIgnoreCase } from '@/core/string/EqualsIgnoreCase.js';
import { Keys } from '@/core/store/Keys.js';

import FriendsListBorder from './FriendsListBorder.vue';
import FriendsListEntry from './FriendsListEntry.vue';
import FriendsListSearchBar from './FriendsListSearchBar.vue';
import FriendsListPlaceholderMessage from './FriendsListPlaceholder.vue';
import FriendsListDivider from './FriendsListDivider.vue';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);

const searchText = ref('');
const searchMode = ref('normal');

const searchResultUser = ref(null);
const searchResultType = ref('me');

// note(ben): Placeholder only shown when this is not falsy (like null)
const searchResultPlaceholderVariant = ref('loading');
const searchResultPlaceholderMessage = ref('Loading Results');

const friendsPlaceholderVariant = ref('loading');
const friendsPlaceholderMessage = ref('Loading Friends');

watch(session.state(), state => {
    if (state === 'ready' && session.activeUser().value && searchText.value === '') {
        searchResultUser.value = session.activeUser().value;
        searchResultType.value = 'me';
        searchResultPlaceholderVariant.value = null;
    }
}, { immediate: true });

watch(session.activeUser(), async user => {
    if (searchResultType.value === 'me') {
        searchResultUser['asdfsdfsdfdsf'] = searchResultUser['asdfsdfsdfdsf'] ? searchResultUser['asdfsdfsdfdsf'] + 1 : 1;
        triggerRef(searchResultUser);
        await nextTick();
        triggerRef(searchResultUser);
    }
});

watch([ friends.state(),
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
        
        case 'error':
            friendsPlaceholderVariant.value = 'error';
            friendsPlaceholderMessage.value = `Couldn't Load Friends`;

            searchResultPlaceholderVariant.value = 'error';
            searchResultPlaceholderMessage.value = `Couldn't Load Friends`;
            break;

        case 'ready':
            if (friends.pendingOutgoing() != null &&
                friends.activeFriends() != null) {
                if (!friends.activeFriends().value.length &&
                    !friends.pendingOutgoing().value.length) {
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
    await friends.createFriendRequest(session, toRaw(user));

    if (toRaw(searchResultUser.value) === toRaw(user))
        searchResultType.value = 'pendingout';
}

async function acceptRequest(user) {
    await friends.acceptFriendRequest(session, toRaw(user));

    if (toRaw(searchResultUser.value) === toRaw(user))
        searchResultType.value = 'active';
}

async function deleteFriendOrRequest(user) {
    await friends.deleteFriendOrRequest(session, toRaw(user));

    if (toRaw(searchResultUser.value) === toRaw(user))
        searchResultType.value = 'notfriends';
}

function search() {
    // note(ben): We check to see if the user is already friended by their
    // username (like a cache)

    // Therefore, If a user changes their name and the friends list hasn't
    // yet been refreshed, the search result will be for the _old_ username.

    if (equalsIgnoreCase(session.activeUser().value['username'], searchText.value)) {
        searchResultPlaceholderVariant.value = null;
        searchResultUser.value = session.activeUser().value;
        searchResultType.value = 'me';
        return;
    }

    const [user, type] = friends.getFriendInCacheByUsername(searchText.value);

    if (user) {
        searchResultUser.value = user;
        searchResultPlaceholderVariant.value = null;

        switch (type) {
            case 'active':     searchResultType.value = 'active';     return;
            case 'pendingin':  searchResultType.value = 'pendingin';  return;
            case 'pendingout': searchResultType.value = 'pendingout'; return;
            default:
                searchResultPlaceholderVariant.value = 'error';
                searchResultPlaceholderMessage.value = `Uhhh... that shouldn't happen`;
                throw new Error('[FriendsList]: Unknown friendship type when getting friend from cache');
        }
    }

    searchMode.value = 'loading';
    Dispatch.Get_UserByUsername(searchText.value)
        .onSuccess(body => {
            searchMode.value = 'normal';

            const duplicate = friends.updateFriendInCacheIfExists(
                body['user']['user_id'], 
                body['user']);

            if (!duplicate) {
                searchResultPlaceholderVariant.value = null;
                searchResultUser.value = duplicate ?? body['user'];
                searchResultType.value = 'notfriends';
            } else {
                // We already know this person after all--use the
                // cache instead
                search();
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

            searchResultUser.value = null;
            searchMode.value = 'normal';
        })
        .onNetworkError(() => {
            searchResultPlaceholderVariant.value = 'error';
            searchResultPlaceholderMessage.value = `Couldn't Connect to the Server!`;
            searchMode.value = 'normal';
        });
}
</script>

<style scoped>
.friends-list {
    container: c / inline-size;

    display: grid;

    width: 600px;
/*
    width: 1000px;
    grid-template-columns: 1fr 1fr; */
    gap: 0px;

    margin-bottom: 12px;

    z-index: 0;
}
</style>