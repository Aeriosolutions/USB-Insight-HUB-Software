
// this file is generated — do not edit it


/// <reference types="@sveltejs/kit" />

/**
 * Environment variables [loaded by Vite](https://vitejs.dev/guide/env-and-mode.html#env-files) from `.env` files and `process.env`. Like [`$env/dynamic/private`](https://kit.svelte.dev/docs/modules#$env-dynamic-private), this module cannot be imported into client-side code. This module only includes variables that _do not_ begin with [`config.kit.env.publicPrefix`](https://kit.svelte.dev/docs/configuration#env) _and do_ start with [`config.kit.env.privatePrefix`](https://kit.svelte.dev/docs/configuration#env) (if configured).
 * 
 * _Unlike_ [`$env/dynamic/private`](https://kit.svelte.dev/docs/modules#$env-dynamic-private), the values exported from this module are statically injected into your bundle at build time, enabling optimisations like dead code elimination.
 * 
 * ```ts
 * import { API_KEY } from '$env/static/private';
 * ```
 * 
 * Note that all environment variables referenced in your code should be declared (for example in an `.env` file), even if they don't have a value until the app is deployed:
 * 
 * ```
 * MY_FEATURE_FLAG=""
 * ```
 * 
 * You can override `.env` values from the command line like so:
 * 
 * ```bash
 * MY_FEATURE_FLAG="enabled" npm run dev
 * ```
 */
declare module '$env/static/private' {
	export const ALLUSERSPROFILE: string;
	export const ANDROID_HOME: string;
	export const APPDATA: string;
	export const APPLICATIONINSIGHTS_CONFIGURATION_CONTENT: string;
	export const APPLICATION_INSIGHTS_NO_DIAGNOSTIC_CHANNEL: string;
	export const CHOCOLATEYINSTALL: string;
	export const CHOCOLATEYLASTPATHUPDATE: string;
	export const CHROME_CRASHPAD_PIPE_NAME: string;
	export const COLOR: string;
	export const COLORTERM: string;
	export const COMMONPROGRAMFILES: string;
	export const COMMONPROGRAMW6432: string;
	export const COMPUTERNAME: string;
	export const COMSPEC: string;
	export const DOTNET_TIEREDPGO: string;
	export const DRIVERDATA: string;
	export const EDITOR: string;
	export const EFC_14936: string;
	export const ELECTRON_RUN_AS_NODE: string;
	export const GIT_ASKPASS: string;
	export const HOME: string;
	export const HOMEDRIVE: string;
	export const HOMEPATH: string;
	export const IDF_PATH: string;
	export const INIT_CWD: string;
	export const JAVA_HOME: string;
	export const LANG: string;
	export const LOCALAPPDATA: string;
	export const LOGONSERVER: string;
	export const NODE: string;
	export const NODE_ENV: string;
	export const NODE_EXE: string;
	export const NO_PROXY: string;
	export const NPM_CLI_JS: string;
	export const npm_command: string;
	export const npm_config_cache: string;
	export const npm_config_engine_strict: string;
	export const npm_config_globalconfig: string;
	export const npm_config_global_prefix: string;
	export const npm_config_init_module: string;
	export const npm_config_legacy_peer_deps: string;
	export const npm_config_local_prefix: string;
	export const npm_config_node_gyp: string;
	export const npm_config_noproxy: string;
	export const npm_config_npm_version: string;
	export const npm_config_prefix: string;
	export const npm_config_userconfig: string;
	export const npm_config_user_agent: string;
	export const npm_execpath: string;
	export const npm_lifecycle_event: string;
	export const npm_lifecycle_script: string;
	export const npm_node_execpath: string;
	export const npm_package_json: string;
	export const npm_package_name: string;
	export const npm_package_version: string;
	export const NPM_PREFIX_JS: string;
	export const NPM_PREFIX_NPM_CLI_JS: string;
	export const NUMBER_OF_PROCESSORS: string;
	export const ONEDRIVE: string;
	export const ONEDRIVECONSUMER: string;
	export const ORIGINAL_XDG_CURRENT_DESKTOP: string;
	export const OS: string;
	export const PATH: string;
	export const PATHEXT: string;
	export const PLATFORMIO_CALLER: string;
	export const PLATFORMIO_IDE: string;
	export const PLATFORMIO_PATH: string;
	export const PROCESSOR_ARCHITECTURE: string;
	export const PROCESSOR_IDENTIFIER: string;
	export const PROCESSOR_LEVEL: string;
	export const PROCESSOR_REVISION: string;
	export const PROGRAMDATA: string;
	export const PROGRAMFILES: string;
	export const PROGRAMW6432: string;
	export const PROMPT: string;
	export const PSMODULEPATH: string;
	export const PUBLIC: string;
	export const PYTHONIOENCODING: string;
	export const SESSIONNAME: string;
	export const SYSTEMDRIVE: string;
	export const SYSTEMROOT: string;
	export const TEMP: string;
	export const TERM_PROGRAM: string;
	export const TERM_PROGRAM_VERSION: string;
	export const TMP: string;
	export const USERDOMAIN: string;
	export const USERDOMAIN_ROAMINGPROFILE: string;
	export const USERNAME: string;
	export const USERPROFILE: string;
	export const VBOX_MSI_INSTALL_PATH: string;
	export const VSCODE_CODE_CACHE_PATH: string;
	export const VSCODE_CRASH_REPORTER_PROCESS_TYPE: string;
	export const VSCODE_CWD: string;
	export const VSCODE_DOTNET_INSTALL_TOOL_ORIGINAL_HOME: string;
	export const VSCODE_ESM_ENTRYPOINT: string;
	export const VSCODE_GIT_ASKPASS_EXTRA_ARGS: string;
	export const VSCODE_GIT_ASKPASS_MAIN: string;
	export const VSCODE_GIT_ASKPASS_NODE: string;
	export const VSCODE_GIT_IPC_HANDLE: string;
	export const VSCODE_HANDLES_UNCAUGHT_ERRORS: string;
	export const VSCODE_IPC_HOOK: string;
	export const VSCODE_L10N_BUNDLE_LOCATION: string;
	export const VSCODE_NLS_CONFIG: string;
	export const VSCODE_PID: string;
	export const WINDIR: string;
}

