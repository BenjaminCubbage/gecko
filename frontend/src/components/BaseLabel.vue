<template>
    <BaseClippableStrokedText 
        v-if="variant === 'no-box'"
        is="label" 
        class="label label--content"
        :class="`label--color-${color}`">
        <slot></slot>
    </BaseClippableStrokedText>

    <div 
        v-else
        class="label label--background">
        <BaseClippableStrokedText 
            is="label" 
            class="label label--content"
            :class="`label--color-${color}`"
            :for="for">
            <slot></slot>
        </BaseClippableStrokedText>
    </div>
</template>

<script setup>
import BaseClippableStrokedText from './BaseClippableStrokedText.vue';

defineProps({
    variant: {
        type:    String,
        default: 'normal',
        validator(value) {
            return [
                'normal',
                'no-box'
            ].includes(value);
        }
    },

    color: {
        type:    String,
        default: 'green',
        validator(value) {
            return [
                'green',
                'orange'
            ].includes(value);
        }
    },

    for: {
        type:     String,
        required: false
    }
})
</script>

<style scoped>
.label {
    font-size:            1.9rem;
    font-variant-numeric: tabular-nums;
    letter-spacing:       0.05em;
    line-height:          1;
    text-transform:       uppercase;

    & > .label--content {
        font-size:            inherit;
        font-variant-numeric: inherit;
        letter-spacing:       inherit;
        line-height:          inherit;
        text-transform:       inherit;
    }
}

.label--background {
    padding: 3px 16px 4px;
        
    border:        var(--border-s);
    border-radius: var(--radius-s);

    box-shadow:
        var(--shadow-inst-lt-gray),
        var(--shadow-s);

    background:
        linear-gradient(
            var(--col-lt-gray-2) 50%,
            var(--col-lt-gray-4) 50%);
            
    corner-shape: notch;
}

.label--content {
    padding:     0 var(--text-stroke-width-s);
    user-select: none;
    white-space: nowrap;
    
    -webkit-text-stroke:  black var(--text-stroke-width-l);
    color:                transparent;

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
}
</style>