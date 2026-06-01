<template>
    <input
        ref="innerElement"
        class="base-input"
        :class="`base-input--variant-${variant}`"
        type="text"
        :value="inputText"
        :disabled="disabled"
        :data-disabled="disabled || temporarilyDisabled"
        spellcheck="false"
        @keydown="keyDown"
        @input="input" />
</template>

<script setup>
import {
    nextTick,
    useTemplateRef
} from 'vue';

const props = defineProps({
    variant: {
        type: String,
        default: 'normal',
        validator(value) {
            return [
                'normal',
                'no-box'
            ].includes(value);
        }
    },

    charPredicate: {
        type:    null,
        default: null,
        validator(value) {
            return !value || value instanceof Function;
        }
    },

    disabled: {
        type:    Boolean,
        default: false
    },

    /*
        Disable input but don't lose focus or announce to SR.
    */
    temporarilyDisabled: {
        type:    Boolean,
        default: false
    }
});

const inputText = defineModel({
    type:     String,
    required: true
});

const innerElement = useTemplateRef('innerElement');

function keyDown(e) {
    if (props.disabled || props.temporarilyDisabled) {
        if (e.key !== 'Tab')
            e.preventDefault();
    }
}

async function input() {
    /*
        nextTick seems to be needed on Firefox
    */
    await nextTick();

    if (props.charPredicate) {
        const chars = innerElement.value.value.split('');
        const filteredChars = chars.filter(props.charPredicate);

        const numFiltered = chars.length - filteredChars.length;

        const oldSelectionStart = innerElement.value.selectionStart;
        const newSelectionStart = oldSelectionStart - numFiltered;

        innerElement.value.value = filteredChars.join('');

        innerElement.value.selectionStart
            = innerElement.value.selectionEnd
            = newSelectionStart;
    }

    inputText.value = innerElement.value.value;
}

defineExpose({
    innerElement
});
</script>

<style scope>
.base-input {
    -webkit-appearance: none;
    appearance:         none;
    margin:             0;
    width:              100%;

    -webkit-text-stroke: var(--text-stroke-s);
    background:          transparent;
    color:               black;
    font-family:         inherit;
    font-size:           inherit;
    letter-spacing:      0.03em;
    outline:             none;
    border:              0;

    &.base-input--variant-normal {
        height:  36px;
        padding: 0 16px;
    }

    &.base-input--variant-no-box {
        padding: 0;
        border:  0;
    }

    &[data-disabled=true] {
        caret-color: transparent;
    }
}
</style>