/**
 * Similar to [`$env/static/private`](https://kit.svelte.dev/docs/modules#$env-static-private), except that it only includes environment variables that begin with [`config.kit.env.publicPrefix`](https://kit.svelte.dev/docs/configuration#env) (which defaults to `PUBLIC_`), and can therefore safely be exposed to client-side code.
 * 
 * Values are replaced statically at build time.
 * 
 * ```ts
 * import { PUBLIC_BASE_URL } from '$env/static/public';
 * ```
 */
declare module '$env/static/public' {
	
}

/**
 * This module provides access to runtime environment variables, as defined by the platform you're running on. For example if you're using [`adapter-node`](https://github.com/sveltejs/kit/tree/master/packages/adapter-node) (or running [`vite preview`](https://kit.svelte.dev/docs/cli)), this is equivalent to `process.env`. This module only includes variables that _do not_ begin with [`config.kit.env.publicPrefix`](https://kit.svelte.dev/docs/configuration#env) _and do_ start with [`config.kit.env.privatePrefix`](https://kit.svelte.dev/docs/configuration#env) (if configured).
 * 
 * This module cannot be imported into client-side code.
 * 
 * ```ts
 * import { env } from '$env/dynamic/private';
 * console.log(env.DEPLOYMENT_SPECIFIC_VARIABLE);
 * ```
 * 
 * > In `dev`, `$env/dynamic` always includes environment variables from `.env`. In `prod`, this behavior will depend on your adapter.
 */
