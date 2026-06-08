<template>
    <button
        class="user-button-log-in shdw shdw--elevated-s"
        :data-pressed="isPressed"
        aria-label="Log Out">
        <IconGoogleLogo height="20px" class="icon-google" />
    </button>
</template>

<script setup>
import IconGoogleLogo from './IconGoogleLogo.vue';

defineProps({
    isPressed: {
        type:     Boolean,
        required: true
    }
});

const isPressed = defineModel('isPressed', {
    required: true,
    type:     Boolean
});
</script>

<style scoped>
.user-button-log-in {
    --elevation-dist: var(--shadow-dist-m);
    --hl:             brightness(1);
    --aura:           drop-shadow(0 0 #0000);

    background: white;

    height: 35px;

    padding: 0 14px;

    zoom: 1.2;

    -webkit-text-stroke: var(--text-stroke-s);
    font-family:         var(--font-main);
    font-size:           3rem;
    letter-spacing:      0.02em;
    line-height:         0;

    text-transform: uppercase;
    
    border:        2px solid black;
    border-radius: 8px;
    corner-shape:  squircle;

    --shdw-etc:
        var(--shadow-dist-m)
        calc(2 * var(--shadow-dist-m))
        var(--col-shadow-alpha);
    
    translate: 0 calc(var(--shadow-dist-m) - var(--elevation-dist));

    @media (hover: hover) {
        &:hover,
        &:active {
            --hl: var(--filter-hl-1);
        }
    }

    &:active,
    &[data-pressed=true] {
        --elevation-dist: 0px;
    }

    &[data-pressed=true] {
        --hl: var(--filter-hl-1);
        --aura:
            drop-shadow(     var(--shadow-aura-dist)       0 white)
            drop-shadow(calc(var(--shadow-aura-dist) * -1) 0 white)
            drop-shadow(0      var(--shadow-aura-dist)       white)
            drop-shadow(0 calc(var(--shadow-aura-dist) * -1) white);
    }
}
</style>