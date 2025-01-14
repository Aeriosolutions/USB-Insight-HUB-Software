import { c as create_ssr_component, b as spread, d as escape_object, v as validate_component } from "./ssr.js";
const Chevron_down = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="m6 9l6 6l6-6"/>`}<!-- HTML_TAG_END --></svg>`;
});
const SettingsCard = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { open = true } = $$props;
  let { collapsible = true } = $$props;
  if ($$props.open === void 0 && $$bindings.open && open !== void 0)
    $$bindings.open(open);
  if ($$props.collapsible === void 0 && $$bindings.collapsible && collapsible !== void 0)
    $$bindings.collapsible(collapsible);
  return `${collapsible ? `<div class="bg-base-200 rounded-box shadow-primary/50 relative grid w-full max-w-2xl self-center overflow-hidden shadow-lg"><div class="min-h-16 flex w-full items-center justify-between space-x-3 p-4 text-xl font-medium"><span class="inline-flex items-baseline">${slots.icon ? slots.icon({}) : ``} ${slots.title ? slots.title({}) : ``}</span> <button class="btn btn-circle btn-ghost btn-sm">${validate_component(Chevron_down, "Down").$$render(
    $$result,
    {
      class: "text-base-content h-auto w-6 transition-transform duration-300 ease-in-out " + (open ? "rotate-180" : "")
    },
    {},
    {}
  )}</button></div> ${open ? `<div class="flex flex-col gap-2 p-4 pt-0">${slots.default ? slots.default({}) : ``}</div>` : ``}</div>` : `<div class="bg-base-200 rounded-box shadow-primary/50 relative grid w-full max-w-2xl self-center overflow-hidden shadow-lg"><div class="min-h-16 w-full p-4 text-xl font-medium"><span class="inline-flex items-baseline">${slots.icon ? slots.icon({}) : ``} ${slots.title ? slots.title({}) : ``}</span></div> <div class="flex flex-col gap-2 p-4 pt-0">${slots.default ? slots.default({}) : ``}</div></div>`}`;
});
export {
  Chevron_down as C,
  SettingsCard as S
};
