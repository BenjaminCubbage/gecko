<template>
    <menu 
        class="friends-list-mode-tabs"
        v-roving-container
        v-roving-home="mode === 'list' ? 0 : 1">
        <li>
            <button 
                class="tab tab--list"
                :data-is-selected="mode === 'list'"
                v-roving-item
                @click="mode = 'list'">
                <span class="tab-icon shdw shdw--inst-orange"></span>
                <span class="tab-text tab-text--friends shdw shdw--inst-orange txtr-vert txtr-vert--orange">
                    Friends
                </span>
            </button>
        </li>

        <li>
            <button
                class="tab tab--search"
                :data-is-selected="mode === 'search'"
                v-roving-item
                @click="mode = 'search'">
                <span class="tab-icon shdw shdw--inst-green"></span>
                <span class="tab-text tab-text--search shdw shdw--inst-green txtr-vert txtr-vert--green">
                    Search
                </span>
            </button>
        </li>
    </menu>
</template>

<script setup>
const mode = defineModel('mode', {
    type:     String,
    required: true,
    validator(value) {
        return [
            'list',
            'search'
        ].includes(value);
    }
});
</script>

<style scoped>
.friends-list-mode-tabs {
    --sz-tab:  35px;
    --pd-tabs: calc((hypot(var(--sz-tab), var(--sz-tab)) - var(--sz-tab)) / 2);

    container: friends-list-mode-tabs / inline-size;

    isolation: isolate;

    display: flex;
    gap:     18px;

    padding: var(--pd-tabs);

    > li {
        list-style: none;
    }
}

.tab {
    display:     flex;
    align-items: center;
    gap:         8px;

    filter: 
        var(--_fx-elevation,)
        drop-shadow(
            var(--shadow-dist-s)
            var(--shadow-dist-s)
            var(--col-shadow-alpha))
        var(--_fx-hl,);

    --_fx-elevation:
        drop-shadow(
            0
            var(--shadow-dist-s)
            black);

    &.tab--list   { --col-grad-1: var(--col-orange-3); --col-grad-2: var(--col-orange-4); --col-shdw-1: var(--col-orange-1); --col-shdw-2: var(--col-orange-5); }
    &.tab--search { --col-grad-1: var(--col-green-2);  --col-grad-2: var(--col-green-4);  --col-shdw-1: var(--col-green-1);  --col-shdw-2: var(--col-green-5); }

    &.tab--list   > .tab-icon::before { content: '\F50E'; }
    &.tab--search > .tab-icon::before { content: '\F50D'; }

    > .tab-icon { z-index: 1; }
    > .tab-text { z-index: 0; }

    @media (hover: hover) {
        &:hover,
        &[data-is-selected=true] {
            --_fx-hl: var(--filter-hl-1);
        }
    }

    &[data-is-selected=true] {
        translate:   0 var(--shadow-dist-xs);
        --_fx-elevation: unset;
        
        > .tab-icon,
        > .tab-text {
            --shdw-etc: var(--shadow-aura);
        }
    }
}

.tab-icon {
    writing-mode: vertical-rl;

    font-size:   34px;
    height:      1ex;
    line-height: 1ex;
    user-select: none;
    
    translate: 0.01em -0.035em;
    rotate:    45deg;
    
    text-shadow: 
        calc(-1 * var(--shadow-dist-xs)) calc(-1 * var(--shadow-dist-xs)) var(--col-shdw-1),
                  var(--shadow-dist-xs)            var(--shadow-dist-xs)  var(--col-shdw-2);
                        
    display:       grid;
    place-content: center;

    width:  var(--sz-tab);
    height: var(--sz-tab);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    border-top-right-radius: 8px;
    corner-top-right-shape:  bevel;
    
    background:
        linear-gradient(to bottom right,
            var(--col-grad-1) 50%,
            var(--col-grad-2) 50%);

    &::before {
        rotate: -45deg;
    }
}

.tab-text {
    padding: 3px 8px;

    border: var(--border-s);
    border-radius: 
        0 
        var(--radius-s) 
        var(--radius-s) 
        0;

    margin-left:  -24px;
    padding-left: 22px;

    -webkit-text-stroke: var(--text-stroke-xs);
    font-size:      2.2rem;
    text-transform: uppercase;
    letter-spacing: 0.025em;

    @container friends-list-mode-tabs (width > 400px) {
        &.tab-text--friends::before { content: 'My '    / ''; }
        &.tab-text--search::before  { content: 'Add / ' / ''; }
    }
}
</style>