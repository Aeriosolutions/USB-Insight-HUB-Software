<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Info from '~icons/tabler/info-circle';
	import Save from '~icons/tabler/device-floppy';
	import Reload from '~icons/tabler/reload';
	import { socket } from '$lib/stores/socket';
	import type { MasterState } from '$lib/types/models';
	import { writable} from 'svelte/store';


	//let masterState = null;
	let masterState: MasterState = {};
	
	
	let startupMode = writable('Persistence');
	let refreshRate = writable('0.5s');
	let filterType = writable('Median');
	let rotation = writable('0');
	let brightness = writable(80);
	let hubMode = writable('USB2 & USB3');

	let prevGlobalConf = {};
	

	onMount(() => {

		socket.on<MasterState>('master', (data)=>{
			masterState = data;
			getFromMaster();
		});

		startupMode.set('Persistence');
		refreshRate.set('0.5s');
		filterType.set('Median');
		rotation.set('0');
		brightness.set(80);
		hubMode.set('USB2 & USB3');		
		
	});

	onDestroy(() => {		
		socket.off('master');
	});

	function getFromMaster(){
		if(prevGlobalConf['startupMode'] !== masterState['features_conf_startUpmode']){
			switch (masterState['features_conf_startUpmode']){
				case 0: startupMode.set('Persistence'); break;
				case 1: startupMode.set('On at startup'); break;
				case 2: startupMode.set('Off at startup'); break;
				case 3: startupMode.set('Timed'); break;
			}
			prevGlobalConf['startupMode'] = masterState['features_conf_startUpmode'];
			//console.log(`Updated startupMode`);
		}
		if(prevGlobalConf['refreshRate'] !== masterState['features_conf_refreshRate']){
			switch (masterState['features_conf_refreshRate']){
				case 0: refreshRate.set('0.5s'); break;
				case 1: refreshRate.set('1.0s'); break;
			}
			prevGlobalConf['refreshRate'] = masterState['features_conf_refreshRate'];
		}
		if(prevGlobalConf['filterType'] !== masterState['features_conf_filterType']){
			switch (masterState['features_conf_filterType']){
				case 0: filterType.set('Moving Avg.'); break;
				case 1: filterType.set('Median'); break;
			}
			prevGlobalConf['filterType'] = masterState['features_conf_filterType'];
		}
		if(prevGlobalConf['rotation'] !== masterState['screen_conf_rotation']){
			switch (masterState['screen_conf_rotation']){
				case 0: rotation.set('0'); break;
				case 1: rotation.set('90'); break;
				case 2: rotation.set('180'); break;
				case 3: rotation.set('270'); break;
			}
			prevGlobalConf['rotation'] = masterState['screen_conf_rotation'];
		}
		if(prevGlobalConf['brightness'] !== masterState['screen_conf_brightness']){
			brightness.set( masterState['screen_conf_brightness']/10);
			prevGlobalConf['brightness'] = masterState['screen_conf_brightness'];
		}
		if(prevGlobalConf['hubMode'] !== masterState['features_conf_hubMode']){
			switch (masterState['features_conf_hubMode']){
				case 0: hubMode.set('USB2 & USB3'); break;
				case 1: hubMode.set('USB2 Only'); break;
				case 2: hubMode.set('USB3 Only'); break;
			}
			prevGlobalConf['hubMode'] = masterState['features_conf_hubMode'];
		}		

	}

	function setStartupMode(){
		let indx = 0;
		switch ($startupMode) {
			case 'Persistence'	 : indx = 0; break;
			case 'On at startup' : indx = 1; break;
			case 'Off at startup': indx = 2; break;
			case 'Timed'		 : indx = 3; break;
		}
		masterState['features_conf_startUpmode'] = indx;
		socket.sendEvent('master', masterState);
	}

	function setRefreshRate(){
		let indx = 0;
		switch ($refreshRate) {
			case '0.5s'	 : indx = 0; break;
			case '1.0s'  : indx = 1; break;
		}
		masterState['features_conf_refreshRate'] = indx;
		socket.sendEvent('master', masterState);
	}

	function setFilterType(){
		let indx = 0;
		switch ($filterType) {
			case 'Moving Avg.' : indx = 0; break;
			case 'Median'  	   : indx = 1; break;
		}
		masterState['features_conf_filterType'] = indx;
		socket.sendEvent('master', masterState);
	}

	function setRotation(){
		let indx = 0;
		switch ($rotation) {
			case '0'  : indx = 0; break;
			case '90' : indx = 1; break;
			case '180': indx = 2; break;
			case '270': indx = 3; break;
		}
		masterState['screen_conf_rotation'] = indx;
		socket.sendEvent('master', masterState);
	}

	function setBrightness(){
		masterState['screen_conf_brightness'] = $brightness*10;
		socket.sendEvent('master', masterState);
	}

	function setHubMode(){
		let indx = 0;
		switch ($hubMode) {
			case 'USB2 & USB3'  : indx = 0; break;
			case 'USB2 Only' 	: indx = 1; break;
			case 'USB3 Only'	: indx = 2; break;
		}
		masterState['features_conf_hubMode'] = indx;
		socket.sendEvent('master', masterState);
	}

