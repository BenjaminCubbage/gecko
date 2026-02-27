<template>
    <button
        ref="innerElement"
        class="drawer-button"
        :class="{
            'drawer-button--button':          variant === 'button',
            'drawer-button--toggle':          variant === 'toggle',
        }"
        :data-toggled="isToggled"
        @click="click"
        v-bind="colorAttrs"
        :aria-expanded="isToggled">
        <slot></slot>
    </button>
</template>

<script setup>
import {
    computed,
    useTemplateRef
} from 'vue';

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
        required: true,
        validator(value) {
            return [
                'green',
                'red'
            ].includes(value);
        }
    }
});

const isToggled = defineModel('is-toggled', {
    type:     Boolean,
    required: false
});

const colorAttrs = computed(() => {
    switch (props.color) {
        case 'green':
            return {
                class: 'txtr-diag txtr-diag--green',
                style: {
                    '--col-bevel-lt': 'var(--col-green-0)',
                    '--col-bevel-dk': 'var(--col-green-5)'
                }
            }

        case 'red':
            return {
                class: 'txtr-diag txtr-diag--red',
                style: {
                    '--col-bevel-lt': 'var(--col-red-0)',
                    '--col-bevel-dk': 'var(--col-red-5)'
                }
            }
    }

    return {};
});

function click() {
    if (props.variant === 'toggle')
        isToggled.value = !isToggled.value;
}

const innerElement = useTemplateRef('innerElement');

defineExpose({
    innerElement
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

    --inner-button-aura:   0 0;
    --inner-button-offset: 0px;

    box-shadow:
        var(--inner-button-aura),
        0 calc(var(--shadow-dist-s) - var(--inner-button-offset))
        0 black,
        inset      var(--shadow-inst-dist)            var(--shadow-inst-dist)       var(--col-bevel-lt),
        inset calc(var(--shadow-inst-dist) * -1) calc(var(--shadow-inst-dist) * -1) var(--col-bevel-dk);

    border-radius: var(--radius-s);
    border:        var(--border-s);

    transition:
        box-shadow 50ms  ease,
        translate  50ms ease;

    translate: 0 var(--inner-button-offset);

    corner-shape: notch;
}

.drawer-button:hover,
.drawer-button:active {
    filter: var(--filter-hl-1);
}

.drawer-button--button:active {
    --inner-button-offset: var(--shadow-dist-s);
}

.drawer-button--toggle[data-toggled=true],
.drawer-button--toggle[data-toggled=true]:hover {
    --inner-button-aura:   var(--shadow-aura);
    --inner-button-offset: var(--shadow-dist-s);

    filter: none;
}

.drawer-button > * {
    color:       black;
    font-size:   2.8rem;
    line-height: 1em;

    filter:
        drop-shadow(0  2px var(--col-bevel-dk))
        drop-shadow(2px 0  var(--col-bevel-dk))
        drop-shadow(0 -2px var(--col-bevel-lt))
        drop-shadow(-2px 0 var(--col-bevel-lt));
}
</style>