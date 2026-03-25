<template>
    <button
        type="button"
        class="
            base-button
            shdw shdw--elevated-s"
        :role="{
            'button': 'button',
            'toggle': 'checkbox',
            'tab':    'tab'
        }[behavior]"
        :data-behavior="behavior"
        :data-toggled="isToggled"
        :aria-selected="behavior === 'tab' && isToggled">
        <slot></slot>
    </button>
</template>

<script setup>
defineProps({
    behavior: {
        type: String,
        default: 'button',
        validator(value) {
            return [
                'button',
                'toggle',
                'tab'
            ].includes(value);
        }
    },

    isToggled: {
        type: Boolean,
        default: false,
        validator(value, props) {
            return !value || props.behavior !== 'button';
        }
    }
});
</script>

<style scoped>
.base-button {
    --hl:         brightness(1);
    --filter-etc: brightness(1);

    color:          black;
    font-size:      2.1rem;
    letter-spacing: 0.04em;
    padding:        4px 0;
    text-transform: uppercase;
    line-height:    1;

    border-radius: var(--radius-s);
    border:        var(--border-s);
    
    translate:
        0 calc(var(--shdw-dist-elevation) * -1);
    
    filter: 
        var(--hl)
        var(--filter-etc);

    &::after,
    &::before {
        text-transform: none;
    }
    
    &:hover,
    &:active {
        --hl: var(--filter-hl-1);
    }
}

.base-button[data-behavior=button] {
    &:active {
        --shdw-dist-elevation: 0;
    }
}

.base-button[data-behavior=toggle],
.base-button[data-behavior=tab] {
    &[data-toggled=true] {
        --hl:                  var(--filter-hl-1);
        --shdw-dist-elevation: 0;
    }
}


.base-button[data-behavior=tab] {
    &[data-toggled=true] {
        pointer-events: none;
    }
}
</style>