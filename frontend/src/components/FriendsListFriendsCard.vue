<template>
    <component
        :is="variant === 'normal' ? 'button' : 'div'"
        :class="`
            friends-list-friends-card
            friends-list-friends-card--variant-${variant}
            txtr-vert`"
        :data-deleting="isDeleting"
        :data-variant="variant"
        :data-expanded="isExpanded"
        :data-vacant="user == null"
        :aria-label="itemLabel"
        v-roving-container
        @click="clicked">
        <div
            class="user-info"
            tabindex="-1"
            v-modal-auto-focus-target>
            <IconFriendUser 
                class="user-icon"
                :variant="
                    user != null 
                        ? 'normal'
                        : 'vacant'" />

            <template v-if="user != null">
                <component
                    :is="variant === 'normal' ? 'button' : 'div'"
                    :aria-label="`Show details for @${user.username}`"
                    class="username text-stroke--s"
                    v-roving-item="variant === 'normal'"
                    @click="clicked"
                    :disabled="variant === 'details'">
                    @{{ user.username }}
                </component>

                <span class="footnote text-stroke--s">
                    <IconFriendRequestArrow 
                        v-if="
                            friend.status === FriendStatus.PendingIncoming ||
                            friend.status === FriendStatus.PendingOutgoing"
                        class="footnote-icon"
                        :variant="friend.status === FriendStatus.PendingIncoming ? 'incoming' : 'outgoing'" />

                    <IconFriendHeart
                        v-if="friend.status === FriendStatus.Active"
                        class="footnote-icon" />

                    {{ footnoteText }}
                </span>
            </template>
        </div>

        <div class="buttons" v-if="user != null">
            <div class="separator" aria-hidden></div>

            <BaseButton
                v-if="friend.status === FriendStatus.PendingIncoming"
                class="
                    button button--accept
                    txtr-vert txtr-vert--green
                    shdw shdw--inst-green
                    hn hn-check-solid"
                aria-label="Accept request"
                v-roving-item
                @click="acceptFriend" />

            <BaseButton
                class="button button--unfriend
                       txtr-vert txtr-vert--red
                       shdw shdw--inst-dk-red"
                aria-label="Unfriend"
                v-roving-item
                @click="deleteFriend" />
        </div>
    </component>
</template>

<script setup>
import {
    computed,
    inject,
    onBeforeUnmount,
    ref,
    watch
} from 'vue';

import BaseButton             from './BaseButton.vue';
import IconFriendHeart        from './IconFriendHeart.vue';
import IconFriendRequestArrow from './IconFriendRequestArrow.vue';
import IconFriendUser         from './IconFriendUser.vue';

import { User }         from '@/models/user.js';
import { FriendStatus } from '@/models/friend.js';
import { Keys }         from '@/core/di/keys.js';

const props = defineProps({
    user: {
        type:    User,
        default: null
    },

    variant: {
        type:    String,
        default: 'normal',
        validator(value) {
            return [
                'normal',
                'details'
            ].includes(value);
        }
    },

    isExpanded: {
        type:    Boolean,
        default: false
    }
});

const emit = defineEmits(['showFriendDetails']);

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);

const isDeleting  = ref(false);
let deletingTimer = null;

/*
    The friend object associated with the provided user ID.

    If the user is not a pending or active friend, this will
    be null.
*/
const friend = computed(() => {
    return props.user != null
        ? friends.getFriendByUserID(props.user.userID)
        : null;
});

const itemLabel = computed(() => {
    if (props.user == null)
        return 'Empty slot';

    return `@${props.user.username}, ${footnoteText.value},`;
});

/*
    This is needed to prevent SR from announcing "clickable"
    on elements that have no-op click events.
*/
const clicked = computed(() => {
    return props.variant === 'normal' 
        ? handleClick
        : null;
});

watch(() => props.friend, () => {
    isDeleting.value = false
});

const footnoteText = computed(() => {
    switch (friend.value?.status) {
    case FriendStatus.Active:
        return props.variant === 'normal'
            ? 'Friends'
            : `Friends since: ${friend.value.acceptedOn}`;
    case FriendStatus.PendingIncoming: return 'Wants to be friends';
    case FriendStatus.PendingOutgoing: return 'Request sent';
    case null:                         return 'Not Friends';
    }
});

async function acceptFriend() {
    await friends.publishAcceptFriendRequest(session, friend.value.user.userID);
}

async function deleteFriend() {
    isDeleting.value = true;

    await new Promise((resolve => (deletingTimer = setTimeout(resolve, 500))));
    await friends.publishDeleteFriendOrRequest(session, friend.value.user.userID);
    isDeleting.value = false;
}

void requestFriend;
async function requestFriend() {

}

function handleClick() {
    if (friend.value != null)
        emit('showFriendDetails', friend.value);
}

onBeforeUnmount(() => {
    clearTimeout(deletingTimer);
});
</script>

<style scoped>
.friends-list-friends-card {
    contain:   content;
    isolation: isolate;

    min-height: 7rem;

    --txtr-vert-col-1: var(--col-gray-0);
    --txtr-vert-col-2: var(--col-gray-1);

    --shadow-color:    var(--col-lt-gray-6);
    --elevation-color: var(--col-gray-4);

    grid-template:
        "user-info buttons" auto /
         auto      1fr;

    align-items: center;
    display:     grid;
    gap:         12px;
    padding:     6px 12px;

    font-size: 2.4rem;

    box-shadow:
        0 var(--shadow-dist-s) var(--elevation-color),
        0 6px color-mix(in hsl, var(--shadow-color), transparent 70%),
        inset 0 0 0 3px var(--col-lt-gray-1);

    border:        var(--border-thickness-s) solid var(--col-gray-4);
    border-radius: var(--radius-s);

    :where(button&:hover),
    &[data-expanded=true] {
        border-color:      var(--col-gray-5);
        --elevation-color: var(--col-gray-5);
        --shadow-color:    var(--col-gray-4);
        scale: 1.01;
    }

    &:focus-visible {
        border-color: black;
        --elevation-color: black;
        --shadow-color: var(--col-gray-4);
        outline: none;
        scale: 1.01;
    }

    &:has(.user-info:focus-visible) {
        & > .user-info {
            outline: 3px solid black;
            outline-offset: -3px;
        }
    }

    & > .user-info { grid-area: user-info; }
    & > .buttons   { grid-area: buttons; }
}

.user-icon {
    height: 50px;
    filter: drop-shadow(3px 3px var(--col-lt-gray-5));
}

.username,
.footnote {
    filter:
        drop-shadow(3px 3px var(--col-lt-gray-5));

    text-align:  left;
    line-height: 0.85;
}

.username {
    overflow-wrap: anywhere;
}

.footnote {
    filter:
        drop-shadow(2px 2px var(--col-lt-gray-5));

    font-size: 0.8em;

    & > .footnote-icon {
        height:         1em;
        vertical-align: middle;
    }
}

.user-info {
    display: grid;
    gap: 3px 8px;

    grid-template:
        "icon username" auto
        "icon footnote" auto /
            auto 1fr;

    & > .user-icon { grid-area: icon;      place-self: center left; }
    & > .username  { grid-area: username;  place-self: end    left; }
    & > .footnote  { grid-area: footnote;  place-self: start  left; }
}

.buttons {
    display:               grid;
    grid-template-columns: 1fr;
    grid-auto-flow:        column;
    grid-auto-columns:     auto;

    gap:         7px;
    place-items: center;

    & > .separator {
        grid-area: 1 / 1;
    }
}

.button {
    --filter-etc: drop-shadow(3px 3px var(--col-lt-gray-5));

    width:  37px;
    height: 30px;

    display:       grid;
    place-content: center;

    padding: 0;

    &.button--accept {
        &::before {
            font-size: 0.85em;
            color: white;
            -webkit-text-stroke: 4.4px black;
            translate: 0.06em 0.065em;
        }
    }

    &.button--unfriend::before {
        content:     'X';
        line-height: 1;
        translate:   0.6px 0.5px;

        color: white;
        -webkit-text-stroke: 4px black;
        scale: 1.3 1;
    }


    &:hover,
    &:active {
        --hl: var(--filter-hl-1);
    }

    &:active {
        --shdw-dist-elevation: 0;
    }
}

.separator {
    --checker-col-1: var(--col-lt-gray-4);
    --checker-col-2: var(--col-lt-gray-5);
    --checker-size: 8px;

    height: var(--checker-size);
    width:  100%;

    background-image:
        conic-gradient(
            var(--checker-col-1) 0.25turn,          transparent 0.25turn 0.50turn,
            var(--checker-col-2) 0.50turn 0.75turn, transparent 0.75turn
        );

    background-size:
        var(--checker-size)
        var(--checker-size);

    background-repeat: repeat-x;

    @supports (width: round(down, 100%, 1px)) {
        max-width: round(down, 100%, var(--checker-size));
    }
}

/*
    Details
*/

/*
    When we are showing the details screen, we want
    to style some classes a bit differently.

    These are variations to the classes defined above.
*/

.friends-list-friends-card[data-variant=details] {
    grid-template:
        "user-info" auto
        "buttons"   auto /
         1fr;

    gap:     0;
    padding: 0;

    .buttons,
    .user-info {
        box-shadow: inset 0 0 0 3px var(--col-lt-gray-1);
    }

    .buttons {
        align-self: stretch;
        border-top: var(--border-thickness-s) solid var(--col-gray-3);
        padding: 12px;
        background:
            linear-gradient(
                var(--col-lt-gray-3) 50%,
                var(--col-lt-gray-4) 50%);
    }

    .user-info {
        background:
            linear-gradient(
                var(--col-gray-0) 50%,
                var(--col-gray-1) 50%);

        display: grid;
        gap:     3px 8px;
        padding: 9px 15px;

        grid-template:
            "icon username" auto
            "icon footnote" auto /
             auto 1fr;

        & > .user-icon { grid-area: icon;      place-self: center start; }
        & > .username  { grid-area: username;  place-self: end start; }
        & > .footnote  { grid-area: footnote;  place-self: start left; }
    }

    .username {
        pointer-events: none;
        font-size:      1.1em;
    }

    .separator {
        min-width: 32px;

        --checker-col-1: var(--col-lt-gray-5);
        --checker-col-2: var(--col-lt-gray-6);
    }
}

.friends-list-friends-card[data-vacant=true] {
    .user-icon {
        filter: none;
    }
}
</style>