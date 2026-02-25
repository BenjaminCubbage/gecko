<template>
    <nav role="tablist" class="navbar-tabs">
        <button
            role="tab"
            :aria-selected="selectedTab === 'canvas'"
            :aria-controls="tabPanelIds.canvas"
            class="tab tab--left tab--green txtr-diag txtr-diag--green"
            @click="selectedTab = 'canvas'">
            canvas
        </button>

        <button
            role="tab"
            :aria-selected="selectedTab === 'friends'"
            :aria-controls="tabPanelIds.friends"
            class="tab tab--right tab--orange txtr-diag txtr-diag--orange"
            @click="selectedTab = 'friends'">
            friends
        </button>

        <div class="pad txtr-diag txtr-diag--lt-gray"></div>
    </nav>
</template>

<script setup>
import { Keys } from '@/core/di/keys.js';
import { useElementIdRegistry } from '@/composables/useElementIdRegistry';

const tabPanelIds = useElementIdRegistry(Keys.AppTabPanelIdsRegistry);

const selectedTab = defineModel({
    type:     String,
    required: true,
    validator(value) {
        return [
            'canvas',
            'friends'
        ].includes(value);
    }
});
</script>

<style scoped>
.navbar-tabs {
    isolation: isolate;

    grid-template-areas:
        "tab-left tab-right"
        "pad      pad";

    display: grid;

    font-family: var(--font-heading);
    font-size:   2.2rem;
    line-height: 1.3;

    & > .tab { z-index: 1; }
    & > .pad { z-index: 0; }
}

.tab {
    width: 120px;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    text-transform:      uppercase;

    --tab-offset: 0px;

    box-shadow:
        0 calc(var(--shadow-dist-s) - var(--tab-offset))
        0 black,
        inset  3px  3px 0 var(--tab-col-bevel-lt),
        inset -3px -3px 0 var(--tab-col-bevel-dk);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transition:
        box-shadow 80ms ease,
        translate  80ms ease;

    translate: 0 var(--tab-offset);

    corner-shape: notch;
    paint-order:  stroke;

    &.tab--left  { grid-area: tab-left;  border-radius: var(--radius-s) 0 0 var(--radius-s); }
    &.tab--right { grid-area: tab-right; border-radius: 0 var(--radius-s) var(--radius-s) 0; }

    &.tab--green  { --tab-col-bevel-lt: var(--col-green-0);  --tab-col-bevel-dk: var(--col-green-5); }
    &.tab--orange { --tab-col-bevel-lt: var(--col-orange-0); --tab-col-bevel-dk: var(--col-orange-7); }

    &:hover,
    &:active {
        filter: var(--filter-hl-1);
    }

    &[aria-selected=true] {
        --tab-offset: var(--shadow-dist-s);
        pointer-events: none;
    }
}

.pad {
    margin-left:  -8px;
    margin-right: -8px;
    margin-top: -28px;

    grid-area: pad;
    height:    38px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    box-shadow:
        0 var(--shadow-dist-s)
        0 black,
        inset  3px  3px var(--col-gray-0),
        inset -3px -3px var(--col-gray-4);

    corner-shape: notch;
}
</style>