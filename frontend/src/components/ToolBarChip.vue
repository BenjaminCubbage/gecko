<template>
    <button
        ref="toolBarChipEl"
        class="tool-bar-chip">
        <div :class="`
            border
            txtr-vert txtr-vert--${color}
            shdw shdw--inst-${color} shdw--elevated-s`"></div>

        <div class="icon-wrapper">
            <slot name="icon"></slot>
        </div>

        <div v-if="size != null" class="sizes">
            <div class="size size--s"  data-active="true"></div>
            <div class="size size--m" :data-active="size === 'large' || size === 'medium'"></div>
            <div class="size size--l" :data-active="size === 'large'"></div>
        </div>
    </button>
</template>

<script setup>
import {
    useId,
    useTemplateRef
} from 'vue';

defineProps({
    color: {
        type:    String,
        default: 'gray'
    },

    size: {
        type:     String,
        required: false,
        validator(value) {
            return [
                'small',
                'medium',
                'large'
            ].includes(value);
        }
    }
});

defineExpose({
    innerElement: useTemplateRef('toolBarChipEl')
});
</script>

<style scoped>
.tool-bar-chip {
    --hl:      brightness(1);
    --outline: drop-shadow(0 0 transparent);

    display:     grid;
    place-items: center;

    width:  54px;
    height: 44px;

    grid-template:
        minmax(0, 1fr) /
        minmax(0, 1fr);

    filter:
        var(--hl)
        var(--outline)
        drop-shadow(3px 3px rgb(0 0 0 / 0.15));

    & > .icon-wrapper { z-index: 1; grid-area: 1 / 1; place-self: end    stretch; }
    & > .border       { z-index: 0; grid-area: 1 / 1; place-self: center stretch; }
    & > .sizes        { z-index: 2; grid-area: 1 / 1; place-self: end    center;}

    /*
        White outline
    */
    &[data-pressed=true] {
        --outline: 
            drop-shadow(0 calc(     var(--shadow-dist-s)) white)
            drop-shadow(0 calc(-1 * var(--shadow-dist-s)) white)
            drop-shadow(calc(     var(--shadow-dist-s)) 0 white)
            drop-shadow(calc(-1 * var(--shadow-dist-s)) 0 white);
    }

    /*
        Brighten + scale
    */
    &:hover,
    &:active,
    &[data-pressed=true] {
        --hl: var(--filter-hl-1);

        &:not([data-pressed=true]) > .icon-wrapper {
            transform-origin: 50% 70%;
            scale:            1.04;
        }
    }

    /*
        Press downward
    */
    &:active,
    &[data-pressed=true] {
        & > .icon-wrapper {
            translate: 0 var(--shadow-dist-s);
        }

        & > .border {
            --shdw-dist-elevation: 0px;
        }
    }
}

.border {
    height: 30px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transform: 
        translateY(calc(-1 * var(--shdw-dist-elevation)));
}

.icon-wrapper {
    transform:   translateY(-18px);
    line-height: 0;
}

.sizes {
    translate: 0 20px;

    width:  50px;
    height: 24px;

    display:         flex;
    align-items:     center;
    justify-content: space-evenly;
    gap:             2px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    background: var(--col-lt-gray-3);

    box-shadow: 
        0 var(--shadow-dist-s) black,
        inset           var(--shadow-dist-m)            var(--shadow-dist-m)  var(--col-lt-gray-0),
        inset calc(-1 * var(--shadow-dist-m)) calc(-1 * var(--shadow-dist-m)) var(--col-gray-2);
}

.size {
    --s: 3px;

    width:  8px;
    height: 8px;
    background: var(--col-gray-4);

    border-radius: var(--radius-xs);

    &.size--s { margin-right: -1.5px; }
    &.size--m { --s: 2.3px; scale: 1.3; }
    &.size--l { --s: 2px;   scale: 1.5; }

    &[data-active=true] {
        background: black;
        filter:
            drop-shadow(0            var(--s)    white)
            drop-shadow(0 calc(0px - var(--s))   white)
            drop-shadow(             var(--s)  0 white)
            drop-shadow(calc(0px -   var(--s)) 0 white);
    }
}
</style>