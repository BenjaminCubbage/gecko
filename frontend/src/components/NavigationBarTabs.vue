<template>
    <div class="navbar-tabs">
        <button
            v-for="tab in tabs"
            class="tab"
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
    { title: 'CANVAS',  value: 'canvas',  texture: ['txtr-diag', 'txtr-diag--green' ] },
    { title: 'FRIENDS', value: 'friends', texture: ['txtr-diag', 'txtr-diag--orange'] }
];
</script>

<style scoped>
.navbar-tabs {
    display:      flex;
    flex-flow:    row nowrap;
    gap:          10px;
    font-family:  var(--font-heading);
    font-size:    2rem;
}

.tab {
    padding: 0px 20px;
    width:   116px;

    --tab-offset: 0px;
    --tab-shadow:
        calc(var(--shadow-dist-s) - var(--tab-offset))
        calc(var(--shadow-dist-s) - var(--tab-offset)) 0 black;

    border:        var(--border-s);
    border-radius: 999px;
    box-shadow:    var(--tab-shadow);

    transition:
        transform  100ms ease,
        box-shadow 100ms ease;

    transform: translate(var(--tab-offset), var(--tab-offset));
}

.tab:hover {
    --tab-offset: calc(var(--shadow-dist-s) / 2);
}

.tab--active,
.tab--active:hover {
    --tab-offset: var(--shadow-dist-s);
    box-shadow:
        var(--tab-shadow),
        var(--shadow-aura);
}
</style>