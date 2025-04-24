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
	() => import('./nodes/14'),
	() => import('./nodes/15'),
	() => import('./nodes/16')
];

export const server_loads = [];

export const dictionary = {
		"/": [2],
		"/connections": [3],
		"/connections/mqtt": [4],
		"/connections/ntp": [5],
		"/panel": [6],
		"/settings": [7],
		"/system": [8],
		"/system/about": [9],
		"/system/metrics": [10],
		"/system/status": [11],
		"/system/update": [12],
		"/user": [13],
		"/wifi": [14],
		"/wifi/ap": [15],
		"/wifi/sta": [16]
	};

export const hooks = {
	handleError: (({ error }) => { console.error(error) }),
};

export { default as root } from '../root.svelte';