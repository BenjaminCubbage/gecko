<template>
    <button
        :class="`
            navigation-bar-tab-button
            navigation-bar-tab-button--corner-${cornerDirection}
            shdw shdw--inst-gray shdw--elevated-s
            shdw-before shdw-before--inst-${color}
            txtr-vert txtr-vert--gray
            txtr-vert-before txtr-vert-before--${color}`"
        :data-selected="isSelected"
        :data-title="title"
        :aria-label="title">
    </button>
</template>

<script setup>
const props = defineProps({
    isSelected: {
        type:     Boolean,
        required: true
    },

    color: {
        type:     String,
        required: true
    },

    title: {
        type:     String,
        required: true
    },

    cornerDirection: {
        type: String,
        required: true,
        validator(value) {
            return [
                'left',
                'right'
            ].includes(value);
        }
    }
});
</script>

<style scoped>
.navigation-bar-tab-button {
    isolation: isolate;
    contain:   strict;

    display: grid;

    width:      130px;
    height:     60px;
    margin-top: 6px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    &:where(.navigation-bar-tab-button--corner-left) {
        border-bottom-left-radius: 16px;
        corner-bottom-left-shape:  bevel;
    }
    
    &:where(.navigation-bar-tab-button--corner-right) {
        border-bottom-right-radius: 16px;
        corner-bottom-right-shape:  bevel;
    }

    &::before {
        content:    attr(data-title);
        grid-area:  1 / 1;
        place-self: start stretch;

        margin: calc(var(--shadow-dist-m) * 2);

        font-size:   2.5rem;
        line-height: 0.6;
            
        font-size: 2.7rem;

        padding: 7.5px 0 6.5px 0;

        border:        var(--border-s);
        border-radius: var(--radius-s);

        --shdw-etc: 0 3px var(--col-gray-4);

        -webkit-text-stroke: var(--text-stroke-s);

        corner-shape: notch;
    }
}

.navigation-bar-tab-button:hover,
.navigation-bar-tab-button[data-selected=true] {
    filter: var(--filter-hl-2);

    @media (hover: hover) {
        scale: 1.02;
    }

    &.navigation-bar-tab-button--corner-left {
        transform-origin: 0% 0%;
    }

    &.navigation-bar-tab-button--corner-right {
        transform-origin: 100% 0px;
    }
}

.navigation-bar-tab-button:where(:not([data-selected=true])) {
    height:        53px;
    margin-bottom: 7px;

    border-bottom-right-radius: var(--radius-s);
    corner-bottom-right-shape:  notch;

    border-bottom-left-radius: var(--radius-s);
    corner-bottom-left-shape:  notch;

    &::before {
        background: 
            linear-gradient(
                var(--col-lt-gray-1) 50%,
                var(--col-lt-gray-2) 50%
            );

        box-shadow:
            inset calc(-1 * var(--shadow-dist-m)) calc(-1 * var(--shadow-dist-m)) var(--col-gray-3),
            inset           var(--shadow-dist-m)            var(--shadow-dist-m)  var(--col-lt-gray-1);
    }
}
</style>