import { c as create_ssr_component, b as spread, d as escape_object, a as subscribe, v as validate_component, i as is_promise, n as noop, e as each, h as add_attribute, f as escape } from "../../../../chunks/ssr.js";
import { u as user } from "../../../../chunks/user.js";
import { p as page } from "../../../../chunks/stores.js";
import "mousetrap";
import { S as SettingsCard } from "../../../../chunks/SettingsCard.js";
import { A as Alert_triangle } from "../../../../chunks/alert-triangle.js";
import { S as Spinner } from "../../../../chunks/Spinner.js";
import { compareVersions } from "compare-versions";
const Brand_github = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M9 19c-4.3 1.4-4.3-2.5-6-3m12 5v-3.5c0-1 .1-1.4-.5-2c2.8-.3 5.5-1.4 5.5-6a4.6 4.6 0 0 0-1.3-3.2a4.2 4.2 0 0 0-.1-3.2s-1.1-.3-3.5 1.3a12.3 12.3 0 0 0-6.2 0C6.5 2.8 5.4 3.1 5.4 3.1a4.2 4.2 0 0 0-.1 3.2A4.6 4.6 0 0 0 4 9.5c0 4.6 2.7 5.7 5.5 6c-.6.6-.6 1.2-.5 2V21"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Cloud_download = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 18a3.5 3.5 0 0 0 0-7h-1A5 4.5 0 0 0 7 9a4.6 4.4 0 0 0-2.1 8.4M12 13v9m-3-3l3 3l3-3"/>`}<!-- HTML_TAG_END --></svg>`;
});
const File_upload = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M14 3v4a1 1 0 0 0 1 1h4"/><path d="M17 21H7a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h7l5 5v11a2 2 0 0 1-2 2zm-5-10v6"/><path d="M9.5 13.5L12 11l2.5 2.5"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const UploadFirmware = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $$unsubscribe_user;
  let $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => value);
  $$unsubscribe_page = subscribe(page, (value) => value);
  $$unsubscribe_user();
  $$unsubscribe_page();
  return `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
    title: () => {
      return `<span slot="title" data-svelte-h="svelte-18zv7sf">Upload Firmware</span>`;
    },
    icon: () => {
      return `${validate_component(File_upload, "OTA").$$render(
        $$result,
        {
          slot: "icon",
          class: "lex-shrink-0 mr-2 h-6 w-6 self-end rounded-full"
        },
        {},
        {}
      )}`;
    },
    default: () => {
      return `<div class="alert alert-warning shadow-lg">${validate_component(Alert_triangle, "Warning").$$render($$result, { class: "h-6 w-6 flex-shrink-0" }, {}, {})} <span data-svelte-h="svelte-1g05xjv">Uploading a new firmware (.bin) file will replace the existing firmware. You may upload a
			(.md5) file first to verify the uploaded firmware.</span></div> <input type="file" id="binFile" class="file-input file-input-bordered file-input-secondary mt-4 w-full" accept=".bin,.md5">`;
    }
  })}`;
});
const Test_pipe = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M20 8.04L7.878 20.164a2.857 2.857 0 1 1-4.041-4.04L15.959 4M7 13h8m4 2l1.5 1.6a2 2 0 1 1-3 0L19 15zM15 3l6 6"/>`}<!-- HTML_TAG_END --></svg>`;
});
const GithubFirmwareManager = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $page, $$unsubscribe_page;
  let $$unsubscribe_user;
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  $$unsubscribe_user = subscribe(user, (value) => value);
  async function getGithubAPI() {
    try {
      const githubResponse = await fetch("https://api.github.com/repos/" + $page.data.github + "/releases", {
        method: "GET",
        headers: {
          accept: "application/vnd.github+json",
          "X-GitHub-Api-Version": "2022-11-28"
        }
      });
      const results = await githubResponse.json();
      return results;
    } catch (error) {
      console.error("Error:", error);
    }
    return;
  }
  $$unsubscribe_page();
  $$unsubscribe_user();
  return `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
    title: () => {
      return `<span slot="title" data-svelte-h="svelte-1iezg3y">Github Firmware Manager</span>`;
    },
    icon: () => {
      return `${validate_component(Brand_github, "Github").$$render(
        $$result,
        {
          slot: "icon",
          class: "lex-shrink-0 mr-2 h-6 w-6 self-end rounded-full"
        },
        {},
        {}
      )}`;
    },
    default: () => {
      return `${function(__value) {
        if (is_promise(__value)) {
          __value.then(null, noop);
          return ` ${validate_component(Spinner, "Spinner").$$render($$result, {}, {}, {})} `;
        }
        return function(githubReleases) {
          return ` <div class="relative w-full overflow-visible"><div class="overflow-x-auto"><table class="table w-full table-auto"><thead data-svelte-h="svelte-yq4ktl"><tr class="font-bold"><th align="left">Release</th> <th align="center" class="hidden sm:block">Release Date</th> <th align="center">Experimental</th> <th align="center">Install</th></tr></thead> <tbody>${each(githubReleases, (release) => {
            return `<tr${add_attribute(
              "class",
              compareVersions($page.data.features.firmware_version, release.tag_name) === 0 ? "bg-primary text-primary-content" : "bg-base-100 h-14",
              0
            )}><td align="left" class="text-base font-semibold"><a${add_attribute("href", release.html_url, 0)} class="link link-hover" target="_blank" rel="noopener noreferrer">${escape(release.name)}</a></td> <td align="center" class="hidden min-h-full align-middle sm:block"><div class="my-2">${escape(new Intl.DateTimeFormat("en-GB", { dateStyle: "medium" }).format(new Date(release.published_at)))} </div></td> <td align="center">${release.prerelease ? `${validate_component(Test_pipe, "Prerelease").$$render($$result, { class: "text-accent h-5 w-5" }, {}, {})}` : ``}</td> <td align="center">${compareVersions($page.data.features.firmware_version, release.tag_name) != 0 ? `<button class="btn btn-ghost btn-circle btn-sm">${validate_component(Cloud_download, "CloudDown").$$render($$result, { class: "text-secondary h-6 w-6" }, {}, {})} </button>` : ``}</td> </tr>`;
          })}</tbody></table></div></div> `;
        }(__value);
      }(getGithubAPI())}`;
    }
  })}`;
});
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $page, $$unsubscribe_page;
  let $user, $$unsubscribe_user;
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  let { data } = $$props;
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  $$unsubscribe_page();
  $$unsubscribe_user();
  return `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${$page.data.features.download_firmware && (!$page.data.features.security || $user.admin) ? `${validate_component(GithubFirmwareManager, "GithubFirmwareManager").$$render($$result, {}, {}, {})}` : ``} ${$page.data.features.upload_firmware && (!$page.data.features.security || $user.admin) ? `${validate_component(UploadFirmware, "UploadFirmware").$$render($$result, {}, {}, {})}` : ``}</div>`;
});
export {
  Page as default
};
