<template>
    <section class="friends-list-details">
        <LoadingSpinner
            v-if="isLoading"
            class="details-spinner"
            color="orange" />

        <transition
            name="details-border-transition"
            :enter-active-class="`details-border-transition--${slideDirection}-enter-active`"
            :leave-active-class="`details-border-transition--${slideDirection}-leave-active`"
            mode="out-in"
            @before-leave="isTransitionLeaving = true"
            @after-leave="isTransitionLeaving = false, ++detailsBorderMemoVersion">
            <div
                class="
                    details-border
                    shdw shdw--otst-orange shdw--inst-lt-gray shdw--recessed"
                :key="detailsBorderKey">
                <template
                    v-if="state === 'userresult'">
                    <h1 class="border-title border-title--username">
                        <span class="title-text">
                            {{ titleText }}
                        </span>
                    </h1>

                    <p class="border-status">
                        {{ statusText }}
                    </p>

                    <div
                        v-if="user != null && !isUserMe"
                        class="border-btns"
                        v-roving-container
                        v-memo="[
                            detailsBorderKey,
                            isAction1Pressed,
                            isAction2Pressed,
                            detailsBorderMemoVersion,
                            areButtonsAnimating]"
                        :data-is-animating="areButtonsAnimating"
                        @animationend="areButtonsAnimating = false"
                        @animationcancel="areButtonsAnimating = false">
                        <button
                            v-if="friend != null && friend.status === FriendStatus.PendingIncoming"
                            :class="`btn btn--${action1Color}`"
                            :data-is-loading="isAction1Pressed"
                            @click="emitAction1"
                            v-roving-item
                            v-interactive-attrs>
                            [{{ action1Label }}]
                        </button>

                        <button
                            :class="`btn btn--${action2Color}`"
                            :data-is-loading="isAction2Pressed"
                            @click="emitAction2"
                            v-roving-item
                            v-interactive-attrs>
                            [{{ action2Label }}]
                        </button>
                    </div>
                </template>
                <template v-else-if="state !== 'empty'">
                    <h1 class="border-title">
                        <span class="title-icon" inert>
                            {{ titleIcon }}
                        </span>
                        <span class="title-text">
                            {{ titleText }}
                        </span>
                    </h1>
                    <p v-if="statusText?.length" class="border-status">
                        {{ statusText }}
                    </p>
                </template>
            </div>
        </transition>
    </section>
</template>

<script setup>
import {
    computed,
    inject,
    nextTick,
    ref,
    useTemplateRef,
    watch
} from 'vue';

import LoadingSpinner from './LoadingSpinner.vue';

import { FriendStatus } from '@/models/friend.js';
import { User }         from '@/models/user.js'
import { Keys }         from '@/core/di/keys.js';

const props = defineProps({
    state: {
        type:     String,
        required: true,
        validator(value, props) {
            if (value === 'userresult' && props.user == null)
                return false;

            return [
                'empty',
                'loadfailed',
                'nofriends',
                'search',
                'searchnotfound',
                'userresult'
            ].includes(value);
        }
    },

    isLoading: {
        type:     Boolean,
        required: true
    },

    user: {
        required: true,
        validator(value) {
            return value == null || value instanceof User;
        }
    },

    slideDirection: {
        required: true,
        validator(value) {
            return [
                'forwards',
                'backwards'
            ].includes(value);
        }
    }
});

const emit = defineEmits([
    'accept',
    'reject',
    'unsend',
    'unfriend',
    'sendRequest'
]);

const friends = inject(Keys.FriendsStore);
const session = inject(Keys.SessionStore);

const detailsBorderKey = computed(() =>
    `${props.state}:${props.user?.userID}`);

/*
    Increment this to trigger the v-memo
*/
const detailsBorderMemoVersion = ref(0);

/*
    Bounce animation
*/
const areButtonsAnimating = ref(false);

/*
    If the user is a friend, this will hold
    the associated friend object
*/
const friend = computed(() => {
    return props.user
        ? friends.getFriendByUserID(props.user.userID)
        : null;
});

const isUserMe = computed(() =>
    props.user.userID === session.activeUserID);

/*
    If the transition is leaving we don't want to update
    the DOM
*/
const isTransitionLeaving = ref(false);

const isAction1Pressed = ref(false);
const isAction2Pressed = ref(false);

const titleIcon = computed(() => {
    switch (props.state) {
        case 'loadfailed':
        case 'nofriends':
            return '\uF50F';
        case 'search':
        case 'searchnotfound':
            return '\uF50D';
    }
    return null;
});

const titleText = computed(() => {
    switch (props.state) {
        case 'userresult':     return `@${props.user.username}`;
        case 'nofriends':      return 'My Friends';
        case 'loadfailed':     return "Couldn't Connect"
        case 'search':         return 'Search Users';
        case 'searchnotfound': return 'No Results';
    }
    return null;
});

