import { w as writable } from "./index.js";
import { c as create_ssr_component, b as spread, d as escape_object } from "./ssr.js";
let analytics_data = {
  uptime: [],
  free_heap: [],
  total_heap: [],
  min_free_heap: [],
  max_alloc_heap: [],
  fs_used: [],
  fs_total: [],
  core_temp: []
};
const maxAnalyticsData = 1e3;
function createAnalytics() {
  const { subscribe, update } = writable(analytics_data);
  return {
    subscribe,
    addData: (content) => {
      update((analytics_data2) => ({
        ...analytics_data2,
        uptime: [...analytics_data2.uptime, content.uptime].slice(-maxAnalyticsData),
        free_heap: [...analytics_data2.free_heap, content.free_heap / 1e3].slice(-maxAnalyticsData),
        total_heap: [...analytics_data2.total_heap, content.total_heap / 1e3].slice(
          -maxAnalyticsData
        ),
        min_free_heap: [...analytics_data2.min_free_heap, content.min_free_heap / 1e3].slice(
          -maxAnalyticsData
        ),
        max_alloc_heap: [...analytics_data2.max_alloc_heap, content.max_alloc_heap / 1e3].slice(
          -maxAnalyticsData
        ),
        fs_used: [...analytics_data2.fs_used, content.fs_used / 1e3].slice(-maxAnalyticsData),
        fs_total: [...analytics_data2.fs_total, content.fs_total / 1e3].slice(-maxAnalyticsData),
        core_temp: [...analytics_data2.core_temp, content.core_temp].slice(-maxAnalyticsData)
      }));
    }
  };
}
const analytics = createAnalytics();
const Report_analytics = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M9 5H7a2 2 0 0 0-2 2v12a2 2 0 0 0 2 2h10a2 2 0 0 0 2-2V7a2 2 0 0 0-2-2h-2"/><path d="M9 5a2 2 0 0 1 2-2h2a2 2 0 0 1 2 2v0a2 2 0 0 1-2 2h-2a2 2 0 0 1-2-2zm0 12v-5m3 5v-1m3 1v-3"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
export {
  Report_analytics as R,
  analytics as a
};
