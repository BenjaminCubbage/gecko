<template>
    <div class="friends-list-details">
        <transition 
            name="details-border-transition"
            :enter-active-class="`details-border-transition--${transitionDirection}-enter-active`"
            :leave-active-class="`details-border-transition--${transitionDirection}-leave-active`"
            mode="out-in">
            <div 
                ref="detailsBorderEl"
                class="
                    details-border
                    shdw shdw--otst-orange shdw--inst-lt-gray shdw--recessed"
                :key="friend.user.userID">
                <h1 class="border-username">
                    <template v-if="friend != null">
                        @{{ friend.user.username }}
                    </template>

                    <template v-else>
                        [NO FRIENDS]
                    </template>
                </h1>

                <p class="border-status">
                    <template v-if="friend?.status === FriendStatus.PendingIncoming">
                        Wants to be friends
                    </template>

                    <template v-else-if="friend?.status === FriendStatus.PendingOutgoing">
                        Friend request sent
                    </template>

                    <template v-else-if="friend?.status === FriendStatus.Active">
                        <span>
                            Friends since
                            <time
                                class="status-date"
                                :datetime="friend.acceptedOn">
                                {{ friend.acceptedOn }}
                            </time>
                        </span>
                    </template>

                    <template v-else>
                        Maybe someday
                    </template>
                </p>

                <div 
                    v-if="friend != null"
                    class="border-btns"
                    v-roving-container>
                    <button
                        v-if="friend.status === FriendStatus.PendingIncoming"
                        class="btn btn--green"
                        v-roving-item
                        @click="$emit('accept-friend', friend)">
                        [Accept]
                    </button>

                    <button
                        class="btn btn--red"
                        v-roving-item
                        @click="$emit(actionEvent, friend)">
                        [{{ actionLabel }}]
                    </button>
                </div>
            </div>
        </transition>
    </div>
</template>

<script setup>
import { 
    computed,
    useTemplateRef,
    watch
} from 'vue';

import IconFriendHeart          from './IconFriendHeart.vue';
import IconFriendChevron        from './IconFriendChevron.vue';
import IconFriendQuestion       from './IconFriendQuestion.vue';
import { Friend, FriendStatus } from '@/models/friend.js';

const props = defineProps({
    state: {
        type:     String,
        required: true,
        validator(value) {
            return [
                'loading',
                'ready'
            ].includes(value);
        }
    },

    friend: {
        required: true,
        validator(value) {
            return value == null || value instanceof Friend;
        }
    },

    transitionDirection: {
        required: true,
        validator(value) {
            return [
                'forwards',
                'backwards'
            ].includes(value);
        }
    }
});

defineEmits([
    'accept-friend',
    'reject-friend',
    'unsend-friend-request',
    'unfriend'
]);

const detailsBorderEl = useTemplateRef('detailsBorderEl');

const actionLabel = computed(() => {
    switch (props.friend.status) {
        case FriendStatus.PendingIncoming: return 'Reject';
        case FriendStatus.PendingOutgoing: return 'Unsend';
        case FriendStatus.Active:          return 'Unfriend';
    }
});

const actionEvent = computed(() => {
    switch (props.friend.status) {
        case FriendStatus.PendingIncoming: return 'reject-friend';
        case FriendStatus.PendingOutgoing: return 'unsend-friend-request';
        case FriendStatus.Active:          return 'unfriend';
    }
});

watch(() => props.transitionDirection, () => {
    console.log(props.transitionDirection);
});
</script>

