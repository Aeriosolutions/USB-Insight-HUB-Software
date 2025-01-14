const prerender = false;
const ssr = false;
const load = async ({ fetch }) => {
  const result = await fetch("/rest/features");
  const item = await result.json();
  return {
    features: item,
    title: "USB-Insight-Hub",
    github: "theelims/ESP32-sveltekit",
    copyright: "2025 Aerio Solutions",
    appName: "USB Insight Hub"
  };
};
export {
  load,
  prerender,
  ssr
};
