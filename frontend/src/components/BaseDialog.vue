<template>
    <dialog 
        ref="innerElement" 
        :class="`
            base-dialog
            txtr-diag txtr-diag--${color}
            shdw shdw--inst-${color} shdw--elevated-l`"
        closedby="any" 
        @close="dialogClosed"
        tabindex="-1">
        <div 
            :class="`
                inner-border
                shdw shdw--recessed shdw--inst-lt-gray shdw--otst-${color}`">
            <slot></slot>
        </div>
    </dialog>
</template>

<script setup>
import { 
    nextTick,
    useTemplateRef, 
    watch 
} from 'vue';

import { focusFirstFocusableChild } from '@/core/dom/focusable';

const props = defineProps({
    color: {
        type:    String,
        default: 'green'
    }
});

const innerElement = useTemplateRef('innerElement');

const isOpen = defineModel('isOpen', {
    type:     Boolean,
    required: true
});

watch([isOpen, innerElement], () => {
    if (innerElement.value == null)
        return;

    const currentlyOpen = innerElement.value.open;

    if (isOpen.value && !currentlyOpen) {
        innerElement.value.showModal();

        nextTick(() => {
            /*
                Even with a tabindex of -1, the dialog was being
                focused instead of its content. This makes sure
                we focus the first focusable child.
            */
            if (innerElement.value != null)
                focusFirstFocusableChild(innerElement.value);
        });
    }
    else if (currentlyOpen)
        innerElement.value.close();
}, {
    immediate: true
});

function dialogClosed() {
    isOpen.value = false;
}
</script>

<style scoped>
.base-dialog {
    max-width: calc(100dvw - var(--vp-margin) * 2);

    padding:       9px;
    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.inner-border {
    background:    white;
    color:         black;
    border:        var(--border-s);
    border-radius: var(--radius-s);

    background: var(--col-lt-gray-1);
}
</style>