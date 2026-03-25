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
        <BaseButton
            behavior="tab"
            :aria-controls="tabPanelIds.canvas"
            class="
                tab tab--left
                txtr-diag txtr-diag--green
                shdw shdw--inst-green
                text-stroke--s"
            :is-toggled="selectedTab === 'canvas'"
            @click="selectedTab = 'canvas'"
            v-roving-item>
            canvas
        </BaseButton>

        <BaseButton
            behavior="tab"
            :aria-controls="tabPanelIds.friends"
            class="
                tab tab--right
                txtr-diag txtr-diag--orange
                shdw shdw--inst-orange
                text-stroke--s"
            :is-toggled="selectedTab === 'friends'"
            @click="selectedTab = 'friends'"
            v-roving-item>
            friends
        </BaseButton>
    </nav>
</template>

<script setup>
import BaseButton               from './BaseButton.vue';
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

    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.tab {
    margin-top: -6px;

    &.tab--left  { grid-area: tab-left;  border-radius: var(--radius-s) 0 0 var(--radius-s); }
    &.tab--right { grid-area: tab-right; border-radius: 0 var(--radius-s) var(--radius-s) 0; }
}
</style>