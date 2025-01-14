import * as universal from '../entries/pages/system/update/_page.ts.js';

export const index = 10;
let component_cache;
export const component = async () => component_cache ??= (await import('../entries/pages/system/update/_page.svelte.js')).default;
export { universal };
export const universal_id = "src/routes/system/update/+page.ts";
export const imports = ["_app/immutable/nodes/10.js","_app/immutable/chunks/utils.js","_app/immutable/chunks/scheduler.js","_app/immutable/chunks/index.js","_app/immutable/chunks/ConfirmDialog.js","_app/immutable/chunks/index3.js","_app/immutable/chunks/_commonjsHelpers.js","_app/immutable/chunks/index2.js","_app/immutable/chunks/navigation.js","_app/immutable/chunks/singletons.js","_app/immutable/chunks/stores.js","_app/immutable/chunks/SettingsCard.js","_app/immutable/chunks/alert-triangle.js","_app/immutable/chunks/await_block.js","_app/immutable/chunks/each.js","_app/immutable/chunks/Spinner.js","_app/immutable/chunks/compareVersions.js","_app/immutable/chunks/InfoDialog.js"];
export const stylesheets = [];
export const fonts = [];
