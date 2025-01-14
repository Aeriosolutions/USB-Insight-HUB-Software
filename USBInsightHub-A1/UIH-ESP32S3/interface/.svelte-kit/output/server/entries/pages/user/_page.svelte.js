import { c as create_ssr_component, b as spread, d as escape_object, a as subscribe, v as validate_component, i as is_promise, n as noop, e as each, f as escape } from "../../../chunks/ssr.js";
import { g as goto } from "../../../chunks/navigation.js";
import { u as user } from "../../../chunks/user.js";
import { p as page } from "../../../chunks/stores.js";
import { I as InputPassword } from "../../../chunks/InputPassword.js";
import { S as SettingsCard } from "../../../chunks/SettingsCard.js";
import "mousetrap";
import { S as Spinner } from "../../../chunks/Spinner.js";
import { P as Pencil, T as Trash } from "../../../chunks/pencil.js";
import { U as Users } from "../../../chunks/users.js";
import { A as Alert_triangle } from "../../../chunks/alert-triangle.js";
const User_plus = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M8 7a4 4 0 1 0 8 0a4 4 0 0 0-8 0m8 12h6m-3-3v6M6 21v-2a4 4 0 0 1 4-4h4"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Key = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="m16.555 3.843l3.602 3.602a2.877 2.877 0 0 1 0 4.069l-2.643 2.643a2.877 2.877 0 0 1-4.069 0l-.301-.301l-6.558 6.558a2 2 0 0 1-1.239.578L5.172 21H4a1 1 0 0 1-.993-.883L3 20v-1.172a2 2 0 0 1 .467-1.284l.119-.13L4 17h2v-2h2v-2l2.144-2.144l-.301-.301a2.877 2.877 0 0 1 0-4.069l2.643-2.643a2.877 2.877 0 0 1 4.069 0zM15 9h.01"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $user, $$unsubscribe_user;
  let $page, $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  let { data } = $$props;
  let securitySettings;
  async function getSecuritySettings() {
    try {
      const response = await fetch("/rest/securitySettings", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      securitySettings = await response.json();
    } catch (error) {
      console.error("Error:", error);
    }
    return;
  }
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  let $$settled;
  let $$rendered;
  let previous_head = $$result.head;
  do {
    $$settled = true;
    $$result.head = previous_head;
    $$rendered = `${$user.admin ? `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
      title: () => {
        return `<span slot="title" data-svelte-h="svelte-je9y8g">Manage Users</span>`;
      },
      icon: () => {
        return `${validate_component(Users, "Users").$$render(
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
        return `${function(__value) {
          if (is_promise(__value)) {
            __value.then(null, noop);
            return ` ${validate_component(Spinner, "Spinner").$$render($$result, {}, {}, {})} `;
          }
          return function(nothing) {
            return ` <div class="relative w-full overflow-visible"><button class="btn btn-primary text-primary-content btn-md absolute -top-14 right-0">${validate_component(User_plus, "AddUser").$$render($$result, { class: "h-6 w-6" }, {}, {})}</button> <div class="overflow-x-auto"><table class="table w-full table-auto"><thead data-svelte-h="svelte-176ku3j"><tr class="font-bold"><th align="left">Username</th> <th align="center">Admin</th> <th align="right" class="pr-8">Edit</th></tr></thead> <tbody>${each(securitySettings.users, (user2, index) => {
              return `<tr><td align="left">${escape(user2.username)}</td> <td align="center">${user2.admin ? `${validate_component(Key, "Admin").$$render($$result, { class: "text-secondary" }, {}, {})}` : ``}</td> <td align="right"><span class="my-auto inline-flex flex-row space-x-2"><button class="btn btn-ghost btn-circle btn-xs">${validate_component(Pencil, "Edit").$$render($$result, { class: "h-6 w-6" }, {}, {})}</button> <button class="btn btn-ghost btn-circle btn-xs">${validate_component(Trash, "Delete").$$render($$result, { class: "text-error h-6 w-6" }, {}, {})}</button> </span></td> </tr>`;
            })}</tbody></table></div></div> <div class="divider mb-0"></div> <span class="pb-2 text-xl font-medium" data-svelte-h="svelte-1arxk0i">Security Settings</span> <div class="alert alert-warning shadow-lg">${validate_component(Alert_triangle, "Warning").$$render($$result, { class: "h-6 w-6 flex-shrink-0" }, {}, {})} <span data-svelte-h="svelte-18xrzs">The JWT secret is used to sign authentication tokens. If you modify the JWT Secret, all
						users will be signed out.</span></div> <label class="label" for="secret" data-svelte-h="svelte-ohw44g"><span class="label-text text-md">JWT Secret</span></label> ${validate_component(InputPassword, "InputPassword").$$render(
              $$result,
              {
                id: "secret",
                value: securitySettings.jwt_secret
              },
              {
                value: ($$value) => {
                  securitySettings.jwt_secret = $$value;
                  $$settled = false;
                }
              },
              {}
            )} <div class="mt-6 flex justify-end"><button class="btn btn-primary" data-svelte-h="svelte-agutg9">Apply Settings</button></div> `;
          }();
        }(getSecuritySettings())}`;
      }
    })}</div>` : `${escape(goto("/"))}`}`;
  } while (!$$settled);
  $$unsubscribe_user();
  $$unsubscribe_page();
  return $$rendered;
});
export {
  Page as default
};
