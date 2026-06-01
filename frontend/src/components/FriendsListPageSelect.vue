<template>
    <div class="
        friends-list-page-select
        shdw shdw--otst-orange">
        <button 
            class="
                select-btn select-btn--left
                shdw shdw--inst-orange shdw--elevated-m
                txtr-vert txtr-vert--orange">
        </button>

        <div
            class="
                select-display
                txtr-vert txtr-vert--lt-gray
                shdw shdw--inst-lt-gray shdw--elevated-m">
            1 / 1
        </div>

        <button 
            class="
                select-btn select-btn--right
                shdw shdw--inst-orange shdw--elevated-m
                txtr-vert txtr-vert--orange">
        </button>
    </div>
</template>

<script setup>
import IconArrow from './IconArrow.vue';
</script>

<style scoped>
.friends-list-page-select {
    --ht-page-select: 34px;
    --wd-page-select: 150px;
    --wd-nav-btn:     38px;

    display: grid;
    grid-template:
        "left              display right" 1fr /
         var(--wd-nav-btn) 1fr     var(--wd-nav-btn);

    height: var(--ht-page-select);
    width:  var(--wd-page-select);

    border-radius: var(--radius-s);

    & > .select-btn--left  { z-index: 2; grid-area: left; }
    & > .select-display    { z-index: 1; grid-area: display; }
    & > .select-btn--right { z-index: 0; grid-area: right; }

    & > * {
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

    &:active {
        --shdw-dist-elevation: 0px;
        filter:                var(--filter-hl-1);
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
    font-size:           2rem;
    word-spacing:        0.2em;

    font-weight: bold;
    
    translate: 
        0 calc(-1 * var(--shdw-dist-elevation));
}
</style>