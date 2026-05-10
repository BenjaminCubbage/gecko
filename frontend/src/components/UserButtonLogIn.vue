<template>
    <button
        class="
            user-button-log-in
            txtr-vert txtr-vert--green
            shdw shdw--inst-green"
        :data-pressed="isPressed"
        aria-label="Log Out">
        <IconGoogle height="45px" class="icon-google" />
        Log In
    </button>
</template>

<script setup>
import IconGoogle from './IconGoogle.vue';

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

    display: flex;

    align-items: center;
    gap:         10px;
    
    padding: 0 18px 0 0;
    height:  calc(var(--user-button-ht) - 3px);

    -webkit-text-stroke: var(--text-stroke-s);
    font-family:         var(--font-main);
    font-size:           3rem;
    letter-spacing:      0.02em;

    text-transform: uppercase;
    
    border:        var(--border-s);
    border-radius: var(--radius-s);

    corner-top-left-shape:    round;
    corner-bottom-left-shape: round;

    border-top-left-radius:    20px;
    border-bottom-left-radius: 20px;
    
    filter: 
        drop-shadow(
            0
            var(--elevation-dist)
            black)
        var(--aura)
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha))
        var(--hl);

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

.icon-google {
    margin-left: -6px;
}
</style>