import { c as create_ssr_component, b as spread, d as escape_object, a as subscribe, o as onDestroy, v as validate_component, i as is_promise, n as noop, f as escape, h as add_attribute, e as each } from "../../../../chunks/ssr.js";
import { I as InputPassword } from "../../../../chunks/InputPassword.js";
import { S as SettingsCard } from "../../../../chunks/SettingsCard.js";
import { u as user } from "../../../../chunks/user.js";
import { p as page } from "../../../../chunks/stores.js";
import { S as Spinner } from "../../../../chunks/Spinner.js";
import { A as Access_point } from "../../../../chunks/access-point.js";
import { H as Home } from "../../../../chunks/home.js";
const Dna_2 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M17 3v1c-.01 3.352-1.68 6.023-5.008 8.014c-3.328 1.99 3.336-2 .008-.014c-3.328 1.99-5 4.662-5.008 8.014v1"/><path d="M17 21.014v-1c-.01-3.352-1.68-6.023-5.008-8.014c-3.328-1.99 3.336 2 .008.014C8.672 10.023 7 7.352 6.992 4V3M7 4h10M7 20h10M8 8h8m-8 8h8"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Devices = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M13 9a1 1 0 0 1 1-1h6a1 1 0 0 1 1 1v10a1 1 0 0 1-1 1h-6a1 1 0 0 1-1-1V9z"/><path d="M18 8V5a1 1 0 0 0-1-1H4a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h9m3-9h2"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Accesspoint = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $user, $$unsubscribe_user;
  let $page, $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  let apSettings;
  let apStatus;
  let formField;
  async function getAPStatus() {
    try {
      const response = await fetch("/rest/apStatus", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      apStatus = await response.json();
    } catch (error) {
      console.error("Error:", error);
    }
    return apStatus;
  }
  async function getAPSettings() {
    try {
      const response = await fetch("/rest/apSettings", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      apSettings = await response.json();
    } catch (error) {
      console.error("Error:", error);
    }
    return apSettings;
  }
  const interval = setInterval(
    async () => {
      getAPStatus();
    },
    5e3
  );
  onDestroy(() => clearInterval(interval));
  let provisionMode = [
    { id: 0, text: `Always` },
    { id: 1, text: `When WiFi Disconnected` },
    { id: 2, text: `Never` }
  ];
  let apStatusDescription = [
    {
      bg_color: "bg-success",
      text_color: "text-success-content",
      description: "Active"
    },
    {
      bg_color: "bg-error",
      text_color: "text-error-content",
      description: "Inactive"
    },
    {
      bg_color: "bg-warning",
      text_color: "text-warning-content",
      description: "Lingering"
    }
  ];
  let $$settled;
  let $$rendered;
  let previous_head = $$result.head;
  do {
    $$settled = true;
    $$result.head = previous_head;
    $$rendered = `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
      title: () => {
        return `<span slot="title" data-svelte-h="svelte-15lmhdt">Access Point</span>`;
      },
      icon: () => {
        return `${validate_component(Access_point, "AP").$$render(
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
            return ` <div class="flex w-full flex-col space-y-1"><div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="${"mask mask-hexagon h-auto w-10 " + escape(apStatusDescription[apStatus.status].bg_color, true)}">${validate_component(Access_point, "AP").$$render(
              $$result,
              {
                class: "h-auto w-full scale-75 " + apStatusDescription[apStatus.status].text_color
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-yizsky">Status</div> <div class="text-sm opacity-75">${escape(apStatusDescription[apStatus.status].description)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Home, "Home").$$render(
              $$result,
              {
                class: "text-primary-content h-auto w-full scale-75"
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-cofw3n">IP Address</div> <div class="text-sm opacity-75">${escape(apStatus.ip_address)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Dna_2, "MAC").$$render(
              $$result,
              {
                class: "text-primary-content h-auto w-full scale-75"
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-okbsad">MAC Address</div> <div class="text-sm opacity-75">${escape(apStatus.mac_address)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Devices, "Devices").$$render(
              $$result,
              {
                class: "text-primary-content h-auto w-full scale-75"
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-12qi1tj">AP Clients</div> <div class="text-sm opacity-75">${escape(apStatus.station_num)}</div></div></div></div> `;
          }();
        }(getAPStatus())}</div> ${!$page.data.features.security || $user.admin ? `<div class="bg-base-200 shadow-lg relative grid w-full max-w-2xl self-center overflow-hidden"><div class="min-h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium" data-svelte-h="svelte-1twspkm">Change AP Settings</div> ${function(__value) {
          if (is_promise(__value)) {
            __value.then(null, noop);
            return ` ${validate_component(Spinner, "Spinner").$$render($$result, {}, {}, {})} `;
          }
          return function(nothing) {
            return ` <div class="flex flex-col gap-2 p-0"><form class="grid w-full grid-cols-1 content-center gap-x-4 p-0 mb-4 sm:grid-cols-2" novalidate${add_attribute("this", formField, 0)}><div><label class="label" for="apmode" data-svelte-h="svelte-1u51onr"><span class="label-text">Provide Access Point ...</span></label> <select class="select select-bordered w-full" id="apmode">${each(provisionMode, (mode) => {
              return `<option${add_attribute("value", mode.id, 0)}>${escape(mode.text)} </option>`;
            })}</select></div> <div><label class="label" for="ssid" data-svelte-h="svelte-sm2u6j"><span class="label-text text-md">SSID</span></label> <input type="text" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="ssid" min="2" max="32" required${add_attribute("value", apSettings.ssid, 0)}> <label class="label" for="ssid"><span class="${"label-text-alt text-error " + escape("hidden", true)}">SSID must be between 2 and 32 characters long</span></label></div> <div><label class="label" for="pwd" data-svelte-h="svelte-1ghr7n9"><span class="label-text text-md">Password</span></label> ${validate_component(InputPassword, "InputPassword").$$render(
              $$result,
              { id: "pwd", value: apSettings.password },
              {
                value: ($$value) => {
                  apSettings.password = $$value;
                  $$settled = false;
                }
              },
              {}
            )}</div> <div><label class="label" for="channel" data-svelte-h="svelte-grqlb0"><span class="label-text text-md">Preferred Channel</span></label> <input type="number" min="1" max="13" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="channel" required${add_attribute("value", apSettings.channel, 0)}> <label class="label" for="channel"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Must be channel 1 to 13</span></label></div> <div><label class="label" for="clients" data-svelte-h="svelte-f6onit"><span class="label-text text-md">Max Clients</span></label> <input type="number" min="1" max="8" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="clients" required${add_attribute("value", apSettings.max_clients, 0)}> <label class="label" for="clients"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Maximum 8 clients allowed</span></label></div> <div><label class="label" for="localIP" data-svelte-h="svelte-3hex3x"><span class="label-text text-md">Local IP</span></label> <input type="text" class="${"input input-bordered w-full " + escape("", true)}" minlength="7" maxlength="15" size="15" id="localIP" required${add_attribute("value", apSettings.local_ip, 0)}> <label class="label" for="localIP"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Must be a valid IPv4 address</span></label></div> <div><label class="label" for="gateway" data-svelte-h="svelte-1sba56o"><span class="label-text text-md">Gateway IP</span></label> <input type="text" class="${"input input-bordered w-full " + escape("", true)}" minlength="7" maxlength="15" size="15" id="gateway" required${add_attribute("value", apSettings.gateway_ip, 0)}> <label class="label" for="gateway"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Must be a valid IPv4 address</span></label></div> <div><label class="label" for="subnet" data-svelte-h="svelte-17ve7bv"><span class="label-text text-md">Subnet Mask</span></label> <input type="text" class="${"input input-bordered w-full " + escape("", true)}" minlength="7" maxlength="15" size="15" id="subnet" required${add_attribute("value", apSettings.subnet_mask, 0)}> <label class="label" for="subnet"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Must be a valid IPv4 address</span></label></div> <label class="label my-auto cursor-pointer justify-start gap-4"><input type="checkbox" class="checkbox checkbox-primary"${add_attribute("checked", apSettings.ssid_hidden, 1)}> <span class="" data-svelte-h="svelte-eoeqkq">Hide SSID</span></label> <div class="place-self-end" data-svelte-h="svelte-21ocql"><button class="btn btn-primary" type="submit">Apply Settings</button></div></form></div> `;
          }();
        }(getAPSettings())}</div>` : ``}`;
      }
    })}`;
  } while (!$$settled);
  $$unsubscribe_user();
  $$unsubscribe_page();
  return $$rendered;
});
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { data } = $$props;
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  return `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${validate_component(Accesspoint, "Accesspoint").$$render($$result, {}, {}, {})}</div>`;
});
export {
  Page as default
};
