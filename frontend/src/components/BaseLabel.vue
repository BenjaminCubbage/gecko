<template>
    <BaseClippableStrokedText
        is="label"
        class="label"
        :class="`label--color-${color}`">
        <slot></slot>
    </BaseClippableStrokedText>
</template>

<script setup>
import BaseClippableStrokedText from './BaseClippableStrokedText.vue';

defineProps({
    color: {
        type:    String,
        default: 'green',
        validator(value) {
            return [
                'green',
                'orange',
                'bitonal'
            ].includes(value);
        }
    }
});
</script>

<style scoped>
.label {
    padding:     0 var(--text-stroke-width-s);
    white-space: nowrap;

    -webkit-text-stroke:  black var(--text-stroke-width-l);
    color:                transparent;
    font-size:            1.9rem;
    letter-spacing:       0.05em;
    line-height:          1;
    text-transform:       uppercase;

    filter: drop-shadow(0 var(--shadow-dist-xs) black);

    &.label--color-green  {
        background-image:
            linear-gradient(
                var(--col-green-0) 35%,
                var(--col-green-1) 35% 50%,
                var(--col-green-3) 50% 65%,
                var(--col-green-4) 65%);
    }

    &.label--color-orange {
        background-image:
            linear-gradient(
                var(--col-orange-0) 35%,
                var(--col-orange-2) 35% 50%,
                var(--col-orange-3) 50% 65%,
                var(--col-orange-5) 65%);
    }

    &.label--color-bitonal {
        color: black;
        -webkit-text-stroke-color: white;
        filter: drop-shadow(0 var(--shadow-dist-xs) white);
    }
}
</style>