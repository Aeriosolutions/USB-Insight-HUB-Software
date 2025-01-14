import { c as create_ssr_component, a as subscribe, v as validate_component, h as add_attribute } from "../../../../chunks/ssr.js";
import { S as SettingsCard } from "../../../../chunks/SettingsCard.js";
import { Chart, registerables } from "chart.js";
import { a as analytics, R as Report_analytics } from "../../../../chunks/report-analytics.js";
import "../../../../chunks/user.js";
import { p as page } from "../../../../chunks/stores.js";
import { g as goto } from "../../../../chunks/navigation.js";
const SystemMetrics = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $$unsubscribe_analytics;
  $$unsubscribe_analytics = subscribe(analytics, (value) => value);
  Chart.register(...registerables);
  let heapChartElement;
  let filesystemChartElement;
  let temperatureChartElement;
  $$unsubscribe_analytics();
  return `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
    title: () => {
      return `<span slot="title" data-svelte-h="svelte-o3oys9">System Metrics</span>`;
    },
    icon: () => {
      return `${validate_component(Report_analytics, "Metrics").$$render(
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
      return `<div class="w-full overflow-x-auto"><div class="flex w-full flex-col space-y-1 h-60"><canvas${add_attribute("this", heapChartElement, 0)}></canvas></div></div> <div class="w-full overflow-x-auto"><div class="flex w-full flex-col space-y-1 h-52"><canvas${add_attribute("this", filesystemChartElement, 0)}></canvas></div></div> <div class="w-full overflow-x-auto"><div class="flex w-full flex-col space-y-1 h-52"><canvas${add_attribute("this", temperatureChartElement, 0)}></canvas></div></div>`;
    }
  })}`;
});
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $page, $$unsubscribe_page;
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  let { data } = $$props;
  if (!$page.data.features.analytics) {
    goto("/");
  }
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  $$unsubscribe_page();
  return `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${validate_component(SystemMetrics, "SystemMetrics").$$render($$result, {}, {}, {})}</div>`;
});
export {
  Page as default
};
