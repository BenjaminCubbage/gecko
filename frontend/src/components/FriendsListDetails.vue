<template>
    <div class="
        friends-list-details
        shdw shdw--otst-orange shdw--inst-lt-gray shdw--recessed">
        <h1
            class="
                details-username-tab
                shdw shdw--inst-lt-gray shdw--elevated-xs
                shdw-before shdw-before--inst-lt-gray
                shdw-after  shdw-after--inst-lt-gray">
            @Gary
        </h1>

        <p class="details-status">
            <IconFriendHeart class="icon-friend-heart" height="20px" /> Wants to be friends
        </p>

        <div
            class="
                details-buttons-tab
                shdw shdw--inst-lt-gray shdw--elevated-s">
            <button 
                class="
                    tab-btn
                    txtr-diag txtr-diag--green
                    shdw shdw--inst-green shdw--elevated-xs">
                Accept
            </button>
            <button 
                class="
                    tab-btn
                    txtr-diag txtr-diag--dk-red
                    shdw shdw--inst-dk-red shdw--elevated-xs">
                Reject
            </button>
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
    --ht-tab:       36px;
    --ht-tab-btn:   36px;
    --rd-tab-bevel: 9px;

    display:     grid;
    flex-flow:   column;
    place-items: center;
    gap:         10px;

    padding: 
        calc(var(--ht-tab) + 6px) 
        0 
        calc(var(--ht-tab-btn) + 4px);

    background: var(--col-lt-gray-1);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    --shdw-etc:
        inset  8px 0 0 9px var(--col-lt-gray-4),
        inset -8px 0 0 9px var(--col-lt-gray-4);
}

.details-username-tab {
    position: absolute;
    top:      calc(-3 * var(--border-thickness-s));

    display:     flex;
    align-items: center;
    gap:         24px;

    height: var(--ht-tab);

    overflow: clip;

    border: var(--border-s);
    border-radius:
        var(--radius-s)
        var(--radius-s)
        var(--rd-tab-bevel)
        var(--rd-tab-bevel);
    corner-shape: notch notch bevel bevel;

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

    &::before { order: 0; border-right: var(--border-s); align-self: stretch; }
    &::after  { order: 1; border-left:  var(--border-s); align-self: stretch; }
}

.details-status {
    display:     flex;
    align-items: center;
    gap:         8px;

    font-family: var(--font-scnd);
    font-weight: bold;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           1.65rem;

    & > .icon-friend-heart {
        translate: 0 -1.5px;
    }
}

.details-buttons-tab {
    position: absolute;
    display:  flex;

    justify-content: end;
    place-items:     end center;

    bottom: calc(-4 * var(--border-thickness-s));
    height: var(--ht-tab);

    padding: 0 16px 4px;
    gap:     8px;

    background: var(--col-gray-2);

    border: var(--border-s);
    border-radius:
        var(--rd-tab-bevel)
        var(--rd-tab-bevel)
        var(--radius-s)
        var(--radius-s);
    corner-shape: bevel bevel notch notch;
}

.tab-btn {
    height: var(--ht-tab-btn);

    -webkit-text-stroke: var(--text-stroke-xs);
    font-size:           2.55rem;
    letter-spacing:      0.03em;

    padding: 0 24px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));
}
</style>