<template>
    <button class="friends-list-item">
        <!-- <IconFriendHeart class="item-status" /> -->
        <IconFriendUser class="item-icon" />
        <div class="item-spotlight"></div>
    </button>
</template>

<script setup>
import BaseClippableStrokedText from './BaseClippableStrokedText.vue';
import IconFriendUser           from './IconFriendUser.vue';
import IconArrowOutgoing        from './IconArrowOutgoing.vue';
import IconFriendHeart          from './IconFriendHeart.vue';
</script>

<style scoped>
.friends-list-item {
    position: relative;

    display:     grid;
    place-items: center;

    --bg-angle: 20deg;
    --bg-perc:  30%;

    filter: brightness(0.9);

    & > .item-icon      { z-index: 1; }
    & > .item-spotlight { z-index: 0; }
    & > .item-status    { z-index: 2; }

    &:hover,
    &:focus {
        filter: none;

        & > .item-status {
            opacity: 1;
            filter:  none;
        }

        & > .item-spotlight {
            --spotlight-alpha: 0.8;
        }

        & > .item-spotlight::after {
            opacity: 1;
        }

        & > .item-icon {
            transform-origin: bottom;
            scale:            1.05;
        }
    }
}

.item-status {
    opacity:  0.7;
    height:   20px;
    position: absolute;
    top:      -24px;
    filter:   saturate(0.8);
}

.item-icon {
    height:    45px;
    translate: 2.5px 0;
}

.item-spotlight {
    --spotlight-alpha: 0;

    content:  '';
    position: absolute;

    left:   -10px;
    right:  -10px;
    bottom: 0;

    height: 95px;

    translate: 0 5px;

    pointer-events: none;

    background:
        radial-gradient(
            circle at 50% 0%,
            rgb(255 255 255 / var(--spotlight-alpha)) 20%,
            transparent 90%);

    clip-path:
        shape(
            from 15% 7px,
            curve to 85% 7px with 50% 0,
            line to 100% 100%,
            line to 0 100%,
            close);

    @supports not (clip-path: shape(from 0 0, line to 1px 1px)) {
        clip-path:
            polygon(
                15%  0,
                85%  0,
                100% 100%,
                0    100%);
    }
}

.item-spotlight::before {
    content: '';
    display:  block;
    position: absolute;

    height: 18px;
    width:  95%;

    inset:  auto 0 0 0;
    margin: 0 auto;

    border-radius: 50%;
    corner-shape:  round;

    background: rgb(255 255 255 / var(--spotlight-alpha));
}

.item-spotlight::after {
    content:  '';
    display:  block;
    position: absolute;

    height: 12px;
    width:  70%;

    inset:  auto 0 2.75px 0;
    margin: 0 auto;

    opacity: 0.6;

    border-radius: var(--radius-xs);
    background:    var(--col-shadow-alpha);
}
</style>