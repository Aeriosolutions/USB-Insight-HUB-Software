import { c as create_ssr_component, b as spread, d as escape_object } from "./ssr.js";
const Stethoscope = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M6 4H5a2 2 0 0 0-2 2v3.5h0a5.5 5.5 0 0 0 11 0V6a2 2 0 0 0-2-2h-1"/><path d="M8 15a6 6 0 1 0 12 0v-3m-9-9v2M6 3v2"/><path d="M18 10a2 2 0 1 0 4 0a2 2 0 1 0-4 0"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
export {
  Stethoscope as S
};
