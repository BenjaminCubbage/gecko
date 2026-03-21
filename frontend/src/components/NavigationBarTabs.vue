<template>
    <nav
        ref="tabListEl"
        role="tablist"
        class="
            navigation-bar-tabs
            txtr-diag txtr-diag--lt-gray
            shdw shdw--inst-lt-gray shdw--elevated-s"
        v-roving-container
        v-roving-home="selectedTab === 'canvas' ? 0 : 1">
        <button
            ref="tabCanvasEl"
            role="tab"
            :aria-selected="selectedTab === 'canvas'"
            :aria-controls="tabPanelIds.canvas"
            class="
                tab tab--left
                txtr-diag txtr-diag--green
                shdw shdw--inst-green shdw--elevated-s"
            @click="selectedTab = 'canvas'"
            v-roving-item>
            canvas
        </button>

        <button
            ref="tabFriendsEl"
            role="tab"
            :aria-selected="selectedTab === 'friends'"
            :aria-controls="tabPanelIds.friends"
            class="
                tab tab--right
                txtr-diag txtr-diag--orange
                shdw shdw--inst-orange shdw--elevated-s"
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

    font-size:   2.2rem;
    line-height: 1.3;

    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.tab {
    margin-top: -8px;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    text-transform:      uppercase;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    translate:
        0
        calc(var(--shadow-dist-s) - var(--shdw-dist-elevation));

    &:hover,
    &:active,
    &[aria-selected=true] {
        filter: var(--filter-hl-1);
    }

    &[aria-selected=true] {
        --shdw-dist-elevation: 0px;
        pointer-events:        none;
    }

    &.tab--left  { grid-area: tab-left;  border-radius: var(--radius-s) 0 0 var(--radius-s); }
    &.tab--right { grid-area: tab-right; border-radius: 0 var(--radius-s) var(--radius-s) 0; }
}
</style>