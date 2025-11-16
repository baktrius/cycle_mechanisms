import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'

// https://vite.dev/config/
export default defineConfig(({ mode }) => {
  // Allow overriding the base path for GitHub Pages deployments
  // e.g. set VITE_BASE="/cycle_mechanisms/" during CI for project pages
  const base = process.env.VITE_BASE || '/'

  return {
    base,
    plugins: [
      vue(),
      vueDevTools(),
    ],
  }
})
