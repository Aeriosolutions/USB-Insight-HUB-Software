import * as universal from '../entries/pages/_layout.ts.js';

export const index = 0;
let component_cache;
export const component = async () => component_cache ??= (await import('../entries/pages/_layout.svelte.js')).default;
export { universal };
export const universal_id = "src/routes/+layout.ts";
export const imports = ["_app/immutable/nodes/0.js","_app/immutable/chunks/utils.js","_app/immutable/chunks/scheduler.js","_app/immutable/chunks/index.js","_app/immutable/chunks/index2.js","_app/immutable/chunks/index3.js","_app/immutable/chunks/navigation.js","_app/immutable/chunks/singletons.js","_app/immutable/chunks/compareVersions.js","_app/immutable/chunks/ConfirmDialog.js","_app/immutable/chunks/_commonjsHelpers.js","_app/immutable/chunks/report-analytics.js","_app/immutable/chunks/socket.js","_app/immutable/chunks/stores.js","_app/immutable/chunks/await_block.js","_app/immutable/chunks/each.js","_app/immutable/chunks/notifications.js","_app/immutable/chunks/alert-triangle.js","_app/immutable/chunks/info-circle.js","_app/immutable/chunks/logo.js","_app/immutable/chunks/users.js","_app/immutable/chunks/stethoscope.js","_app/immutable/chunks/RSSIIndicator.js","_app/immutable/chunks/access-point.js","_app/immutable/chunks/topology-star-3.js","_app/immutable/chunks/clock-check.js","_app/immutable/chunks/InputPassword.js"];
export const stylesheets = ["_app/immutable/assets/0.css"];
export const fonts = [];
