<template>
    <button
        ref="innerElement"
        class="account-widget-drawer-toggle txtr-diag txtr-diag--green"
        :class="{ 'account-widget-drawer-toggle--toggled': isToggled }"
        @click="toggle"
        :aria-expanded="isToggled">
        <i class="user-icon hn hn-user-solid"></i>
    </button>
</template>

<script setup>
    import { useTemplateRef } from 'vue';

    const isToggled = defineModel({ 
        type: Boolean,
        required: false 
    });

    function toggle() {
        isToggled.value = !isToggled.value;
    }

    const innerElement = useTemplateRef('innerElement');

    defineExpose({
        innerElement
    });
</script>

<style scoped>
.account-widget-drawer-toggle {
    display:       grid;
    place-content: center;
    width:         48px;
    height:        46px;

    --inner-button-aura:   0 0;
    --inner-button-offset: 0px;

    box-shadow:
        var(--inner-button-aura),
        calc(var(--shadow-dist-s) - var(--inner-button-offset))
        calc(var(--shadow-dist-s) - var(--inner-button-offset))
        0 black,
        inset  3px  3px var(--col-green-0),
        inset -3px -3px var(--col-green-5);

    border-radius: var(--radius-s);
    border:        var(--border-s);

    transform:
        translate(
            var(--inner-button-offset),
            var(--inner-button-offset));

    transition:
        box-shadow 100ms  ease,
        transform  100ms ease;

    corner-shape: notch;
}

.account-widget-drawer-toggle:hover {
    --inner-button-offset: calc(var(--shadow-dist-s) / 2);
}

.account-widget-drawer-toggle--toggled,
.account-widget-drawer-toggle--toggled:hover {
    --inner-button-aura:   var(--shadow-aura);
    --inner-button-offset: var(--shadow-dist-s);
}

.user-icon {
    margin-top:  -1px;
    color:       black;
    font-size:   2.8rem;
    line-height: 1em;

    paint-order: stroke;

    filter:
        drop-shadow(0  2px var(--col-green-5))
        drop-shadow(2px 0  var(--col-green-5))
        drop-shadow(0 -2px var(--col-green-1))
        drop-shadow(-2px 0 var(--col-green-1));
}
</style>