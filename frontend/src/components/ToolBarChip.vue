<template>
    <button
        ref="toolBarChipEl"
        class="tool-bar-chip"
        :data-is-pressed="isPressed"
        :data-is-busy="isBusy"
        :data-pen-size="penSize">
        <div :class="`
            border
            txtr-vert txtr-vert--${color}
            shdw shdw--inst-${color} shdw--elevated-s`"></div>

        <div class="icon-wrapper">
            <slot name="icon"></slot>
        </div>

        <div class="sizes">
            <div class="size-dots"></div>
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

    penSize: {
        type:     String,
        required: false,
        validator(value) {
            return [
                'small',
                'medium',
                'large'
            ].includes(value);
        }
    },

    isPressed: {
        type:    Boolean,
        default: false
    },

    isBusy: {
        type:    Boolean,
        default: false
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
        White border around base
    */
    &[data-is-busy=true] > .border {
        --shdw-etc: 0 0 0 var(--shadow-dist-s) white;
    }

    /*
        White outline
    */
    &[data-is-pressed=true] {
        --outline:
            drop-shadow(0 calc(     var(--shadow-dist-s)) white)
            drop-shadow(0 calc(-1 * var(--shadow-dist-s)) white)
            drop-shadow(calc(     var(--shadow-dist-s)) 0 white)
            drop-shadow(calc(-1 * var(--shadow-dist-s)) 0 white);
    }

    /*
        Brighten + scale
    */
    &:active,
    &[data-is-pressed=true],
    &[data-is-busy=true],
    &:hover {
        --hl: var(--filter-hl-1);
    }

    @media (hover: hover) {
        &:not([data-is-pressed=true], [data-is-busy=true]):is(:active, :hover) {
            & > .icon-wrapper {
                scale: 1.04;
            }
        }
    }

    /*
        Press down
    */
    &:active,
    &[data-is-pressed=true],
    &[data-is-busy=true] {
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
    transform-origin: 50% 70%;
    transform:        translateY(-18px);
    line-height:      0;
}

.sizes {
    translate: 0 19px;

    width:  50px;
    height: 24px;

    display:         flex;
    align-items:     center;
    justify-content: center;

    border:        var(--border-s);
    border-radius:
        0               0
        var(--radius-s) var(--radius-s);

    background: var(--col-lt-gray-3);

    box-shadow:
        0 var(--shadow-dist-s) black,
        inset           var(--shadow-dist-m)            var(--shadow-dist-m)  var(--col-lt-gray-0),
        inset calc(-1 * var(--shadow-dist-m)) calc(-1 * var(--shadow-dist-m)) var(--col-gray-2);
}

.tool-bar-chip:not([data-pen-size]) > .sizes {
    display: none;
}

.size-dots {
    position:    relative;
    scale:       1.3;
    margin-left: -1.5px;

    &,
    &::before,
    &::after {
        width:  8px;
        height: 8px;

        border-radius: var(--radius-xs);
        background:    var(--col-gray-3);
    }

    &::before,
    &::after {
        content:  '';
        position: absolute;
        top:      0;
        display:  block;
    }

    &::before {
        right:      calc(100% + 2px);
        scale:      calc(1 / 1.3);
        background: black;
    }

    &::after {
        left:  calc(100% + 3px);
        scale: calc(1.5 / 1.3);
    }
}

.tool-bar-chip[data-pen-size=medium] > .sizes > .size-dots,
.tool-bar-chip[data-pen-size=large]  > .sizes > .size-dots,
.tool-bar-chip[data-pen-size=large]  > .sizes > .size-dots::after {
    background: black;
}
</style>