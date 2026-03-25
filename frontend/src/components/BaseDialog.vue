<template>
    <dialog
        v-modal-auto-focus
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

const innerElement  = useTemplateRef('innerElement');

const isOpen = defineModel('isOpen', {
    type:     Boolean,
    required: true
});

watch([isOpen, innerElement], () => {
    if (innerElement.value == null)
        return;

    const currentlyOpen = innerElement.value.open;

    if (isOpen.value && !currentlyOpen)
        innerElement.value.showModal();
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
    --transition-dur: 100ms;

    overflow: visible;
    max-width: calc(100dvw - var(--vp-margin));

    padding:       9px;
    border:        var(--border-s);
    border-radius: var(--radius-s);

    & > .inner-border { z-index: 0; }
    & > .close-button { z-index: 1; }
}

.inner-border {
    background:    white;
    color:         black;
    border:        var(--border-s);
    border-radius: var(--radius-s);

    background: var(--col-lt-gray-1);
}

.close-button {
    --transition-dur: 50ms;
    --press-depth:    0px;

    position: absolute;
    inset:    -8px -8px auto auto;

    width:  1.4em;
    height: 1.4em;

    display:       grid;
    place-content: center;

    padding-bottom: 3px;

    -webkit-text-stroke: initial;
    font-size:           2.2rem;
    color:               black;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    text-shadow:
        -1.5px -1.5px var(--col-red-1),
         1.5px  1.5px var(--col-red-5);

    translate: 0 calc(var(--shadow-dist-s) - var(--shdw-dist-elevation));

    &:hover,
    &:active {
        filter: var(--filter-hl-1);
    }

    &:active {
        --shdw-dist-elevation: 0px;
    }
}
</style>