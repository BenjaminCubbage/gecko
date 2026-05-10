<template>
    <button
        ref="innerEl"
        class="
            user-button-log-out
            txtr-vert txtr-vert--red
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
.user-button-log-out {
    width:  var(--user-button-wd);
    height: var(--user-button-ht);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    overflow-y: clip;

    @media (hover: hover) {
        &:hover {
            filter: var(--filter-hl-1);
        }
    }

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