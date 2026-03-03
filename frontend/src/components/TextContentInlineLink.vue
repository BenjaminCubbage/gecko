<template>
    <a class="text-content-inline-button">
        <slot />
    </a>
</template>

<style>
.text-content-inline-button {
    text-decoration:       underline 3px;
    color:                 var(--col-blue-4);
    text-decoration-color: var(--col-blue-4);
    text-underline-offset: 3px;

    &:hover,
    &:focus {
        color:                 var(--col-blue-3);
        text-decoration-color: var(--col-blue-3);
    }
}
</style>