import { c as create_ssr_component, b as spread, d as escape_object, o as onDestroy, v as validate_component, h as add_attribute } from "../../../chunks/ssr.js";
import "../../../chunks/user.js";
import { S as SettingsCard } from "../../../chunks/SettingsCard.js";
import { I as Info_circle } from "../../../chunks/info-circle.js";
import { s as socket } from "../../../chunks/socket.js";
const Bulb = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 12h1m8-9v1m8 8h1M5.6 5.6l.7.7m12.1-.7l-.7.7M9 16a5 5 0 1 1 6 0a3.5 3.5 0 0 0-1 3a2 2 0 0 1-4 0a3.5 3.5 0 0 0-1-3m.7 1h4.6"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Demo = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let masterState = { power_on: false, switch_on: false };
  onDestroy(() => {
    socket.off("master");
  });
  return `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
    title: () => {
      return `<span slot="title" data-svelte-h="svelte-9gcziw">Light State</span>`;
    },
    icon: () => {
      return `${validate_component(Bulb, "Light").$$render(
        $$result,
        {
          slot: "icon",
          class: "lex-shrink-0 mr-2 h-6 w-6 self-end"
        },
        {},
        {}
      )}`;
    },
    default: () => {
      return `<div class="w-full"> <h1 class="text-xl font-semibold" data-svelte-h="svelte-bbdd4z">Event Socket Example</h1> <div class="alert alert-info my-2 shadow-lg">${validate_component(Info_circle, "Info").$$render(
        $$result,
        {
          class: "h-6 w-6 flex-shrink-0 stroke-current"
        },
        {},
        {}
      )} <span data-svelte-h="svelte-cbfd8n">The switch below controls the LED via the event system which uses WebSocket under the hood.
				It will automatically update whenever the LED state changes.</span></div>  <div class="form-control w-52"><label class="label cursor-pointer"><span class="" data-svelte-h="svelte-9n5sy1">To ESP32</span> <input type="checkbox" class="toggle toggle-primary rounded-full"${add_attribute("checked", masterState.power_on, 1)}></label></div> <div class="form-control w-52"><label class="label cursor-pointer"><span class="" data-svelte-h="svelte-1ke2gde">From ESP32</span> <input type="checkbox" class="toggle toggle-primary rounded-full"${add_attribute("checked", masterState.switch_on, 1)}></label></div></div>`;
    }
  })}`;
});
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { data } = $$props;
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  return `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${validate_component(Demo, "Demo").$$render($$result, {}, {}, {})}</div>`;
});
export {
  Page as default
};
