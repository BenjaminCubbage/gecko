<template>
    <button
        ref="innerElement"
        :class="`
            drawer-button drawer-button--variant-${variant}
            shdw shdw--elevated-s shdw--inst-${color}
            shdw-filter shdw-filter--s shdw-filter--${color}
            txtr-diag txtr-diag--${color}`"
        :data-toggled="isToggled"
        :data-temporarily-disabled="temporarilyDisabled"
        @click="click">
        <slot></slot>
    </button>
</template>

<script setup>
import { useTemplateRef } from 'vue';

const props = defineProps({
    variant: {
        type:     String,
        required: true,
        validator(value) {
            return [
                'button',
                'toggle'
            ].includes(value);
        }
    },

    color: {
        type: String,
        required: true
    },

    temporarilyDisabled: {
        type:    Boolean,
        default: false
    }
});

const isToggled = defineModel('is-toggled', {
    type:     Boolean,
    required: false
});

const emit = defineEmits([
    'click'
]);

function click() {
    if (props.temporarilyDisabled)
        return;

    if (props.variant === 'toggle')
        isToggled.value = !isToggled.value;

    emit('click');
}

defineExpose({
    innerElement: useTemplateRef('innerElement')
});
</script>

<style scoped>
.drawer-button {
    display:       grid;
    place-content: center;
    width:         48px;
    height:        46px;

    --col-bevel-lt: transparent;
    --col-bevel-dk: transparent;

    border-radius: var(--radius-s);
    border:        var(--border-s);

    translate: 
        0 
        calc(var(--shadow-dist-s) - var(--shdw-dist-elevation));
        
    &:hover,
    &:active,
    &.drawer-button--variant-toggle[data-toggled=true] {
        filter: var(--filter-hl-1);
    }
        
    &[temporarily-disabled=true] {
        filter: none;
    }

    &.drawer-button--variant-button:active {
        --shdw-dist-elevation: 0px;
    }

    &.drawer-button--variant-toggle[data-toggled=true],
    &.drawer-button--variant-toggle[data-toggled=true]:hover {
        --shdw-dist-elevation: 0px;
        --shdw-etc: var(--shadow-aura);
    }

    & > * {
        color:       black;
        font-size:   2.8rem;
    }
}
</style>