const statusText = computed(() => {
    switch (props.state) {
        case 'userresult':
            if (isUserMe.value)
                return 'You earned the "search for myself" award please redeem at any Arby\'s';

            if (friend.value == null)
                return 'Not friends';

            switch (friend.value.status) {
                case FriendStatus.PendingIncoming: return 'Wants to be friends';
                case FriendStatus.PendingOutgoing: return 'Friend request sent';
                case FriendStatus.Active:          return `Friends since ${friend.value.acceptedOn}`;
            }
            break;
        case 'searchnotfound': return 'User not found';
        case 'nofriends':      return 'It\'s quiet here';
        case 'loadfailed':     return 'I don\'t freaking know okay';
    }
    return null;
});

const action1Label = ref('Accept');
const action1Color = ref('green');
const action1Event = ref('accept');

const action2Label = computed(() => {
    if (props.state !== 'userresult' || isUserMe.value)
        return null;

    if (friend.value != null) {
        switch (friend.value.status) {
            case FriendStatus.PendingIncoming: return 'Reject';
            case FriendStatus.PendingOutgoing: return 'Unsend';
            case FriendStatus.Active:          return 'Unfriend';
        }
    } else
        return 'Send Request'
});

const action2Color = computed(() => {
    if (props.state !== 'userresult' || isUserMe.value)
        return null;

    return friend.value != null
        ? 'red'
        : 'green';
});

const action2Event = computed(() => {
    if (props.state !== 'userresult' || isUserMe.value)
        return null;

    if (friend.value != null) {
        switch (friend.value.status) {
            case FriendStatus.PendingIncoming: return 'reject';
            case FriendStatus.PendingOutgoing: return 'unsend';
            case FriendStatus.Active:          return 'unfriend';
        }
    } else
        return 'sendRequest';
});

watch([action2Event, action2Color, action2Label], () => {
    /*
        Defer DOM updates during slide transition.
    */
    if (!isTransitionLeaving.value)
        ++detailsBorderMemoVersion.value;
}, { flush: 'post' });

/*
    Emit the actions, and track whether the action is done yet
    (pressed state / animation trigger)
*/

function emitAction1() {
    isAction1Pressed.value    = true;
    areButtonsAnimating.value = false;
    emit(action1Event.value, props.user, () => {
        isAction1Pressed.value    = false;
        areButtonsAnimating.value = true;
    });
}

function emitAction2() {
    isAction2Pressed.value    = true;
    areButtonsAnimating.value = false;
    emit(action2Event.value, props.user, () => {
        isAction2Pressed.value    = false;
        areButtonsAnimating.value = true;
    });
}
</script>