declare module '$env/dynamic/private' {
	export const env: {
		ALLUSERSPROFILE: string;
		ANDROID_HOME: string;
		APPDATA: string;
		APPLICATIONINSIGHTS_CONFIGURATION_CONTENT: string;
		APPLICATION_INSIGHTS_NO_DIAGNOSTIC_CHANNEL: string;
		CHOCOLATEYINSTALL: string;
		CHOCOLATEYLASTPATHUPDATE: string;
		CHROME_CRASHPAD_PIPE_NAME: string;
		COLOR: string;
		COLORTERM: string;
		COMMONPROGRAMFILES: string;
		COMMONPROGRAMW6432: string;
		COMPUTERNAME: string;
		COMSPEC: string;
		DOTNET_TIEREDPGO: string;
		DRIVERDATA: string;
		EDITOR: string;
		EFC_14936: string;
		ELECTRON_RUN_AS_NODE: string;
		GIT_ASKPASS: string;
		HOME: string;
		HOMEDRIVE: string;
		HOMEPATH: string;
		IDF_PATH: string;
		INIT_CWD: string;
		JAVA_HOME: string;
		LANG: string;
		LOCALAPPDATA: string;
		LOGONSERVER: string;
		NODE: string;
		NODE_ENV: string;
		NODE_EXE: string;
		NO_PROXY: string;
		NPM_CLI_JS: string;
		npm_command: string;
		npm_config_cache: string;
		npm_config_engine_strict: string;
		npm_config_globalconfig: string;
		npm_config_global_prefix: string;
		npm_config_init_module: string;
		npm_config_legacy_peer_deps: string;
		npm_config_local_prefix: string;
		npm_config_node_gyp: string;
		npm_config_noproxy: string;
		npm_config_npm_version: string;
		npm_config_prefix: string;
		npm_config_userconfig: string;
		npm_config_user_agent: string;
		npm_execpath: string;
		npm_lifecycle_event: string;
		npm_lifecycle_script: string;
		npm_node_execpath: string;
		npm_package_json: string;
		npm_package_name: string;
		npm_package_version: string;
		NPM_PREFIX_JS: string;
		NPM_PREFIX_NPM_CLI_JS: string;
		NUMBER_OF_PROCESSORS: string;
		ONEDRIVE: string;
		ONEDRIVECONSUMER: string;
		ORIGINAL_XDG_CURRENT_DESKTOP: string;
		OS: string;
		PATH: string;
		PATHEXT: string;
		PLATFORMIO_CALLER: string;
		PLATFORMIO_IDE: string;
		PLATFORMIO_PATH: string;
		PROCESSOR_ARCHITECTURE: string;
		PROCESSOR_IDENTIFIER: string;
		PROCESSOR_LEVEL: string;
		PROCESSOR_REVISION: string;
		PROGRAMDATA: string;
		PROGRAMFILES: string;
		PROGRAMW6432: string;
		PROMPT: string;
		PSMODULEPATH: string;
		PUBLIC: string;
		PYTHONIOENCODING: string;
		SESSIONNAME: string;
		SYSTEMDRIVE: string;
		SYSTEMROOT: string;
		TEMP: string;
		TERM_PROGRAM: string;
		TERM_PROGRAM_VERSION: string;
		TMP: string;
		USERDOMAIN: string;
		USERDOMAIN_ROAMINGPROFILE: string;
		USERNAME: string;
		USERPROFILE: string;
		VBOX_MSI_INSTALL_PATH: string;
		VSCODE_CODE_CACHE_PATH: string;
		VSCODE_CRASH_REPORTER_PROCESS_TYPE: string;
		VSCODE_CWD: string;
		VSCODE_DOTNET_INSTALL_TOOL_ORIGINAL_HOME: string;
		VSCODE_ESM_ENTRYPOINT: string;
		VSCODE_GIT_ASKPASS_EXTRA_ARGS: string;
		VSCODE_GIT_ASKPASS_MAIN: string;
		VSCODE_GIT_ASKPASS_NODE: string;
		VSCODE_GIT_IPC_HANDLE: string;
		VSCODE_HANDLES_UNCAUGHT_ERRORS: string;
		VSCODE_IPC_HOOK: string;
		VSCODE_L10N_BUNDLE_LOCATION: string;
		VSCODE_NLS_CONFIG: string;
		VSCODE_PID: string;
		WINDIR: string;
		[key: `PUBLIC_${string}`]: undefined;
		[key: `${string}`]: string | undefined;
	}
}

/**
 * Similar to [`$env/dynamic/private`](https://kit.svelte.dev/docs/modules#$env-dynamic-private), but only includes variables that begin with [`config.kit.env.publicPrefix`](https://kit.svelte.dev/docs/configuration#env) (which defaults to `PUBLIC_`), and can therefore safely be exposed to client-side code.
 * 
 * Note that public dynamic environment variables must all be sent from the server to the client, causing larger network requests — when possible, use `$env/static/public` instead.
 * 
 * ```ts
 * import { env } from '$env/dynamic/public';
 * console.log(env.PUBLIC_DEPLOYMENT_SPECIFIC_VARIABLE);
 * ```
 */
declare module '$env/dynamic/public' {
	export const env: {
		[key: `PUBLIC_${string}`]: string | undefined;
	}
}
