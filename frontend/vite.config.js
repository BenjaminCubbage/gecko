import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import mkcert from 'vite-plugin-mkcert';
import path from "path";

// https://vite.dev/config/
export default defineConfig({
  plugins: [ mkcert(), vue() ],
  //    Development only:
  //    Listen on port 3000. API / Auth Server is expected to be running on 3001.
  resolve: {
    alias: {
      '@': path.resolve(import.meta.dirname, 'src')
    }
  },
  server: {
    port: 3000,
    proxy: {
      '/api': {
        target: 'https://localhost:3001',
        changeOrigin: true,
        // secure: false tolerates self-signed certificate
        secure: false
      },
      '/auth': {
        target: 'https://localhost:3001',
        changeOrigin: true,
        // secure: false allows self-signed certificate
        secure: false
      }
    }
  }
});