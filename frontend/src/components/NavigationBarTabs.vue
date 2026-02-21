<template>
    <div class="navbar-tabs">
        <button
            v-for="tab in tabs"
            class="tab tab--green"
            :class="[
                { 'tab--active': selectedTab === tab.value },
                ...tab.texture
            ]"
            @click="selectedTab = tab.value"
            :key="tab.value">
            <StrokedText>{{ tab.title }}</StrokedText>
        </button>
    </div>
</template>

<script setup>
import StrokedText from './StrokedText.vue';

const selectedTab = defineModel({ default: 'canvas' });

const tabs = [
    { title: 'CANVAS',  value: 'canvas',  texture: ['tab--green',  'txtr-diag', 'txtr-diag--green' ] },
    { title: 'FRIENDS', value: 'friends', texture: ['tab--orange', 'txtr-diag', 'txtr-diag--orange'] }
];
</script>

<style scoped>
.navbar-tabs {
    display:      flex;
    flex-flow:    row nowrap;
    gap:          10px;

    font-family:  var(--font-heading);
    font-size:    2.2rem;
    line-height:  1.4;
}

.tab {
    padding: 0px 20px;
    width:   116px;

    --tab-offset: 0px;

    --tab-aura: 0 0;
    --tab-shadow:
        calc(var(--shadow-dist-s) - var(--tab-offset))
        calc(var(--shadow-dist-s) - var(--tab-offset)) 0 black;

    box-shadow:
        var(--tab-shadow),
        inset  3px  3px 0 var(--tab-inset-col-top),
        inset -3px -3px 0 var(--tab-inset-col-bottom),
        var(--tab-aura);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transition:
        transform  100ms ease,
        box-shadow 100ms ease;

    transform: translate(var(--tab-offset), var(--tab-offset));

    corner-shape: notch;
}

.tab--green {
    --tab-inset-col-bottom: var(--col-green-5);
    --tab-inset-col-top:    var(--col-green-0);
}

.tab--orange {
    --tab-inset-col-bottom: var(--col-orange-6);
    --tab-inset-col-top:    var(--col-orange-0);
}

.tab:hover {
    --tab-offset: calc(var(--shadow-dist-s) / 2);
}

.tab--active,
.tab--active:hover {
    --tab-aura:   var(--shadow-aura);
    --tab-offset: var(--shadow-dist-s);
}
</style>