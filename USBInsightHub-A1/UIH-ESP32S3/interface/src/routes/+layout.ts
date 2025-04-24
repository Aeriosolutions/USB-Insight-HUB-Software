import type { LayoutLoad } from './$types';

// This can be false if you're using a fallback (i.e. SPA mode)
export const prerender = false;
export const ssr = false;

export const load = (async ({ fetch }) => {
	const result = await fetch('/rest/features');
	const item = await result.json();
	return {
		features: item,
		title: 'UIH Control Panel',
		github: 'Aeriosolutions/USB-Insight-HUB-Software',
		copyright: '2025 Aerio Solutions',
		appName: 'USB Insight Hub'
	};
}) satisfies LayoutLoad;
