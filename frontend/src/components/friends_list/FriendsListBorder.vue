<template>
    <div class="friends-list-border">
        <div class="title">
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
import StrokedText from '@/components/stroked_text/StrokedText.vue';

const props = defineProps({
    title: { type: String, required: true }
});
</script>

<style scoped>
.friends-list-border {
    z-index: 0;
    display: grid;
    font-family: var(--font-heading);
    align-self: start;
}

/* Border top tab */
.title {
    position: relative;
    justify-self: center;

    font-size: 2.3rem;
    letter-spacing: 0.08em;
    text-transform: uppercase;

    margin-top: 24px;
    margin-bottom: -3px;

    background: #ffe7ca;

    padding: 1px 20px 1px 23px;
    border: var(--border-small);
    border-radius: var(--border-radius-large);
    box-shadow: inset 0 2px 0 #ffffff;

    border-bottom: 0;
    border-bottom-right-radius: 0;
    border-bottom-left-radius: 0;

    corner-shape: bevel;
}

/* Shadow */
.title::before {
    content: "";
    z-index: -1;
    position: absolute;
    inset: -3px;
    background: black;
    border: var(--border-small);
    border-radius: var(--border-radius-large);
    box-shadow: var(--border-shadow-small);

    corner-shape: bevel;
}

.border-outer {
    background: linear-gradient(-22deg,
        #ffb35a 25%,
        #ffd8a3 25% 50%,
        #ffbf73 50% 75%,
        #ffe7ca 75%
    );
    background-size: 1% 100%;
    background-repeat: repeat;

    padding: 10px;
    border: var(--border-small);
    border-radius: var(--border-radius-large);
    corner-shape: bevel;

    box-shadow: var(--border-shadow-large);
}

.border-inner {
    overflow: hidden;

    display: flex;
    flex-flow: column nowrap;
    background: white;
    border: var(--border-small);
    border-radius: var(--border-radius-small);
    box-shadow: var(--border-shadow-small);
    corner-shape: bevel;

    background: white;
}
</style>