import { c as create_ssr_component, b as spread, d as escape_object, a as subscribe, o as onDestroy, v as validate_component, i as is_promise, n as noop, f as escape } from "../../../../chunks/ssr.js";
import { u as user } from "../../../../chunks/user.js";
import { p as page } from "../../../../chunks/stores.js";
import "mousetrap";
import { S as SettingsCard } from "../../../../chunks/SettingsCard.js";
import { S as Spinner } from "../../../../chunks/Spinner.js";
import { S as Stethoscope } from "../../../../chunks/stethoscope.js";
import { _ as _24_hours } from "../../../../chunks/24-hours.js";
import { s as socket } from "../../../../chunks/socket.js";
const Reload = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M19.933 13.041a8 8 0 1 1-9.925-8.788c3.899-1 7.935 1.007 9.425 4.747"/><path d="M20 4v5h-5"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Cpu = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M5 6a1 1 0 0 1 1-1h12a1 1 0 0 1 1 1v12a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1z"/><path d="M9 9h6v6H9zm-6 1h2m-2 4h2m5-11v2m4-2v2m7 5h-2m2 4h-2m-5 7v-2m-4 2v-2"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Binary = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M11 10V5h-1m8 14v-5h-1m-2-8.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v4a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5zm-5 9a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v4a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5zM6 10h.01M6 19h.01"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Zzz = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 12h6l-6 8h6m4-16h6l-6 8h6"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Refresh_dot = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M20 11A8.1 8.1 0 0 0 4.5 9M4 5v4h4m-4 4a8.1 8.1 0 0 0 15.5 2m.5 4v-4h-4m-5-3a1 1 0 1 0 2 0a1 1 0 1 0-2 0"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Activity = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 12h4l3 8l4-16l3 8h4"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Device_sd_card = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M7 21h10a2 2 0 0 0 2-2V5a2 2 0 0 0-2-2h-6.172a2 2 0 0 0-1.414.586L5.586 7.414A2 2 0 0 0 5 8.828V19a2 2 0 0 0 2 2zm6-15v2m3-2v2m-6-1v1"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Pyramid = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M11.105 21.788a1.994 1.994 0 0 0 1.789 0l8.092-4.054c.538-.27.718-.951.385-1.452l-8.54-13.836a.999.999 0 0 0-1.664 0l-8.54 13.836a1.005 1.005 0 0 0 .386 1.452l8.092 4.054zM12 2v20"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Chart_pie = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M10 3.2A9 9 0 1 0 20.8 14a1 1 0 0 0-1-1H13a2 2 0 0 1-2-2V4a.9.9 0 0 0-1-.8"/><path d="M15 3.5A9 9 0 0 1 20.5 9H16a1 1 0 0 1-1-1V3.5"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Folder = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M5 4h4l3 3h7a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Box_model = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M8 8h8v8H8z"/><path d="M4 6a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v12a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2zm12 10l3.3 3.3M16 8l3.3-3.3M8 8L4.7 4.7M8 16l-3.3 3.3"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Temperature = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M10 13.5a4 4 0 1 0 4 0V5a2 2 0 0 0-4 0v8.5M10 9h4"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Sdk = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M7 8H4a1 1 0 0 0-1 1v2a1 1 0 0 0 1 1h2a1 1 0 0 1 1 1v2a1 1 0 0 1-1 1H3m14-8v8m4-8l-3 4l3 4m-4-4h1m-8-4v8h2a2 2 0 0 0 2-2v-4a2 2 0 0 0-2-2h-2z"/>`}<!-- HTML_TAG_END --></svg>`;
});
function convertSeconds(seconds) {
  let minutes = Math.floor(seconds / 60);
  let hours = Math.floor(minutes / 60);
  let days = Math.floor(hours / 24);
  hours = hours % 24;
  minutes = minutes % 60;
  seconds = seconds % 60;
  let result = "";
  if (days > 0) {
    result += days + " day" + (days > 1 ? "s" : "") + " ";
  }
  if (hours > 0) {
    result += hours + " hour" + (hours > 1 ? "s" : "") + " ";
  }
  if (minutes > 0) {
    result += minutes + " minute" + (minutes > 1 ? "s" : "") + " ";
  }
  result += seconds + " second" + (seconds > 1 ? "s" : "");
  return result;
}
const SystemStatus = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $user, $$unsubscribe_user;
  let $page, $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  let systemInformation;
  async function getSystemStatus() {
    try {
      const response = await fetch("/rest/systemStatus", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      systemInformation = await response.json();
    } catch (error) {
      console.log("Error:", error);
    }
    return systemInformation;
  }
  onDestroy(() => socket.off("analytics", handleSystemData));
  const handleSystemData = (data) => systemInformation = { ...systemInformation, ...data };
  $$unsubscribe_user();
  $$unsubscribe_page();
  return `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
    title: () => {
      return `<span slot="title" data-svelte-h="svelte-1kqonm0">System Status</span>`;
    },
    icon: () => {
      return `${validate_component(Stethoscope, "Health").$$render(
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
      return `<div class="w-full overflow-x-auto">${function(__value) {
        if (is_promise(__value)) {
          __value.then(null, noop);
          return ` ${validate_component(Spinner, "Spinner").$$render($$result, {}, {}, {})} `;
        }
        return function(nothing) {
          return ` <div class="flex w-full flex-col space-y-1"><div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Cpu, "CPU").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-siijuu">Chip</div> <div class="text-sm opacity-75">${escape(systemInformation.cpu_type)} Rev ${escape(systemInformation.cpu_rev)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Sdk, "SDK").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-1fuh3uy">SDK Version</div> <div class="text-sm opacity-75">ESP-IDF ${escape(systemInformation.sdk_version)} / Arduino ${escape(systemInformation.arduino_version)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Binary, "CPP").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-1mnmy3j">Firmware Version</div> <div class="text-sm opacity-75">${escape(systemInformation.firmware_version)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Activity, "Speed").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-gyx5je">CPU Frequency</div> <div class="text-sm opacity-75">${escape(systemInformation.cpu_freq_mhz)} MHz ${escape(systemInformation.cpu_cores == 2 ? "Dual Core" : "Single Core")}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Box_model, "Heap").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-k1676v">Heap (Free / Max Alloc)</div> <div class="text-sm opacity-75">${escape(systemInformation.free_heap.toLocaleString("en-US"))} / ${escape(systemInformation.max_alloc_heap.toLocaleString("en-US"))} bytes</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Pyramid, "Pyramid").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-1iv4obs">PSRAM (Size / Free)</div> <div class="text-sm opacity-75">${escape(systemInformation.psram_size.toLocaleString("en-US"))} / ${escape(systemInformation.psram_size.toLocaleString("en-US"))} bytes</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Chart_pie, "Sketch").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-s4pamj">Sketch (Used / Free)</div> <div class="flex flex-wrap justify-start gap-1 text-sm opacity-75"><span>${escape((systemInformation.sketch_size / systemInformation.free_sketch_space * 100).toFixed(1))} % of
								${escape((systemInformation.free_sketch_space / 1e6).toLocaleString("en-US"))} MB used</span> <span>(${escape(((systemInformation.free_sketch_space - systemInformation.sketch_size) / 1e6).toLocaleString("en-US"))} MB free)</span></div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Device_sd_card, "Flash").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-1l1e6nw">Flash Chip (Size / Speed)</div> <div class="text-sm opacity-75">${escape((systemInformation.flash_chip_size / 1e6).toLocaleString("en-US"))} MB / ${escape((systemInformation.flash_chip_speed / 1e6).toLocaleString("en-US"))} MHz</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Folder, "Folder").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-12agvsw">File System (Used / Total)</div> <div class="flex flex-wrap justify-start gap-1 text-sm opacity-75"><span>${escape((systemInformation.fs_used / systemInformation.fs_total * 100).toFixed(1))} % of ${escape((systemInformation.fs_total / 1e6).toLocaleString("en-US"))} MB used</span> <span>(${escape(((systemInformation.fs_total - systemInformation.fs_used) / 1e6).toLocaleString("en-US"))}
								MB free)</span></div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Temperature, "Temperature").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-wk2qzr">Core Temperature</div> <div class="text-sm opacity-75">${escape(systemInformation.core_temp == 53.33 ? "NaN" : systemInformation.core_temp.toFixed(2) + " °C")}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(_24_hours, "Stopwatch").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-1pnqi1s">Uptime</div> <div class="text-sm opacity-75">${escape(convertSeconds(systemInformation.uptime))}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">${validate_component(Reload, "Power").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-skiv8j">Reset Reason</div> <div class="text-sm opacity-75">${escape(systemInformation.cpu_reset_reason)}</div></div></div></div> `;
        }();
      }(getSystemStatus())}</div> <div class="mt-4 flex flex-wrap justify-end gap-2">${$page.data.features.sleep ? `<button class="btn btn-primary inline-flex items-center">${validate_component(Zzz, "Sleep").$$render($$result, { class: "mr-2 h-5 w-5" }, {}, {})}<span data-svelte-h="svelte-1amzpfv">Sleep</span></button>` : ``} ${!$page.data.features.security || $user.admin ? `<button class="btn btn-primary inline-flex items-center">${validate_component(Reload, "Power").$$render($$result, { class: "mr-2 h-5 w-5" }, {}, {})}<span data-svelte-h="svelte-1an552f">Restart</span></button> <button class="btn btn-secondary inline-flex items-center">${validate_component(Refresh_dot, "FactoryReset").$$render($$result, { class: "mr-2 h-5 w-5" }, {}, {})}<span data-svelte-h="svelte-gxdx59">Factory Reset</span></button>` : ``}</div>`;
    }
  })}`;
});
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { data } = $$props;
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  return `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${validate_component(SystemStatus, "SystemStatus").$$render($$result, {}, {}, {})}</div>`;
});
export {
  Page as default
};