<style scoped>
.friends-list-details {
    display:   flex;
    flex-flow: column;

    &::after {
        content: '';
        position: absolute;
        inset: 0;
        z-index: 999;
        background:
            repeating-linear-gradient(
                rgb(0 0 0 / 0.01) 0   3px,
                #0000             3px 6px);

        pointer-events: none;
    }

    & > .details-username-plate { z-index: 1; }
    & > .details-border         { z-index: 0; }
}

.details-border {
    position: relative;

    display:   flex;
    flex-flow: column;

    padding: 24px 0 24px;
    gap:     6px;

    background: var(--col-lt-gray-1);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    overflow: clip;

    --shdw-etc:
        inset  17px  9px var(--col-lt-gray-4),
        inset  17px -9px var(--col-lt-gray-4),
        inset -17px  9px var(--col-lt-gray-4),
        inset -17px -9px var(--col-lt-gray-4);

    & > .border-username { align-self: stretch; }
    & > .border-status   { align-self: center; }
    & > .border-btns     { align-self: center; }
}

.border-username {
    display: grid;
    grid-template:
        "l   .    r" auto /
         1fr auto 1fr;

    align-items: center;
    gap:         9px;
    margin:      0 20px;

    font-size:      2.8rem;
    letter-spacing: 0.03em;
    line-height:    0.8;

    &::before,
    &::after {
        content: '';

        height: 12px;
        width:  100%;

        filter: drop-shadow(0 -3px var(--col-orange-2));
    }

    &::before {
        grid-area:      l;
        --orn-edge-x:  right;
        --orn-inner-x: calc(100% - 2px);
        --orn-y:       0%;
    }

    &::after {
        grid-area:     r;
        --orn-edge-x:  left;
        --orn-inner-x: 2px;
        --orn-y:       0%;
    }
}

.border-status {
    display:     flex;
    align-items: center;
    gap:         8px;

    font-family: var(--font-scnd);
    font-weight: bold;

    font-size: 1.65rem;
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
    display: flex;
    gap:     36px;

    margin-top: 9px;
}

.btn {
    position: relative;

    padding: 4px 20px;

    -webkit-text-stroke: 4px white;

    font-size:      2.55rem;
    letter-spacing: 0.05em;
    text-transform: uppercase;
    line-height:    0.8;

    background:
        linear-gradient(
            var(--col-grad-1) 50%,
            var(--col-grad-2) 50%);

    border-radius: 3px;

    filter: 
        drop-shadow(0 4px var(--col-elevation))
        var(--hl, brightness(1));

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

    @media (hover: hover) {
        &:hover {
            --hl: var(--filter-hl-1);

            &::before,
            &::after {
                --inst: -7px;
            }
        }
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

        filter: drop-shadow(0 -2.5px var(--col-orange-1));
    }

    &::before {
        left: var(--inst);

        --orn-edge-x:  left;
        --orn-inner-x: 2px;
        --orn-y:       center;
    }

    &::after {
        right: var(--inst);

        --orn-edge-x:  right;
        --orn-inner-x: calc(100% - 2px);
        --orn-y:       center;
    }
}

/*
    Ornament
*/
.border-username::before,
.border-username::after,
.btn::before,
.btn::after {
    background:
        linear-gradient(var(--col-orange-4) 0 0) var(--orn-edge-x)               / 4px  66.6% no-repeat,
        linear-gradient(var(--col-orange-4) 0 0) var(--orn-inner-x) var(--orn-y) / 33.3%  100%  no-repeat,
        linear-gradient(var(--col-orange-4) 0 0) var(--orn-inner-x) var(--orn-y) / 66.6%  66.6% no-repeat,
        linear-gradient(var(--col-orange-4) 0 0) var(--orn-inner-x) var(--orn-y) / 99.9% 33.3% no-repeat;
}

/*
    Slide animation
*/

.border-username,
.border-status,
.border-btns {
    translate: 0 var(--details-slide);
}

.details-border-transition--forwards-enter-active  { animation: details-border-up   180ms steps(4); }
.details-border-transition--forwards-leave-active  { animation: details-border-down 180ms steps(4); }
.details-border-transition--backwards-enter-active { animation: details-border-down 180ms steps(4) reverse; }
.details-border-transition--backwards-leave-active { animation: details-border-up   180ms steps(4) reverse; }

@property --details-slide {
    initial-value: 0px;
    syntax:        '<length>';
    inherits:      true;
}

@keyframes details-border-up   { from { --details-slide: -120px; } to { --details-slide: 0px; } }
@keyframes details-border-down { from { --details-slide: 0px; }    to { --details-slide: 120px; } }
</style>