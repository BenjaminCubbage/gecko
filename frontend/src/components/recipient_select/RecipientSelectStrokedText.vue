<template>
    <div class="stroked">
        <div class="stroked-back">
            <slot />
        </div>

        <div class="stroked-front">
            <slot />
        </div>
    </div>
</template>

<script setup>
</script>

<style scoped>
.stroked {
    display: grid;
}

.stroked-front, .stroked-back {
    grid-area: 1/1;
    padding: 0 3px;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}

/* Firefox implemented this before Chrome. There's a
   first for everything!
   I actually prefer the ellipses, but Firefox doesn't
   apply the text stroke and so it looks bad in Moz. */
@supports(text-overflow: "") {
    .stroked-front, .stroked-back {
        text-overflow: "";
    }
}

.stroked-back {
    -webkit-text-stroke: 4px white;
}
</style>