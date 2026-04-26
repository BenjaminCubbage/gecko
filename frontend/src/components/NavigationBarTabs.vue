<template>
    <nav
        ref="tabListEl"
        role="tablist"
        class="navigation-bar-tabs"
        v-roving-container
        v-roving-home="selectedTab === 'canvas' ? 0 : 1">

        <NavigationBarTabButton 
            class="navigation-bar-tab-button--canvas"
            title="CANVAS"
            color="green"
            :is-selected="selectedTab === 'canvas'"
            @click="selectedTab = 'canvas'"
            v-roving-item />

        <NavigationBarTabButton 
            class="navigation-bar-tab-button--friends"
            title="FRIENDS"
            color="orange"
            :is-selected="selectedTab === 'friends'"
            @click="selectedTab = 'friends'"
            v-roving-item />
    </nav>
</template>

<script setup>
import NavigationBarTabButton   from './NavigationBarTabButton.vue';
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
    contain:   layout size;
    isolation: isolate;

    height: 60px;
    width: 224px;

    grid-template:
        "tab-left tab-right" 1fr /
         1fr      1fr;

    display:       grid;
    border-radius: var(--radius-s);

    & > .navigation-bar-tab-button--canvas  { place-self: center; grid-area: tab-left; }
    & > .navigation-bar-tab-button--friends { place-self: center; grid-area: tab-right; }
}

.rail {
    grid-area: 
        tab-left  / 
        tab-left  / 
        tab-right / 
        tab-right;

    height: 57px;
    place-self: start stretch;

    margin-top: -12px;
    margin-left: -6px;
    margin-right: -6px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    border-color: var(--col-gray-7);
    
    --txtr-vert-col-1: var(--col-gray-2);
    --txtr-vert-col-2: var(--col-gray-3);

    --shdw-inst-1: var(--col-gray-1);
    --shdw-inst-2: var(--col-gray-4);
}
</style>