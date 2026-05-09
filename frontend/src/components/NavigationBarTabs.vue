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
            corner-direction="right"
            :is-selected="selectedTab === 'canvas'"
            @click="selectedTab = 'canvas'"
            v-roving-item />

        <NavigationBarTabButton 
            class="navigation-bar-tab-button--friends"
            title="FRIENDS"
            color="orange"
            corner-direction="left"
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
    width: 280px;

    grid-template:
        "tab-left tab-right" 1fr /
         1fr      1fr;

    display:       grid;
    border-radius: var(--radius-s);

    & > .navigation-bar-tab-button--canvas  { place-self: center; grid-area: tab-left; }
    & > .navigation-bar-tab-button--friends { place-self: center; grid-area: tab-right; }
}
</style>