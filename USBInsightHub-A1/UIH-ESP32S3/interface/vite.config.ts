import { sveltekit } from '@sveltejs/kit/vite';
import type { UserConfig } from 'vite';
import Icons from 'unplugin-icons/vite';
import viteLittleFS from './vite-plugin-littlefs';

const config: UserConfig = {
	plugins: [
		sveltekit(),
		Icons({
			compiler: 'svelte'
		}),
		// Shorten file names for LittleFS 32 char limit
		//is necessary to enable hashing as it allows that a change in files
		//are reflected in the browser without the need to clear the cache

		//viteLittleFS() //this function deletes the hash as there is a limit of 32 chars in LittleFS
	],
	server: {
		proxy: {
			// Proxying REST: http://localhost:5173/rest/bar -> http://192.168.1.83/rest/bar
			'/rest': {
				target: 'http://192.168.2.88',
				changeOrigin: true
			},
			// Proxying websockets ws://localhost:5173/ws -> ws://192.168.1.83/ws
			'/ws': {
				target: 'ws://192.168.2.88',
				changeOrigin: true,
				ws: true
			}
		}
	}
};

export default config;
