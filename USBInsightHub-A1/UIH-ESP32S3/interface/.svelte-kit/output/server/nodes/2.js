

export const index = 2;
let component_cache;
export const component = async () => component_cache ??= (await import('../entries/pages/_page.svelte.js')).default;
export const imports = ["_app/immutable/nodes/2.js","_app/immutable/chunks/utils.js","_app/immutable/chunks/scheduler.js","_app/immutable/chunks/index.js","_app/immutable/chunks/logo.js","_app/immutable/chunks/notifications.js","_app/immutable/chunks/index3.js"];
export const stylesheets = [];
export const fonts = [];
