import { c as create_ssr_component, b as spread, d as escape_object, a as subscribe, o as onDestroy, v as validate_component, i as is_promise, n as noop, f as escape, h as add_attribute } from "../../../../chunks/ssr.js";
import { I as InputPassword } from "../../../../chunks/InputPassword.js";
import { S as SettingsCard } from "../../../../chunks/SettingsCard.js";
import { u as user } from "../../../../chunks/user.js";
import { p as page } from "../../../../chunks/stores.js";
import { S as Spinner } from "../../../../chunks/Spinner.js";
import { C as Collapsible } from "../../../../chunks/Collapsible.js";
import { T as Topology_star_3 } from "../../../../chunks/topology-star-3.js";
import { I as Info_circle } from "../../../../chunks/info-circle.js";
const Robot = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 6a2 2 0 0 1 2-2h8a2 2 0 0 1 2 2v4a2 2 0 0 1-2 2H8a2 2 0 0 1-2-2zm6-4v2m-3 8v9m6-9v9M5 16l4-2m6 0l4 2M9 18h6M10 8v.01M14 8v.01"/>`}<!-- HTML_TAG_END --></svg>`;
});
const MQTT_1 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $user, $$unsubscribe_user;
  let $page, $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  let mqttSettings;
  let mqttStatus;
  let formField;
  async function getMQTTStatus() {
    try {
      const response = await fetch("/rest/mqttStatus", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      mqttStatus = await response.json();
    } catch (error) {
      console.error("Error:", error);
    }
    return mqttStatus;
  }
  const interval = setInterval(
    async () => {
      getMQTTStatus();
    },
    5e3
  );
  onDestroy(() => clearInterval(interval));
  let $$settled;
  let $$rendered;
  let previous_head = $$result.head;
  do {
    $$settled = true;
    $$result.head = previous_head;
    $$rendered = `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
      title: () => {
        return `<span slot="title" data-svelte-h="svelte-onyccr">MQTT</span>`;
      },
      icon: () => {
        return `${validate_component(Topology_star_3, "MQTT").$$render(
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
            return ` <div class="flex w-full flex-col space-y-1"><div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="${"mask mask-hexagon h-auto w-10 " + escape(
              mqttStatus.connected === true ? "bg-success" : "bg-error",
              true
            )}">${validate_component(Topology_star_3, "MQTT").$$render(
              $$result,
              {
                class: "h-auto w-full scale-75 " + (mqttStatus.connected === true ? "text-success-content" : "text-error-content")
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-yizsky">Status</div> <div class="text-sm opacity-75">${mqttStatus.connected ? `Connected` : `${!mqttStatus.enabled ? `MQTT Disabled` : `${escape(mqttStatus.last_error)}`}`}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Robot, "Client").$$render(
              $$result,
              {
                class: "text-primary-content h-auto w-full scale-75"
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-6cwb7m">Client ID</div> <div class="text-sm opacity-75">${escape(mqttStatus.client_id)}</div></div></div></div> `;
          }();
        }(getMQTTStatus())}</div> ${!$page.data.features.security || $user.admin ? `${validate_component(Collapsible, "Collapsible").$$render($$result, { open: false, class: "shadow-lg" }, {}, {
          title: () => {
            return `<span slot="title" data-svelte-h="svelte-1pncc4g">Change MQTT Settings</span>`;
          },
          default: () => {
            return `<form novalidate${add_attribute("this", formField, 0)}><div class="grid w-full grid-cols-1 content-center gap-x-4 px-4 sm:grid-cols-2"> <label class="label inline-flex cursor-pointer content-end justify-start gap-4"><input type="checkbox" class="checkbox checkbox-primary"${add_attribute("checked", mqttSettings.enabled, 1)}> <span data-svelte-h="svelte-1xm9qiz">Enable MQTT</span></label> <div class="hidden sm:block"></div>  <div class="sm:col-span-2"><label class="label" for="host" data-svelte-h="svelte-3joszr"><span class="label-text text-md">URI</span></label> <input type="text" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="host" min="3" max="64" required${add_attribute("value", mqttSettings.uri, 0)}> <label class="label" for="host"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Must be a valid URI</span></label></div>  <div><label class="label" for="user" data-svelte-h="svelte-w3c29a"><span class="label-text text-md">Username</span></label> <input type="text" class="input input-bordered w-full" id="user"${add_attribute("value", mqttSettings.username, 0)}></div>  <div><label class="label" for="pwd" data-svelte-h="svelte-1b9ak8x"><span class="label-text text-md">Password</span></label> ${validate_component(InputPassword, "InputPassword").$$render(
              $$result,
              { id: "pwd", value: mqttSettings.password },
              {
                value: ($$value) => {
                  mqttSettings.password = $$value;
                  $$settled = false;
                }
              },
              {}
            )}</div>  <div><label class="label" for="clientid" data-svelte-h="svelte-qq41wd"><span class="label-text text-md">Client ID</span></label> <input type="text" class="input input-bordered w-full" id="clientid"${add_attribute("value", mqttSettings.client_id, 0)}></div>  <div><label class="label" for="keepalive" data-svelte-h="svelte-1pztk6r"><span class="label-text text-md">Keep Alive</span></label> <label for="keepalive" class="input-group"><input type="number" min="1" max="600" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="keepalive" required${add_attribute("value", mqttSettings.keep_alive, 0)}> <span data-svelte-h="svelte-1wvw9tt">Seconds</span></label> <label for="keepalive" class="label"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Must be between 1 and 600 seconds</span></label></div>  <label class="label inline-flex cursor-pointer content-end justify-start gap-4"><input type="checkbox" class="checkbox checkbox-primary"${add_attribute("checked", mqttSettings.clean_session, 1)}> <span class="" data-svelte-h="svelte-l47zf3">Clean Session?</span></label></div> <div class="divider mb-2 mt-0"></div> <div class="mx-4 flex flex-wrap justify-end gap-2" data-svelte-h="svelte-19wrugp"><button class="btn btn-primary" type="submit">Apply Settings</button></div></form>`;
          }
        })}` : ``}`;
      }
    })}`;
  } while (!$$settled);
  $$unsubscribe_user();
  $$unsubscribe_page();
  return $$rendered;
});
const MQTTConfig = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $user, $$unsubscribe_user;
  let $page, $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  let brokerSettings;
  let formField;
  async function getBrokerSettings() {
    try {
      const response = await fetch("/rest/brokerSettings", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      brokerSettings = await response.json();
    } catch (error) {
      console.error("Error:", error);
    }
    return;
  }
  $$unsubscribe_user();
  $$unsubscribe_page();
  return `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: true, open: false }, {}, {
    title: () => {
      return `<span slot="title" data-svelte-h="svelte-xvjdcx">MQTT Broker Settings</span>`;
    },
    icon: () => {
      return `${validate_component(Topology_star_3, "MQTT").$$render(
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
          return ` <form novalidate${add_attribute("this", formField, 0)}><div class="alert alert-info my-2 shadow-lg">${validate_component(Info_circle, "Info").$$render(
            $$result,
            {
              class: "h-6 w-6 flex-shrink-0 stroke-current"
            },
            {},
            {}
          )} <span data-svelte-h="svelte-1r3kcmm">The LED is controllable via MQTT with the demo project designed to work with Home
						Assistant&#39;s auto discovery feature.</span></div> <div class="grid w-full grid-cols-1 content-center gap-x-4 px-4"><div><label class="label" for="uid" data-svelte-h="svelte-qfhemx"><span class="label-text text-md">Unique ID</span></label> <input type="text" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="uid" min="3" max="32" required${add_attribute("value", brokerSettings.unique_id, 0)}> <label class="label" for="uid"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Unique ID must be between 3 and 32 characters long</span></label></div> <div><label class="label" for="name" data-svelte-h="svelte-1ds62kb"><span class="label-text text-md">Name</span></label> <input type="text" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="name" min="3" max="32" required${add_attribute("value", brokerSettings.name, 0)}> <label class="label" for="name"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Name must be between 3 and 32 characters long</span></label></div> <div><label class="label" for="path" data-svelte-h="svelte-10qo2tt"><span class="label-text text-md">MQTT Path</span></label> <input type="text" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="path" min="0" max="64" required${add_attribute("value", brokerSettings.mqtt_path, 0)}> <label class="label" for="path"><span class="${"label-text-alt text-error " + escape("hidden", true)}">MQTT path is limited to 64 characters</span></label></div></div> <div class="divider mb-2 mt-0"></div> <div class="mx-4 flex flex-wrap justify-end gap-2" data-svelte-h="svelte-19wrugp"><button class="btn btn-primary" type="submit">Apply Settings</button></div></form> `;
        }();
      }(getBrokerSettings())}</div>`;
    }
  })}`;
});
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { data } = $$props;
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  return `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${validate_component(MQTT_1, "MQTT").$$render($$result, {}, {}, {})} ${validate_component(MQTTConfig, "MqttConfig").$$render($$result, {}, {}, {})}</div>`;
});
export {
  Page as default
};
