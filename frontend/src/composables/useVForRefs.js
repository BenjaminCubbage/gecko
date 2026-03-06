import {
    onBeforeUpdate,
    onUnmounted,
    shallowRef
} from 'vue';

/*
    Create a reactive array of element refs.
*/
export function useVForRefs() {
    let elements = shallowRef([]);

    function registerElement(element) {
        elements.value = [...elements.value, element];
    }

    onBeforeUpdate(() => elements.value = []);
    onUnmounted   (() => elements.value = []);

    return {
        elements,
        registerElement
    };
}