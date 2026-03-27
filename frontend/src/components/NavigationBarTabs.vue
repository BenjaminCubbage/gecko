<template>
    <nav
        ref="tabListEl"
        role="tablist"
        class="navigation-bar-tabs"
        v-roving-container
        v-roving-home="selectedTab === 'canvas' ? 0 : 1">
        <div class="slider-bar shdw shdw--inst-lt-gray shdw--elevated-s">
        </div>

        <button
            type="button"
            role="tab"
            :aria-controls="tabPanelIds.canvas"
            class="
                tab tab--canvas
                shdw shdw--inst-green"
            :data-selected="selectedTab === 'canvas'"
            @click="selectedTab = 'canvas'"
            v-roving-item>
            <IconCanvas class="icon-canvas" />
            <span class="text text-stroke--s">
                CANVAS
            </span>
        </button>

        <button
            type="button"
            role="tab"
            :aria-controls="tabPanelIds.canvas"
            class="
                tab tab--friends
                shdw shdw--inst-orange"
            :data-selected="selectedTab === 'friends'"
            @click="selectedTab = 'friends'"
            v-roving-item>
            <IconFriends class="icon-friends" />
            <span class="text text-stroke--s">
                FRIENDS
            </span>
        </button>
    </nav>
</template>

<script setup>
import IconCanvas  from './IconCanvas.vue';
import IconFriends from './IconFriends.vue';

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

    grid-template:
        "tab-left tab-right" 42px /
         140px    140px;

    display: grid;
    border-radius: var(--radius-s);
    gap: 6px;

    & > .tab--canvas { grid-area: tab-left; }
    & > .tab--friends { grid-area: tab-right; }
}

.tab {
    justify-self: stretch;

    display:       flex;
    align-items: center;
    justify-content: center;
    gap: 8px;

    padding: 6px;

    opacity: 0.7;
    scale: 1;

    --hl: brightness(1);

    filter:
        var(--hl);

    &:hover {
        opacity: 1;
    }

    &.tab--friends {
        background:
            linear-gradient(
                var(--col-orange-2) 50%,
                var(--col-orange-4) 50%);
        
        & > svg {
            filter: drop-shadow(var(--shadow-dist-s) var(--shadow-dist-s) var(--col-orange-7));
        }

        & .text {
            filter: drop-shadow(3px 3px var(--col-orange-6));
        }
    }

    &.tab--canvas {
        background:
            linear-gradient(
                var(--col-green-2) 50%,
                var(--col-green-4) 50%);
        
        & > svg {
            filter: drop-shadow(var(--shadow-dist-s) var(--shadow-dist-s) var(--col-green-6));
        }

        & .text {
            filter: drop-shadow(3px 3px var(--col-green-5));
        }
    }
        border:        var(--border-s);
        border-radius: var(--radius-s);

    &[data-selected=true] {
        filter:
            var(--hl);

        --shdw-etc: 3px 3px var(--col-gray-3);

        &.tab--friends {
            background:
                linear-gradient(
                    var(--col-orange-2) 50%,
                    var(--col-orange-4) 50%);
            
            & > svg {
                filter: drop-shadow(var(--shadow-dist-s) var(--shadow-dist-s) var(--col-orange-7));
            }

            & .text {
                filter: drop-shadow(3px 3px var(--col-orange-6));
            }
        }

        &.tab--canvas {
            background:
                linear-gradient(
                    var(--col-green-2) 50%,
                    var(--col-green-4) 50%);
            
            & > svg {
                filter: drop-shadow(var(--shadow-dist-s) var(--shadow-dist-s) var(--col-green-6));
            }

            & .text {
                filter: drop-shadow(3px 3px var(--col-green-5));
            }
        }

        opacity: 1;
    }
}

.icon-canvas {
    height: 30px;
}

.icon-friends {
    height: 33px;
}

.text {
    font-size: 2.3rem;
    margin-inline-end: 2px;
}

.slider-bar {
    grid-area: tab-left / tab-left / tab-right / tab-right;
    margin: -6px;

    background: 
        linear-gradient(
            var(--col-gray-0) 50%,
            var(--col-gray-1) 50%);
    border: 3px solid black;

    border-radius: var(--radius-s);
    --shdw-etc: 
        0 3px black;
}
</style>