import globals from 'globals';
import js      from '@eslint/js';
import vue     from 'eslint-plugin-vue';
import { defineConfig, globalIgnores } from 'eslint/config';

export default defineConfig([
    globalIgnores([ 'public/wasm/*' ]),
    {
        files: ['**/*.{js,vue}'],
        plugins: { js, vue },
        extends: [
            'js/recommended',
            'vue/flat/essential'
        ],
        languageOptions: {
            globals: {
                ...globals.browser,
                'Module': true // WASM
            }
        },
        rules: {
            'prefer-const': 'error',
            'indent': ['error', 4],
            'vue/multi-word-component-names': 'off',

            'no-unused-vars': ['error', {
                'varsIgnorePattern': '^_'
            }]
        }
    }
]);
