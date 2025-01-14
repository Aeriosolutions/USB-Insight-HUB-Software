import { c as create_ssr_component, b as spread, d as escape_object } from "./ssr.js";
const Clock_check = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M20.942 13.021a9 9 0 1 0-9.407 7.967"/><path d="M12 7v5l3 3m0 4l2 2l4-4"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
export {
  Clock_check as C
};
