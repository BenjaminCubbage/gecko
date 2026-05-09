<template>
    <dialog
        ref="innerEl"
        class="
            aside-modal
            shdw shdw--elevated-l shdw--inst-green
            txtr-vert txtr-vert--green"
        :aria-label="title"
        closedby="any"
        @close="dialogClosed">
        <h1 class="
            header
            shdw shdw--inst-gray
            txtr-vert txtr-vert--gray">
            <slot name="icon"></slot>
            {{ title }}
        </h1>
    </dialog>
</template>

<script setup>
import {
    useTemplateRef,
    watch
} from 'vue';

defineProps({
    title: {
        type:     String,
        required: true
    }
});

const isOpen = defineModel('isOpen', {
    type:     Boolean,
    required: true
});

const innerElement = useTemplateRef('innerEl');

watch([isOpen, innerElement], () => {
    if (innerElement.value == null) {
        isOpen.value = false;
        return;
    }

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
.aside-modal {
    --aside-padding-x: 12px;
    --aside-padding-y: 15px;

    anchor-scope: --content;
    isolation:    isolate;

    padding:
        var(--aside-padding-y)
        var(--aside-padding-x);

    display:        flex;
    flex-direction: column;
    max-width:      min(430px, calc(100dvw - 4 * var(--vp-margin)));
    max-height:     80dvh;
    overflow:       visible;
    position:       relative;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    &:not([open]) {
        display: none;
    }
}

.knobs {
    position:       absolute;
    inset:          0;
    pointer-events: none;

    &::before,
    &::after {
        content:  '';
        display:  block;
        position: absolute;

        width:  27px;
        height: 23px;

        background:    var(--col-green-4);
        border:        var(--border-s);
        border-radius: var(--radius-s);
    }

    &.knobs--left {
        &::before { inset: -6px auto auto -6px; border-radius: var(--radius-s) 0 var(--radius-s) 0; }
        &::after  { inset: auto auto -6px -6px; border-radius: 0 var(--radius-s) 0 var(--radius-s); }
    }
    &.knobs--right {
        &::before { inset: -6px -6px auto auto; border-radius: 0 var(--radius-s) 0 var(--radius-s); }
        &::after  { inset: auto -6px -6px auto; border-radius: var(--radius-s) 0 var(--radius-s) 0; }
    }
}

.header {
    --cantilever: calc(4 * var(--shadow-dist-m));

    align-self:  start;

    height: 1.4em;

    margin: 0 calc(-1 * var(--aside-padding-x) - var(--cantilever));
    padding: 
        0
        33px   
        0
        calc(var(--aside-padding-x) + var(--cantilever) - var(--shadow-dist-m));
    
    margin-top: -6px;
    margin-bottom: -18px;

    z-index:     0;
    position:    relative;
    display:     flex;
    align-items: center;
    gap:         12px;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.7rem;
    line-height:         1;

    border:        var(--border-s);
    border-radius: var(--radius-s);
    border-bottom-right-radius: 16px;
    corner-bottom-right-shape:  bevel;

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            rgb(0 0 0 / 0.15));
}

.content {
    contain:     none;
    container:   content / scroll-state;
    anchor-name: --content;

    text-indent: 20px;

    overflow:            auto;
    overscroll-behavior: contain;
    padding:             15px 15px;
    padding-top: 27px;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.3rem;
    line-height:         1.12;
    letter-spacing: 0.02em;

    background: white;

    border-radius: var(--radius-s);
    border:        var(--border-s);

    display:        flex;
    flex-direction: column;
    gap:            12px;

    /*
        Transparent gradient overlay on mobile to indicate content
        is scrollable, currently doesn't work on Safari.
    */
    @supports (anchor-name: --a) {
        @container content style(--scrollbar-width < 1px) {
            &::after,
            &::before {
                content:    '';
                visibility: hidden;

                position:        fixed;
                position-anchor: --content;

                height: 15px;
            }

            &::after  { inset: auto        anchor(right) anchor(bottom) anchor(left); border-radius: 0 0 var(--radius-s) var(--radius-s); }
            &::before { inset: anchor(top) anchor(right) auto           anchor(left); border-radius: var(--radius-s) var(--radius-s) 0 0; }

            &::after  { background: linear-gradient(to bottom, transparent, rgb(30 20 40 / 0.15)); }
            &::before { background: linear-gradient(to top,    transparent, rgb(30 20 40 / 0.15)); }

            @container content scroll-state(scrollable: bottom) {
                &::after { visibility: visible; }
            }

            @container content scroll-state(scrollable: top) {
                &::before { visibility: visible; }
            }
        }
    }
}
</style>