<template>
    <button
        ref="toolBarChipEl"
        class="tool-bar-chip"
        :data-is-selected="isSelected"
        :data-is-busy="isBusy"
        :data-pen-size="penSize"
        @click="onClick"
        v-interactive-attrs>
        <span class="chip-main">
            <span 
                :class="`
                    base
                    txtr-vert txtr-vert--${color}
                    shdw shdw--inst-${color} shdw--elevated-s`"></span>

            <span class="icon-wrapper">
                <slot name="icon"></slot>
            </span>
        </span>

        <span class="sizes">
            <span class="size-dots"></span>
        </span>
    </button>
</template>

<script setup>
const props = defineProps({
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

    isSelected: {
        type:    Boolean,
        default: false
    },

    isBusy: {
        type:    Boolean,
        default: false
    }
});

const emit = defineEmits([ 'click' ]);

function onClick() {
    if (!props.isBusy)
        emit('click');
}
</script>

<style scoped>
.tool-bar-chip {
    position: relative;
    display:  grid;

    width:  54px;
    height: 44px;

    grid-template:
        minmax(0, 1fr) /
        minmax(0, 1fr);

    filter:
        var(--_fx-hl,)
        drop-shadow(3px 3px rgb(0 0 0 / 0.15));
        
    /*
        White border around base
    */
    &[data-is-busy=true] > .chip-main > .base {
        --shdw-etc: 0 0 0 var(--shadow-dist-s) white;
    }

    /*
        White outline
    */
    &[data-is-selected=true] > .chip-main {
        filter: var(--filter-aura-s);
    }

    /*
        Brighten + scale
    */
    &[data-is-busy=true],
    &[data-hovered] {
        --_fx-hl: var(--filter-hl-1);
    }

    &:not([data-is-busy=true])[data-hovered] > .chip-main > .icon-wrapper {
        scale: 1.02;
    }

    /*
        Press down
    */
    &:is([data-pressed], [data-is-busy=true]) > .chip-main {
        > .icon-wrapper {
            translate: 0 var(--shadow-dist-s);
        }

        > .base {
            --shdw-dist-elevation: 0px;
        }
    }

    /*
        Sizes
    */
    &:not([data-is-selected=true]) > .sizes {
        opacity: 0.5;
    }
}

.chip-main {
    display:     grid;
    place-items: center;

    width:  54px;
    height: 44px;

    grid-template:
        minmax(0, 1fr) /
        minmax(0, 1fr);

    > .icon-wrapper { z-index: 1; grid-area: 1 / 1; place-self: end    stretch; }
    > .base       { z-index: 0; grid-area: 1 / 1; place-self: center stretch; }
}

.base {
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
    position: absolute;
    inset:    auto 0 -20px;
    margin:   0 auto;

    width:  50px;
    height: 24px;

    display:         flex;
    align-items:     center;
    justify-content: center;

    border: var(--border-s);
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