import { c as create_ssr_component, h as add_attribute } from "../../chunks/ssr.js";
import { l as logo } from "../../chunks/logo.js";
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { data } = $$props;
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  return `<div class="hero bg-base-100 h-screen"><div class="card md:card-side bg-base-200 shadow-primary shadow-2xl"><figure class="bg-base-200" data-svelte-h="svelte-1isxel"><img${add_attribute("src", logo, 0)} alt="Logo" class="h-auto w-64"></figure> <div class="card-body w-80"><h2 class="card-title text-center text-2xl" data-svelte-h="svelte-19ctrtr">Welcome to ESP32-SvelteKit</h2> <p class="py-6 text-center" data-svelte-h="svelte-1wut2o8">A simple, secure and extensible framework for IoT projects for ESP32 platforms with
				responsive <a href="https://kit.svelte.dev/" class="link" target="_blank" rel="noopener noreferrer">SvelteKit</a>
				front-end built with
				<a href="https://tailwindcss.com/" class="link" target="_blank" rel="noopener noreferrer">TailwindCSS</a>
				and
				<a href="https://daisyui.com/" class="link" target="_blank" rel="noopener noreferrer">DaisyUI</a>.</p> <a class="btn btn-primary" href="/demo" data-svelte-h="svelte-wmqcmt">Start Demo</a></div></div></div>`;
});
export {
  Page as default
};
