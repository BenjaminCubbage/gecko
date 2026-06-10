<template>
    <nav
        ref="tabListEl"
        role="tablist"
        class="
            navigation-bar-tabs
            shdw-before shdw-before--elevated-m shdw-before--inst-lt-gray"
        v-roving-container
        v-roving-home="selectedTab === 'canvas' ? 0 : 1">
        <button
            role="tab"
            class="
                tabs-btn tabs-btn--canvas
                txtr-diag txtr-diag--green
                shdw shdw--inst-green shdw--elevated-m
                shdw-after shdw-after--inst-lt-gray shdw-after--elevated-m
                shdw-before shdw-before--inst-lt-gray shdw-before--elevated-m"
            :data-is-selected="selectedTab == 'canvas'"
            :aria-controls="tabPanelIds.canvas"
            @click="selectedTab = 'canvas'"
            v-roving-item>
            Canvas
        </button>

        <button
            role="tab"
            class="
                tabs-btn tabs-btn--friends
                txtr-diag txtr-diag--orange
                shdw shdw--inst-orange shdw--elevated-m
                shdw-after shdw-after--inst-lt-gray shdw-after--elevated-m
                shdw-before shdw-before--inst-lt-gray shdw-before--elevated-m"
            :data-is-selected="selectedTab == 'friends'"
            :aria-controls="tabPanelIds.friends"
            @click="selectedTab = 'friends'"
            v-roving-item>
            Friends
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
    display:         grid;
    justify-content: center;

    --pd-x-tabs: 16px;
    --pd-y-tabs: 6px;
    --ht-tabs:   42px;
    --wd-gap:    12px;
    --wd-btn:    150px;

    grid-template:
        ".                btn-canvas    .             btn-friends    ." var(--ht-tabs) /
         var(--pd-x-tabs) var(--wd-btn) var(--wd-gap) var(--wd-btn)  var(--pd-x-tabs);

    width:  calc(var(--pad-x-tabs) * 2 + var(--wd-gap) + var(--wd-btn) * 2);
    height: calc(var(--ht-tabs) + var(--pd-tabs) * 2);

    padding: var(--pd-y-tabs) 0;

    &::before {
        content: '';

        height: 27px;

        background:    var(--col-gray-2);
        border:        var(--border-s);
        border-radius: var(--radius-s);
    }

    > .tabs-btn--canvas  { grid-area: btn-canvas; }
    > .tabs-btn--friends { grid-area: btn-friends; }
    &::before              { grid-area: 1 / 1 / 1 / 6; align-self: center; }

    @media (width < 400px) {
        --wd-gap:    8px;
        --pd-x-tabs: calc(var(--shadow-aura-dist-s) + var(--shadow-dist-m));
        margin: 0 calc(-1 * var(--pd-x-tabs));

        &::before {
            margin: 0 9px;
        }
    }
}

.tabs-btn {
    --_wd-nob: 24px;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.8rem;
    letter-spacing:      0.03em;
    text-transform:      uppercase;

    /* Center text a bit better */
    text-indent: 0.1em;

    margin:
        0
        calc(var(--_wd-nob) - var(--shadow-dist-m));

    border: var(--border-s);

    filter:
        var(--_fx-aura,)
        var(--_fx-hl,)
        drop-shadow(3px 3px var(--col-shadow-alpha));

    transform: translateY(-2px);

    @media (hover: hover) {
        &:hover,
        &[data-is-selected=true] {
            --_fx-hl: var(--filter-hl-0);
        }
    }
    
    &::before,
    &::after {
        z-index: -1;
    }

    &[data-is-selected=true] {
        --_fx-aura:
            drop-shadow(0           var(--shadow-aura-dist-s)  white)
            drop-shadow(0 calc(-1 * var(--shadow-aura-dist-s)) white)
            drop-shadow(          var(--shadow-aura-dist-s)  0 white)
            drop-shadow(calc(-1 * var(--shadow-aura-dist-s)) 0 white);

        --shdw-dist-elevation: 0px;

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
    width:    var(--_wd-nob);

    inset:
        calc(-1 * var(--border-thickness-s))
        auto
        calc(-1 * var(--border-thickness-s))
        auto;

    border: var(--border-s);

    background:
        linear-gradient(var(--col-gray-5) 0 0) center / 3px 9px no-repeat,
        linear-gradient(var(--col-gray-5) 0 0) center / 9px 3px no-repeat
        var(--col-gray-2);
}

.tabs-btn::before {
  left:          calc(-1 * var(--_wd-nob));
  border-radius: var(--radius-s) 0 0 var(--radius-s);
}

.tabs-btn::after {
  right:         calc(-1 * var(--_wd-nob));
  border-radius: 0 var(--radius-s) var(--radius-s) 0;
}
</style>