<template>
    <div class="stroked">
        <div
            class="stroked-back"
            :class="{ 'ellipses': ellipses }"
            :style="{ '-webkit-text-stroke': `${strokeThickness} ${strokeColor}` }">
            <slot />
        </div>

        <div
            class="stroked-front"
            :class="{ 'ellipses': ellipses }">
            <slot />
        </div>
    </div>
</template>

<script setup>
defineProps({
    strokeColor:     { type: String,  default: 'white' },
    strokeThickness: { type: String,  default: '3px'   },
    ellipses:        { type: Boolean, default: false   }
});
</script>

<style scoped>
.stroked {
    display: grid;
    color:   black;
}

.stroked-front,
.stroked-back {
    grid-area:   1 / 1;
    white-space: nowrap;
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