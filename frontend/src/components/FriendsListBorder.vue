<template>
    <div class="friends-list-border">
        <div class="tab">
            <StrokedText>{{ title }}</StrokedText>
        </div>

        <div class="border-outer">
            <div class="border-inner">
                <slot />
            </div>
        </div>
    </div>
</template>

<script setup>
import StrokedText from './StrokedText.vue';

defineProps({
    title: { type: String, required: true }
});
</script>

<style scoped>
.friends-list-border {
    align-self:  start;
    display:     grid;
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
        var(--radius-s)
        var(--radius-s)
        0 0;

    box-shadow:
        inset  3px 3px 0 var(--col-orange-0),
        inset -3px 0   0 var(--col-orange-3),
        var(--shadow-s);

    background:    var(--col-orange-2);
    border:        var(--border-s);
    border-bottom: 0;
    corner-shape:  notch;
}

/*
    Make tab seamless
*/
.tab::before {
    content: "";

    bottom:   -3px;
    height:   calc(var(--border-thickness-s) + 4.5px);
    left:     0;
    position: absolute;
    right:    0;
    z-index:  10;

    background: var(--col-orange-2);
}

.border-outer {
    padding: 10px;
    z-index: 1;

    background:
        linear-gradient(to bottom,
            var(--col-orange-2) 50%,
            var(--col-orange-4) 50%);

    box-shadow:
        var(--shadow-l),
        inset  3px  3px 0 var(--col-orange-0),
        inset -3px -3px 0 var(--col-orange-6);

    border-radius: var(--radius-s);
    border:        var(--border-s);
    corner-shape:  notch;
}

.border-inner {
    display:   flex;
    flex-flow: column nowrap;
    overflow:  hidden;

    background:    white;
    border-radius: var(--radius-s);
    border:        var(--border-s);
    box-shadow:    var(--shadow-s);
    corner-shape:  notch;
}
</style>