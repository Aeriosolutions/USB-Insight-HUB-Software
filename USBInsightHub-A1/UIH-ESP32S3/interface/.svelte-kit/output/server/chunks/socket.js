import { w as writable } from "./index.js";
import msgpack from "msgpack-lite";
function createWebSocket() {
  let listeners = /* @__PURE__ */ new Map();
  const { subscribe, set } = writable(false);
  const socketEvents = ["open", "close", "error", "message", "unresponsive"];
  let unresponsiveTimeoutId;
  let reconnectTimeoutId;
  let ws;
  let socketUrl;
  let event_use_json = false;
  function init(url, use_json = false) {
    socketUrl = url;
    event_use_json = use_json;
    connect();
  }
  function disconnect(reason, event) {
    ws.close();
    set(false);
    clearTimeout(unresponsiveTimeoutId);
    clearTimeout(reconnectTimeoutId);
    listeners.get(reason)?.forEach((listener) => listener(event));
    reconnectTimeoutId = setTimeout(connect, 1e3);
  }
  function connect() {
    ws = new WebSocket(socketUrl);
    ws.binaryType = "arraybuffer";
    ws.onopen = (ev) => {
      set(true);
      clearTimeout(reconnectTimeoutId);
      listeners.get("open")?.forEach((listener) => listener(ev));
      for (const event of listeners.keys()) {
        if (socketEvents.includes(event))
          continue;
        sendEvent("subscribe", event);
      }
    };
    ws.onmessage = (message) => {
      resetUnresponsiveCheck();
      let payload = message.data;
      const binary = payload instanceof ArrayBuffer;
      listeners.get(binary ? "binary" : "message")?.forEach((listener) => listener(payload));
      try {
        payload = binary ? msgpack.decode(new Uint8Array(payload)) : JSON.parse(payload);
      } catch (error) {
        listeners.get("error")?.forEach((listener) => listener(error));
        return;
      }
      listeners.get("json")?.forEach((listener) => listener(payload));
      const { event, data } = payload;
      if (event)
        listeners.get(event)?.forEach((listener) => listener(data));
    };
    ws.onerror = (ev) => disconnect("error", ev);
    ws.onclose = (ev) => disconnect("close", ev);
  }
  function unsubscribe(event, listener) {
    let eventListeners = listeners.get(event);
    if (!eventListeners)
      return;
    if (!eventListeners.size) {
      sendEvent("unsubscribe", event);
    }
    if (listener) {
      eventListeners?.delete(listener);
    } else {
      listeners.delete(event);
    }
  }
  function resetUnresponsiveCheck() {
    clearTimeout(unresponsiveTimeoutId);
    unresponsiveTimeoutId = setTimeout(() => disconnect("unresponsive"), 2e3);
  }
  function send(msg) {
    if (!ws || ws.readyState !== WebSocket.OPEN)
      return;
    if (event_use_json) {
      ws.send(JSON.stringify(msg));
    } else {
      ws.send(msgpack.encode(msg));
    }
  }
  function sendEvent(event, data) {
    send({ event, data });
  }
  return {
    subscribe,
    send,
    sendEvent,
    init,
    on: (event, listener) => {
      let eventListeners = listeners.get(event);
      if (!eventListeners) {
        if (!socketEvents.includes(event)) {
          sendEvent("subscribe", event);
        }
        eventListeners = /* @__PURE__ */ new Set();
        listeners.set(event, eventListeners);
      }
      eventListeners.add(listener);
      return () => {
        unsubscribe(event, listener);
      };
    },
    off: (event, listener) => {
      unsubscribe(event, listener);
    }
  };
}
const socket = createWebSocket();
export {
  socket as s
};
