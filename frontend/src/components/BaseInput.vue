<template>
    <input
        ref="innerElement"
        class="base-input"
        :value="inputText"
        spellcheck="false"
        type="text"
        @input="input" />
</template>

<script setup>
import { nextTick, useTemplateRef } from 'vue';

const props = defineProps({
    charPredicate: {
        type:    null,
        default: null,
        validator(fn) {
            return !fn || fn instanceof Function;
        }
    }
});

const inputText = defineModel({
    type:     String,
    required: true
});

const innerElement = useTemplateRef('innerElement');

async function input() {
    /*
        note(ben): nextTick seems to be needed on Firefox
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
    appearance:    none;
    box-sizing:    border-box;
    margin:        0;
    padding-right: 0;
    border:        0;

    color:       inherit;
    font-family: inherit;
    font-size:   inherit;

    background: transparent;
    outline:    none;

    paint-order: stroke;
}
</style>