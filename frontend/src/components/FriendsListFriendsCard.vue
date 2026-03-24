<template>
    <div
        :class="`
            friends-list-friends-card
            friends-list-friends-card--variant-${variant}`"
        :data-deleting="isDeleting"
        v-roving-container>
        <!-- User icon -->
        <svg class="icon" viewBox="0 0 36.3 36.2">
            <path class="st0 stroke" d="M25.3 27.5v-2.4H24v-1.2h-1.2v-1.2h-1.2v-1.2h-9.7v1.2h-1.2v1.2H9.4v1.2H8.2v2.4H7v3.7h1.2v1.2h17v-1.2h1.2v-3.7zM10.7
                     16.6h1.2v1.2h9.7v-1.2h1.2v-1.2H24V8.1h-1.2V6.8h-1.2V5.6h-9.7v1.2h-1.2V8H9.4v7.3h1.2v1.3z" />
            <path class="st1" d="M25.3 27.5v-2.4H24v-1.2h-1.2v-1.2h-1.2v2.4h1.2v2.4H24v2.4H9.4v1.2H8.2v1.2h17v-1.2h1.2v-3.7h-1.1zM14.3
                     17.8h7.3v-1.2h1.2v-1.2H24V8.1h-1.2V6.8h-2.4V8h1.2v6.1h-1.2v1.2h-7.3v1.2h-1.2v1.2c.6.1 1.8.1 2.4.1" />
            <path class="st2" d="M20.4 5.6h-8.5v1.2h-1.2V8H9.4v7.3h1.2v1.2H13v-1.2h-1.2V9.2H13V8.1h7.3V6.8h1.2V5.6zM19.2
                     21.4h-7.3v1.2h-1.2v1.2H9.4V25H8.2v2.4H7v3.7h2.4v-3.7h1.2V25h1.2v-1.2h9.7v-2.4z"/>
            <path class="st3 stroke" d="M28.6 9.9V8.7h-3.7v1.2h-1.2v3.7H25v1.2h3.7v-1.2h1.2V9.9zM17.3 8.7h-3.7v1.2h-1.2v3.7h1.2v1.2h3.7v-1.2h1.2V9.9h-1.2z"/>
            <!-- <path class="st3 stroke"  d="M13.7 11.1h3.7v1.2h-3.7zM25 11.1h3.7v1.2H25z"/> -->
        </svg>

        <div
            class="
                icon
                txtr-diag txtr-diag--orange
                shdw shdw--inst-orange shdw--otst-lt-gray"
            :style="{
                '--blink-freq':   blinkFreq,
                '--blink-delay':  blinkDelay,
                '--glance-x-freq':  glanceXFreq,
                '--glance-x-delay': glanceXDelay,
                '--glance-y-freq':  glanceYFreq,
                '--glance-y-delay': glanceYDelay
            }"
            aria-hidden>
        </div>

        <component
            :is="variant === 'normal' ? 'button' : 'h1'"
            class="username"
            @click="emit('showFriendDetails', friend)"
            v-roving-item="variant === 'normal'">
            @{{ friend.user.username }}
        </component>

        <span
            class="footnote">
            <!-- Squiggly arrow -->
            <svg
                v-if="
                    friend.status === FriendStatus.PendingIncoming ||
                    friend.status === FriendStatus.PendingOutgoing"
                :class="`
                    footnote-icon
                    footnote-icon--arrow-${friend.status === FriendStatus.PendingIncoming ? 'right' : 'left'}`"
                viewBox="-3 -3 39.3 23.5">
                <path class="st0 stroke" fill="#f5f9ee" d="M31.9 8.2V6.3H30v-2h-2v-2h-2v2
                         2h-1.9-2v1.9h-2V6.3h-2v-2h-1.9-2v2h-2-1.9v1.9h-2V6.3h-2v-2H4.4v2h-2v1.9
                         2h2v2h1.9 2v2h2 1.9v-2h2v-2h2v2h1.9 2v2h2 2v-2H26v2 1.9h2v-1.9h2v-2h1.9v-2h2v-2z" />

                <g class="st1">
                    <path d="M30 8.2V6.3h-2-2v1.9h-1.9-2v2h-2v2h2 2v-2H26v2h2 2v-2h1.9v-2z"/>
                    <path d="M18.1 8.2V6.3h-1.9-2v1.9h2v2h1.9 2v-2z"/>
                    <path d="M10.3 8.2v2h-2v2h2 1.9v-2h2v-2h-2z"/>
                    <path d="M6.3 8.2V6.3H4.4v1.9 2h1.9 2v-2z"/>
                </g>

                <g class="st2">
                    <path d="M31.9 8.2h2v2h-2z"/><path d="M30 10.2h2v2h-2z"/>
                    <path d="M26 12.2v2 1.9h2v-1.9h2v-2h-2z"/><path d="M24.1 10.2h2v2h-2z"/>
                    <path d="M20.1 12.2v2h2 2v-2h-2z"/><path d="M18.1 10.2h-1.9v2h1.9 2v-2z"/>
                    <path d="M14.2 8.2h2v2h-2z"/><path d="M12.2 10.2h2v2h-2z"/>
                    <path d="M8.3 12.2v2h2 1.9v-2h-1.9z"/><path d="M6.3 10.2H4.4v2h1.9 2v-2z"/>
                </g>
            </svg>

            <svg
                v-if="friend.status === FriendStatus.Active"
                class="footnote-icon footnote-icon--heart"
                viewBox="0 0 40.6 39.2">
                <path class="st0 stroke" d="M30 9.9v-4h-7.8v3.9h-3.9V5.9h-7.8v3.9H6.7v11.7h3.9v3.9h3.9v3.9h3.9v3.9h3.9v-3.9h3.9v-3.9H30v-3.9h3.9V9.8H30z" />
                <path class="st1" d="M30 13.8v7.8h3.9v-7.8zM26.1 21.5H30v3.9h-3.9zM22.3 25.4h3.9v3.9h-3.9zM18.4 29.3h3.9v3.9h-3.9z" />
                <path class="st2" d="M22.3 9.9v3.9h-3.9V9.9h-7.8v11.7h3.9v3.9h3.9v3.9h3.9v-3.9h3.9v-3.9H30V9.9z" />
            </svg>

            {{ footnoteText }}
        </span>

        <div class="separator" aria-hidden></div>

        <div class="buttons">
            <button
                v-if="friend.status === FriendStatus.PendingIncoming"
                class="
                    button button--accept
                    txtr-diag txtr-diag--green
                    shdw shdw--inst-green shdw--elevated-s
                    hn hn-check-solid"
                @click="acceptFriend"
                v-roving-item>
            </button>

            <button
                v-if="friend != null"
                class="
                    button button--unfriend
                    txtr-diag txtr-diag--dk-red
                    shdw shdw--inst-dk-red shdw--elevated-s"
                @click="deleteFriend"
                v-roving-item>
            </button>

            <!-- <button ref="button1El" v-roving-item v-if="computedFriendType === 'incoming'"    class="button" @click="acceptFriend">accept request</button>
            <button ref="button2El" v-roving-item v-if="computedFriendType === 'incoming'"    class="button" @click="deleteFriend">delete request</button>
            <button ref="button1El" v-roving-item v-if="computedFriendType === 'outgoing'"    class="button" @click="deleteFriend">cancel request</button>
            <button ref="button1El" v-roving-item v-if="computedFriendType === 'not-friends'" class="button" @click="requestFriend">send request</button> -->
        </div>
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    onBeforeUnmount,
    ref,
    watch
} from 'vue';

import { User }         from '@/models/user.js';
import { FriendStatus } from '@/models/friend.js';
import { Keys }         from '@/core/di/keys.js';

const props = defineProps({
    user: {
        type:     User,
        required: true
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
    }
});

const emit = defineEmits(['showFriendDetails']);

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);

const blinkFreq  = ref(`${12000 + Math.random() * 3000}ms`);
const blinkDelay = ref(`${Math.random() * 12000}ms`);

const glanceXFreq  = ref(`${20000 + Math.random() * 5000}ms`);
const glanceXDelay = ref(`${Math.random() * 20000}ms`);

const glanceYFreq  = ref(`${28000 + Math.random() * 3000}ms`);
const glanceYDelay = ref(`${Math.random() * 28000}ms`);

const isDeleting  = ref(false);
let deletingTimer = null;

/*
    The friend object associated with the provided user ID.

    If the user is not a pending or active friend, this will
    be null.
*/
const friend = computed(() => {
    return friends.getFriendByUserID(props.user.userID);
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

async function requestFriend() {

}

onBeforeUnmount(() => {
    clearTimeout(deletingTimer);
});
</script>

<style scoped>
.friends-list-friends-card {
    contain:   content;
    isolation: isolate;

    display: grid;
    padding: 6px 12px;

    color:     black;
    font-size: 2.4rem;

    &:focus-visible {
        outline: 3px solid black;
    }

    &.friends-list-friends-card--variant-normal {
        gap: 3px 8px;

        grid-template:
            "icon username separator buttons" auto
            "icon footnote separator buttons" auto /
            auto auto     1fr        auto;

        & > .icon      { grid-area: icon;      place-self: center start; }
        & > .username  { grid-area: username;  place-self: end start; }
        & > .footnote  { grid-area: footnote;  place-self: start left; }
        & > .separator { grid-area: separator; place-self: center right; }
        & > .buttons   { grid-area: buttons;   place-self: center right; }
    }

    &.friends-list-friends-card--variant-details {
        gap:     0 15px;
        padding: 15px 15px 9px;

        grid-template:
            "icon      username  separator" auto
            "icon      footnote  footnote"  auto
            ".         .         ."         12px
            "buttons   buttons buttons"  auto /
             auto      auto    1fr;

        & > .icon      { grid-area: icon;      place-self: center;}
        & > .username  { grid-area: username;  place-self: center left; }
        & > .footnote  { grid-area: footnote;  place-self: end    left; }
        & > .separator { grid-area: separator; place-self: center; }
        & > .buttons   { grid-area: buttons;   place-self: center; }

        /*
            These are variations to the classes defined below.

            When we are showing the details screen, we want
            to style some classes a bit differently.
        */

        & > .username {
            pointer-events: none;
            font-size:      1.1em;
        }

        & > .buttons > .button {
            height:    32px;
            padding:   0 20px;
            width:     auto;

            -webkit-text-stroke: var(--text-stroke-s);
            font-size:           1.9rem;
            letter-spacing:      0.07em;
            text-shadow:         none;

            &::after {
                content: 'UNFRIEND';
            }
        }
    }
}

.icon {
    height: 50px;
    margin: 0 -7px;

    filter: drop-shadow(3px 3px var(--col-lt-gray-5));

    & > .stroke {
        stroke: black;
        stroke-width: 4px;
    }

    & > .st0 { fill: var(--col-orange-4); }
    & > .st1 { fill: var(--col-orange-8); }
    & > .st2 { fill: var(--col-orange-0); }
    & > .st3 {
        transform-origin: center;
        scale: 0.9;
        translate: -1px -1px;
        fill:#FFFFFF;
    }
/*
    &::after {
        content: 'o_o';
        display: block;

        -webkit-text-stroke: var(--text-stroke-s);

        animation:
            blink    var(--blink-freq)  var(--blink-delay)      linear infinite,
            glance-x var(--glance-x-freq) var(--glance-x-delay) ease   infinite,
            glance-y var(--glance-y-freq) var(--glance-y-delay) ease   infinite;

        translate:
            var(--glance-x)
            var(--glance-y);
    } */
}

@keyframes blink {
    2% { content: '-_-'; }
    3% { content: 'o_o'; }
}

@property --glance-x { inherits: false; syntax: '<length>'; initial-value: 0px; }
@property --glance-y { inherits: false; syntax: '<length>'; initial-value: 0px; }

@keyframes glance-x {
    0%,  21% { --glance-x:  0px; }
    1%,  10% { --glance-x: -2px; }
    11%, 20% { --glance-x:  2px; }
}

@keyframes glance-y {
    0%,  21% { --glance-y:  0px; }
    1%,  10% { --glance-y: -2px; }
    11%, 20% { --glance-y:  2px; }
}

.username,
.footnote {
    filter:
        drop-shadow(3px 3px var(--col-lt-gray-5));

    line-height: 0.85;

    -webkit-text-stroke: var(--text-stroke-s);
    text-align:          left;

    &.username {
        overflow-wrap: anywhere;

        &:hover,
        &:focus-visible,
        &:active {
            scale: 1.03;
        }
    }

    &.footnote {
        filter:
            drop-shadow(2px 2px var(--col-lt-gray-5));

        cursor: default;
        font-size:            0.8em;
        font-variant-numeric: tabular-nums;

        & > .footnote-icon {
            height:         1em;
            vertical-align: middle;

            &.footnote-icon--arrow-right,
            &.footnote-icon--arrow-left {
                & > .stroke {
                    stroke-width: 5px;
                    stroke: black;
                }
            }

            &.footnote-icon--arrow-right {
                & > .st0 { fill: var(--col-green-0); }
                & > .st1 { fill: var(--col-green-3); }
                & > .st2 { fill: var(--col-green-5); }
            }

            &.footnote-icon--arrow-left {
                & > .st0 { fill: var(--col-lt-blue-1); }
                & > .st1 { fill: var(--col-lt-blue-4); }
                & > .st2 { fill: var(--col-lt-blue-7); }

                scale: -1 1;
            }

            &.footnote-icon--heart {
                & > .st0 { fill: var(--col-magenta-1); }
                & > .st1 { fill: var(--col-magenta-6); }
                & > .st2 { fill: var(--col-magenta-3); }
                    
                & > .stroke {
                    stroke-width: 9px;
                    stroke: black;
                }
            }
        }
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
            var(--checker-col-1) 0.25turn,
            transparent          0.25turn 0.50turn,
            var(--checker-col-2) 0.50turn 0.75turn,
            transparent          0.75turn
        );

    background-repeat: repeat-x;
    background-position: right;
    background-size: var(--checker-size) var(--checker-size);

    @supports (width: round(down, 100%, 1px)) {
        width: round(down, 100%, var(--checker-size));
    }
}

.buttons {
    display:        flex;
    flex-direction: row;
    gap: 7px;
}

.button {
    --hl: brightness(1);

    width:  32px;
    height: 32px;

    display:       grid;
    place-content: center;

    padding-left:   1.2px;
    padding-bottom: 1.4px;

    font-size:      2.1rem;
    letter-spacing: 0.04em;

    border-radius: var(--radius-s);
    border:        var(--border-s);

    translate:
        0
        calc(var(--shdw-dist-elevation) * -1);

    filter:
        drop-shadow(3px 3px var(--col-lt-gray-5))
        var(--hl);

    &.button--accept {
        text-shadow:
             1.5px  1.5px var(--col-green-5),
            -1.5px -1.5px var(--col-green-1);

        background:
            linear-gradient(
                var(--col-green-2) 50%,
                var(--col-green-4) 50%);

        font-size: 0.65em;
        padding-bottom: 0;
        padding-left: 1.5px;
        padding-top: 1.5px;
    }

    /*
        Variants
    */
    &.button--unfriend {
        text-shadow:
             1.5px  1.5px var(--col-red-4),
            -1.5px -1.5px var(--col-red-1);
            
        background:
            linear-gradient(
                var(--col-red-2) 50%,
                var(--col-red-3) 50%);

        &::after {
            content: 'x';
        }
    }

    &:hover,
    &:active {
        --hl: var(--filter-hl-1);
    }

    &:active {
        --shdw-dist-elevation: 0;
    }
}
</style>