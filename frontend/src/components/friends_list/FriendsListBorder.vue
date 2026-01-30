<template>
    <div class="friends-list-border">
        <div class="tab">
            <StrokedText>{{ title }}</StrokedText>
        </div>

        <div
            class="border-outer txtr-spiky txtr-spiky--orange"
            style="--spiky-spikiness: 5%">
            <div class="border-inner">
                <slot />
            </div>
        </div>
    </div>
</template>

<script setup>
import StrokedText from '@/components/stroked_text/StrokedText.vue';

defineProps({
    title: { type: String, required: true }
});
</script>

<style scoped>
.friends-list-border {
    align-self:  start;
    display:     grid;
    z-index:     0;
    font-family: var(--font-heading);
}

.tab {
    justify-self:  center;
    margin-bottom: -3px;
    margin-top:    24px;
    padding:       1px 20px 1px 23px;
    position:      relative;

    font-size:      2.3rem;
    letter-spacing: 0.08em;
    text-transform: uppercase;

    border-radius:
        var(--radius-l)
        var(--radius-l)
        0 0;

    background:    var(--col-orange-1);
    border:        var(--border-s);
    border-bottom: 0;
    box-shadow:    inset 0 2px 0 white;
    corner-shape:  bevel;
}

/*
    Shadow
*/
.tab::before {
    content: "";

    inset:    -3px;
    position: absolute;
    z-index:  -1;

    background:    black;
    border-radius: var(--radius-l);
    border:        var(--border-s);
    box-shadow:    var(--shadow-s);
    corner-shape:  bevel;
}

.border-outer {
    padding:       10px;
    border-radius: var(--radius-l);
    border:        var(--border-s);
    box-shadow:    var(--shadow-l);
    corner-shape:  bevel;
}

.border-inner {
    display:   flex;
    flex-flow: column nowrap;
    overflow:  hidden;

    background:    white;
    border-radius: var(--radius-s);
    border:        var(--border-s);
    box-shadow:    var(--shadow-s);
    corner-shape:  bevel;
}
</style>