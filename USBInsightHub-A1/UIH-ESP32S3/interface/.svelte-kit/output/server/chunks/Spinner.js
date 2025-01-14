import { c as create_ssr_component, b as spread, d as escape_object, v as validate_component } from "./ssr.js";
const Loader_2 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 3a9 9 0 1 0 9 9"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Spinner = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<div class="flex h-full w-full flex-col items-center justify-center p-6">${validate_component(Loader_2, "Loader").$$render(
    $$result,
    {
      class: "text-primary h-14 w-auto animate-spin stroke-2"
    },
    {},
    {}
  )} <p class="text-xl" data-svelte-h="svelte-11bq9zx">Loading...</p></div>`;
});
export {
  Spinner as S
};
