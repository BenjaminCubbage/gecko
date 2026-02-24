import { onMounted, ref }               from 'vue';
import { getComputedCSSVar } from '@/core/dom/cssVar.js';

/*
    Attempts to load a font, and tracks whether or not the font
    is loaded yet.

    The fontFamily argument can be a css variable on :root, like
    --font-heading.
*/
export function useWaitOnFont(fontFamily, fontModifiers = '1rem') {
    const isFontLoaded = ref(false);

    onMounted(() => {
        if (fontFamily.startsWith('--'))
            fontFamily = getComputedCSSVar(fontFamily);

        const font = `${fontModifiers} ${fontFamily}`;

        if (document.fonts.check(font)) {
            isFontLoaded.value = true;
        } else {
            document.fonts
                .load(font)
                .then(() => isFontLoaded.value = true);
        }
    });

    return {
        isFontLoaded
    };
}