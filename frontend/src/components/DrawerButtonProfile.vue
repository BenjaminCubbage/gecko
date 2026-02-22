<template>
    <DrawerButton
        ref="drawerButtonEl"
        v-model:is-toggled="isToggled"
        color="green"
        variant="toggle">
        <i class="icon hn hn-user-solid"></i>
    </DrawerButton>
</template>

<script setup>
    import {
        computed,
        useTemplateRef
    } from 'vue';

    import DrawerButton from './DrawerButton.vue';

    const isToggled = defineModel({
        type:     Boolean,
        required: true
    });

    const drawerButtonEl = useTemplateRef('drawerButtonEl');

    defineExpose({
        innerElement: computed(() => {
            return drawerButtonEl.value?.innerElement;
        })
    });
</script>

<style scoped>
.icon {
    margin-top: -1px;
}
</style>