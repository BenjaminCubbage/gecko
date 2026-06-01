<template>
    <div class="friends-list-search-view">
        <div class="view-arrows">
            <span class="arrow" :data-active="active === 0">&#x25B2;</span>
            <span class="arrow" :data-active="active === 1">&#x25B2;</span>
            <span class="arrow" :data-active="active === 2">&#x25B2;</span>
            <span class="arrow" :data-active="active === 3">&#x25B2;</span>
        </div>

        <div 
            class="
                view-bar
                shdw shdw--otst-orange">
            <div 
                class="bar-field
                    txtr-vert txtr-vert--lt-gray
                    shdw shdw--inst-lt-gray shdw--elevated-m">
                <div class="field-at-symbol" inert>
                    @
                </div>

                <BaseInput 
                    class="field-input" 
                    variant="no-box" 
                    placeholder="username" />
            </div>

            <button 
                class="
                    bar-btn
                    shdw shdw--inst-green shdw--elevated-m
                    txtr-vert txtr-vert--green"
                type="button">
                &#xF50D;
            </button>
        </div>
    </div>
</template>

<script setup>
import { ref }             from 'vue';
import IconFriendListArrow from './IconFriendListArrow.vue';
import BaseInput           from './BaseInput.vue';

const active = ref(0);

(function stepActive() {
    active.value = (active.value + 1) % 4;
    setTimeout(stepActive, 180);
})();
</script>

<style scoped>
.friends-list-search-view {
    --ht-bar: 37px;
    --wd-btn: 48px;

    display:   flex;
    flex-flow: column;
    gap:       6px;

    & > .view-arrows { align-self: center; }
}

.view-arrows {
    display: flex;
    gap:     6px;

    margin: 5px 0 2px;

    font-size:    2rem;
    color:        var(--col-orange-8);
    word-spacing: 0em;

    padding-inline-start: 0.1em;
}

.arrow[data-active=true] {
    translate: 0 -2px;
}

.view-bar {
    position: relative;
    display:  grid;

    grid-template:
        'at   input btn' var(--ht-bar) /
         auto 1fr   var(--wd-btn);

    border-radius: var(--radius-s);

    & > .bar-field { z-index: 1; grid-area: input; }
    & > .bar-btn   { z-index: 0; grid-area: btn; }
}

.bar-field {
    display: flex;

    padding: 0 9px;

    border: var(--border-s);
    border-radius: 
        var(--radius-s) 
        0 
        0
        var(--radius-s);
        

    -webkit-text-stroke: var(--text-stroke-s);
    font-family:         var(--font-scnd);
    font-size:           1.7rem;
    font-weight:         bold;
    line-height:         1.2;

    translate: 
        0 calc(-1 * var(--shdw-dist-elevation));

    --shdw-etc: 
        calc(-1 * var(--shadow-dist-m)) 
        calc(-1 * var(--shadow-dist-m)) 
        var(--col-orange-0);

    &:focus-visible {
        outline: none;
    }
}

.field-at-symbol {
    display:       grid;
    place-content: center;
}

.field-input {
    /* Text indent because otherwise text stroke gets clipped */
    text-indent: 3px;
}

.bar-btn {
    border: var(--border-s);
    border-radius: 
        0 
        var(--radius-s)
        var(--radius-s)
        0;

    font-family: var(--font-main);

    font-size: 3.3rem;
    line-height: 1;

    display:       grid;
    place-content: center;

    padding-bottom: 0.03em;
        
    text-shadow: 
        calc(-1 * var(--shadow-dist-xs)) calc(-1 * var(--shadow-dist-xs)) var(--col-green-1),
                  var(--shadow-dist-xs)            var(--shadow-dist-xs)  var(--col-green-5);

    margin-left: 
        calc(-1 * var(--border-thickness-s));

    translate: 
        0 calc(-1 * var(--shdw-dist-elevation));
        
    --shdw-etc: -3px -3px var(--col-orange-0);
}
</style>