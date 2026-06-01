<template>
    <label class="friends-list-requests-toggle">
        <input
            class="
                toggle-input
                shdw shdw--inst-white shdw--otst-orange shdw--recessed"
            v-model="isChecked"
            type="checkbox"  />
        Show requests
    </label>
</template>

<script setup>
const isChecked = defineModel('isChecked');
</script>

<style scoped>
.friends-list-requests-toggle {
    --sz-checkbox: 24px;

    --ofst-y-txt: 0.1em;

    display:     flex;
    align-items: center;
    gap:         10px;

    -webkit-text-stroke: var(--text-stroke-s);
    font-family: var(--font-scnd);
    font-weight: bold;
    font-size:   1.6rem;

    translate: 0 var(--ofst-y-txt);
}

.toggle-input {
    appearance: none;

    display:       grid;
    place-content: center;

    width:  var(--sz-checkbox);
    height: var(--sz-checkbox);
    
    border:        var(--border-s);
    border-radius: var(--radius-s);

    background: var(--col-lt-gray-1);

    translate: 0 calc(-1 * var(--ofst-y-txt));
    
    &:checked::after {
        content: '✓';

        font-family: var(--font-main);
        font-size:   2.5rem;
        line-height: 0.5;

        translate: 0.12em -0.06em;
    }
}
</style>