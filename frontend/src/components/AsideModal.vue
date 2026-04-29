<template>
    <div class="
        pic-editor-log-in-prompt
        shdw shdw--elevated-s">
        <div inert class="knobs knobs--left  shdw-after shdw-after--inst-green shdw-after--elevated-s shdw-before shdw-before--inst-green"></div>
        <div inert class="knobs knobs--right shdw-after shdw-after--inst-green shdw-after--elevated-s shdw-before shdw-before--inst-green"></div>

        <h1
            class="header">
            <slot name="icon"></slot>
            {{ title }}
        </h1>

        <p class="content">
            <slot name="content"></slot>
        </p>

        <slot name="buttons"></slot>
    </div>
</template>

<script setup>
defineProps({
    title: {
        type:     String,
        required: true
    }
});
</script>

<style scoped>
.pic-editor-log-in-prompt {
    --aside-padding-x: 18px;
    --aside-padding-y: 18px;

    isolation: isolate;

    padding:
        var(--aside-padding-y)
        var(--aside-padding-x);

    display:        flex;
    flex-direction: column;
    gap:            10px;
    max-width:      430px;
    position:       relative;

    --shdw-etc:
        inset  3px  3px var(--col-lt-gray-0),
        inset -3px -3px var(--col-gray-4);

    background:    var(--col-lt-gray-4);
    border:        var(--border-s);
    border-radius: var(--radius-s);

    filter: drop-shadow(3px 3px rgb(0 0 0 / 0.15));
}

.knobs {
    position:       absolute;
    inset:          0;
    pointer-events: none;

    &::before,
    &::after {
        content:  '';
        display:  block;
        position: absolute;

        width:  21px;
        height: 18px;

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
    --cantilever: calc(2 * var(--shadow-dist-m));

    height: 1.3em;

    margin:  6px calc(-1 * var(--aside-padding-x) - var(--cantilever));
    padding: 0   calc(     var(--aside-padding-x) + var(--cantilever));

    z-index: 0;
    position:    relative;
    display:     flex;
    align-items: center;
    gap:         8px;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.7rem;
    line-height:         1;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    background: var(--col-lt-gray-2);
    box-shadow:
        inset calc(-1 * var(--shadow-dist-m)) calc(-1 * var(--shadow-dist-m)) var(--col-gray-3),
        inset           var(--shadow-dist-m)            var(--shadow-dist-m)  var(--col-lt-gray-1);

    /*
        Heading bar's shadow, cast over the modal but not beyond
        the cantilever.
    */
    &::after {
        content:  '';
        position: absolute;
        height:   var(--shadow-dist-m);
        z-index:  -1;

        inset:
            auto
            calc(var(--cantilever) - var(--shadow-dist-m))
            calc(0px - var(--shadow-dist-m) - var(--border-thickness-s) + 0.5px);

        background: var(--col-gray-4);
    }
}

.content {
    padding: 10px 13px;

    white-space: pre-wrap;
    font-size: 2.2rem;
    line-height: 1.1;
    -webkit-text-stroke: var(--text-stroke-s);

    background: var(--col-lt-gray-2);

    box-shadow:
        inset      var(--shadow-dist-m)            var(--shadow-dist-m)       var(--col-gray-3),
        inset calc(var(--shadow-dist-m) * -1) calc(var(--shadow-dist-m) * -1) var(--col-lt-gray-0),
                   var(--shadow-dist-m)            var(--shadow-dist-m)       var(--col-lt-gray-0),
              calc(var(--shadow-dist-m) * -1) calc(var(--shadow-dist-m) * -1) var(--col-gray-3);

    border-radius: var(--radius-s);
    border:        var(--border-s);
}
</style>