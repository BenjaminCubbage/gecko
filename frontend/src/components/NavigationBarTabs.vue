<template>
    <nav class="navbar-tabs">
        <button
            v-for="tab in tabs"
            class="tab tab--green"
            :class="[
                { 'tab--active': selectedTab === tab.value },
                ...tab.classes
            ]"
            @click="selectedTab = tab.value"
            :key="tab.value">
            {{ tab.title }}
        </button>

        <div class="pad txtr-diag txtr-diag--lt-gray"></div>
    </nav>
</template>

<script setup>
const selectedTab = defineModel({
    type:     String,
    required: true,
    validator(value) {
        return [
            'canvas',
            'friends'
        ].includes(value);
    }
});

const tabs = [
    { title: 'CANVAS',  value: 'canvas',  classes: ['tab--left',  'tab--green',  'txtr-diag', 'txtr-diag--green' ] },
    { title: 'FRIENDS', value: 'friends', classes: ['tab--right', 'tab--orange', 'txtr-diag', 'txtr-diag--orange'] }
];
</script>

<style scoped>
.navbar-tabs {
    grid-template-areas:
        "tab-left tab-right"
        "pad      pad";

    display: grid;

    font-family:  var(--font-heading);
    font-size:    2.2rem;
    line-height:  1.3;
}

.tab {
    padding: 0px 20px;
    width:   116px;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;

    --tab-offset: 0px;

    --tab-aura: 0 0;
    --tab-shadow:
        0 calc(var(--shadow-dist-s) - var(--tab-offset))
        0 black;

    box-shadow:
        var(--tab-shadow),
        inset  3px  3px 0 var(--tab-inset-col-top),
        inset -3px -3px 0 var(--tab-inset-col-bottom),
        var(--tab-aura);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transition:
        box-shadow 80ms ease,
        translate  80ms ease;

    translate: 0 var(--tab-offset);

    corner-shape: notch;
    paint-order:  stroke;
}

.tab--left {
    grid-area:                  tab-left;
    border-bottom-right-radius: 0;
    border-top-right-radius:    0;
}

.tab--right {
    grid-area:                  tab-right;
    border-bottom-left-radius: 0;
    border-top-left-radius:    0;
}

.tab--green {
    --tab-inset-col-bottom: var(--col-green-5);
    --tab-inset-col-top:    var(--col-green-0);
}

.tab--orange {
    --tab-inset-col-bottom: var(--col-orange-6);
    --tab-inset-col-top:    var(--col-orange-0);
}

.tab:hover,
.tab:active {
    filter: var(--filter-hl-1);
}

.tab--active,
.tab--active:hover {
    --tab-offset: var(--shadow-dist-s);
    pointer-events: none;
}

.pad {
    margin-left:  -8px;
    margin-right: -8px;
    margin-top: -28px;

    grid-area: pad;
    height:    38px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    box-shadow:
        0 var(--shadow-dist-s)
        0 black,
        inset  3px  3px var(--col-gray-0),
        inset -3px -3px var(--col-gray-4);

    corner-shape: notch;
}
</style>