<template>
    <dialog 
        ref="innerElement" 
        :class="`
            base-dialog
            txtr-diag txtr-diag--${color}
            shdw shdw--inst-${color} shdw--elevated-l`"
        closedby="any" 
        @close="dialogClosed">
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
    useTemplateRef, 
    watch 
} from 'vue';

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
    if (isOpen.value)
        innerElement.value?.showModal();
    else
        innerElement.value?.close();
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