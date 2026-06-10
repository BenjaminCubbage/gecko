<template>
    <div
        v-show="fontsLoaded"
        ref="layoutEl"
        class="layout"
        :inert="isLoading"
        :style="{
            'opacity': isLoading ? '0.01' : '1'
        }">
        <NavigationBar class="navigation-bar" v-model:selected-tab="selectedTab" />

        <main class="main-content">
            <CanvasSection
                v-show="selectedTab === 'canvas'"
                role="tabpanel"
                :id="tabPanelIds.canvas" />

            <FriendsSection
                v-show="selectedTab === 'friends'"
                role="tabpanel"
                :id="tabPanelIds.friends" />
        </main>
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    provide,
    ref,
    useId,
    useTemplateRef,
    watch
} from 'vue';

import CanvasSection   from './sections/CanvasSection.vue';
import FriendsSection  from './sections/FriendsSection.vue';
import NavigationBar   from './components/NavigationBar.vue';

import { useElementIdRegistry }      from './composables/useElementIdRegistry.js';
import { useWaitOnFont }             from './composables/useWaitOnFont.js';
import { useWaitOnTransition }       from './composables/useWaitOnTransition.js';
import { useScrollbarWidthProperty } from './composables/useScrollbarWidthProperty.js';

import { Keys } from './core/di/keys.js';

useScrollbarWidthProperty()
    .updateScrollbarWidthProperty();

const tabPanelIds = useElementIdRegistry(Keys.AppTabPanelIdsRegistry, {
    canvas:  useId(),
    friends: useId()
});

const friends = inject(Keys.FriendsStore);
const session = inject(Keys.SessionStore);
const devices = inject(Keys.DevicesStore);

const selectedTab = ref('canvas');

const isLoading = computed(() =>
    session.state.value === 'loading');

const { isFontLoaded: isMainFontLoaded } = useWaitOnFont('--font-main');
const { isFontLoaded: isScndFontLoaded } = useWaitOnFont('--font-scnd');
const { isFontLoaded: isIconFontLoaded } = useWaitOnFont('iconfont');

const fontsLoaded = computed(() => {
    return isMainFontLoaded.value
        && isScndFontLoaded.value
        && isIconFontLoaded.value;
});

provide(Keys.IsDocumentReady, fontsLoaded);
</script>

<style scoped>
.layout {
    contain: content;

    height:           100svh;
    overflow:         auto;
    scrollbar-gutter: stable both-edges;

    align-content:  center;
    display:        flex;
    flex-flow:      column;
    padding-bottom: calc(var(--shadow-dist-l) * 4);

    > .navigation-bar { order: 0; z-index: 0; }
    > .main-content   { order: 2; z-index: 1; }

    /* Flex spacer */
    &::before { order: 1; content: ''; flex: 0 1 20px; }
}

.main-content {
    grid-template-columns:
        1fr [main] min(550px, 100%) 1fr;

    display:        grid;
    place-content:  stretch;
    pointer-events: none;

    > * {
        grid-column:    main;
        margin-left:    var(--vp-margin);
        margin-right:   var(--vp-margin);
        pointer-events: auto;
    }
}
</style>