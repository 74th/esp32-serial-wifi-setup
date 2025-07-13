import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import { ngrok } from 'vite-plugin-ngrok'
const NGROK_AUTH_TOKEN = process.env.NGROK_AUTH_TOKEN || '';

export default defineConfig({
  plugins: [react(),
  ngrok({
    authtoken: NGROK_AUTH_TOKEN,
  }),
  ],
})
