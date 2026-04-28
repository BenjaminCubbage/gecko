<template>
    <button
        :class="`
            navigation-bar-tab-button
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
    }
});
</script>

<style scoped>
.navigation-bar-tab-button {
    isolation: isolate;
    contain:   strict;

    display: grid;

    width:      115px;
    height:     69px;
    margin-top: 6px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    border-bottom-right-radius: 22px;
    corner-bottom-right-shape:  bevel;

    &::before {
        content:    attr(data-title);
        grid-area:  1 / 1;
        place-self: start stretch;

        margin: var(--shadow-dist-m);

        font-size:   2.5rem;
        line-height: 0.58;

        padding: 7.5px 0 6.5px 0;

        border:        var(--border-s);
        border-radius: var(--radius-s);

        --shdw-etc: 0 3px var(--col-gray-4);

        -webkit-text-stroke: var(--text-stroke-s);

        corner-shape: notch;
    }

    &::after {
        content:    '';
        grid-area:  1 / 1;
        place-self: end left;

        --h-margin: calc(var(--shadow-dist-m) * 0.99);

        box-sizing: content-box;
        height:     15px;
        margin:     0 var(--h-margin);

        width: calc(
            round(down, 100% - var(--h-margin) * 2, 9px) - 12px);
        
        border:                     var(--border-s);
        border-bottom-right-radius: var(--radius-s);

        background:
            repeating-linear-gradient(to right,
                var(--col-gold-0) 0 3px,
                var(--col-gold-0) 3px 6px,
                black           6px 9px),
            repeating-linear-gradient(to right,
                var(--col-gold-1) 0 3px,
                var(--col-gold-2) 3px 6px,
                black           6px 9px);

        background-size:     100% 3px, 100%;
        background-position: top,      center;
        background-repeat:   no-repeat;

        corner-shape: notch;
    }

    @supports not (corner-shape: notch) {
        border-bottom-right-radius: var(--radius-s);

        &::after {
            justify-self: center;

            width: calc(
                round(down, 100% - var(--h-margin) * 2, 9px) - 3px);

            border-radius: 0;
        }
    }
}

.navigation-bar-tab-button:hover,
.navigation-bar-tab-button[data-selected=true] {
    filter: var(--filter-hl-2);
}

.navigation-bar-tab-button:where(:not([data-selected=true])) {
    height:        63px;
    margin-bottom: 6px;

    &::before {
        background: var(--col-lt-gray-1);
        box-shadow: 
            inset  3px  3px var(--col-lt-gray-0),
            inset -3px -3px var(--col-lt-gray-5),
            var(--shdw-etc);
    }

    &::after {
        height: 6px;
    }
}
</style>