<style scoped>
.friends-list-details {
    position: relative;

    display:   flex;
    flex-flow: column;

    &::after {
        content:  '';
        position: absolute;
        inset:    0;
        z-index:  999;

        background:
            repeating-linear-gradient(
                rgb(0 0 0 / 0.01) 0   3px,
                #0000             3px 6px);

        pointer-events: none;
    }

    > .details-spinner        { z-index: 2; }
    > .details-username-plate { z-index: 1; }
    > .details-border         { z-index: 0; }
}

.details-border {
    contain:  strict;
    position: relative;

    display:         flex;
    flex-flow:       column;
    justify-content: center;
    gap:             6px;

    height: 142px;

    background: var(--col-lt-gray-1);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    overflow: clip;

    --shdw-etc:
        inset  17px  9px var(--col-lt-gray-4),
        inset  17px -9px var(--col-lt-gray-4),
        inset -17px  9px var(--col-lt-gray-4),
        inset -17px -9px var(--col-lt-gray-4);

    align-items: center;
    > .border-title {
        align-self: stretch;
    }

    > *                { z-index: 0; }
    > .details-spinner { z-index: 1; }
}

.details-spinner {
    --_spinner-inset: 18px;

    position: absolute;

    bottom: var(--_spinner-inset);
    right:  var(--_spinner-inset);
}

.border-title {
    font-size:      2.8rem;
    letter-spacing: 0.03em;
    line-height:    0.8;

    display: flex;

    align-items: center;
    gap:         9px;
    margin:      0 20px;

    &::before,
    &::after {
        content: '';

        height: 12px;
        width:  100%;

        --wing-col: var(--col-orange-4);
        --wing-y:   50%;
    }

    &::before { flex: 1 0 20px; --wing-edge-x: right; --wing-inner-x: calc(100% - 2px); }
    &::after  { flex: 1 0 20px; --wing-edge-x: left;  --wing-inner-x:  2px;             }

    &.border-title--username::before,
    &.border-title--username::after {
        --wing-col: var(--col-orange-4);
        --wing-y:   0%;
    }
}

.title-icon {
    font-size:   4rem;
    translate:   0 -1px;
    line-height: 0.5;
}

.title-text {
    min-width:     0;
    line-height:   0.6;
    overflow-wrap: break-word;
    text-align:    center;
    cursor:        text;
}

.border-status {
    padding:   0 10px;

    text-align: center;
    font:
        bold
        1.65rem
        var(--font-scnd);
}

.status-icon {
    display: none;

    height:    20px;
    translate: 0 -1px;
}

.status-date {
    color: var(--col-green-9);
}

.border-btns {
    display:    flex;
    gap:        36px;
    margin-top: 9px;

    transform-origin: bottom;

    /* Disable all buttons if one is loading. */
    &:has(.btn[data-is-loading=true]) {
        pointer-events: none;
    }
}

.btn {
    --elevation-dist: 4px;

    position: relative;
    padding:  4px 20px;

    -webkit-text-stroke: 4px white;
    font-size:           2.55rem;
    letter-spacing:      0.05em;
    text-transform:      uppercase;
    line-height:         0.8;

    background:
        linear-gradient(
            var(--col-grad-1) 50%,
            var(--col-grad-2) 50%);

    border-radius: 3px;

    filter:
        drop-shadow(
            0
            var(--elevation-dist)
            var(--col-elevation))
        var(--hl, brightness(1));

    translate: 0 calc(-1 * var(--elevation-dist));

    &.btn--green {
        --col-grad-1:    var(--col-green-3);
        --col-grad-2:    var(--col-green-4);
        --col-elevation: var(--col-green-5);
    }

    &.btn--red {
        --col-grad-1:    var(--col-red-2);
        --col-grad-2:    var(--col-red-3);
        --col-elevation: var(--col-red-5);
    }

    &[data-hovered],
    &[data-is-loading=true] {
        --hl: var(--filter-hl-1);

        &::before,
        &::after {
            --inst: -7px;
        }
    }

    &[data-pressed],
    &[data-is-loading=true] {
        --elevation-dist: 2px;
    }

    &[data-is-loading=true] {
        pointer-events: none;
    }

    &::before,
    &::after {
        --inst: -10px;

        content:  '';
        position: absolute;

        width:  18px;
        height: 15px;

        top:    0;
        bottom: 0;
        margin: auto 0;

        --wing-col: var(--col-orange-4);
    }

    &::before { left:  var(--inst); --wing-edge-x:  left;  --wing-inner-x: 2px;              --wing-y: center; }
    &::after  { right: var(--inst); --wing-edge-x:  right; --wing-inner-x: calc(100% - 2px); --wing-y: center; }
}

/*
    Wings ornament
*/

.border-title::before,
.border-title::after,
.btn::before,
.btn::after {
    translate: 0 1.5px;
    filter:    drop-shadow(0 -3px color-mix(in oklab, var(--wing-col), white 40%));

    background:
        linear-gradient(var(--wing-col) 0 0) var(--wing-edge-x)               / 4px  66.6% no-repeat,
        linear-gradient(var(--wing-col) 0 0) var(--wing-inner-x) var(--wing-y) / 33.3%  100%  no-repeat,
        linear-gradient(var(--wing-col) 0 0) var(--wing-inner-x) var(--wing-y) / 66.6%  66.6% no-repeat,
        linear-gradient(var(--wing-col) 0 0) var(--wing-inner-x) var(--wing-y) / 99.9% 33.3% no-repeat;
}

/*
    Loading spinner animation
*/

@keyframes details-loading-spinner {
    0%  { content: '\f510'; }
    30% { content: '\f511'; }
    55% { content: '\f512'; }
}

/*
    Slide animation
*/

.border-title,
.border-status,
.border-btns {
    will-change: transform;
    translate:   0 var(--details-border-slide);
}

.details-border-transition--forwards-enter-active  { animation: details-border-up   100ms; }
.details-border-transition--forwards-leave-active  { animation: details-border-down 100ms; }
.details-border-transition--backwards-enter-active { animation: details-border-down 100ms reverse; }
.details-border-transition--backwards-leave-active { animation: details-border-up   100ms reverse; }

@property --details-border-slide {
    initial-value: 0px;
    syntax:        '<length>';
    inherits:      true;
}

@keyframes details-border-up   { from { --details-border-slide: -120px; } to { --details-border-slide: 0px; } }
@keyframes details-border-down { from { --details-border-slide: 0px; }    to { --details-border-slide: 120px; } }

/*
    Buttons bounce
*/

.border-btns[data-is-animating=true] {
    animation: details-btns-bounce 180ms steps(3);
}

@keyframes details-btns-bounce {
    from { scale: 1; }
    50%  { scale: 1.02; }
    to   { scale: 0.98; }
}
</style>