<template>
    <div class="
        friends-list-view
        shdw shdw--otst-orange">
        <div class="
            view-inner
            shdw shdw--inst-white">
            <button
                v-for="(name, index) in names"
                class="
                    inner-item
                    shdw shdw--inst-orange"
                tabindex="-1"
                :data-is-selected="index === selectedIndex"
                @click="selectedIndex = index">
                <span class="item-text">
                    @{{ name }}
                </span>
            </button>
        </div>

        <div class="
            view-outer
            shdw-after  shdw-after--inst-gray
            shdw-before shdw-before--inst-gray">
            <button
                v-for="(_, index) in names"
                class="
                    outer-item
                    shdw shdw--inst-lt-gray"
                :data-is-selected="index === selectedIndex"
                @click="selectedIndex = index">
                <IconListArrow height="19px" class="item-arrow item-arrow--l" direction="right" :variant="index === selectedIndex ? 'normal' : 'inactive'" />
                <IconListArrow height="19px" class="item-arrow item-arrow--r" direction="left"  :variant="index === selectedIndex ? 'normal' : 'inactive'" />
            </button>
        </div>
    </div>
</template>

<script setup>
import { 
    ref,
    onMounted 
} from 'vue';
import IconListArrow from './IconListArrow.vue';

const names = [
    'Ben',
    'Joey',
    'Garren',
    'Bartholomew',
    'WWWWWWWWWWWWWWWWWW'
];

const selectedIndex = ref(2);
</script>

<style scoped>
.friends-list-view {
    --wd-outer-gutter: 38px;
    --ht-list-item:    34px;
    --ht-elevation:    var(--border-thickness-s);

    display: grid;
    grid-template:
        "outer-l                inner outer-r" auto /
         var(--wd-outer-gutter) 1fr   var(--wd-outer-gutter);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    & > .view-inner { z-index: 1; grid-area: inner; }
    & > .view-outer { z-index: 0; grid-area: outer-l / outer-l / outer-r / outer-r; }
}

.view-outer {
    isolation: isolate;
    position:  relative;

    display:   flex;
    flex-flow: column;

    background: var(--col-gray-2);

    border-radius: var(--radius-s);

    &::before       { z-index: 0; }
    &::after        { z-index: 0; }
    & > .outer-item { z-index: 1; }

    &::before,
    &::after {
        content:  '';
        position: absolute;

        width: var(--wd-outer-gutter);
    }

    &::before { inset: 0 auto 0 0; border-radius: var(--radius-s) 0 0 var(--radius-s); }
    &::after  { inset: 0 0 0 auto; border-radius: 0 var(--radius-s) var(--radius-s) 0; }
}

.outer-item {
    position: relative;
    height:   var(--ht-list-item);

    display:     grid;
    place-items: center;

    grid-template:
        "arrow-l                .   arrow-r" auto /
         var(--wd-outer-gutter) 1fr var(--wd-outer-gutter);

    padding: 0 1px;

    & > .item-arrow--l { grid-area: arrow-l; }
    & > .item-arrow--r { grid-area: arrow-r; }

    &:has(~ .outer-item) {
        border-bottom: 3px solid var(--col-gray-4);
    }

    &[data-is-selected=true] {
        translate:  0 calc(-1 * var(--ht-elevation));
        background: var(--col-lt-gray-4);

        --shdw-etc: 
            0 0 0 var(--border-thickness-s) black;
    }

    &:not([data-is-selected=true]) {
        box-shadow: none;
    }

    &[data-is-selected=true],
    &:has(+ .outer-item[data-is-selected=true]) {
        border-bottom-color: black;
    }

    &:nth-child(1 of .outer-item)      { border-radius: var(--radius-s) var(--radius-s) 0 0; }
    &:nth-last-child(1 of .outer-item) { border-radius: 0 0 var(--radius-s) var(--radius-s); }
}

.item-arrow {
    line-height: 0;
}

.view-inner {
    display:   flex;
    flex-flow: column;

    border:        var(--border-s);
    border-top:    0;
    border-bottom: 0;

    background: var(--col-lt-gray-1);
}

.inner-item {
    height: var(--ht-list-item);

    font-family: var(--font-scnd);
    font-size:   1.7rem;
    font-weight: bold;

    display:     grid;
    place-items: center left;

    padding: 0 12px;
    
    &:has(~ .inner-item) {
        border-bottom: 3px solid var(--col-gray-3);
    }

    &:hover {
        background: rgb(0 0 0 / 0.05);
    }

    &[data-is-selected=true] {
        background: var(--col-orange-2);

        filter: 
            drop-shadow(
                0
                var(--shadow-dist-m) 
                var(--col-shadow-alpha));

        --shdw-etc: 
            0 0 0 var(--ht-elevation) black;
        
        translate: 0 calc(-1 * var(--ht-elevation));

        & > .item-arrow > .arrow-icon {
            visibility: visible;
        }
    }

    &[data-is-selected=true],
    &:has(+ .inner-item[data-is-selected=true]) {
        border-bottom-color: black;
    }
    
    &:not([data-is-selected=true]) {
        box-shadow: none;
    }
}

.arrow-icon {
    height:      20px;
    visibility:  hidden;
}

.item-icon {
    height: 18px;
}

.item-text {
    -webkit-text-stroke: var(--text-stroke-xs);
    translate:           0 1px;
}
</style>