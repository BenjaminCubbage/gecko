<template>
    <component :is="is" class="clippable-stroked-text">
        <span
            class="text text--back"
            aria-hidden="true">
            <slot />
        </span>

        <span
            class="text text--front">
            <slot />
        </span>
    </component>
</template>

<script setup>
/*
    Text stroke and background-clip: text generally don't work together,
    but this components allows you to use both simultaneously using two
    seperate spans, one of which overlaps the other.
*/
defineProps({
    is: {
        type:    String,
        default: 'div'
    }
});
</script>

<style scoped>
.clippable-stroked-text {
    isolation:         isolate;
    display:           grid;

    -webkit-text-stroke: var(--text-stroke-s);

    /*
        Hide background on the parent component, but inherit it on
        the child.
    */
    background-repeat: no-repeat;
    background-size:   0;
}

.text {
    grid-area: 1 / 1;

    &.text--back {
        pointer-events: none;
        user-select:    none;
    }

    &.text--front {
        -webkit-text-stroke: 0 black;
        
        background-image: inherit;
        background-clip:  text;
    }
}
</style>