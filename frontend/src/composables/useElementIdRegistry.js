import {
    inject,
    isReactive,
    isRef,
    provide,
    reactive
} from 'vue';

export function useElementIdRegistry(registryKey, definition = null) {
    if (definition && !isReactive(definition)) {
        if (isRef(definition))
            throw new Error('definition should not be a ref.');

        definition = reactive(definition);
    }

    if (definition) {
        provide(registryKey, definition);
        return definition;
    }

    return inject(registryKey);
}