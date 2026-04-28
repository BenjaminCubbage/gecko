<template>
    <button class="
        pic-editor-dismiss-button
        shdw shdw--elevated-s">
        Dismiss
    </button>
</template>

<style scoped>
.pic-editor-dismiss-button {
    letter-spacing: 0.05em;
    background: var(--col-lt-gray-2);
    
    margin-top: var(--shadow-dist-s);
    font-size: 2.4rem;
    border: 3px solid black;
    line-height: 0.8;
    padding: 4px 16px;
    padding-bottom: 5px;
    border-radius: var(--radius-s);
    --shdw-etc: 
        inset  3px  3px var(--col-lt-gray-0),
        inset -3px -3px var(--col-lt-gray-5);

    -webkit-text-stroke: var(--text-stroke-s);

    translate: 0 calc(var(--shadow-dist-s) * -1);
}
</style>