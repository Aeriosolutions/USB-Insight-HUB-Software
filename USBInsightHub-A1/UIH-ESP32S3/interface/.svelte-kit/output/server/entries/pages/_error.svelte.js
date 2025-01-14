import { c as create_ssr_component, a as subscribe, f as escape } from "../../chunks/ssr.js";
import { g as goto } from "../../chunks/navigation.js";
import { p as page } from "../../chunks/stores.js";
const Error = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $page, $$unsubscribe_page;
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  if ($page.status == 404) {
    goto("/");
  }
  $$unsubscribe_page();
  return `<div class="min-w-screen flex h-screen"><div class="mx-auto flex w-8/12 max-w-lg flex-col justify-center self-center p-2 align-middle"><div class="flex flex-wrap items-end justify-start"><div class="text-secondary pr-2 text-7xl font-bold">${escape($page.status)}</div> <div class="text-base-content text-5xl font-semibold">${escape($page.error?.message)}</div></div> <div class="divider"></div> <p class="text-xl" data-svelte-h="svelte-1tkub0p">Oops! Something has gone wrong.</p></div></div>`;
});
export {
  Error as default
};
