<template>
    <button
        ref="innerEl"
        class="
            account-widget-user-drawer-toggle
            txtr-vert txtr-vert--green
            shdw shdw--inst-green shdw--elevated-s"
        :data-is-expanded="isExpanded"
        :aria-expanded="isExpanded"
        v-interactive-attrs>
        <IconUser class="icon-friend-user" />
    </button>
</template>

<script setup>
import { useTemplateRef } from 'vue';
import IconUser           from './IconUser.vue';

const props = defineProps({
    isExpanded: {
        type:     Boolean,
        required: true
    }
});

defineExpose({
    innerElement: useTemplateRef('innerEl')
});
</script>

<style scoped>
.account-widget-user-drawer-toggle {
    display: grid;

    width:  var(--user-button-wd);
    height: var(--user-button-ht);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    align-items:     end;
    justify-content: center;
    overflow-y:      clip;

    filter:
        var(--_fx-hl,)
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));

    &[data-hovered],
    &[data-is-expanded=true] {
        --_fx-hl: var(--filter-hl-2);
    }

    &[data-is-expanded=true] {
        --shdw-dist-elevation: 0;
        translate: 0 var(--shadow-dist-s);
        --shdw-etc: var(--shadow-aura);
    }
}

.icon-friend-user {
    height:     45px;
    translate:  1px 4px;
}
</style>