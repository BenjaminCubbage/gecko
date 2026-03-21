<template>
    <button
        ref="toolBarChipEl"
        :aria-disabled="disabled"
        :data-disabled="disabled || temporarilyDisabled"
        :aria-pressed="pressed"
        :data-pressed="temporarilyPressed || (pressed ?? temporarilyPressed)"
        :aria-describedby="statusElId"
        :class="`
            tool-bar-chip
            tool-bar-chip--icon-${iconPlacement}
            tool-bar-chip--hl-style-${highlightStyle}
            tool-bar-chip--disable-style-${disableStyle}`"
        @click="click">
        <div v-if="iconPlacement !== 'none'" class="icon" aria-hidden="true">
            <slot name="icon"></slot>
        </div>

        <div :class="`label label--${color}`">
            <slot name="label"></slot>
        </div>

        <div :class="`
            pad pad--${color} 
            txtr-diag txtr-diag--${color}
            shdw shdw--inst-${color} shdw--elevated-s`"></div>

        <span :id="statusElId" class="util-sr-only">
            {{ srStatus }}
        </span>
    </button>
</template>

<script setup>
import {
    useId,
    useTemplateRef
} from 'vue';

const props = defineProps({
    color: {
        type:     String,
        required: true,
        validator(value) {
            return [
                'dk-green',
                'magenta',
                'orange',
                'dk-red'
            ].includes(value);
        }
    },

    iconPlacement: {
        type:    String,
        default: 'left',
        validator(value) {
            return [
                'left',
                'right'
            ].includes(value);
        }
    },

    highlightStyle: {
        type:    String,
        default: 'shadow',
        validator(value) {
            return [
                'shadow',
                'filter'
            ].includes(value);
        }
    },

    disableStyle: {
        type:    String,
        default: 'grayed',
        validator(value) {
            return [
                'grayed',
                'none'
            ].includes(value);
        }
    },

    disabled: {
        type:    null,
        default: null
    },

    /*
        For short animations. Like disabled, but doesn't announce it
        to SR.
    */
    temporarilyDisabled: {
        type:    null,
        default: null
    },

    pressed: {
        type:    null,
        default: null
    },

    /*
        See temporarilyDisabled.
    */
    temporarilyPressed: {
        type:    null,
        default: null
    },

    /*
        Screenreader status.
    */
    srStatus: {
        type:    String,
        default: null
    }
});

const emit = defineEmits([
    'click'
]);

const statusElId = useId();

function click() {
    if (!props.disabled && !props.temporarilyDisabled)
        emit('click');
}

defineExpose({
    innerElement: useTemplateRef('toolBarChipEl')
});
</script>

<style scoped>
.tool-bar-chip {
    contain: content;

    --chip-padding-x: 10px;
    --transition-dur: 50ms;
    --press-depth:    0px;
    --select-aura:    0 0;

    display:     grid;
    height:      40px;
    place-items: center;
    padding:     0 calc(3px + var(--chip-padding-x) + var(--shadow-aura-dist));

    &.tool-bar-chip--icon-left  { grid-template: [pad icon label] auto / [pad-start icon]  auto [label] auto [pad-end]; }
    &.tool-bar-chip--icon-right { grid-template: [pad icon label] auto / [pad-start label] auto [icon]  auto [pad-end]; }

    &:hover:not([data-disabled=true]),
    &:active {
        filter: var(--filter-hl-1);
    }

    &:active {
        --press-depth: var(--shadow-dist-s);
    }

    &:active[data-pressed],
    &[data-pressed=true] {
        --press-depth: var(--shadow-dist-s);

        &.tool-bar-chip--hl-style-shadow {
            --select-aura: var(--shadow-aura);
        }

        &.tool-bar-chip--hl-style-filter {
            --w: 2.2px;

            filter:
                var(--filter-hl-1)
                drop-shadow(calc(var(--w) *  1.0) calc(var(--w) *  0.0) 0 white)
                drop-shadow(calc(var(--w) *  0.0) calc(var(--w) *  1.0) 0 white)
                drop-shadow(calc(var(--w) * -0.8) calc(var(--w) *  0.0) 0 white)
                drop-shadow(calc(var(--w) *  0.0) calc(var(--w) * -0.8) 0 white);
        }
    }

    &[data-disabled=true] {
        pointer-events: none;
    }

    &[aria-disabled=true] {
        pointer-events: none;

        &.tool-bar-chip--disable-style-grayed {
            opacity: 0.5;
            filter:  grayscale();
        }
    }

    & > .icon  { grid-area: icon;  z-index: 2; }
    & > .label { grid-area: label; z-index: 1; }
    & > .pad   { grid-area: pad;   z-index: 0; }

    & > .icon  { translate: 0 calc(-3.75px + var(--press-depth)); }
    & > .label { translate: 0 calc(-3.75px + var(--press-depth)); }
    & > .pad   { translate: 0 calc( 3.75px + var(--press-depth)); }
}

.icon {
    display:       grid;
    align-self:    stretch;
    place-content: center;
    transition:    translate var(--transition-dur);
}

.label {
    display:     flex;
    align-items: center;

    -webkit-text-stroke:  var(--text-stroke-s);
    font-size:            3rem;
    font-variant-numeric: tabular-nums;
    letter-spacing:       0.02em;
    line-height:          1;

    /* Make text a bit taller */
    scale:      1 1.1;
    transition: translate var(--transition-dur);

    &.label--dk-green { color: var(--col-green-5); }
    &.label--magenta  { color: var(--col-magenta-6); }
    &.label--orange   { color: var(--col-orange-8); }
    &.label--dk-red   { color: var(--col-red-6); }
}

.pad {
    --shdw-dist-elevation: calc(var(--shadow-dist-s) - var(--press-depth));
    --shdw-etc:            var(--select-aura);

    --chip-col-bevel-lt: 0;
    --chip-col-bevel-dk: 0;

    display:      grid;
    height:       24px;
    justify-self: stretch;
    z-index:      0;
    margin:       0 calc(var(--chip-padding-x) * -1);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transition:
        translate  var(--transition-dur),
        box-shadow var(--transition-dur);

    &.pad--dk-green { --chip-col-bevel-lt: var(--col-green-0);   --chip-col-bevel-dk: var(--col-green-6); }
    &.pad--dk-red   { --chip-col-bevel-lt: var(--col-red-0);     --chip-col-bevel-dk: var(--col-red-6); }
    &.pad--magenta  { --chip-col-bevel-lt: var(--col-magenta-0); --chip-col-bevel-dk: var(--col-magenta-6); }
    &.pad--orange   { --chip-col-bevel-lt: var(--col-orange-0);  --chip-col-bevel-dk: var(--col-orange-8); }
}
</style>