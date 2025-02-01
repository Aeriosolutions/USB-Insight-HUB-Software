export { matchers } from './matchers.js';

export const nodes = [
	() => import('./nodes/0'),
	() => import('./nodes/1'),
	() => import('./nodes/2'),
	() => import('./nodes/3'),
	() => import('./nodes/4'),
	() => import('./nodes/5'),
	() => import('./nodes/6'),
	() => import('./nodes/7'),
	() => import('./nodes/8'),
	() => import('./nodes/9'),
	() => import('./nodes/10'),
	() => import('./nodes/11'),
	() => import('./nodes/12'),
	() => import('./nodes/13'),
	() => import('./nodes/14')
];

export const server_loads = [];

export const dictionary = {
		"/": [2],
		"/connections": [3],
		"/connections/mqtt": [4],
		"/connections/ntp": [5],
		"/settings": [6],
		"/system": [7],
		"/system/metrics": [8],
		"/system/status": [9],
		"/system/update": [10],
		"/user": [11],
		"/wifi": [12],
		"/wifi/ap": [13],
		"/wifi/sta": [14]
	};

export const hooks = {
	handleError: (({ error }) => { console.error(error) }),
};

export { default as root } from '../root.svelte';