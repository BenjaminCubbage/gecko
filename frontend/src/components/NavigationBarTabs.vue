<template>
    <nav
        ref="tabListEl"
        role="tablist"
        class="navigation-bar-tabs"
        v-roving-container
        v-roving-home="selectedTab === 'canvas' ? 0 : 1">
        <button
            class="
                tabs-btn tabs-btn--canvas
                txtr-diag txtr-diag--green"
            :data-is-selected="selectedTab == 'canvas'"
            @click="selectedTab = 'canvas'"
            v-roving-item>
            Canvas
        </button>

        <button
            class="
                tabs-btn tabs-btn--friends
                txtr-diag txtr-diag--orange"
            :data-is-selected="selectedTab == 'friends'"
            @click="selectedTab = 'friends'"
            v-roving-item>
            Friends
        </button>
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
    contain: content;

    display:         grid;
    justify-content: center;
    justify-self:    center;

    --pd-x-tabs: 20px;
    --pd-y-tabs: 6px;
    --ht-tabs: 42px;
    --wd-gap:  20px;
    --wd-btn:  150px;

    grid-template:
        ".                btn-canvas    .             btn-friends    ." var(--ht-tabs) /
         var(--pd-x-tabs) var(--wd-btn) var(--wd-gap) var(--wd-btn)  var(--pd-x-tabs);

    width:  calc(var(--pad-x-tabs) * 2 + var(--wd-gap) + var(--wd-btn) * 2);
    height: calc(var(--ht-tabs) + var(--pd-tabs) * 2);

    padding: var(--pd-y-tabs) 0;

    &::before {
        content: '';

        height: 27px;

        background:    var(--col-gray-3);
        border:        var(--border-s);
        border-radius: var(--radius-s);

        box-shadow:
            inset var(--shadow-dist-m) var(--shadow-dist-m) var(--col-gray-1),
            0 var(--shadow-dist-m) black;
    }

    & > .tabs-btn--canvas  { grid-area: btn-canvas; }
    & > .tabs-btn--friends { grid-area: btn-friends; }
    &::before              { grid-area: 1 / 1 / 1 / 6; place-self: center stretch; }

    @media (width < 400px) {
        --wd-gap:    8px;
        --pd-x-tabs: calc(var(--shadow-aura-dist) + var(--shadow-dist-m));
        margin: 0 calc(-1 * var(--pd-x-tabs));

        &::before {
            display: none;
        }
    }
}

.tabs-btn {
    --wd-nob:    24px;
    --aura:      drop-shadow(0 0 #0000);
    --elevation: var(--shadow-dist-m);

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.8rem;
    letter-spacing:      0.03em;
    text-transform:      uppercase;

    margin:
        0
        calc(var(--wd-nob) - var(--shadow-dist-m));

    border: var(--border-s);

    box-shadow:
        inset calc(-1 * var(--shadow-dist-m)) calc(-1 * var(--shadow-dist-m)) var(--shadow-2),
        inset var(--shadow-dist-m) var(--shadow-dist-m) var(--shadow-1),
        0 var(--elevation) black;

    filter:
        var(--aura)
        drop-shadow(3px 3px var(--col-shadow-alpha));

    transform: translateY(-2px);

    &.tabs-btn--canvas {
        --shadow-1: var(--col-green-1);
        --shadow-2: var(--col-green-5);
    }

    &.tabs-btn--friends {
        --shadow-1: var(--col-orange-0);
        --shadow-2: var(--col-orange-6);
    }

    &[data-is-selected=true] {
        --aura:
            drop-shadow(0  2px white)
            drop-shadow(0 -2px white)
            drop-shadow( 2px 0 white)
            drop-shadow(-2px 0 white);

        --elevation: 0px;

        translate: 0 var(--shadow-dist-m);

        &::before,
        &::after {
            translate: 0 calc(-1 * var(--shadow-dist-m));
        }
    }
}

.tabs-btn::before,
.tabs-btn::after {
    content:  '';
    position: absolute;
    width:    var(--wd-nob);

    inset:
        calc(-1 * var(--border-thickness-s))
        auto
        calc(-1 * var(--border-thickness-s))
        auto;

    border: var(--border-s);

    background:
                linear-gradient(var(--col-gray-4) 0 0) center / 3px 9px no-repeat,
                linear-gradient(var(--col-gray-4) 0 0) center / 9px 3px no-repeat
                var(--col-gray-2);

    box-shadow:
        inset      var(--shadow-dist-m)            var(--shadow-dist-m)       var(--col-gray-1),
        inset calc(var(--shadow-dist-m) * -1) calc(var(--shadow-dist-m) * -1) var(--col-gray-3),
        0 var(--shadow-dist-m) black;
}

.tabs-btn::before {
  left: calc(-1 * var(--wd-nob));
  border-radius: var(--radius-s) 0 0 var(--radius-s);
}

.tabs-btn::after {
  right: calc(-1 * var(--wd-nob));
  border-radius: 0 var(--radius-s) var(--radius-s) 0;
}
</style>