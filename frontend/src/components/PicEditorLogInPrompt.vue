<template>
    <div class="
        pic-editor-log-in-prompt
        shdw shdw--elevated-s">

        <div class="knobs knobs--left  shdw-after shdw-after--inst-green shdw-after--elevated-s shdw-before shdw-before--inst-green"></div>
        <div class="knobs knobs--right shdw-after shdw-after--inst-green shdw-after--elevated-s shdw-before shdw-before--inst-green"></div>

        <div class="header">
            <div class="icon">
                <IconSDCard class="icon-sd-card" />
                <IconInfo class="icon-info" />
            </div>

            <h1 class="title">
                Log in to send images!
            </h1>
        </div>
        <div class="separator"></div>
        <div class="bottom">
            <PicEditorLogInButton class="pic-editor-log-in-button" />
            <div class="or">or</div>
            <PicEditorDismissButton></PicEditorDismissButton>
        </div>
    </div>
</template>

<script setup>
import IconInfo               from './IconInfo.vue';
import IconSDCard             from './IconSDCard.vue';
import PicEditorLogInButton   from './PicEditorLogInButton.vue';
import PicEditorDismissButton from './PicEditorDismissButton.vue';
</script>

<style scoped>
.pic-editor-log-in-prompt {
    position: relative;
    filter: drop-shadow(3px 3px rgb(0 0 0 / 0.15));

    padding: 12px 18px;

    background: var(--col-lt-gray-4);

    border: var(--border-s);
    border-radius: var(--radius-s);

    isolation: isolate;

    --shdw-etc:
        inset  3px  3px var(--col-lt-gray-0),
        inset -3px -3px var(--col-gray-4);

    display:        flex;
    flex-direction: column;
    gap:            12px;
    
    & > .header { z-index: 1; }
    & > .border { z-index: 0; }
}

.knobs {
    position: absolute;
    inset: 0;
    pointer-events: none;

    &::before,
    &::after {
        content: '';
        display: block;

        width:  21px;
        height: 18px;

        position: absolute;

        background:    var(--col-green-4);
        border:        var(--border-s);
        border-radius: var(--radius-s);
    }

    &.knobs--left {
        &::before { inset: -6px auto auto -6px; border-radius: var(--radius-s) 0 var(--radius-s) 0; }
        &::after  { inset: auto auto -6px -6px; border-radius: 0 var(--radius-s) 0 var(--radius-s); } 
    }
    &.knobs--right {
        &::before { inset: -6px -6px auto auto; border-radius: 0 var(--radius-s) 0 var(--radius-s); }
        &::after  { inset: auto -6px -6px auto; border-radius: var(--radius-s) 0 var(--radius-s) 0; } 
    }
}

.header {
    isolation: isolate;

    display: flex;
    gap: 8px;

    height: 46px;

    & > .title { z-index: 0; grid-area: 1 / 1; place-self: center left; }
    & > .icon  { z-index: 1; grid-area: 1 / 1; place-self: center left; }
}

.separator {
    height: calc(var(--shadow-dist-m) * 2);
    background:
        linear-gradient(
            var(--col-lt-gray-0) 50%,
            var(--col-gray-4) 50%);
}

.title {
    -webkit-text-stroke: var(--text-stroke-s);
    font-size: 2.7rem;
    line-height: 1;

    &::after {
        z-index: -1;
        content: '';

        border-bottom: 3px solid var(--col-gray-2);
        border-top: 3px solid var(--col-lt-gray-0);
        position: absolute;
        inset: -4px;
        right: -15px;
        left: -68px;
        background: var(--col-lt-gray-2);
    }
}

.icon {
    display: grid;

    top:    0;
    bottom: 0;
    
    filter: brightness(1.1) drop-shadow(2px 2px var(--col-lt-gray-5));

    place-items: center;

    & > .icon-sd-card {
        grid-area: 1 / 1;
        height: 46px;
    }

    & > .icon-info {
        grid-area: 1 / 1;
        height: 23px;
        translate: -3px -4px;
    }
}

.bottom {
    display: grid;
    grid-template:
        "left middle right" auto /
         1fr  auto   1fr;

    justify-content: center;
    gap: 14px;

    align-items: center;

    margin-bottom: 4px;

    border-radius: 0 0 var(--radius-s) var(--radius-s);
}

.pic-editor-log-in-button {
}

.or {
    font-size: 2rem;
    color: var(--col-gray-5);

    border: 2.5px solid var(--col-gray-4);
    line-height: 0.7;
    padding-bottom: 1.5px;
    border-left: 0;
    border-right: 0;
}
</style>