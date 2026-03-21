<template>
    <div
        class="friends-list-friends-card"
        :data-deleting="isDeleting"
        v-roving-container>
        <div
            class="
                icon
                txtr-diag txtr-diag--orange
                shdw shdw--inst-orange shdw--otst-white"
            :style="{
                '--blink-freq':   blinkFreq,
                '--blink-delay':  blinkDelay,
                '--glance-x-freq':  glanceXFreq,
                '--glance-x-delay': glanceXDelay,
                '--glance-y-freq':  glanceYFreq,
                '--glance-y-delay': glanceYDelay
            }"
            inert>
        </div>

        <span class="username">@{{ friend.user.username }}</span>

        <div class="separator"></div>

        <div class="buttons">
            <button ref="button1El" v-roving-item v-if="computedFriendType === 'active'"      class="button txtr-diag txtr-diag--dk-red shdw shdw--inst-red shdw--elevated-s" @click="deleteFriend">x</button>
            <button ref="button1El" v-roving-item v-if="computedFriendType === 'incoming'"    class="button" @click="acceptFriend">accept request</button>
            <button ref="button2El" v-roving-item v-if="computedFriendType === 'incoming'"    class="button" @click="deleteFriend">delete request</button>
            <button ref="button1El" v-roving-item v-if="computedFriendType === 'outgoing'"    class="button" @click="deleteFriend">cancel request</button>
            <button ref="button1El" v-roving-item v-if="computedFriendType === 'not-friends'" class="button" @click="requestFriend">send request</button>
        </div>
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    onBeforeUnmount,
    ref
} from 'vue';

import { Friend } from '@/models/friend.js';
import { Keys }   from '@/core/di/keys.js';

const props = defineProps({
    friend: {
        type:     Friend,
        required: true
    },

    friendType: {
        type:    String,
        default: 'auto-detect',
        validator(value) {
            return [
                'auto-detect',
                'active',
                'incoming',
                'outgoing',
                'not-friends'
            ].includes(value);
        }
    }
});

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

const computedFriendType = computed(() => {
    if (props.friendType !== 'auto-detect')
        return props.friendType;

    if (friends.pendingIncoming.some(f => f.user.userID === props.friend.user.userID)) return 'incoming';
    if (friends.pendingOutgoing.some(f => f.user.userID === props.friend.user.userID)) return 'outgoing';
    if (friends.activeFriends  .some(f => f.user.userID === props.friend.user.userID)) return 'active';

    return 'not-friends';
});

async function acceptFriend() {
    await friends.publishAcceptFriendRequest(session, props.friend.user.userID);
}

async function deleteFriend() {
    isDeleting.value = true;

    await new Promise((resolve => (deletingTimer = setTimeout(resolve, 500))));
    await friends.publishDeleteFriendOrRequest(session, props.friend.user.userID);
}

async function requestFriend() {

}

onBeforeUnmount(() => {
    clearTimeout(deletingTimer);
});
</script>

<style scoped>
.friends-list-friends-card {
    contain: content;

    grid-template:
        "icon username separator buttons" auto /
         auto auto     1fr       auto;

    display: grid;
    gap:     6px 12px;
    padding: 12px 24px;

    color:     black;
    font-size: 2.4rem;

    &[data-deleting=true] {
        filter:         grayscale(1);
        opacity:        0.8;
        pointer-events: none;

        & > .icon::after {
            content:   'XoX';
            animation: none;
            translate: 0 3px;
        }
    }

    & > .icon      { grid-area: icon;      place-self: stretch start; }
    & > .username  { grid-area: username;  place-self: center start; }
    & > .separator { grid-area: separator; place-self: center right; }
    & > .buttons   { grid-area: buttons;   place-self: center start; }
}

.icon {
    width: 42px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    line-height: 0.8;
    text-align: center;

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
    }
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

.username {
    text-shadow:
        var(--shadow-dist-s)
        var(--shadow-dist-s)
        var(--col-lt-gray-2);

    cursor:      default;
    line-height: 1.2;
}

.separator {
    --checker-col-1: var(--col-orange-3);
    --checker-col-2: var(--col-orange-4);
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
    flex-direction: row-reverse;

    & > .button {
        --press-depth: 0px;

        width:  32px;
        height: 26px;

        display:       grid;
        place-content: center;

        padding-left:   1.2px;
        padding-bottom: 1.4px;

        font-size:      2.1rem;
        letter-spacing: 0.04em;

        text-shadow:
             1.5px  1.5px var(--col-red-4),
            -1.5px -1.5px var(--col-red-1);

        background:
            linear-gradient(
                var(--col-red-2) 50%,
                var(--col-red-3) 50%);

        border-radius: var(--radius-s);
        border:        var(--border-s);

        translate:
            0
            calc(var(--shdw-dist-elevation) * -1);

        &:hover,
        &:active {
            filter: var(--filter-hl-1);
        }

        &:active {
            translate: 0;
            --shdw-dist-elevation: 0;
        }
    }
}
</style>