<template>
    <div
        :class="`
            friends-list-friends-card
            friends-list-friends-card--variant-${variant}`"
        :data-deleting="isDeleting"
        v-roving-container>
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
            inert>
        </div>

        <component
            :is="variant === 'normal' ? 'button' : 'span'"
            class="username"
            @click="emit('showFriendDetails', friend)"
            v-roving-item="variant === 'normal'">
            @{{ friend.user.username }}
        </component>

        <span
            v-if="variant === 'details' && footnoteText"
            class="footnote">
            {{ footnoteText }}
        </span>

        <div class="separator" aria-hidden></div>

        <div class="buttons">
            <button
                v-if="friend.status === FriendStatus.Active"
                ref="button1El"
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

import { 
    Friend,
    FriendStatus
} from '@/models/friend.js';
import { Keys }   from '@/core/di/keys.js';

const props = defineProps({
    friend: {
        type:     Friend,
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

watch(() => props.friend, () => isDeleting.value = false);

const footnoteText = computed(() => {
    switch (props.friend.status) {
    case FriendStatus.Active: return `Friends since: ${props.friend.acceptedOn}`;
    }
});

async function acceptFriend() {
    await friends.publishAcceptFriendRequest(session, props.friend.user.userID);
}

async function deleteFriend() {
    isDeleting.value = true;

    await new Promise((resolve => (deletingTimer = setTimeout(resolve, 500))));
    await friends.publishDeleteFriendOrRequest(session, props.friend.user.userID);
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
    gap:     12px;
    padding: 12px;

    color:     black;
    font-size: 2.4rem;

    &.friends-list-friends-card--variant-normal {
        gap: 0 12px;

        grid-template:
            "icon username separator buttons" 30px /
            auto auto     1fr        auto;

        & > .icon      { grid-area: icon;      place-self: center start; }
        & > .username  { grid-area: username;  place-self: center start; }
        & > .separator { grid-area: separator; place-self: center right; }
        & > .buttons   { grid-area: buttons;   place-self: center start; }
    }

    &.friends-list-friends-card--variant-details {
        gap:     0 15px;
        padding: 15px 15px 9px;

        grid-template:
            "icon      username  separator" auto
            ".         footnote  footnote" 24px
            ".         .         ."        12px
            "buttons   buttons buttons"  auto /
             auto      auto    1fr;

        & > .icon      { grid-area: icon;      place-self: center;}
        & > .username  { grid-area: username;  place-self: center left; }
        & > .footnote  { grid-area: footnote;  place-self: end   left; }
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
            height:    28px;
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
}

.icon {
    height: 30px;
    width:  42px;

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

.username,
.footnote {
    filter:
        drop-shadow(3px 3px var(--col-lt-gray-5));

    -webkit-text-stroke: var(--text-stroke-s);
    text-align:          left;

    &.username {
        line-height: 0.85;
        word-break:  break-all;

        &:hover,
        &:focus-visible,
        &:active {
            transform-origin: 0% 50%;
            scale: 1.03;
        }
    }

    &.footnote {
        cursor: default;
        font-size:            0.8em;
        font-variant-numeric: tabular-nums;
    }
}

.separator {
    --checker-col-1: var(--col-orange-3);
    --checker-col-2: var(--col-orange-5);
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

    filter: drop-shadow(3px 3px var(--col-lt-gray-4));

    @supports (width: round(down, 100%, 1px)) {
        width: round(down, 100%, var(--checker-size));
    }
}

.buttons {
    display:        flex;
    flex-direction: row-reverse;
}

.button {
    --hl: brightness(1);

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

    border-radius: var(--radius-s);
    border:        var(--border-s);

    translate:
        0
        calc(var(--shdw-dist-elevation) * -1);

    filter:
        drop-shadow(3px 3px var(--col-lt-gray-5))
        var(--hl);

    /*
        Variants
    */
    &.button--unfriend {
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