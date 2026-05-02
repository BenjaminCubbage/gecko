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
    }
});

defineExpose({
    innerElement: useTemplateRef('toolBarChipEl')
});
</script>

<style scoped>
.tool-bar-chip {
    --hl: brightness(1);

    display:     grid;
    place-items: center;

    width:  54px;
    height: 44px;

    grid-template:
        minmax(0, 1fr) /
        minmax(0, 1fr);

    filter:
        drop-shadow(3px 3px rgb(0 0 0 / 0.15))
        var(--hl);

    & > .icon-wrapper,
    & > .border {
        grid-area: 1 / 1;
    }

    &:hover,
    &:active {
        --hl: var(--filter-hl-1);
    }

    &:active {
        & > .icon-wrapper {
            translate: 0 var(--shadow-dist-s);
        }

        & > .border {
            --shdw-dist-elevation: 0px;
        }
    }
}

.border {
    place-self: center stretch;

    height: 30px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    translate: 0 calc(-1 * var(--shdw-dist-elevation));
}

.icon-wrapper {
    place-self:  end stretch;
    transform:   translateY(-18px);
    line-height: 0;
}
</style>