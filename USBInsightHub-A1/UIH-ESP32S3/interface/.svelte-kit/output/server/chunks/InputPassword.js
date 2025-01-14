import { c as create_ssr_component, h as add_attribute, f as escape } from "./ssr.js";
const InputPassword = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let type;
  let { value = "" } = $$props;
  let { id = "" } = $$props;
  if ($$props.value === void 0 && $$bindings.value && value !== void 0)
    $$bindings.value(value);
  if ($$props.id === void 0 && $$bindings.id && id !== void 0)
    $$bindings.id(id);
  type = "password";
  return `<div class="relative"><input${add_attribute("type", type, 0)} class="input input-bordered w-full"${add_attribute("value", value, 0)}${add_attribute("id", id, 0)}> <div class="absolute inset-y-0 right-0 flex items-center pr-1"> <svg xmlns="http://www.w3.org/2000/svg" class="${"text-base-content/50 h-6 " + escape("hidden", true)}" width="40" height="40" viewBox="0 0 24 24" stroke-width="2" stroke="currentColor" fill="none" stroke-linecap="round" stroke-linejoin="round"><path stroke="none" d="M0 0h24v24H0z" fill="none"></path><path d="M10.585 10.587a2 2 0 0 0 2.829 2.828"></path><path d="M16.681 16.673a8.717 8.717 0 0 1 -4.681 1.327c-3.6 0 -6.6 -2 -9 -6c1.272 -2.12 2.712 -3.678 4.32 -4.674m2.86 -1.146a9.055 9.055 0 0 1 1.82 -.18c3.6 0 6.6 2 9 6c-.666 1.11 -1.379 2.067 -2.138 2.87"></path><path d="M3 3l18 18"></path></svg>  <svg xmlns="http://www.w3.org/2000/svg" class="${"text-base-content/50 h-6 " + escape("block", true)}" width="40" height="40" viewBox="0 0 24 24" stroke-width="2" stroke="currentColor" fill="none" stroke-linecap="round" stroke-linejoin="round"><path stroke="none" d="M0 0h24v24H0z" fill="none"></path><path d="M10 12a2 2 0 1 0 4 0a2 2 0 0 0 -4 0"></path><path d="M21 12c-2.4 4 -5.4 6 -9 6c-3.6 0 -6.6 -2 -9 -6c2.4 -4 5.4 -6 9 -6c3.6 0 6.6 2 9 6"></path></svg></div></div>`;
});
export {
  InputPassword as I
};
