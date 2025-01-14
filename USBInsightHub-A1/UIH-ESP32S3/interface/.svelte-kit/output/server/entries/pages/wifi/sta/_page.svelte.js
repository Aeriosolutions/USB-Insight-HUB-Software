import { c as create_ssr_component, a as subscribe, g as createEventDispatcher, f as escape, j as null_to_empty, h as add_attribute, e as each, k as set_store_value, b as spread, d as escape_object, o as onDestroy, v as validate_component, i as is_promise, n as noop } from "../../../../chunks/ssr.js";
import { u as user } from "../../../../chunks/user.js";
import { p as page } from "../../../../chunks/stores.js";
import { w as writable } from "../../../../chunks/index.js";
import { S as SettingsCard, C as Chevron_down } from "../../../../chunks/SettingsCard.js";
import "mousetrap";
import { R as Router, W as Wifi$1 } from "../../../../chunks/router.js";
import { A as Access_point } from "../../../../chunks/access-point.js";
import { S as Spinner } from "../../../../chunks/Spinner.js";
import { H as Home } from "../../../../chunks/home.js";
import { P as Pencil, T as Trash } from "../../../../chunks/pencil.js";
var Direction;
(function(Direction2) {
  Direction2[Direction2["Vertical"] = 0] = "Vertical";
  Direction2[Direction2["Horizontal"] = 1] = "Horizontal";
})(Direction || (Direction = {}));
var EventType;
(function(EventType2) {
  EventType2[EventType2["Programatic"] = 0] = "Programatic";
  EventType2[EventType2["UserDrag"] = 1] = "UserDrag";
  EventType2[EventType2["UserCopy"] = 2] = "UserCopy";
})(EventType || (EventType = {}));
const DragDropList_svelte_svelte_type_style_lang = "";
const css = {
  code: "div[data-dnd-zone].svelte-5t9ew.svelte-5t9ew.svelte-5t9ew{display:flex;position:relative;height:100%;width:100%;overflow:auto;overflow-anchor:none}div[data-dnd-zone].horizontal.svelte-5t9ew.svelte-5t9ew.svelte-5t9ew{flex-direction:row}div[data-dnd-zone].vertical.svelte-5t9ew.svelte-5t9ew.svelte-5t9ew{flex-direction:column}div[data-dnd-zone].center.horizontal.svelte-5t9ew div[data-dnd-item].svelte-5t9ew.svelte-5t9ew:first-child,div[data-dnd-zone].center.horizontal.svelte-5t9ew div[data-dnd-item][data-dnd-dragging].svelte-5t9ew:first-child+.svelte-5t9ew{margin-left:auto}div[data-dnd-zone].center.horizontal.svelte-5t9ew div[data-dnd-item].svelte-5t9ew.svelte-5t9ew:last-child{margin-right:auto}div[data-dnd-zone].center.horizontal.svelte-5t9ew div[data-dnd-placeholder]:first-child{margin-left:auto}div[data-dnd-zone].center.horizontal.svelte-5t9ew div[data-dnd-placeholder]:last-child{margin-right:auto}div[data-dnd-zone].svelte-5t9ew div[data-dnd-placeholder]{flex-shrink:0;flex-grow:0}div[data-dnd-item].svelte-5t9ew.svelte-5t9ew.svelte-5t9ew{position:relative;user-select:none;touch-action:none;flex-shrink:0;flex-grow:0}*[data-dnd-handle]{cursor:grab !important}",
  map: null
};
new Array();
let active = void 0;
let raf;
const dragging = writable(void 0);
const DragDropList = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let itemStyle;
  let $dragging, $$unsubscribe_dragging;
  $$unsubscribe_dragging = subscribe(dragging, (value) => $dragging = value);
  let { id } = $$props;
  let { itemCount } = $$props;
  let { itemSize } = $$props;
  let { type } = $$props;
  let { priority = 1 } = $$props;
  let { scrollSpeed = 15 } = $$props;
  let { itemClass = "" } = $$props;
  let { zoneClass = "" } = $$props;
  let { keyFn = (i) => i } = $$props;
  let { useHandle = false } = $$props;
  let { allowDrag = () => true } = $$props;
  let { copy = false } = $$props;
  const dropzone = new type(id, priority, itemCount, itemSize);
  const dispatch = createEventDispatcher();
  let items = new Array(itemCount);
  function finalizeDrag(ev) {
    const { el, destZone, sourceZone, sourceIndex, resetZones, placeholder, type: type2 } = active;
    const hoverIndex = active.hoverIndex ?? sourceIndex;
    if (el && type2 === EventType.UserCopy) {
      el.remove();
    }
    if (ev && ev.target !== el) {
      return;
    }
    if (raf)
      cancelAnimationFrame(raf);
    raf = void 0;
    const from = {
      dropZoneID: sourceZone.id,
      index: sourceIndex
    };
    const to = destZone ? destZone === sourceZone && hoverIndex === sourceIndex ? from : {
      dropZoneID: destZone.id,
      index: hoverIndex
    } : void 0;
    dispatch("drop", { from, to });
    if (placeholder) {
      sourceZone.el.removeChild(placeholder);
    }
    resetZones.forEach((zone) => zone.styleRemove());
    el.removeEventListener("transitionend", finalizeDrag);
    active.onResolve?.();
    active = void 0;
    set_store_value(dragging, $dragging = void 0, $dragging);
  }
  async function move(srcIndex, destIndex, destZone, transitionDur = 500) {
    return new Promise((resolve) => {
      if (active !== void 0) {
        resolve();
        return;
      }
      const el = dropzone.items[srcIndex];
      if (!el) {
        resolve();
        return;
      }
      {
        const tx = dropzone.dragXOffset(srcIndex);
        const ty = dropzone.dragYOffset(srcIndex);
        const height = dropzone.itemHeight();
        const width = dropzone.itemWidth();
        el.style.cssText = `
					z-index: 1000;
					height: ${height}px;
					width: ${width}px;
					position: fixed;
					transform: translate(${tx}px,${ty}px);
				`;
      }
      dropzone.styleSourceMove(srcIndex, srcIndex, false);
      if (destZone !== dropzone) {
        setTimeout(
          () => {
            active?.type === EventType.Programatic && dropzone.styleSourceMissing(srcIndex);
          },
          transitionDur * 0.4
        );
        destZone.styleDestMove(destIndex);
      } else {
        setTimeout(
          () => {
            active?.type === EventType.Programatic && dropzone.styleSourceMove(destIndex, srcIndex, true);
          },
          transitionDur * 0.25
        );
      }
      active = {
        type: EventType.Programatic,
        el,
        placeholder: void 0,
        resetZones: /* @__PURE__ */ new Set([dropzone, destZone]),
        sourceIndex: srcIndex,
        hoverIndex: destIndex,
        sourceZone: dropzone,
        destZone,
        dragLeft: 0,
        dragTop: 0,
        onResolve: resolve
      };
      set_store_value(dragging, $dragging = active, $dragging);
      {
        const tx = destZone.dragXOffset(destIndex, destZone.count + 1);
        const ty = destZone.dragYOffset(destIndex, destZone.count + 1);
        const height = destZone.itemHeight();
        const width = destZone.itemWidth();
        el.addEventListener("transitionend", finalizeDrag);
        el.style.cssText = `
					z-index: 1000; 
					position: fixed; 
					top:0; left: 0;
					height: ${height}px; 
					width: ${width}px; 
					transform: translate(${tx}px,${ty}px); 
					transition:
						transform ${transitionDur}ms cubic-bezier(0.2, 0, 0, 1), 
						height ${transitionDur}ms cubic-bezier(0.2, 0, 0, 1), 
						width ${transitionDur}ms cubic-bezier(0.2, 0, 0, 1);
				`;
      }
    });
  }
  async function remove(index, transitionDur = 500) {
    return new Promise((resolve) => {
      if (active !== void 0) {
        resolve();
        return;
      }
      const el = dropzone.items[index];
      if (!el) {
        resolve();
        return;
      }
      el.style.cssText = `height:${dropzone.itemHeight()}px; width:${dropzone.itemWidth()}px; position:fixed; opacity: 1;`;
      dropzone.styleSourceMove(index, index, false);
      setTimeout(
        () => {
          active?.type === EventType.Programatic && dropzone.styleSourceMissing(index);
        },
        transitionDur * 0.4
      );
      active = {
        type: EventType.Programatic,
        el,
        placeholder: void 0,
        resetZones: /* @__PURE__ */ new Set([dropzone]),
        sourceIndex: index,
        hoverIndex: index,
        sourceZone: dropzone,
        destZone: void 0,
        dragLeft: 0,
        dragTop: 0,
        onResolve: resolve
      };
      set_store_value(dragging, $dragging = active, $dragging);
      {
        const tx = dropzone.dragXOffset(index, dropzone.count + 1);
        const ty = dropzone.dragYOffset(index, dropzone.count + 1);
        const height = dropzone.itemHeight();
        const width = dropzone.itemWidth();
        el.addEventListener("transitionend", finalizeDrag);
        el.style.cssText = `
					position: fixed; 
					top:0; left: 0;
					height: ${height}px; 
					width: ${width}px; 
					opacity: 0.6;
					transform: translate(${tx}px,${ty}px); 
					transition:
						opacity ${transitionDur}ms cubic-bezier(0.2, 0, 0, 1);
						transform ${transitionDur}ms cubic-bezier(0.2, 0, 0, 1), 
						height ${transitionDur}ms cubic-bezier(0.2, 0, 0, 1), 
						width ${transitionDur}ms cubic-bezier(0.2, 0, 0, 1);
				`;
      }
    });
  }
  if ($$props.id === void 0 && $$bindings.id && id !== void 0)
    $$bindings.id(id);
  if ($$props.itemCount === void 0 && $$bindings.itemCount && itemCount !== void 0)
    $$bindings.itemCount(itemCount);
  if ($$props.itemSize === void 0 && $$bindings.itemSize && itemSize !== void 0)
    $$bindings.itemSize(itemSize);
  if ($$props.type === void 0 && $$bindings.type && type !== void 0)
    $$bindings.type(type);
  if ($$props.priority === void 0 && $$bindings.priority && priority !== void 0)
    $$bindings.priority(priority);
  if ($$props.scrollSpeed === void 0 && $$bindings.scrollSpeed && scrollSpeed !== void 0)
    $$bindings.scrollSpeed(scrollSpeed);
  if ($$props.itemClass === void 0 && $$bindings.itemClass && itemClass !== void 0)
    $$bindings.itemClass(itemClass);
  if ($$props.zoneClass === void 0 && $$bindings.zoneClass && zoneClass !== void 0)
    $$bindings.zoneClass(zoneClass);
  if ($$props.keyFn === void 0 && $$bindings.keyFn && keyFn !== void 0)
    $$bindings.keyFn(keyFn);
  if ($$props.useHandle === void 0 && $$bindings.useHandle && useHandle !== void 0)
    $$bindings.useHandle(useHandle);
  if ($$props.allowDrag === void 0 && $$bindings.allowDrag && allowDrag !== void 0)
    $$bindings.allowDrag(allowDrag);
  if ($$props.copy === void 0 && $$bindings.copy && copy !== void 0)
    $$bindings.copy(copy);
  if ($$props.dropzone === void 0 && $$bindings.dropzone && dropzone !== void 0)
    $$bindings.dropzone(dropzone);
  if ($$props.move === void 0 && $$bindings.move && move !== void 0)
    $$bindings.move(move);
  if ($$props.remove === void 0 && $$bindings.remove && remove !== void 0)
    $$bindings.remove(remove);
  $$result.css.add(css);
  dropzone.id = id;
  {
    if (itemCount != dropzone.count || itemSize !== dropzone.itemSize) {
      dropzone.count = itemCount;
      dropzone.itemSize = itemSize;
      items = new Array(itemCount);
      if (dropzone.el) {
        dropzone.styleContainerBaseStyle();
      }
    }
  }
  itemStyle = `${dropzone.direction === Direction.Vertical ? "height" : "width"}: ${itemSize}px;`;
  $$unsubscribe_dragging();
  return `   <div data-dnd-zone class="${escape(null_to_empty(`${zoneClass} ${dropzone.containerClass}`), true) + " svelte-5t9ew"}"${add_attribute("this", dropzone.el, 0)}>${each(items, (_, i) => {
    return `<div data-dnd-item${add_attribute(
      "data-dnd-dragging",
      active?.sourceIndex === i && active?.sourceZone.id === id || $dragging === null ? true : void 0,
      0
    )} class="${escape(null_to_empty(itemClass), true) + " svelte-5t9ew"}"${add_attribute("style", itemStyle, 0)}${add_attribute("this", dropzone.items[i], 0)}>${slots.default ? slots.default({
      index: i,
      drag: $dragging?.sourceZone?.id === id || $dragging?.destZone?.id === id ? $dragging : void 0
    }) : ``} </div>`;
  })} </div>`;
});
class VerticalDropZone {
  direction = Direction.Vertical;
  id;
  priority;
  itemSize;
  count;
  el;
  items;
  containerClass;
  constructor(id, count, priority, itemSize) {
    this.id = id;
    this.priority = priority;
    this.count = count;
    this.itemSize = itemSize;
    this.items = new Array(count);
    this.el = void 0;
    this.containerClass = "vertical";
  }
  pointIndex(x, y) {
    const { el, itemSize, count } = this;
    const b = el.getBoundingClientRect();
    const top = b.top - el.scrollTop;
    const rawOver = Math.floor((y - top) / itemSize);
    return Math.min(Math.max(rawOver, 0), count);
  }
  placeholderStyleStr() {
    return `height: ${this.itemSize}px; width: 100%;`;
  }
  dragXOffset(index) {
    const b = this.el.getBoundingClientRect();
    return b.left;
  }
  dragYOffset(index) {
    const b = this.el.getBoundingClientRect();
    return index * this.itemSize + b.top - this.el.scrollTop;
  }
  itemHeight() {
    return this.itemSize;
  }
  itemWidth() {
    return this.el.clientWidth;
  }
  styleSourceMove(hover, source, transition) {
    const { items, itemSize } = this;
    for (let i = 0; i < items.length; ++i) {
      const base = hover > source && (i < source || i > source && i <= hover) || hover < source && i < hover || hover == source && i < source;
      const raise = hover > source && i > hover || hover < source && (i >= hover && i < source || i > source) || hover == source && i > source;
      const item = items[i];
      if (base) {
        item && (item.style.cssText = `transform: translateY(0px); transition: transform 0.2s cubic-bezier(0.2, 0, 0, 1); height: ${itemSize}px;`);
      } else if (raise) {
        if (transition) {
          item && (item.style.cssText = `transform: translateY(${itemSize}px); transition: transform 0.2s cubic-bezier(0.2, 0, 0, 1); height: ${itemSize}px;`);
        } else {
          item && (item.style.cssText = `transform: translateY(${itemSize}px); height: ${itemSize}px;`);
        }
      }
    }
  }
  styleSourceMissing(index) {
    const { items, itemSize } = this;
    for (let i = 0; i < items.length; ++i) {
      const item = items[i];
      item && i !== index && (items[i].style.cssText = `transform: translateY(0px); transition: transform 0.2s cubic-bezier(0.2, 0, 0, 1); height: ${itemSize}px;`);
    }
  }
  styleDestMove(index) {
    const { items, itemSize, el } = this;
    el.style.cssText = `transition: padding-bottom 0.2s cubic-bezier(0.2, 0, 0, 1); padding-bottom: ${itemSize}px;`;
    for (let i = 0; i < items.length; ++i) {
      const item = items[i];
      if (i < index) {
        item && (item.style.cssText = `transform: translateY(0px); transition: transform 0.2s cubic-bezier(0.2, 0, 0, 1); height: ${itemSize}px;`);
      } else {
        item && (item.style.cssText = `transform: translateY(${itemSize}px); transition: transform 0.2s cubic-bezier(0.2, 0, 0, 1); height: ${itemSize}px;`);
      }
    }
  }
  styleDestReset() {
    const { items, itemSize, el } = this;
    el.style.cssText = `transition: padding-bottom 0.2s cubic-bezier(0.2, 0, 0, 1); padding-bottom: 0px;`;
    for (let i = 0; i < items.length; ++i) {
      const item = items[i];
      item && (items[i].style.cssText = `transform:translateY(0px); transition:transform 0.2s cubic-bezier(0.2, 0, 0, 1); height: ${itemSize}px;`);
    }
  }
  styleRemove() {
    this.styleContainerBaseStyle();
  }
  styleContainerBaseStyle() {
    const { items, itemSize, el } = this;
    el.style.cssText = "";
    for (let i = 0; i < items.length; ++i) {
      const item = items[i];
      item && (item.style.cssText = `height: ${itemSize}px;`);
    }
  }
}
const Radar_2 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M11 12a1 1 0 1 0 2 0a1 1 0 1 0-2 0"/><path d="M15.51 15.56A5 5 0 1 0 12 17"/><path d="M18.832 17.86A9 9 0 1 0 12 21m0-9v9"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Circle_plus = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 12a9 9 0 1 0 18 0a9 9 0 0 0-18 0m6 0h6m-3-3v6"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Wifi = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $user, $$unsubscribe_user;
  let $page, $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  let wifiStatus;
  let wifiSettings;
  let dndNetworkList = [];
  let formField;
  async function getWifiStatus() {
    try {
      const response = await fetch("/rest/wifiStatus", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      wifiStatus = await response.json();
    } catch (error) {
      console.error("Error:", error);
    }
    return wifiStatus;
  }
  async function getWifiSettings() {
    try {
      const response = await fetch("/rest/wifiSettings", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      wifiSettings = await response.json();
    } catch (error) {
      console.error("Error:", error);
    }
    dndNetworkList = wifiSettings.wifi_networks;
    return wifiSettings;
  }
  const interval = setInterval(
    async () => {
      getWifiStatus();
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
    $$rendered = `  ${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
      title: () => {
        return `<span slot="title" data-svelte-h="svelte-15ucxea">WiFi Connection</span>`;
      },
      icon: () => {
        return `${validate_component(Router, "Router").$$render(
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
            return ` <div class="flex w-full flex-col space-y-1"><div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="${"mask mask-hexagon h-auto w-10 " + escape(wifiStatus.status === 3 ? "bg-success" : "bg-error", true)}">${validate_component(Access_point, "AP").$$render(
              $$result,
              {
                class: "h-auto w-full scale-75 " + (wifiStatus.status === 3 ? "text-success-content" : "text-error-content")
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-yizsky">Status</div> <div class="text-sm opacity-75">${escape(wifiStatus.status === 3 ? "Connected" : "Inactive")}</div></div></div> ${wifiStatus.status === 3 ? `<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Router, "SSID").$$render(
              $$result,
              {
                class: "text-primary-content h-auto w-full scale-75"
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-zh9z0x">SSID</div> <div class="text-sm opacity-75">${escape(wifiStatus.ssid)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Home, "Home").$$render(
              $$result,
              {
                class: "text-primary-content h-auto w-full scale-75"
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-cofw3n">IP Address</div> <div class="text-sm opacity-75">${escape(wifiStatus.local_ip)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Wifi$1, "WiFi").$$render(
              $$result,
              {
                class: "text-primary-content h-auto w-full scale-75"
              },
              {},
              {}
            )}</div> <div><div class="font-bold" data-svelte-h="svelte-1r08m3l">RSSI</div> <div class="text-sm opacity-75">${escape(wifiStatus.rssi)} dBm</div></div> <div class="grow"></div> <button class="btn btn-circle btn-ghost btn-sm modal-button">${validate_component(Chevron_down, "Down").$$render(
              $$result,
              {
                class: "text-base-content h-auto w-6 transition-transform duration-300 ease-in-out "
              },
              {},
              {}
            )}</button></div>` : ``}</div>  ${``} `;
          }();
        }(getWifiStatus())}</div> ${!$page.data.features.security || $user.admin ? `<div class="bg-base-200 shadow-lg relative grid w-full max-w-2xl self-center overflow-hidden"><div class="h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium" data-svelte-h="svelte-o884ho">Saved Networks</div> ${function(__value) {
          if (is_promise(__value)) {
            __value.then(null, noop);
            return ` ${validate_component(Spinner, "Spinner").$$render($$result, {}, {}, {})} `;
          }
          return function(nothing) {
            return ` <div class="relative w-full overflow-visible"><button class="btn btn-primary text-primary-content btn-md absolute -top-14 right-16">${validate_component(Circle_plus, "Add").$$render($$result, { class: "h-6 w-6" }, {}, {})}</button> <button class="btn btn-primary text-primary-content btn-md absolute -top-14 right-0">${validate_component(Radar_2, "Scan").$$render($$result, { class: "h-6 w-6" }, {}, {})}</button> <div class="overflow-x-auto space-y-1">${validate_component(DragDropList, "DragDropList").$$render(
              $$result,
              {
                id: "networks",
                type: VerticalDropZone,
                itemSize: 60,
                itemCount: dndNetworkList.length
              },
              {},
              {
                default: ({ index }) => {
                  return ` <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10 shrink-0">${validate_component(Router, "Router").$$render(
                    $$result,
                    {
                      class: "text-primary-content h-auto w-full scale-75"
                    },
                    {},
                    {}
                  )}</div> <div><div class="font-bold">${escape(dndNetworkList[index].ssid)}</div></div> ${!$page.data.features.security || $user.admin ? `<div class="flex-grow"></div> <div class="space-x-0 px-0 mx-0"><button class="btn btn-ghost btn-sm">${validate_component(Pencil, "Edit").$$render($$result, { class: "h-6 w-6" }, {}, {})}</button> <button class="btn btn-ghost btn-sm">${validate_component(Trash, "Delete").$$render($$result, { class: "text-error h-6 w-6" }, {}, {})}</button></div>` : ``}</div>`;
                }
              }
            )}</div></div> <div class="divider mb-0"></div> <div class="flex flex-col gap-2 p-0"><form class="" novalidate${add_attribute("this", formField, 0)}><div class="grid w-full grid-cols-1 content-center gap-x-4 px-4 sm:grid-cols-2"><div><label class="label" for="channel" data-svelte-h="svelte-16gychd"><span class="label-text text-md">Host Name</span></label> <input type="text" min="1" max="32" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="channel" required${add_attribute("value", wifiSettings.hostname, 0)}> <label class="label" for="channel"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Host name must be between 2 and 32 characters long</span></label></div> <label class="label inline-flex cursor-pointer content-end justify-start gap-4"><input type="checkbox" class="checkbox checkbox-primary sm:-mb-5"${add_attribute("checked", wifiSettings.priority_RSSI, 1)}> <span class="sm:-mb-5" data-svelte-h="svelte-215cz6">Connect to strongest WiFi</span></label></div> ${``} <div class="divider mb-2 mt-0"></div> <div class="mx-4 mb-4 flex flex-wrap justify-end gap-2"><button class="btn btn-primary" type="submit" ${"disabled"}>${escape("Add Network")}</button> <button class="btn btn-primary" type="button" data-svelte-h="svelte-1xca8zi">Apply Settings</button></div></form></div> `;
          }();
        }(getWifiSettings())}</div>` : ``}`;
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
  return `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${validate_component(Wifi, "Wifi").$$render($$result, {}, {}, {})}</div>`;
});
export {
  Page as default
};
