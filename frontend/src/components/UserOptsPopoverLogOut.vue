<template>
    <button
        ref="innerEl"
        class="
            user-button-log-out
            txtr-vert txtr-vert--red
            shdw shdw--inst-dk-red shdw--elevated-s"
        :data-pressed="isPressed"
        aria-label="Log Out">
        <span class="icon-boundary">
            <IconLogOut class="icon-log-out" />
        </span>
    </button>
</template>

<script setup>
import { useTemplateRef } from 'vue';
import IconLogOut from './IconLogOut.vue';

const props = defineProps({
    isPressed: {
        type:     Boolean,
        required: true
    }
});

defineExpose({
    innerElement: useTemplateRef('innerEl')
});
</script>

<style scoped>
.user-button-log-out {
    position: relative;

    width:  var(--user-button-wd);
    height: var(--user-button-ht);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    filter:
        var(--_fx-aura,)
        var(--_fx-hl,);

    @media (hover: hover) {
        &:hover {
            --_fx-hl: var(--filter-hl-1);
        }
    }

    &[data-pressed=true] {
        --_fx-hl: var(--filter-hl-1);
    }

    &[data-pressed=true] {
        --shdw-dist-elevation: 0;
        --_fx-aura:            var(--filter-aura-s);

        translate: 0 var(--shadow-dist-s);
    }

    /* Caret */
    &::before {
        content: '';

        position:   absolute;
        box-sizing: content-box;

        inset:
            calc(-3 * var(--border-thickness-s) - 0.5px)
            0
            auto;

        width:  calc(3 * var(--border-thickness-s));
        height: calc(2 * var(--border-thickness-s) + 1px);
        margin: 0 auto;

        border:        var(--border-s);
        border-bottom: 0;

        border-radius:
            var(--radius-s)
            var(--radius-s) 0 0;

        background: var(--col-red-0);
    }

    /* Caret flush */
    &::after {
        content: '';

        position: absolute;
        inset:    -1px 0 auto;

        width:  calc(3 * var(--border-thickness-s));
        height: 2px;
        margin: 0 auto;

        background: var(--col-red-0);
    }
}

/* Clip boundary */
.icon-boundary {
    display:    grid;
    width:      100%;
    height:     100%;
    overflow-y: clip;
}

.icon-log-out {
    height:    45px;
    translate: -6px 3px;
}
</style>