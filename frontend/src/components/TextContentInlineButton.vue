<template>
    <button class="text-content-inline-button">
        <slot />
    </button>
</template>

<style>
.text-content-inline-button {
    text-decoration:       underline 3px;
    color:                 var(--col-blue-4);
    text-decoration-color: var(--col-blue-4);

    &:hover,
    &:focus {
        color:                 var(--col-blue-3);
        text-decoration-color: var(--col-blue-3);
    }
}
</style>