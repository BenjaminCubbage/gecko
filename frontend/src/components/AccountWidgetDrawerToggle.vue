<template>
    <button 
        ref="innerEl" 
        class="
            account-widget-drawer-toggle
            txtr-vert txtr-vert--green
            shdw shdw--inst-green shdw--elevated-s"
        :data-expanded="isExpanded">
        <IconFriendUser
            class="icon-friend-user"
            variant="normal" />
    </button>
</template>

<script setup>
import { useTemplateRef } from 'vue';
import IconFriendUser     from './IconFriendUser.vue';

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
.account-widget-drawer-toggle {
    display:  flex;
    position: relative;

    width:  50px;
    height: 45px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    justify-content: center;

    overflow: clip;

    &:hover,
    &[data-expanded=true] {
        filter: var(--filter-hl-1);
    }

    &[data-expanded=true] {
        --shdw-dist-elevation: 0;
        translate: 0 var(--shadow-dist-s);
        --shdw-etc: var(--shadow-aura);
    }
}

.icon-friend-user {

    align-self: end;
    height:     50px;
    translate:  1px 11px;
}
</style>