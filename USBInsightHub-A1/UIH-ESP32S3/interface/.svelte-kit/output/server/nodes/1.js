

export const index = 1;
let component_cache;
export const component = async () => component_cache ??= (await import('../entries/pages/_error.svelte.js')).default;
export const imports = ["_app/immutable/nodes/1.js","_app/immutable/chunks/utils.js","_app/immutable/chunks/scheduler.js","_app/immutable/chunks/index.js","_app/immutable/chunks/navigation.js","_app/immutable/chunks/singletons.js","_app/immutable/chunks/index3.js","_app/immutable/chunks/stores.js"];
export const stylesheets = [];
export const fonts = [];
