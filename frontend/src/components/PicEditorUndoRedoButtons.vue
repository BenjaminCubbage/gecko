<template>
    <div
        class="pic-editor-undo-redo-buttons"
        v-roving-container>
        <button
            class="
                btn btn--undo
                txtr-vert txtr-vert--blue
                shdw shdw--inst-blue shdw--elevated-s"
            :aria-disabled="!canUndo"
            v-roving-item
            @pointerdown="onPointerDown('undo')"
            @pointerup="onPointerUp"
            @pointerleave="onPointerLeave"
            @contextmenu="$event.preventDefault()"
            v-interactive-attrs>
        </button>

        <button
            class="
                btn btn--redo
                txtr-vert txtr-vert--blue
                shdw shdw--inst-blue shdw--elevated-s"
            :aria-disabled="!canRedo"
            v-roving-item
            @pointerdown="onPointerDown('redo')"
            @pointerup="onPointerUp"
            @pointerleave="onPointerLeave"
            @contextmenu="$event.preventDefault()"
            v-interactive-attrs>
        </button>
    </div>
</template>

<script setup>
import { 
    onUnmounted,
    watch
} from 'vue';

const props = defineProps({
    canUndo: {
        type:     Boolean,
        required: true
    },

    canRedo: {
        type:     Boolean,
        required: true
    }
});

const emit = defineEmits([
    'undo',
    'redo'
]);

let actionType = 'undo';
let inSpamMode = false;

const spamDelay = 400;
const spamFreq  = 70;

let spamTimer = null;
let tickTimer = null;

watch(() => [ props.canUndo, props.canRedo ], () => {
    if ((!props.canUndo && actionType === 'undo') ||
        (!props.canRedo && actionType === 'redo')) {
        clearTimeout(tickTimer);
        clearTimeout(spamTimer);
    }
});

function spamTick() {
    inSpamMode = true;
    tickTimer  = setTimeout(spamTick, spamFreq);
    emit(actionType);
}

function onPointerDown(type) {
    actionType = type;
    inSpamMode = false;
    spamTimer  = setTimeout(spamTick, spamDelay);
}

function onPointerUp() {
    clearTimeout(tickTimer);
    clearTimeout(spamTimer);

    if (!inSpamMode)
        emit(actionType);
    inSpamMode = false;
}

function onPointerLeave() {
    clearTimeout(tickTimer);
    clearTimeout(spamTimer);
    inSpamMode = false;
}

onUnmounted(() => {
    clearTimeout(tickTimer);
    clearTimeout(spamTimer);
});
</script>

<style scoped>
.pic-editor-undo-redo-buttons {
    position: absolute;
    display:  flex;

    filter:
        var(--filter-hl-1)
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));

    gap: var(--_gp-btns);
}

.btn {
    width:  var(--_wd-btn);
    height: var(--_ht-btn);

    border: var(--border-s);

    font: 2.8rem/0 var(--font-main);

    -webkit-text-stroke: var(--text-stroke-xs);

    &[data-hovered]:not([aria-disabled=true]) {
        filter: var(--filter-hl-1);
    }
    

    &[data-pressed],
    &[aria-disabled=true] {
        --shdw-dist-elevation: 0px;
        translate: 
            0 var(--shadow-dist-s);
    }

    &[aria-disabled=true] {
        pointer-events: none;
        opacity:        0.6;
    }

    &::after { display: inline-block; translate: 0 -0.03em; }
    &.btn--undo::after { content: '\F516' / 'Undo'; }
    &.btn--redo::after { content: '\F516' / 'Redo'; scale: -1 1; }
}

/*
    Media switches
*/

.pic-editor-undo-redo-buttons {
    @media (width <= 700px) {
        --_wd-btn: 60px;
        --_ht-btn: 34px;
        --_gp-btns: 4px;

        inset:  auto 0 calc(-1 * var(--_ht-btn) - 24px);
        margin: 0 auto;

        justify-content: center;
        align-items:     center;

        width: calc(var(--_wd-btn) * 2 + var(--_gp-btns));

        > .btn {
            &:nth-child(1)      { border-radius: var(--radius-s) 0 0 var(--radius-s); }
            &:nth-last-child(1) { border-radius: 0 var(--radius-s) var(--radius-s) 0; }
        }

        transform-origin: 50% 0;
        scale:            0;
        transition:       scale 180ms;

        &:has(> :not([aria-disabled=true])) {
            scale:   1;
        }
    }

    /*
        Make sure to update --ht-undo-redo media query when changing
        this one.
    */
    @media (width > 700px) {
        --_wd-btn: 36px;
        --_ht-btn: 48px;
        --_gp-btns: 5px;

        inset:     6px calc(-1 * var(--_wd-btn) - 3px) auto auto;
        flex-flow: column;

        > .btn {
            &:nth-child(1)      { border-top-right-radius: var(--radius-s); }
            &:nth-last-child(1) { border-bottom-right-radius: var(--radius-s); }
        }
    }
}
</style>