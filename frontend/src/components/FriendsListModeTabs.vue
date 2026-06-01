<template>
    <div class="friends-list-mode-tabs">
        <button 
            class="
                tab tab--list
                shdw shdw--inst-orange"
            :data-is-selected="mode === 'list'"
            @click="mode = 'list'">
        </button>

        <button 
            class="
                tab tab--search
                shdw shdw--inst-green"
            :data-is-selected="mode === 'search'"
            @click="mode = 'search'">
        </button>
    </div>
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
    --sz-tab:  34px;
    --pd-tabs: calc((hypot(var(--sz-tab), var(--sz-tab)) - var(--sz-tab)) / 2);

    isolation: isolate;

    display:   flex;
    flex-flow: column;
    gap:       8px;

    padding: var(--pd-tabs);
}

.tab {
    display:       grid;
    place-content: center;

    width:  var(--sz-tab);
    height: var(--sz-tab);

    border:        var(--border-s);
    border-radius: var(--radius-s);
    
    rotate: 45deg;
    
    background:
        linear-gradient(to bottom right,
            var(--col-grad-1) 50%,
            var(--col-grad-2) 50%);

    filter: 
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));

    --shdw-etc: var(--shadow-dist-xs) var(--shadow-dist-xs) black;

    &::after {
        font-size:   34px;
        height:      1ex;
        line-height: 1ex;
        user-select: none;
        
        translate: 0.01em -0.035em;
        rotate:    -45deg;
        
        text-shadow: 
            calc(-1 * var(--shadow-dist-xs)) calc(-1 * var(--shadow-dist-xs)) var(--col-shdw-1),
                      var(--shadow-dist-xs)            var(--shadow-dist-xs)  var(--col-shdw-2);
    }

    &.tab--list   { --col-grad-1: var(--col-orange-3); --col-grad-2: var(--col-orange-4); --col-shdw-1: var(--col-orange-1); --col-shdw-2: var(--col-orange-5); }
    &.tab--search { --col-grad-1: var(--col-green-2);  --col-grad-2: var(--col-green-3);  --col-shdw-1: var(--col-green-1);  --col-shdw-2: var(--col-green-5); }

    &.tab--list::after   { content: '\F50E'; }
    &.tab--search::after { content: '\F50D'; }
    
    &[data-is-selected=true] {
        --shdw-etc: var(--shadow-aura);
        translate:  0 var(--shadow-dist-xs);
        filter:     var(--filter-hl-2);
    }

    &:nth-child(1)      { border-bottom-right-radius: 8px; corner-bottom-right-shape: bevel; }
    &:nth-last-child(1) { border-top-left-radius:     8px; corner-top-left-shape:     bevel; }
}
</style>