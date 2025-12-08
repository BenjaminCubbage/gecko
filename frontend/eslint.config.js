import js from "@eslint/js";
import globals from "globals";
import pluginVue from "eslint-plugin-vue";
import { defineConfig, globalIgnores } from "eslint/config";

export default defineConfig([
    globalIgnores([ 'public/wasm/*' ]),
    {
        files: ["**/*.{js,mjs,cjs,vue}"], 
        plugins: { js },
        extends: ["js/recommended"], 
        languageOptions: { 
            globals: {
                ...globals.browser,
                "Module": true // WASM
            }
        },
        rules: {
            "prefer-const": "error",
            "indent": ["error", 4]
        }
    },
    pluginVue.configs["flat/essential"],
]);
