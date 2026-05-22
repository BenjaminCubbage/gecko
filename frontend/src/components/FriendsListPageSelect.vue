<template>
    <div class="friends-list-page-select">
        <div 
            class="
                select-btn select-btn--left
                shdw shdw--inst-gray shdw--elevated-s
                txtr-vert txtr-vert--gray">
            <IconArrow height="20px" direction="left" />
        </div>

        <div
            class="
                select-display
                shdw shdw--inst-gray shdw--elevated-s
                txtr-vert txtr-vert--lt-gray">
            1 / 1
        </div>

        <div 
            class="
                select-btn select-btn--right
                shdw shdw--inst-gray shdw--elevated-s
                txtr-vert txtr-vert--gray">
            <IconArrow height="20px" direction="right" />
        </div>
    </div>
</template>

<script setup>
import IconArrow from './IconArrow.vue';
</script>

<style scoped>
.friends-list-page-select {
    --ht-page-select: 38px;
    --wd-page-select: 230px;
    --wd-nav-btn:     46px;

    display: grid;
    grid-template:
        "left              display right" 1fr /
         var(--wd-nav-btn) 1fr     var(--wd-nav-btn);

    height: var(--ht-page-select);
    width:  var(--wd-page-select);

    & > .select-btn--left  { grid-area: left; }
    & > .select-display    { grid-area: display; }
    & > .select-btn--right { grid-area: right; }
}

.select-btn {
    display:       grid;
    place-content: center;

    border: var(--border-s);

    &.select-btn--right { border-radius: 0 var(--radius-s) var(--radius-s) 0; }
    &.select-btn--left  { border-radius: var(--radius-s) 0 0 var(--radius-s); }
}

.select-display {
    display:       grid;
    place-content: center;

    border: var(--border-s);

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2rem;
    word-spacing:        0.2em;

    font-weight: bold;
}
</style>