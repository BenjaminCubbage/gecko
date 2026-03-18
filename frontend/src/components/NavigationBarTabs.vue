<template>
    <nav
        ref="tabListEl"
        role="tablist"
        class="navigation-bar-tabs txtr-diag txtr-diag--lt-gray"
        v-roving-container
        v-roving-home="selectedTab === 'canvas' ? 0 : 1">
        <button
            ref="tabCanvasEl"
            role="tab"
            :aria-selected="selectedTab === 'canvas'"
            :aria-controls="tabPanelIds.canvas"
            class="tab tab--left tab--green txtr-diag txtr-diag--green"
            @click="selectedTab = 'canvas'"
            v-roving-item>
            canvas
        </button>

        <button
            ref="tabFriendsEl"
            role="tab"
            :aria-selected="selectedTab === 'friends'"
            :aria-controls="tabPanelIds.friends"
            class="tab tab--right tab--orange txtr-diag txtr-diag--orange"
            @click="selectedTab = 'friends'"
            v-roving-item>
            friends
        </button>
    </nav>
</template>

<script setup>
import { Keys }                 from '@/core/di/keys.js';
import { useElementIdRegistry } from '@/composables/useElementIdRegistry';

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

const tabPanelIds = useElementIdRegistry(Keys.AppTabPanelIdsRegistry);
</script>

<style scoped>
.navigation-bar-tabs {
    contain-intrinsic-size: 220px 26px;
    contain:                layout size;
    isolation:              isolate;

    grid-template-areas:
        "tab-left tab-right";

    display:        grid;
    padding-bottom: var(--shadow-dist-s);
    padding:        0 6px 8px 6px;
    user-select:    none;

    font-size:   2.2rem;
    line-height: 1.3;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    box-shadow:
        0 var(--shadow-dist-s)
        0 black,
        var(--shadow-inst-gray);

    corner-shape: notch;
}

.tab {
    margin-top: -8px;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    text-transform:      uppercase;

    --tab-offset: 0px;

    box-shadow:
        0 calc(var(--shadow-dist-s) - var(--tab-offset))
        0 black,
        inset      var(--shadow-inst-dist)            var(--shadow-inst-dist)       0 var(--tab-col-bevel-lt),
        inset calc(var(--shadow-inst-dist) * -1) calc(var(--shadow-inst-dist) * -1) 0 var(--tab-col-bevel-dk);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transition:
        box-shadow 50ms linear,
        translate  50ms linear;

    translate: 0 var(--tab-offset);

    corner-shape: notch;
    paint-order:  stroke;

    &:hover,
    &:active {
        filter: var(--filter-hl-1);
    }

    &[aria-selected=true] {
        --tab-offset: var(--shadow-dist-s);
        pointer-events: none;
    }

    &.tab--left  { grid-area: tab-left;  border-radius: var(--radius-s) 0 0 var(--radius-s); }
    &.tab--right { grid-area: tab-right; border-radius: 0 var(--radius-s) var(--radius-s) 0; }

    &.tab--green  { --tab-col-bevel-lt: var(--col-green-0);  --tab-col-bevel-dk: var(--col-green-5); }
    &.tab--orange { --tab-col-bevel-lt: var(--col-orange-0); --tab-col-bevel-dk: var(--col-orange-7); }
}
</style>