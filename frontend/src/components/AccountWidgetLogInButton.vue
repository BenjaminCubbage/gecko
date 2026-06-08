<template>
    <button
        class="
            user-button-log-in
            shdw shdw--inst-lt-gray shdw--elevated-s
            txtr-vert txtr-vert--white"
        :data-is-pressed="isPressed"
        aria-label="Log Out">
        <IconGoogleLogo height="26px" class="icon-google" />
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
</script>

<style scoped>
.user-button-log-in {
    --elevation-dist: var(--shadow-dist-m);

    position: relative;

    display:       grid;
    place-content: center;

    height:  44px;
    padding: 0 16px 0 14px;

    -webkit-text-stroke: var(--text-stroke-s);
    font-family:         var(--font-main);
    font-size:           3rem;
    letter-spacing:      0.02em;
    line-height:         0;

    text-transform: uppercase;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    translate: 0 calc(-1 * var(--shdw-dist-elevation));

    filter:
        var(--_fx-hl,)
        drop-shadow(
            var(--shadow-dist-m) 
            var(--shadow-dist-m) 
            var(--col-shadow-alpha));

    @media (hover: hover) {
        &:hover,
        &:active {
            --_fx-hl: var(--filter-hl-0);
        }
    }

    &:active,
    &[data-is-pressed=true] {
        --shdw-dist-elevation: 0px;
        --shdw-etc: var(--shadow-aura);
    }

    &[data-is-pressed=true] {
        --_fx-hl: var(--filter-hl-1);
    }
}
</style>