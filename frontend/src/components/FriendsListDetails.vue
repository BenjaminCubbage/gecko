<template>
    <div class="
        friends-list-details
        shdw shdw--otst-orange shdw--inst-lt-gray shdw--recessed">
        <h1
            class="
                details-username
                shdw shdw--inst-lt-gray shdw--elevated-xs
                shdw-before shdw-before--inst-lt-gray
                shdw-after  shdw-after--inst-lt-gray">
            @Gary
        </h1>

        <div class="details-status">
            <IconFriendHeart height="20px" /> Wants to be friends
        </div>

        <!-- <dl class="details-content">
            <div class="content-row">
                <dt class="row-term">Username</dt>
                <dd class="row-desc">@Gary</dd>
            </div>

            <div class="content-row">
                <dt class="row-term">Status</dt>
                <dd class="row-desc">Friends</dd>
            </div>

            <div class="content-row">
                <dt class="row-term">Friends Since</dt>
                <dd class="row-desc">02/26/2002</dd>
            </div>
        </dl> -->
    </div>
</template>

<script setup>
import IconFriendHeart from './IconFriendHeart.vue';
</script>

<style scoped>
.friends-list-details {
    display:     grid;
    flex-flow:   column;
    place-items: center;
    gap:         10px;

    background: var(--col-lt-gray-1);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    --shdw-etc:
        inset  8px 0 0 9px var(--col-lt-gray-4),
        inset -8px 0 0 9px var(--col-lt-gray-4);

    padding: 40px 0 40px;
}

.details-username {
    position: absolute;
    top:      calc(-3 * var(--border-thickness-s));

    display:     flex;
    align-items: stretch;
    gap:         24px;

    overflow: clip;

    border:        var(--border-s);
    border-radius: var(--radius-s) var(--radius-s) 9px 9px;
    corner-shape:  notch notch bevel bevel;

    background: var(--col-lt-gray-3);

    line-height:         1.05;
    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.8rem;
    letter-spacing:      0.03em;

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));

    &::before,
    &::after {
        content: '';

        width: 36px;

        background:
            linear-gradient(var(--col-gray-4) 0 0) center / 3px 9px no-repeat,
            linear-gradient(var(--col-gray-4) 0 0) center / 9px 3px no-repeat
            var(--col-gray-2);
    }

    &::before { order: 0; border-right: var(--border-s); }
    &::after  { order: 1; border-left:  var(--border-s); }
}

.details-status {
    display:     flex;
    align-items: center;
    gap:         8px;

    font-family: var(--font-scnd);
    font-weight: bold;

    padding: 0 20px;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           1.6rem;

    & > svg {
        translate: 0 -1.5px;
    }
}
</style>