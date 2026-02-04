<template>
    <div class="toolbar-chip-layout">
        <div class="chip-icons">
            <slot name="icons"></slot>
        </div>

        <div class="chip-pad">
            <slot name="pad"></slot>
        </div>
    </div>
</template>

<style scoped>
.toolbar-chip-layout {
    display: grid;
    height:  36px;
    z-index: 0;
}

.chip-icons,
.chip-pad {
    grid-area: 1 / 1;
}

.chip-icons {
    display:          flex;
    justify-content:  space-evenly;
    place-self:       stretch;
    z-index:          1;
}

.chip-pad {
    align-self: end;
    display:    grid;
    height:     24px;
    z-index:    0;
}
</style>