<template>
    <button class="
        pic-editor-log-in-button
        shdw shdw--inst-green shdw--elevated-s
        txtr-vert txtr-vert--green">
        Log In
    </button>
</template>

<style scoped>
.pic-editor-log-in-button {
    letter-spacing: 0.05em;
    
    margin-top: var(--shadow-dist-s);
    font-size: 2.4rem;
    border: 3px solid black;
    line-height: 0.8;
    padding: 4px 16px;
    padding-bottom: 5px;
    border-radius: var(--radius-s);

    -webkit-text-stroke: var(--text-stroke-s);

    translate: 0 calc(var(--shadow-dist-s) * -1);
}
</style>