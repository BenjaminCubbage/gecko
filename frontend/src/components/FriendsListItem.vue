<template>
    <div class="friends-list-item">
        <IconFriendUser class="item-icon" />
        <div class="item-username">
          @Gary
        </div>
        <div class="item-description">
            <BaseClippableStrokedText class="description-text" is="span">
                ⬅ Request Sent
            </BaseClippableStrokedText>
        </div>
        <button class="
            item-btn-expand
            shdw shdw--inst-lt-gray shdw--elevated-s"></button>

        <IconArrowOutgoing class="item-badge" height="28px" />
    </div>
</template>

<script setup>
import BaseClippableStrokedText from './BaseClippableStrokedText.vue';

import IconFriendUser    from './IconFriendUser.vue';
import IconArrowOutgoing from './IconArrowOutgoing.vue';
</script>

<style scoped>
.friends-list-item {
    padding:    13px 24px 12px;
    gap:        10px 10px;

    display: grid;
    grid-template:
        "icon username    expand" 1.3fr
        "icon description expand" 1fr /
         auto 1fr         auto;

    border: var(--border-thickness-s) dashed var(--col-gray-4);

    & > .item-icon        { grid-area: icon;        place-self: center; }
    & > .item-username    { grid-area: username;    place-self: end    left; }
    & > .item-description { grid-area: description; place-self: start  left; }
    & > .item-btn-expand  { grid-area: expand;      place-self: center right; }
}

.item-icon {
    height: 45px;

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));
}

.item-username {
    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.7rem;
    line-height:         0.6;

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));
}

.item-description {
    font-size:      1.8rem;
    line-height:    0.6;
    letter-spacing: 0.03em;
    color:          var(--col-green-3);
    text-transform: uppercase;
}

.description-text {
    -webkit-text-stroke: 4.5px black;

    background-image:
        linear-gradient(
            var(--col-green-0) 25%,
            var(--col-green-1) 25% 50%,
            var(--col-green-3) 50% 75%,
            var(--col-green-4) 75%);

    color: transparent;
    filter: drop-shadow(0 1.5px black);
}

.item-btn-expand {
    position: relative;

    width:  40px;
    height: 28px;
    background: var(--col-lt-gray-2);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));

    &::after {
        position: absolute;
        inset:    0;
        margin:   auto;

        width:  0;
        height: 0;

        display:       grid;
        place-content: center;

        content:        '...';
        font-size:      4rem;
        letter-spacing: 0.06em;
        line-height:    0;

        translate: 0.15ch -0.45ex;

        -webkit-text-stroke: 0.7px black;
    }
}

.item-badge {
    position: absolute;
    inset:
        -8px auto
        auto -8px;

        filter:
            drop-shadow(
                var(--shadow-dist-m)
                var(--shadow-dist-m)
                var(--col-shadow-alpha));
}
</style>