import { c as create_ssr_component, b as spread, d as escape_object } from "./ssr.js";
const _24_hours = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M4 13c.325 2.532 1.881 4.781 4 6m12-8A8.1 8.1 0 0 0 4.5 9"/><path d="M4 5v4h4m4 6h2a1 1 0 0 1 1 1v1a1 1 0 0 1-1 1h-1a1 1 0 0 0-1 1v1a1 1 0 0 0 1 1h2m3-6v2a1 1 0 0 0 1 1h1m1-3v6"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
export {
  _24_hours as _
};
