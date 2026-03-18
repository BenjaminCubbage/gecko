<template>
    <div class="friends-list-friends">
        <ul v-roving-container>
            <li
                v-for="friend in friends.pendingIncoming"
                :key="friend.user.userID"
                v-roving-item>
                <FriendsListFriendsCard
                    :friend="friend"
                    friend-type="incoming" />
            </li>
        </ul>
<!--
        <div
            class="list-banner">
            <div class="label-border">
                <BaseLabel color="orange" class="label">~ Friends ~</BaseLabel>
            </div>
        </div> -->

        <div class="list-border txtr-diag txtr-diag--orange">
            <ul class="list"
                v-roving-container>
                <li
                    v-for="friend in friends.pendingOutgoing"
                    :key="friend.user.userID">
                    <FriendsListFriendsCard
                        :friend="friend"
                        friend-type="outgoing"
                        v-roving-item />
                </li>

                <li
                    v-for="friend in friends.activeFriends"
                    :key="friend.user.userID"
                    class="list-item">
                    <FriendsListFriendsCard
                        :friend="friend"
                        friend-type="active"
                        v-roving-item />
                </li>
            </ul>
        </div>
    </div>
</template>

<script setup>
import { inject } from 'vue';

import FriendsListFriendsCard from './FriendsListFriendsCard.vue';
import BaseLabel              from './BaseLabel.vue';

import { Keys } from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);
</script>

<style scoped>
.friends-list-friends {
    display:        flex;
    flex-direction: column;
}

.list-banner {
    --banner-padding-x: 20px;
    --banner-padding-y: 10px;

    padding:
        0
        var(--banner-padding-x)
        var(--banner-padding-y);

    & > .label-border { grid-area: 1 / 1; place-self: end    stretch; }
    & > .label-dither { grid-area: 1 / 1; place-self: center stretch; }

    & > .label-border > .label {
        -webkit-text-stroke: 5.5px black;
        font-size:           3.8rem;
        filter: drop-shadow(0 3.5px black);
    }

    @supports not (width: round(down, 1%, 1px)) {
        width: 100%;
    }
}

.list-border {
    padding: 9px;

    box-shadow:
        var(--shadow-l),
        var(--shadow-inst-orange);

    border: var(--border-s);
    border-radius: var(--radius-s);
    corner-shape: notch;

    & > .list {
        padding: 15px 18px;
        gap: 21px;
        display: flex;
        flex-direction: column;

        background: white;

        box-shadow:
            inset var(--shadow-inst-dist)            var(--shadow-inst-dist)       var(--col-lt-gray-3),
                  var(--shadow-inst-dist)            var(--shadow-inst-dist)       var(--col-orange-1),
             calc(var(--shadow-inst-dist) * -1) calc(var(--shadow-inst-dist) * -1) var(--col-orange-4);

        border: var(--border-s);
        border-radius: var(--radius-s);
        corner-shape: notch;
    }
}
</style>