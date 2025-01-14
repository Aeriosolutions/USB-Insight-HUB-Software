import { c as create_ssr_component, g as createEventDispatcher, f as escape, v as validate_component } from "./ssr.js";
import { C as Chevron_down } from "./SettingsCard.js";
const Collapsible = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  createEventDispatcher();
  let { open = false } = $$props;
  if ($$props.open === void 0 && $$bindings.open && open !== void 0)
    $$bindings.open(open);
  return `<div class="${escape($$props.class || "", true) + " relative grid w-full max-w-2xl self-center overflow-hidden"}"><div class="min-h-16 flex w-full items-center justify-between space-x-3 p-4 text-xl font-medium"><span class="inline-flex items-baseline">${slots.icon ? slots.icon({}) : ``} ${slots.title ? slots.title({}) : ``}</span> <button class="btn btn-circle btn-ghost btn-sm">${validate_component(Chevron_down, "Down").$$render(
    $$result,
    {
      class: "text-base-content h-auto w-6 transition-transform duration-300 ease-in-out " + (open ? "rotate-180" : "")
    },
    {},
    {}
  )}</button></div> ${open ? `<div class="flex flex-col gap-2 p-4 pt-0">${slots.default ? slots.default({}) : ``}</div>` : ``}</div>`;
});
export {
  Collapsible as C
};
