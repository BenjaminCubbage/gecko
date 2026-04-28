<template>
    <button
        ref="innerEl"
        class="
            drawer-button-log-out
            txtr-vert txtr-vert--dk-red
            shdw shdw--inst-dk-red shdw--elevated-s"
        :data-pressed="isPressed"
        aria-label="Log Out">
        <IconLogOut class="icon-log-out" />
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
.drawer-button-log-out {
    width:  50px;
    height: 45px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    overflow-y: clip;

    &:hover,
    &[data-pressed=true] {
        filter: var(--filter-hl-1);
    }

    &[data-pressed=true] {
        --shdw-dist-elevation: 0;
        --shdw-etc:            var(--shadow-aura);

        translate: 0 var(--shadow-dist-s);
    }
}

.icon-log-out {
    height: 45px;
    translate: -6px 3px;
}
</style>