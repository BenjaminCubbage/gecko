<template>
    <div 
        class="
            text-content-border 
            txtr-diag txtr-diag--green
            shdw shdw--inst-green shdw--elevated-l">
        <div 
            class="
                inner-content
                shdw shdw--inst-gray shdw--otst-green">
            <slot></slot>
        </div>
    </div>
</template>

<style scoped>
.text-content-border {
    padding: 6px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    max-width: min(600px, calc(100cqw - (2 * var(--vp-margin))));
}

.inner-content {
    -webkit-text-stroke: var(--text-stroke-s);

    font-size:   2.2rem;
    line-height: 1;
    color:       black;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    padding: 6px 22px;

    background:
        linear-gradient(
            var(--col-lt-gray-2) 50%,
            var(--col-lt-gray-4) 50%);
}
</style>