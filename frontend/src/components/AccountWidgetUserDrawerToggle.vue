<template>
    <button 
        ref="innerEl" 
        class="
            account-widget-user-drawer-toggle
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
.account-widget-user-drawer-toggle {
    --hl: brightness(1);

    position: relative;

    display:  flex;
    position: relative;

    width:  var(--user-button-wd);
    height: var(--user-button-ht);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    justify-content: center;
    overflow-y:      clip;
            
    filter: 
        var(--hl)
        drop-shadow(
            var(--shadow-dist-m) 
            var(--shadow-dist-m) 
            var(--col-shadow-alpha));

    &:hover,
    &[data-expanded=true] {
        --hl: var(--filter-hl-2);
    }

    &[data-expanded=true] {
        --shdw-dist-elevation: 0;
        translate: 0 var(--shadow-dist-s);
        --shdw-etc: var(--shadow-aura);
    }
}

.icon-friend-user {
    align-self: end;
    height:     45px;
    translate:  1px 8px;
}
</style>