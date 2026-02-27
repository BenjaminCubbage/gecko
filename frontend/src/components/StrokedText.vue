<template>
    <component :is="is" class="stroked">
        <span
            class="stroked-back"
            :class="{ 'ellipses': ellipses }"
            :style="{ '-webkit-text-stroke': `${strokeThickness} ${strokeColor}` }"
            aria-hidden="true">
            <slot />
        </span>

        <span
            class="stroked-front"
            :class="{ 'ellipses': ellipses }">
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
.stroked {
    isolation: isolate;
    display:   grid;

    & > .stroked-front,
    & > .stroked-back {
        grid-area:   1 / 1;
        white-space: nowrap;
    }

    & > .stroked-front { z-index: 1; }
    & > .stroked-back  { z-index: 0; }
}

.stroked-front {
    background: inherit;
}

.ellipses {
    overflow:      hidden;
    text-overflow: ellipsis;
}

/*
    Firefox implemented this before Chrome. I actually
    prefer the ellipses, but Firefox doesn't apply the
    text stroke and so it looks bad in Moz.
*/
@supports(-moz-appearance: none) {
    .stroked-front,
    .stroked-back {
        text-overflow: "";
    }
}

.stroked-back {
    pointer-events: none;
    user-select:     none;
}
</style>