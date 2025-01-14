import * as universal from '../entries/pages/demo/_page.ts.js';

export const index = 6;
let component_cache;
export const component = async () => component_cache ??= (await import('../entries/pages/demo/_page.svelte.js')).default;
export { universal };
export const universal_id = "src/routes/demo/+page.ts";
export const imports = ["_app/immutable/nodes/6.js","_app/immutable/chunks/utils.js","_app/immutable/chunks/scheduler.js","_app/immutable/chunks/index.js","_app/immutable/chunks/index2.js","_app/immutable/chunks/index3.js","_app/immutable/chunks/navigation.js","_app/immutable/chunks/singletons.js","_app/immutable/chunks/SettingsCard.js","_app/immutable/chunks/info-circle.js","_app/immutable/chunks/socket.js","_app/immutable/chunks/_commonjsHelpers.js"];
export const stylesheets = [];
export const fonts = [];
