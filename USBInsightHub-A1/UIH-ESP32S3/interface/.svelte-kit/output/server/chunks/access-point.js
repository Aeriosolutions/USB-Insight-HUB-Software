import { c as create_ssr_component, b as spread, d as escape_object } from "./ssr.js";
const Access_point = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 12v.01m2.828-2.838a4 4 0 0 1 0 5.656m2.829-8.485a8 8 0 0 1 0 11.314m-8.489-2.829a4 4 0 0 1 0-5.656m-2.831 8.485a8 8 0 0 1 0-11.314"/>`}<!-- HTML_TAG_END --></svg>`;
});
export {
  Access_point as A
};
