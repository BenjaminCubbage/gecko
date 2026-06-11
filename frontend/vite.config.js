import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import mkcert from 'vite-plugin-mkcert';
import path from "path";

// https://vite.dev/config/
export default {
    plugins: [mkcert(), vue({
        template: {
            compilerOptions: {
                whitespace: 'preserve'
            }
        }
    })],
    resolve: {
        alias: {
            '@': path.resolve(import.meta.dirname, 'src')
        }
    },
    envDir: './env',
    server: {
        port: 3000
    }
};