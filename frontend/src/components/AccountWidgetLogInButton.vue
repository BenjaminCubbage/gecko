<template>
    <button
        class="
            user-button-log-in
            shdw shdw--inst-lt-gray shdw--elevated-s
            txtr-vert txtr-vert--lt-gray"
        :data-is-pressed="isPressed"
        aria-label="Log Out">
        <span 
            class="
                white-border
                lt-gray-border 
                txtr-vert txtr-vert--lt-gray
                shdw shdw--inst-lt-gray">
            <IconGoogleLogo height="25px" class="icon-google" />
        </span>
        <span 
            class="
                blue-border 
                txtr-vert txtr-vert--blue
                shdw shdw--inst-blue">
            Sign in with Google
        </span>
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

    contain:  paint;
    position: relative;
    overflow: clip;

    display:     flex;
    align-items: stretch;

    height: 43px;

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
            --_fx-hl: var(--filter-hl-1);
        }
    }

    &[data-is-pressed=true] {
        --shdw-dist-elevation: 0px;
        --shdw-etc: var(--shadow-aura);
    }

    &[data-is-pressed=true] {
        --_fx-hl: var(--filter-hl-1);
    }
}

.white-border {
    display:       grid;
    place-content: center;

    padding: 0 8px 0 9px;

    border-right: var(--border-s);
}

.blue-border {
    display:       grid;
    place-content: center;

    -webkit-text-stroke: var(--text-stroke-s);
    font:                2.4rem var(--font-main);
    letter-spacing:      0.02em;

    padding: 0 16px;
}
</style>