<template>
    <component :is="is" class="stroked-text">
        <span
            class="text text--back"
            :class="{ 'text--ellipses': ellipses }"
            :style="{ '-webkit-text-stroke': `${strokeThickness} ${strokeColor}` }"
            aria-hidden="true">
            <slot />
        </span>

        <span
            class="text text--front"
            :class="{ 'text--ellipses': ellipses }">
            <slot />
        </span>
    </component>
</template>

<script setup>
defineProps({
    is: {
        type:    String,
        default: 'div'
    },
    strokeColor:     { type: String,  default: 'white' },
    strokeThickness: { type: String,  default: '4px'   },
    ellipses:        { type: Boolean, default: false   }
});
</script>

<style scoped>
.stroked-text {
    isolation: isolate;
    display:   grid;

    & > .text {
        grid-area: 1 / 1;
    }
}

.text--back {
    pointer-events: none;
    user-select:    none;
}

.text--front {
    background: inherit;
}

.text--ellipses {
    overflow:      hidden;
    text-overflow: ellipsis;
}

/*
    Firefox implemented this before Chrome. I actually
    prefer the ellipses, but Firefox doesn't apply the
    text stroke and so it looks bad in Moz.
*/
@supports(-moz-appearance: none) {
    .text-front,
    .text-back {
        text-overflow: "";
    }
}
</style>