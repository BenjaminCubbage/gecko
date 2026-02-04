<template>
    <div class="friends-list-divider">
        <label class="label">
            <slot />
        </label>
    </div>
</template>

<style scoped>
    .friends-list-divider {
        display:       flex;
        height:        3px;
        margin:        8px 4px;
        place-content: center;
        place-items:   center;

        text-align: center;

        background: black;
    }

    .label {
        padding:     0 4px;
        color:       black;
        font-size:   1.9rem;
        line-height: 0.9;
        background:  white;
    }
</style>