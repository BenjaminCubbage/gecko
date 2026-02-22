<template>
    <DrawerButton
        ref="drawerButtonEl"
        color="red"
        variant="button">
        <svg class="icon" version="1.1" viewBox="0 0 143.4 142.5">
            <path d="M48.5,79.9c2.9,0,8.7,0,11.6,0v-5.8h5.8c0-5.2,0-12.2,0-17.4h-5.8V51c-9.3,0-19.7,0-29,0c0-2.9,0-8.7,0-11.6h-5.8v5.8h-5.8
                V51h-5.8v5.8H8v5.8H2.2v5.8H8v5.8h5.8v5.8h5.8v5.8h5.8v5.8h5.8c0-2.9,0-8.7,0-11.6C36.3,79.9,43.4,79.9,48.5,79.9z"/>
            <path d="M135.4,10.4V4.7c-28.8,0-58,0-86.9,0v5.8h-5.8c0,9.3,0,19.7,0,29c2.9,0,8.7,0,11.6,0c0-5.2,0-12.2,0-17.4h5.8v-5.8
                c17.2,0,34.9,0,52.1,0V22c-5.2,0-12.2,0-17.4,0v5.8c-7.3,0-15.9,0-23.2,0c0,28.8,0,58,0,86.8c-2.9,0-8.7,0-11.6,0v-5.8h-5.8
                c0-5.2,0-12.2,0-17.4c-2.9,0-8.7,0-11.6,0c0,9.3,0,19.7,0,29h5.8v5.8c7.3,0,15.9,0,23.2,0c0,2.9,0,8.7,0,11.6c7.3,0,15.9,0,23.2,0
                V132c5.2,0,12.2,0,17.4,0v-5.8c7.3,0,15.9,0,23.2,0v-5.8h5.8c0-36.6,0-73.4,0-110H135.4z"/>
        </svg>
    </DrawerButton>
</template>

<script setup>
    import {
        computed,
        useTemplateRef
    } from 'vue';

    import DrawerButton from './DrawerButton.vue';

    const drawerButtonEl = useTemplateRef('drawerButtonEl');

    defineExpose({
        innerElement: computed(() => {
            return drawerButtonEl.value?.innerElement;
        })
    });
</script>

<style scoped>
.icon {
    width:     30px;
    translate: -2px 0px;
}
</style>