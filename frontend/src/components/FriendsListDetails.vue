<template>
    <div class="
        friends-list-details
        shdw shdw--otst-orange shdw--inst-lt-gray shdw--recessed">
        <h1
            class="
                details-username-tab
                txtr-vert txtr-vert--lt-gray
                shdw shdw--inst-lt-gray shdw--elevated-xs
                shdw-before shdw-before--inst-gray
                shdw-after  shdw-after--inst-gray">
            <template v-if="friend != null">
                @{{ friend.user.username }}
            </template>

            <template v-else>
                [NO FRIENDS]
            </template>
        </h1>

        <p class="details-status">
            <template v-if="friend?.status === FriendStatus.PendingIncoming">
                <IconFriendQuestion class="status-icon" /> Wants to be friends
            </template>

            <template v-else-if="friend?.status === FriendStatus.PendingOutgoing">
                <IconFriendChevron class="status-icon" /> Friend request sent
            </template>

            <template v-else-if="friend?.status === FriendStatus.Active">
                <IconFriendHeart class="status-icon" />
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
                <IconFriendHeart class="status-icon" /> Maybe someday
            </template>
        </p>

        <div
            v-if="friend != null"
            class="
                details-buttons-tab
                shdw shdw--inst-lt-gray shdw--elevated-s"
            v-roving-container>
            <button
                v-if="friend.status === FriendStatus.PendingIncoming"
                class="
                    tab-btn
                    txtr-vert txtr-vert--green
                    shdw shdw--inst-green shdw--elevated-s"
                v-roving-item
                @click="$emit('accept-friend', friend)">
                Accept
            </button>

            <button
                class="
                    tab-btn
                    txtr-vert txtr-vert--red
                    shdw shdw--inst-red shdw--elevated-s"
                v-roving-item
                @click="$emit(actionEvent, friend)">
                {{ actionLabel }}
            </button>
        </div>
    </div>
</template>

<script setup>
import { computed }             from 'vue';
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
    }
});

defineEmits([
    'accept-friend',
    'reject-friend',
    'unsend-friend-request',
    'unfriend'
]);

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
</script>

<style scoped>
.friends-list-details {
    --ht-details:   116px;
    --ht-tab:       36px;
    --ht-tab-btn:   37px;
    --rd-tab-bevel: 9px;

    position:    relative;
    display:     grid;
    place-items: center;

    height: var(--ht-details);

    padding:
        calc(var(--ht-tab) + 6px)
        0
        calc(var(--ht-tab-btn) + 4px);

    background: var(--col-lt-gray-1);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    --shdw-etc:
        inset  17px  9px var(--col-lt-gray-4),
        inset  17px -9px var(--col-lt-gray-4),
        inset -17px  9px var(--col-lt-gray-4),
        inset -17px -9px var(--col-lt-gray-4);
}

.details-username-tab {
    min-width: 240px;

    position: absolute;
    top:      calc(-3 * var(--border-thickness-s));

    display:     grid;
    place-items: center;
    gap:         24px;

    grid-template:
        "bracket-l text bracket-r" auto /
         auto      1fr  auto;

    height: var(--ht-tab);

    overflow: clip;

    border: var(--border-s);
    border-radius:
        var(--radius-s)
        var(--radius-s)
        var(--rd-tab-bevel)
        var(--rd-tab-bevel);
    corner-shape: notch notch bevel bevel;

    line-height:         1.05;
    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.8rem;
    letter-spacing:      0.03em;

    &::before,
    &::after {
        content: '';
        width:   36px;

        background:
            linear-gradient(var(--col-gray-4) 0 0) center / 9px 9px no-repeat,
            linear-gradient(
                var(--col-gray-2) 50%,
                var(--col-gray-3) 50%
            );
    }

    &::before { grid-area: bracket-l; border-right: var(--border-s); align-self: stretch; border-bottom-left-radius:  calc(var(--rd-tab-bevel) * 0.6); corner-bottom-left-shape:  bevel; }
    &::after  { grid-area: bracket-r; border-left:  var(--border-s); align-self: stretch; border-bottom-right-radius: calc(var(--rd-tab-bevel) * 0.6); corner-bottom-right-shape: bevel; }
}

.details-status {
    display:     flex;
    align-items: center;
    gap:         8px;

    font-family: var(--font-scnd);
    font-weight: bold;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           1.65rem;
}

.status-icon {
    display: none;

    height:    20px;
    translate: 0 -1px;
}

.status-date {
    color: var(--col-green-9);
}

.details-buttons-tab {
    position: absolute;
    display:  flex;

    justify-content: end;
    place-items:     end center;

    bottom: calc(-4 * var(--border-thickness-s));
    height: var(--ht-tab);

    padding: 0 9px 6px;
    gap:     8px;

    background:
        linear-gradient(
            var(--col-gray-2) 50%,
            var(--col-gray-3) 50%);

    border: var(--border-s);
    border-radius:
        var(--rd-tab-bevel)
        var(--rd-tab-bevel)
        var(--radius-s)
        var(--radius-s);

    corner-shape: bevel bevel notch notch;
}

.tab-btn {
    width:  118px;
    height: var(--ht-tab-btn);

    -webkit-text-stroke: var(--text-stroke-xs);
    font-size:           2.55rem;
    letter-spacing:      0.06em;
    text-transform:      uppercase;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));
}
</style>