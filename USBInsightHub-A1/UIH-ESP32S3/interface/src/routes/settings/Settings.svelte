<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Light from '~icons/tabler/bulb';
	import Info from '~icons/tabler/info-circle';
	import Save from '~icons/tabler/device-floppy';
	import Reload from '~icons/tabler/reload';
	import { socket } from '$lib/stores/socket';
	import type { LightState } from '$lib/types/models';
	import type { MasterState } from '$lib/types/models';

	//let lightState: LightState = { led_on: false };
	let masterState: MasterState = {power_on: false, switch_on:false};


	onMount(() => {
		/*socket.on<LightState>('led', (data) => {
			lightState = data;
		});*/
		socket.on<MasterState>('master', (data)=>{
			masterState = data;
		});
		//getLightstate();
	});

	onDestroy(() => {
		//socket.off('led');
		socket.off('master');
	});


</script>

<SettingsCard collapsible={false}>
	<Light slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Light State</span>
	<div class="w-full">

		<h1 class="text-xl font-semibold">Event Socket Example</h1>
		<div class="alert alert-info my-2 shadow-lg">
			<Info class="h-6 w-6 flex-shrink-0 stroke-current" />
			<span
				>The switch below controls the LED via the event system which uses WebSocket under the hood.
				It will automatically update whenever the LED state changes.</span
			>
		</div>

		<div class="form-control w-52">
			<label class="label cursor-pointer">
				<span class="">To ESP32</span>
				<input
					type="checkbox"
					class="toggle toggle-primary rounded-full"
					bind:checked={masterState.power_on}
					on:change={() => {
						socket.sendEvent('master', masterState);
					}}					
				/>
			</label>
		</div>
		<div class="form-control w-52">
			<label class="label cursor-pointer">
				<span class="">From ESP32</span>
				<input
					type="checkbox"
					class="toggle toggle-primary rounded-full"
					bind:checked={masterState.switch_on}
					on:change={() => {
						socket.sendEvent('master', masterState);
					}}					
				/>
			</label>
		</div>					
	</div>
</SettingsCard>