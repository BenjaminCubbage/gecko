<template>
    <div 
        role="menu"
        class="
            friends-list-page-select
            shdw shdw--otst-orange">
        <button
            class="
                select-btn select-btn--left
                shdw shdw--inst-orange shdw--elevated-m
                txtr-diag txtr-diag--orange"
            :disabled="!hasPrev"
            aria-label="Next page"
            tabindex="-1"
            @click="pagePrev">
        </button>

        <span
            class="
                select-display
                txtr-vert txtr-vert--lt-gray
                shdw shdw--inst-lt-gray shdw--elevated-m"
            :aria-valuenow="selectedPage + 1"
            :aria-valuemin="1"
            :aria-valuemax="pageCount"
            :aria-label="`Page ${selectedPage + 1} of ${pageCount}`"
            tabindex="0"
            @keydown.right="pageNext"
            @keydown.left="pagePrev">
            {{ selectedPage + 1 }} / {{ pageCount }}
        </span>

        <button
            class="
                select-btn select-btn--right
                shdw shdw--inst-orange shdw--elevated-m
                txtr-diag txtr-diag--orange"
            :disabled="!hasNext"
            aria-label="Previous page"
            tabindex="-1"
            @click="pageNext">
        </button>
    </div>
</template>

<script setup>
import {
    computed,
    watch
} from 'vue';

const props = defineProps({
    pageCount: {
        type: Number,
        validator(value) {
            return value >= 1;
        }
    }
});

/* Zero-indexed */
const selectedPage = defineModel('selectedPage', {
    required: true,
    validator(value, props) {
        return value < props.pageCount;
    }
});

watch(() => props.pageCount, newValue =>
    selectedPage.value = Math.min(selectedPage.value, newValue - 1), 
    { immediate: true });

const hasPrev = computed(() => selectedPage.value > 0);
const hasNext = computed(() => selectedPage.value < props.pageCount - 1);

function pagePrev() { selectedPage.value = Math.max(selectedPage.value - 1, 0); }
function pageNext() { selectedPage.value = Math.min(selectedPage.value + 1, props.pageCount - 1); }
</script>

<style scoped>
.friends-list-page-select {
    --ht-page-select: 34px;
    --wd-page-select: 160px;
    --wd-nav-btn:     40.8px;

    display: grid;
    grid-template:
        "left              display right" 1fr /
         var(--wd-nav-btn) 1fr     var(--wd-nav-btn);

    height: var(--ht-page-select);
    width:  var(--wd-page-select);

    border-radius: var(--radius-s);

    > .select-btn--left  { z-index: 2; grid-area: left; }
    > .select-display    { z-index: 1; grid-area: display; }
    > .select-btn--right { z-index: 0; grid-area: right; }

    > * {
        --shdw-etc:
            calc(-1 * var(--shadow-dist-m))
            calc(-1 * var(--shadow-dist-m))
            var(--col-orange-0);
    }
}

.select-btn {
    display:       grid;
    place-content: center;

    border: var(--border-s);

    font-size:   2rem;
    font-weight: bold;

    text-shadow:
        calc(-1 * var(--shadow-dist-xs)) calc(-1 * var(--shadow-dist-xs)) var(--col-orange-0),
                  var(--shadow-dist-xs)            var(--shadow-dist-xs)  var(--col-orange-5);

    translate:
        0 calc(-1 * var(--shdw-dist-elevation));

    @media (hover: hover) {
        &:hover,
        &:active {
            filter: var(--filter-hl-1);
        }
    }

    &:active {
        --shdw-dist-elevation: 0px;
    }

    &:disabled::after {
        opacity: 0.5;
    }

    &.select-btn--left::after  { content: '<'; }
    &.select-btn--right::after { content: '>'; }

    &.select-btn--left  { margin-right: calc(-1 * var(--border-thickness-s)); border-radius: var(--radius-s) 0 0 var(--radius-s); }
    &.select-btn--right { margin-left:  calc(-1 * var(--border-thickness-s)); border-radius: 0 var(--radius-s) var(--radius-s) 0; }
}

.select-display {
    display:       grid;
    place-content: center;

    border: var(--border-s);

    -webkit-text-stroke: var(--text-stroke-s);
    font:                bold 2rem var(--font-main);
    word-spacing:        0.1em;
    user-select:         none;

    translate:
        0 calc(-1 * var(--shdw-dist-elevation));

    &:focus-visible {
        z-index: 3;
    }
}
</style>