</script>

<div class="space-y-4 p-4">
	<!-- Startup Mode -->
	<div class="bg-gray-200 p-4 rounded-lg shadow-md">
	  <h2 class="font-bold">Startup Mode</h2>
	  <div class="flex gap-4 mt-2 ml-4">
		{#each ['Persistence', 'On at startup', 'Off at startup', 'Timed'] as mode}
		  <label class="flex items-center gap-2">
			<input 
			  type="radio" bind:group={$startupMode} value={mode} class="accent-blue-600" 
			  on:change={()=>{setStartupMode()}}
			/>
			{mode}
		  </label>
		{/each}
	  </div>
	</div>
  
	<!-- Meter -->
	<div class="bg-gray-200 p-4 rounded-lg shadow-md">
	  <h2 class="font-bold">Meter</h2>
	  <div class="mt-2 ml-6">
		<label class="block font-semibold">Refresh rate:</label>
		<div class="flex gap-4">
		  {#each ['0.5s', '1.0s'] as rate}
			<label class="flex items-center gap-2">
			  <input 
			  	type="radio" bind:group={$refreshRate} value={rate} class="accent-blue-600" 
				on:change={()=>{setRefreshRate()}}
			  />
			  {rate}
			</label>
		  {/each}
		</div>
	  </div>
	  <div class="mt-2 ml-6">
		<label class="block font-semibold">Filter Type:</label>
		<div class="flex gap-4">
		  {#each ['Moving Avg.', 'Median'] as filter}
			<label class="flex items-center gap-2">
			  <input 
			  	type="radio" bind:group={$filterType} value={filter} class="accent-blue-600" 
				  on:change={()=>{setFilterType()}}
			  />
			  {filter}
			</label>
		  {/each}
		</div>
	  </div>
	</div>
  
	<!-- Screen -->
	<div class="bg-gray-200 p-4 rounded-lg shadow-md">
	  <h2 class="font-bold">Screen</h2>
	  <div class="mt-2 ml-6">
		<label class="block font-semibold">Rotation:</label>
		<div class="flex gap-4">
		  {#each ['0', '90', '180', '270'] as angle}
			<label class="flex items-center gap-2">
			  <input 
			  	type="radio" bind:group={$rotation} value={angle} class="accent-blue-600" 
				  on:change={()=>{setRotation()}}
			  />
			  {angle}&deg;
			</label>
		  {/each}
		</div>
	  </div>
	  <div class="mt-2 ml-6">
		<label class="block font-semibold">Brightness: {$brightness}%</label>
		<div class="relative w-full max-w-lg">
			<input 
			  type="range" min="10" max="100" step="5" bind:value={$brightness} class="w-full" 
			  on:change={()=>{setBrightness()}}	
			/>			
		</div>
	  </div>
	</div>
  
	<!-- Hub Mode -->
	<div class="bg-gray-200 p-4 rounded-lg shadow-md">
	  <h2 class="font-bold">Hub Mode</h2>
	  <div class="flex gap-4 mt-2 ml-6">
		{#each ['USB2 & USB3', 'USB2 Only', 'USB3 Only'] as mode}
		  <label class="flex items-center gap-2">
			<input 
				type="radio" bind:group={$hubMode} value={mode} class="accent-blue-600"
				on:change={()=>{setHubMode()}}	 
			/>
			{mode}
		  </label>
		{/each}
	  </div>
	</div>
  </div>
  