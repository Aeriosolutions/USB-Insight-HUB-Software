import { c as create_ssr_component, a as subscribe, e as each, i as is_promise, n as noop, v as validate_component, m as missing_component, b as spread, d as escape_object, f as escape, g as createEventDispatcher, h as add_attribute, o as onDestroy } from "../../chunks/ssr.js";
import { u as user } from "../../chunks/user.js";
import { w as writable } from "../../chunks/index.js";
import { R as Report_analytics, a as analytics } from "../../chunks/report-analytics.js";
import { s as socket } from "../../chunks/socket.js";
import { p as page } from "../../chunks/stores.js";
import { A as Alert_triangle } from "../../chunks/alert-triangle.js";
import { I as Info_circle } from "../../chunks/info-circle.js";
import { l as logo } from "../../chunks/logo.js";
import { U as Users } from "../../chunks/users.js";
import { S as Stethoscope } from "../../chunks/stethoscope.js";
import { W as Wifi, R as Router } from "../../chunks/router.js";
import { A as Access_point } from "../../chunks/access-point.js";
import { T as Topology_star_3 } from "../../chunks/topology-star-3.js";
import { C as Clock_check } from "../../chunks/clock-check.js";
import "mousetrap";
import { I as InputPassword } from "../../chunks/InputPassword.js";
let telemetry_data = {
  rssi: {
    rssi: 0,
    ssid: "",
    disconnected: true
  },
  battery: {
    soc: 100,
    charging: false
  },
  download_ota: {
    status: "none",
    progress: 0,
    error: ""
  }
};
function createTelemetry() {
  const { subscribe: subscribe2, set, update } = writable(telemetry_data);
  return {
    subscribe: subscribe2,
    setRSSI: (data) => {
      if (!isNaN(Number(data.rssi))) {
        update((telemetry_data2) => ({
          ...telemetry_data2,
          rssi: { rssi: Number(data.rssi), ssid: data.ssid, disconnected: false }
        }));
      } else {
        update((telemetry_data2) => ({
          ...telemetry_data2,
          rssi: { rssi: 0, ssid: data.ssid, disconnected: true }
        }));
      }
    },
    setBattery: (data) => {
      update((telemetry_data2) => ({
        ...telemetry_data2,
        battery: { soc: data.soc, charging: data.charging }
      }));
    },
    setDownloadOTA: (data) => {
      update((telemetry_data2) => ({
        ...telemetry_data2,
        download_ota: { status: data.status, progress: data.progress, error: data.error }
      }));
    }
  };
}
const telemetry = createTelemetry();
const exitBeforeEnter = writable(false);
const transitioning = writable(null);
const modals = writable([]);
function isLazyModal(component) {
  return typeof component.prototype === "undefined";
}
async function getComponent(component) {
  return component().then((res) => res.default);
}
const Modals = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $modals, $$unsubscribe_modals;
  let $transitioning, $$unsubscribe_transitioning;
  let $$unsubscribe_exitBeforeEnter;
  $$unsubscribe_modals = subscribe(modals, (value) => $modals = value);
  $$unsubscribe_transitioning = subscribe(transitioning, (value) => $transitioning = value);
  $$unsubscribe_exitBeforeEnter = subscribe(exitBeforeEnter, (value) => value);
  $$unsubscribe_modals();
  $$unsubscribe_transitioning();
  $$unsubscribe_exitBeforeEnter();
  return `${$modals.length > 0 ? `${slots.backdrop ? slots.backdrop({}) : ``}` : ``} ${slots.default ? slots.default({}) : ` ${each($modals, (modal, i) => {
    return ` ${isLazyModal(modal.component) ? `${function(__value) {
      if (is_promise(__value)) {
        __value.then(null, noop);
        return ` ${slots.loading ? slots.loading({}) : ``} `;
      }
      return function(component) {
        return ` ${validate_component(component || missing_component, "svelte:component").$$render(
          $$result,
          Object.assign(
            {},
            {
              isOpen: i === $modals.length - 1 && !$transitioning
            },
            modal.props
          ),
          {},
          {}
        )} `;
      }(__value);
    }(getComponent(modal.component))}` : ` ${validate_component(modal.component || missing_component, "svelte:component").$$render(
      $$result,
      Object.assign(
        {},
        {
          isOpen: i === $modals.length - 1 && !$transitioning
        },
        modal.props
      ),
      {},
      {}
    )}`}`;
  })} `}`;
});
function createNotificationStore() {
  const state = [];
  const notifications2 = writable(state);
  const { subscribe: subscribe2 } = notifications2;
  function send(message, type = "info", timeout) {
    const id = generateId();
    setTimeout(() => {
      notifications2.update((state2) => {
        return state2.filter((n) => n.id !== id);
      });
    }, timeout);
    notifications2.update((state2) => {
      return [...state2, { id, type, message }];
    });
  }
  return {
    subscribe: subscribe2,
    send,
    error: (msg, timeout) => send(msg, "error", timeout),
    warning: (msg, timeout) => send(msg, "warning", timeout),
    info: (msg, timeout) => send(msg, "info", timeout),
    success: (msg, timeout) => send(msg, "success", timeout)
  };
}
function generateId() {
  return "_" + Math.random().toString(36).substr(2, 9);
}
const notifications = createNotificationStore();
const Circle_x = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 12a9 9 0 1 0 18 0a9 9 0 1 0-18 0m7-2l4 4m0-4l-4 4"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Circle_check = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M3 12a9 9 0 1 0 18 0a9 9 0 1 0-18 0"/><path d="m9 12l2 2l4-4"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Toast = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $notifications, $$unsubscribe_notifications;
  $$unsubscribe_notifications = subscribe(notifications, (value) => $notifications = value);
  let { theme = {
    error: "alert-error",
    success: "alert-success",
    warning: "alert-warning",
    info: "alert-info"
  } } = $$props;
  let { icon = { error: Circle_x, success: Circle_check, warning: Alert_triangle, info: Info_circle } } = $$props;
  if ($$props.theme === void 0 && $$bindings.theme && theme !== void 0)
    $$bindings.theme(theme);
  if ($$props.icon === void 0 && $$bindings.icon && icon !== void 0)
    $$bindings.icon(icon);
  $$unsubscribe_notifications();
  return `<div class="toast toast-end mr-4">${each($notifications, (notification) => {
    return `<div class="${"alert animate-none " + escape(theme[notification.type], true)}">${validate_component(icon[notification.type] || missing_component, "svelte:component").$$render($$result, { class: "h-6 w-6 flex-shrink-0" }, {}, {})} <span>${escape(notification.message)}</span> </div>`;
  })}</div>`;
});
const app = "";
const Settings = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M10.325 4.317c.426-1.756 2.924-1.756 3.35 0a1.724 1.724 0 0 0 2.573 1.066c1.543-.94 3.31.826 2.37 2.37a1.724 1.724 0 0 0 1.065 2.572c1.756.426 1.756 2.924 0 3.35a1.724 1.724 0 0 0-1.066 2.573c.94 1.543-.826 3.31-2.37 2.37a1.724 1.724 0 0 0-2.572 1.065c-.426 1.756-2.924 1.756-3.35 0a1.724 1.724 0 0 0-2.573-1.066c-1.543.94-3.31-.826-2.37-2.37a1.724 1.724 0 0 0-1.065-2.572c-1.756-.426-1.756-2.924 0-3.35a1.724 1.724 0 0 0 1.066-2.573c-.94-1.543.826-3.31 2.37-2.37c1 .608 2.296.07 2.572-1.065z"/><path d="M9 12a3 3 0 1 0 6 0a3 3 0 0 0-6 0"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Refresh_alert = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M20 11A8.1 8.1 0 0 0 4.5 9M4 5v4h4m-4 4a8.1 8.1 0 0 0 15.5 2m.5 4v-4h-4m-4-6v3m0 3h.01"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Network = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M6 9a6 6 0 1 0 12 0A6 6 0 0 0 6 9"/><path d="M12 3c1.333.333 2 2.333 2 6s-.667 5.667-2 6m0-12c-1.333.333-2 2.333-2 6s.667 5.667 2 6M6 9h12M3 20h7m4 0h7m-11 0a2 2 0 1 0 4 0a2 2 0 0 0-4 0m2-5v3"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Adjustments = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 10a2 2 0 1 0 4 0a2 2 0 0 0-4 0m2-6v4m0 4v8m4-4a2 2 0 1 0 4 0a2 2 0 0 0-4 0m2-12v10m0 4v2m4-13a2 2 0 1 0 4 0a2 2 0 0 0-4 0m2-3v1m0 4v11"/>`}<!-- HTML_TAG_END --></svg>`;
});
const User_circle = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M3 12a9 9 0 1 0 18 0a9 9 0 1 0-18 0"/><path d="M9 10a3 3 0 1 0 6 0a3 3 0 1 0-6 0m-2.832 8.849A4 4 0 0 1 10 16h4a4 4 0 0 1 3.834 2.855"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Logout = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M14 8V6a2 2 0 0 0-2-2H5a2 2 0 0 0-2 2v12a2 2 0 0 0 2 2h7a2 2 0 0 0 2-2v-2"/><path d="M9 12h12l-3-3m0 6l3-3"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Copyright = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M3 12a9 9 0 1 0 18 0a9 9 0 1 0-18 0"/><path d="M14 9.75a3.016 3.016 0 0 0-4.163.173a2.993 2.993 0 0 0 0 4.154A3.016 3.016 0 0 0 14 14.25"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Menu = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $page, $$unsubscribe_page;
  let $user, $$unsubscribe_user;
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  ({
    href: "https://github.com/" + $page.data.github,
    active: false
  });
  let menuItems = [
    {
      title: "Demo App",
      icon: Adjustments,
      href: "/demo",
      feature: true
    },
    {
      title: "Connections",
      icon: Network,
      feature: $page.data.features.mqtt || $page.data.features.ntp,
      submenu: [
        {
          title: "MQTT",
          icon: Topology_star_3,
          href: "/connections/mqtt",
          feature: $page.data.features.mqtt
        },
        {
          title: "NTP",
          icon: Clock_check,
          href: "/connections/ntp",
          feature: $page.data.features.ntp
        }
      ]
    },
    {
      title: "WiFi",
      icon: Wifi,
      feature: true,
      submenu: [
        {
          title: "WiFi Station",
          icon: Router,
          href: "/wifi/sta",
          feature: true
        },
        {
          title: "Access Point",
          icon: Access_point,
          href: "/wifi/ap",
          feature: true
        }
      ]
    },
    {
      title: "Users",
      icon: Users,
      href: "/user",
      feature: $page.data.features.security && $user.admin
    },
    {
      title: "System",
      icon: Settings,
      feature: true,
      submenu: [
        {
          title: "System Status",
          icon: Stethoscope,
          href: "/system/status",
          feature: true
        },
        {
          title: "System Metrics",
          icon: Report_analytics,
          href: "/system/metrics",
          feature: $page.data.features.analytics
        },
        {
          title: "Firmware Update",
          icon: Refresh_alert,
          href: "/system/update",
          feature: ($page.data.features.ota || $page.data.features.upload_firmware || $page.data.features.download_firmware) && (!$page.data.features.security || $user.admin)
        }
      ]
    }
  ];
  const dispatch = createEventDispatcher();
  function setActiveMenuItem(targetTitle) {
    menuItems.forEach((item) => {
      item.active = item.title === targetTitle;
      item.submenu?.forEach((subItem) => {
        subItem.active = subItem.title === targetTitle;
      });
    });
    menuItems = menuItems;
    dispatch("menuClicked");
  }
  {
    setActiveMenuItem($page.data.title);
  }
  $$unsubscribe_page();
  $$unsubscribe_user();
  return `<div class="bg-base-200 text-base-content flex h-full w-80 flex-col p-4"> <a href="/" class="rounded-box mb-4 flex items-center hover:scale-[1.02] active:scale-[0.98]"><img${add_attribute("src", logo, 0)} alt="Logo" class="h-12 w-12"> <h1 class="px-4 text-2xl font-bold">${escape($page.data.appName)}</h1></a> <ul class="menu rounded-box menu-vertical flex-nowrap overflow-y-auto">${each(menuItems, (menuItem, i) => {
    return `${menuItem.feature ? `<li>${menuItem.submenu ? `<details><summary class="text-lg font-bold">${validate_component(menuItem.icon || missing_component, "svelte:component").$$render($$result, { class: "h-6 w-6" }, {}, {})} ${escape(menuItem.title)}</summary> <ul>${each(menuItem.submenu, (subMenuItem) => {
      return `${subMenuItem.feature ? `<li class="hover-bordered"><a${add_attribute("href", subMenuItem.href, 0)} class="${["text-ml font-bold", subMenuItem.active ? "bg-base-100" : ""].join(" ").trim()}">${validate_component(subMenuItem.icon || missing_component, "svelte:component").$$render($$result, { class: "h-5 w-5" }, {}, {})}${escape(subMenuItem.title)}</a> </li>` : ``}`;
    })}</ul> </details>` : `<a${add_attribute("href", menuItem.href, 0)} class="${["text-lg font-bold", menuItem.active ? "bg-base-100" : ""].join(" ").trim()}">${validate_component(menuItem.icon || missing_component, "svelte:component").$$render($$result, { class: "h-6 w-6" }, {}, {})}${escape(menuItem.title)}</a>`} </li>` : ``}`;
  })}</ul> <div class="flex-col"></div> <div class="flex-grow"></div> ${$page.data.features.security ? `<div class="flex items-center">${validate_component(User_circle, "Avatar").$$render($$result, { class: "h-8 w-8" }, {}, {})} <span class="flex-grow px-4 text-xl font-bold">${escape($user.username)}</span>   <div class="btn btn-ghost">${validate_component(Logout, "Logout").$$render($$result, { class: "h-8 w-8 rotate-180" }, {}, {})}</div></div>` : ``} <div class="divider my-0"></div> <div class="flex items-center">${``} ${``} <div class="inline-flex flex-grow items-center justify-end text-sm">${validate_component(Copyright, "Copyright").$$render($$result, { class: "h-4 w-4" }, {}, {})}<span class="px-2">${escape($page.data.copyright)}</span></div></div></div>`;
});
const Wifi_off = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 18h.01m-2.838-2.828a4 4 0 0 1 5.656 0m-8.485-2.829a7.963 7.963 0 0 1 3.864-2.14m4.163.155a7.965 7.965 0 0 1 3.287 2M3.515 9.515A12 12 0 0 1 7.059 7.06m3.101-.92a12 12 0 0 1 10.325 3.374M3 3l18 18"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Menu_2 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 6h16M4 12h16M4 18h16"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Power = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M7 6a7.75 7.75 0 1 0 10 0m-5-2v8"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Wifi_0 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 18h.01"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Wifi_1 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 18h.01m-2.838-2.828a4 4 0 0 1 5.656 0"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Wifi_2 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 18h.01m-2.838-2.828a4 4 0 0 1 5.656 0m-8.485-2.829a8 8 0 0 1 11.314 0"/>`}<!-- HTML_TAG_END --></svg>`;
});
const RSSIIndicator = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { showDBm = false } = $$props;
  let { rssi_dbm = 0 } = $$props;
  let { ssid = "" } = $$props;
  if ($$props.showDBm === void 0 && $$bindings.showDBm && showDBm !== void 0)
    $$bindings.showDBm(showDBm);
  if ($$props.rssi_dbm === void 0 && $$bindings.rssi_dbm && rssi_dbm !== void 0)
    $$bindings.rssi_dbm(rssi_dbm);
  if ($$props.ssid === void 0 && $$bindings.ssid && ssid !== void 0)
    $$bindings.ssid(ssid);
  {
    if (ssid === "") {
      ssid = "Unknown";
    }
  }
  return `<div class="indicator"><div class="tooltip tooltip-left"${add_attribute("data-tip", ssid, 0)}>${showDBm ? `<span class="indicator-item indicator-start badge badge-accent badge-outline badge-xs">${escape(rssi_dbm)} dBm</span>` : ``} ${rssi_dbm >= -55 ? `${validate_component(Wifi, "WiFi").$$render($$result, { class: $$props.class || "" }, {}, {})}` : `${rssi_dbm >= -75 ? `<div class="${escape($$props.class || "", true) + " relative"}">${validate_component(Wifi, "WiFi").$$render(
    $$result,
    {
      class: "absolute inset-0 h-full w-full opacity-30"
    },
    {},
    {}
  )} ${validate_component(Wifi_2, "WiFi2").$$render($$result, { class: "absolute inset-0 h-full w-full" }, {}, {})}</div>` : `${rssi_dbm >= -85 ? `<div class="${escape($$props.class || "", true) + " relative"}">${validate_component(Wifi, "WiFi").$$render(
    $$result,
    {
      class: "absolute inset-0 h-full w-full opacity-30"
    },
    {},
    {}
  )} ${validate_component(Wifi_1, "WiFi1").$$render($$result, { class: "absolute inset-0 h-full w-full" }, {}, {})}</div>` : `<div class="${escape($$props.class || "", true) + " relative"}">${validate_component(Wifi, "WiFi").$$render(
    $$result,
    {
      class: "absolute inset-0 h-full w-full opacity-30"
    },
    {},
    {}
  )} ${validate_component(Wifi_0, "WiFi0").$$render($$result, { class: "absolute inset-0 h-full w-full" }, {}, {})}</div>`}`}`}</div></div>`;
});
const Battery = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 7h11a2 2 0 0 1 2 2v.5a.5.5 0 0 0 .5.5a.5.5 0 0 1 .5.5v3a.5.5 0 0 1-.5.5a.5.5 0 0 0-.5.5v.5a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V9a2 2 0 0 1 2-2"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Battery_1 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 7h11a2 2 0 0 1 2 2v.5a.5.5 0 0 0 .5.5a.5.5 0 0 1 .5.5v3a.5.5 0 0 1-.5.5a.5.5 0 0 0-.5.5v.5a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V9a2 2 0 0 1 2-2m1 3v4"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Battery_2 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 7h11a2 2 0 0 1 2 2v.5a.5.5 0 0 0 .5.5a.5.5 0 0 1 .5.5v3a.5.5 0 0 1-.5.5a.5.5 0 0 0-.5.5v.5a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V9a2 2 0 0 1 2-2m1 3v4m3-4v4"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Battery_3 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 7h11a2 2 0 0 1 2 2v.5a.5.5 0 0 0 .5.5a.5.5 0 0 1 .5.5v3a.5.5 0 0 1-.5.5a.5.5 0 0 0-.5.5v.5a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V9a2 2 0 0 1 2-2m1 3v4m3-4v4m3-4v4"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Battery_4 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 7h11a2 2 0 0 1 2 2v.5a.5.5 0 0 0 .5.5a.5.5 0 0 1 .5.5v3a.5.5 0 0 1-.5.5a.5.5 0 0 0-.5.5v.5a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V9a2 2 0 0 1 2-2m1 3v4m3-4v4m3-4v4m3-4v4"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Battery_charging_2 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 9a2 2 0 0 1 2-2h11a2 2 0 0 1 2 2v.5a.5.5 0 0 0 .5.5a.5.5 0 0 1 .5.5v3a.5.5 0 0 1-.5.5a.5.5 0 0 0-.5.5v.5a2 2 0 0 1-2 2h-4.5M3 15h6v2a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-2zm3 7v-3m-2-4v-2.5M8 15v-2.5"/>`}<!-- HTML_TAG_END --></svg>`;
});
const BatteryIndicator = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { charging = false } = $$props;
  let { soc = 100 } = $$props;
  if ($$props.charging === void 0 && $$bindings.charging && charging !== void 0)
    $$bindings.charging(charging);
  if ($$props.soc === void 0 && $$bindings.soc && soc !== void 0)
    $$bindings.soc(soc);
  return `<div class="tooltip tooltip-bottom" data-tip="${escape(soc, true) + " %"}">${charging ? `${validate_component(Battery_charging_2, "BatteryCharging").$$render(
    $$result,
    {
      class: ($$props.class || "") + " -rotate-90 animate-pulse"
    },
    {},
    {}
  )}` : `${soc > 75 ? `${validate_component(Battery_4, "Battery100").$$render(
    $$result,
    {
      class: ($$props.class || "") + " -rotate-90"
    },
    {},
    {}
  )}` : `${soc > 55 ? `${validate_component(Battery_3, "Battery75").$$render(
    $$result,
    {
      class: ($$props.class || "") + " -rotate-90"
    },
    {},
    {}
  )}` : `${soc > 30 ? `${validate_component(Battery_2, "Battery50").$$render(
    $$result,
    {
      class: ($$props.class || "") + " -rotate-90"
    },
    {},
    {}
  )}` : `${soc > 5 ? `${validate_component(Battery_1, "Battery25").$$render(
    $$result,
    {
      class: ($$props.class || "") + " -rotate-90"
    },
    {},
    {}
  )}` : `${validate_component(Battery, "Battery0").$$render(
    $$result,
    {
      class: ($$props.class || "") + " text-error -rotate-90 animate-pulse"
    },
    {},
    {}
  )}`}`}`}`}`}</div>`;
});
const UpdateIndicator = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $$unsubscribe_user;
  let $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => value);
  $$unsubscribe_page = subscribe(page, (value) => value);
  let { update = false } = $$props;
  let firmwareVersion;
  if ($$props.update === void 0 && $$bindings.update && update !== void 0)
    $$bindings.update(update);
  $$unsubscribe_user();
  $$unsubscribe_page();
  return `${update ? `<button class="btn btn-square btn-ghost h-9 w-9"><span class="indicator-item indicator-top indicator-center badge badge-info badge-xs top-2 scale-75 lg:top-1">${escape(firmwareVersion)}</span> ${validate_component(Refresh_alert, "Firmware").$$render($$result, { class: "h-7 w-7" }, {}, {})}</button>` : ``}`;
});
const Statusbar = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $$unsubscribe_user;
  let $page, $$unsubscribe_page;
  let $telemetry, $$unsubscribe_telemetry;
  $$unsubscribe_user = subscribe(user, (value) => value);
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  $$unsubscribe_telemetry = subscribe(telemetry, (value) => $telemetry = value);
  $$unsubscribe_user();
  $$unsubscribe_page();
  $$unsubscribe_telemetry();
  return `<div class="navbar bg-base-300 sticky top-0 z-10 h-12 min-h-fit drop-shadow-lg lg:h-16"><div class="flex-1"> <label for="main-menu" class="btn btn-ghost btn-circle btn-sm drawer-button lg:hidden">${validate_component(Menu_2, "Hamburger").$$render($$result, { class: "h-6 w-auto" }, {}, {})}</label> <span class="px-2 text-xl font-bold lg:text-2xl">${escape($page.data.title)}</span></div> <div class="indicator flex-none">${validate_component(UpdateIndicator, "UpdateIndicator").$$render($$result, {}, {}, {})}</div> <div class="flex-none">${$telemetry.rssi.disconnected ? `${validate_component(Wifi_off, "WiFiOff").$$render($$result, { class: "h-7 w-7" }, {}, {})}` : `${validate_component(RSSIIndicator, "RssiIndicator").$$render(
    $$result,
    {
      showDBm: false,
      rssi_dbm: $telemetry.rssi.rssi,
      ssid: $telemetry.rssi.ssid,
      class: "h-7 w-7"
    },
    {},
    {}
  )}`}</div> ${$page.data.features.battery ? `<div class="flex-none">${validate_component(BatteryIndicator, "BatteryIndicator").$$render(
    $$result,
    {
      charging: $telemetry.battery.charging,
      soc: $telemetry.battery.soc,
      class: "h-7 w-7"
    },
    {},
    {}
  )}</div>` : ``} ${$page.data.features.sleep ? `<div class="flex-none"><button class="btn btn-square btn-ghost h-9 w-10">${validate_component(Power, "Power").$$render($$result, { class: "text-error h-9 w-9" }, {}, {})}</button></div>` : ``}</div>`;
});
const Login = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M15 8V6a2 2 0 0 0-2-2H6a2 2 0 0 0-2 2v12a2 2 0 0 0 2 2h7a2 2 0 0 0 2-2v-2"/><path d="M21 12H8l3-3m0 6l-3-3"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const login_svelte_svelte_type_style_lang = "";
const css = {
  code: ".failure.svelte-198cgfo{animation:svelte-198cgfo-shake 0.82s cubic-bezier(0.36, 0.07, 0.19, 0.97) both;transform:translate3d(0, 0, 0);backface-visibility:hidden;perspective:1000px}@keyframes svelte-198cgfo-shake{10%,90%{transform:translatex(-1px)}20%,80%{transform:translatex(2px)}30%,50%,70%{transform:translatex(-4px)}40%,60%{transform:translatex(4px)}}",
  map: null
};
const Login_1 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $$unsubscribe_user;
  $$unsubscribe_user = subscribe(user, (value) => value);
  let username = "";
  let password = "";
  $$result.css.add(css);
  let $$settled;
  let $$rendered;
  let previous_head = $$result.head;
  do {
    $$settled = true;
    $$result.head = previous_head;
    $$rendered = `<div class="hero from-primary/30 to-secondary/30 min-h-screen bg-gradient-to-br"><div class="${"card lg:card-side bg-base-100 shadow-primary face shadow-2xl " + escape("", true) + " svelte-198cgfo"}"><figure class="bg-base-200" data-svelte-h="svelte-pg0p5y"><img${add_attribute("src", logo, 0)} alt="Logo" class="h-auto w-48 lg:w-64"></figure> <div class="card-body w-80"><h2 class="card-title text-2xl" data-svelte-h="svelte-c1oa49">Login</h2> <form class="form-control w-full max-w-xs"><label class="label" for="user" data-svelte-h="svelte-1e953ba"><span class="label-text text-md">Username</span></label> <input type="text" class="input input-bordered w-full max-w-xs" id="user"${add_attribute("value", username, 0)}> <label class="label" for="pwd" data-svelte-h="svelte-1ur0rr9"><span class="label-text text-md">Password</span></label> ${validate_component(InputPassword, "InputPassword").$$render(
      $$result,
      { id: "pwd", value: password },
      {
        value: ($$value) => {
          password = $$value;
          $$settled = false;
        }
      },
      {}
    )} <div class="card-actions mt-4 justify-end"><button class="btn btn-primary inline-flex items-center">${validate_component(Login, "Login").$$render($$result, { class: "mr-2 h-5 w-5" }, {}, {})}<span data-svelte-h="svelte-sdljcx">Login</span></button></div></form></div></div> </div>`;
  } while (!$$settled);
  $$unsubscribe_user();
  return $$rendered;
});
const Layout = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $page, $$unsubscribe_page;
  let $user, $$unsubscribe_user;
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  let { data } = $$props;
  onDestroy(() => {
    removeEventListeners();
  });
  const removeEventListeners = () => {
    socket.off("analytics", handleAnalytics);
    socket.off("open", handleOpen);
    socket.off("close", handleClose);
    socket.off("rssi", handleNetworkStatus);
    socket.off("notification", handleNotification);
    socket.off("battery", handleBattery);
    socket.off("otastatus", handleOAT);
  };
  const handleOpen = () => {
    notifications.success("Connection to device established", 5e3);
  };
  const handleClose = () => {
    notifications.error("Connection to device lost", 5e3);
    telemetry.setRSSI({ rssi: 0, ssid: "" });
  };
  const handleNotification = (data2) => {
    switch (data2.type) {
      case "info":
        notifications.info(data2.message, 5e3);
        break;
      case "warning":
        notifications.warning(data2.message, 5e3);
        break;
      case "error":
        notifications.error(data2.message, 5e3);
        break;
      case "success":
        notifications.success(data2.message, 5e3);
        break;
    }
  };
  const handleAnalytics = (data2) => analytics.addData(data2);
  const handleNetworkStatus = (data2) => telemetry.setRSSI(data2);
  const handleBattery = (data2) => telemetry.setBattery(data2);
  const handleOAT = (data2) => telemetry.setDownloadOTA(data2);
  let menuOpen = false;
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  $$unsubscribe_page();
  $$unsubscribe_user();
  return `${$$result.head += `<!-- HEAD_svelte-13v4y7l_START -->${$$result.title = `<title>${escape($page.data.title)}</title>`, ""}<!-- HEAD_svelte-13v4y7l_END -->`, ""} ${$page.data.features.security && $user.bearer_token === "" ? `${validate_component(Login_1, "Login").$$render($$result, {}, {}, {})}` : `<div class="drawer lg:drawer-open"><input id="main-menu" type="checkbox" class="drawer-toggle"${add_attribute("checked", menuOpen, 1)}> <div class="drawer-content flex flex-col"> ${validate_component(Statusbar, "Statusbar").$$render($$result, {}, {}, {})}  ${slots.default ? slots.default({}) : ``}</div>  <div class="drawer-side z-30 shadow-lg"><label for="main-menu" class="drawer-overlay"></label> ${validate_component(Menu, "Menu").$$render($$result, {}, {}, {})}</div></div>`} ${validate_component(Modals, "Modals").$$render($$result, {}, {}, {
    backdrop: () => {
      return `<div slot="backdrop" class="fixed inset-0 z-40 max-h-full max-w-full bg-black/20 backdrop-blur"></div>`;
    }
  })} ${validate_component(Toast, "Toast").$$render($$result, {}, {}, {})}`;
});
export {
  Layout as default
};
