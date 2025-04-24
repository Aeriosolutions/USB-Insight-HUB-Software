<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { socket } from '$lib/stores/socket';
	import type { MasterState } from '$lib/types/models';
	import {Help} from '$lib/types/help';


	//let masterState = null;
	
	let masterState: MasterState = {};
	
	//let tempParams ={};
	let initialized = false;
	let sync = false;
	let synchEpoch = 0;
	
	let tempParams = {
		c1_fwdCLim: 1000,
		c1_backCLim: 20,
		c1_startupTime: 2.0,
		c2_fwdCLim: 1000,
		c2_backCLim: 20,
		c2_startupTime: 2.0,
		c3_fwdCLim: 1000,
		c3_backCLim: 20,
		c3_startupTime: 2.0,
	};

	let prevLimitParams = {};

	let prevControlVals = {
		c1_pwr: false,
		c2_pwr: false,
		c3_pwr: false,
		c1_data: false,
		c2_data: false,
		c3_data: false,
	};

	let updateLimits = {
		c1: false,
		c2: false,
		c3: false,
	}

	let toggleFocus = {
		c1_pwr: false,
		c2_pwr: false,
		c3_pwr: false,
		c1_data: false,
		c2_data: false,
		c3_data: false,
	}

	let enumText = {
		c1_txt: '',
		c2_txt: '',
		c3_txt: '',
	}

	let channels = [
    {id: 1,},
    {id: 2,},
    {id: 3,},
  	];

	onMount(() => {
		
		socket.on<MasterState>('master', (data)=>{
			masterState = data;					
			checkChangesAndUpdate();
			needUpdate();
			formatEnumerator();
			initialized = true;	
			sync = true;
												
		});
		
	});


	onDestroy(() => {		
			//socket.sendEvent('unsubscribe','master');
			socket.off('master');						
			
		});

	function validateInput(event, key, min, max) {
		let value = parseFloat(event.target.value); // Supports both integers & decimals

		if (isNaN(value)) {
			value = min; // Default to min if input is invalid
		} else if (value < min) {
			value = min; // Enforce lower bound
		} else if (value > max) {
			value = max; // Enforce upper bound
		}

		tempParams[key] = value;
		//console.log(`Updated ${key} to ${value}`); // Debugging statement
  	}

	function checkChangesAndUpdate(){
		
		let ch = [1,2,3];
		ch.forEach((i) => {
			if(prevLimitParams[`c${i}_fwdCLim`] !== masterState[`c${i}_meter_conf_fwdCLim`]){
				//console.log(`Old ${prevLimitParams[`c${i}_fwdCLim`]} new ${masterState[`c${i}_meter_conf_fwdCLim`]}`); // Debugging statement
				tempParams[`c${i}_fwdCLim`] = masterState[`c${i}_meter_conf_fwdCLim`];
				prevLimitParams[`c${i}_fwdCLim`] = masterState[`c${i}_meter_conf_fwdCLim`];
			}
			if(prevLimitParams[`c${i}_backCLim`] !== masterState[`c${i}_meter_conf_backCLim`]){
				tempParams[`c${i}_backCLim`] = masterState[`c${i}_meter_conf_backCLim`];
				prevLimitParams[`c${i}_backCLim`] = masterState[`c${i}_meter_conf_backCLim`];
			}
			if(prevLimitParams[`c${i}_startupTime`] !== masterState[`c${i}_startup_conf_timer`]){
				tempParams[`c${i}_startupTime`] = (masterState[`c${i}_startup_conf_timer`] /10).toFixed(1);
				prevLimitParams[`c${i}_startupTime`] = masterState[`c${i}_startup_conf_timer`];
			}
			if(!initialized) {
				prevControlVals[`c${i}_pwr`] = masterState[`c${i}_BaseMCU_pwr_en`];
				prevControlVals[`c${i}_data`] = masterState[`c${i}_BaseMCU_data_en`];
			}
			//This mechanism is implemented to prevent bouncing of the toggle controls.
			//the synchEpoch provides time for socket.sendEvent to take effect and in this way
			//synchronize all variables. This is done as the sendEvent does not have a means
			//to check if the event has been effective.
			if(synchEpoch === 0){
				prevControlVals[`c${i}_pwr`] = masterState[`c${i}_BaseMCU_pwr_en`];
				prevControlVals[`c${i}_data`] = masterState[`c${i}_BaseMCU_data_en`];
			} else {
				synchEpoch--;
				//console.log(`Syncing in ${synchEpoch}`);
			}			
		});
  	}

	function needUpdate(){
		let ch = [1,2,3];
		ch.forEach((i) => {
			if(tempParams[`c${i}_fwdCLim`] !== masterState[`c${i}_meter_conf_fwdCLim`] ||
				tempParams[`c${i}_backCLim`] !== masterState[`c${i}_meter_conf_backCLim`] ||
				tempParams[`c${i}_startupTime`] !== (masterState[`c${i}_startup_conf_timer`]/10).toFixed(1)){
					updateLimits[`c${i}`] = true;
				}
			else{
				updateLimits[`c${i}`] = false;
			}
		});

	}

	function clearFault(ch){
		masterState[`c${ch}_BaseMCU_pwr_en`] = false;
		if(masterState[`c${ch}_meter_fwdAlertSet`]) masterState[`c${ch}_meter_fwdAlertSet`] = false;
		if(masterState[`c${ch}_meter_backAlertSet`]) masterState[`c${ch}_meter_backAlertSet`] = false;
		socket.sendEvent('master', masterState);
	}

	function updateParams(ch){
		masterState[`c${ch}_meter_conf_fwdCLim`] = tempParams[`c${ch}_fwdCLim`];
		masterState[`c${ch}_meter_conf_backCLim`] = tempParams[`c${ch}_backCLim`];
		masterState[`c${ch}_startup_conf_timer`] = tempParams[`c${ch}_startupTime`] * 10;
		socket.sendEvent('master', masterState);
	}

	function formatEnumerator(){
		let ch = [1,2,3];
		let len = 2;
		
		//check max length of lines in all channels
		ch.forEach((i) => {
			if(masterState[`c${i}_USBInfo_numDev`] == 10){
				let pO = {};
				try {					
					pO = JSON.parse(masterState[`c${i}_USBInfo_Dev1_Name`]);
					if(pO?.T1 && pO?.T2 && pO?.T3){						
						len = 3;
					}																
				} catch (e) {
					console.error('Invalid JSON:', e);
				}								
			}
		});	
		
		ch.forEach((i) => {
			if(masterState[`c${i}_USBInfo_numDev`] == 10){
				let pO = {};
				try {
					
					pO = JSON.parse(masterState[`c${i}_USBInfo_Dev1_Name`]);
					if(pO?.T1 && pO?.T2 && pO?.T3){
						enumText[`c${i}_txt`] = [String(pO.T1.txt), String(pO.T2.txt), String(pO.T3.txt)].join('\n');
					}
					else if (pO?.T1 && pO?.T2 && !pO?.T3){
						if (len ==2) enumText[`c${i}_txt`] = [String(pO.T1.txt), String(pO.T2.txt)].join('\n');
						if (len ==3) enumText[`c${i}_txt`] = [String(pO.T1.txt), String(pO.T2.txt), ' '].join('\n');
					}
					else if (pO?.T1 && !pO?.T2 && !pO?.T3){
						if (len ==2) enumText[`c${i}_txt`] = [String(pO.T1.txt), '-'].join('\n');
						if (len ==3) enumText[`c${i}_txt`] = [String(pO.T1.txt), '-', ' '].join('\n');
					}
					else{
						if (len ==2) enumText[`c${i}_txt`] = ['-', '-'].join('\n');
						if (len ==3) enumText[`c${i}_txt`] = ['-', '-', ' '].join('\n');
					}										
					
				} catch (e) {
					console.error('Invalid JSON:', e);
				}				
					
			}
			else {
				if(len === 2) enumText[`c${i}_txt`] = [masterState[`c${i}_USBInfo_Dev1_Name`],masterState[`c${i}_USBInfo_Dev2_Name`]].join('\n');
				if(len === 3) enumText[`c${i}_txt`] = [masterState[`c${i}_USBInfo_Dev1_Name`],masterState[`c${i}_USBInfo_Dev2_Name`],' '].join('\n');
			}
			
		});
	}

</script>

<div class="bg-gray-100 min-h-screen p-4" style="font-size: 20px;">
	<!-- Header -->
	<div class="flex justify-around items-center bg-gray-300 p-4 rounded-lg shadow-md mb-4">
	  <div class="flex items-center gap-2">
		<span>Host Link</span>
		<span class="text-sm cursor-help" title={Help.CONTROL.HOSTLINK}>ℹ️:</span>
		<span class="text-green-500 font-bold">
			{#if masterState[`features_usbHostState`] != 0} ✔
			{:else} -		
			{/if}
		</span>
	  </div>
	  <div class="flex items-center gap-2">
		<span >Host Comm</span>
		<span class="text-sm cursor-help" title={Help.CONTROL.HOSTCOM}>ℹ️:</span>
		<span class="text-green-500 font-bold">
			{#if masterState[`features_pcConnected`]} ✔
			{:else} -		
			{/if}
		</span>
	  </div>
	  <div class="flex items-center gap-2">
		<span>Power</span>
		<span class="text-sm cursor-help" title={Help.CONTROL.POWER}>ℹ️:</span>
		{#if masterState[`BaseMCU_pwr_source`]} AUX
		{:else} HOST		
		{/if}
	  </div>
	</div>
  
	<!-- Channels -->
	<div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4">

	<style>
		.flex-container {
			display: flex;
			align-items: center; /* Align items vertically */
			gap: 0.5rem; /* Add some space between items */
		}
		
		.flex-container label {
			margin-bottom: 0; /* Remove default margin */
		}

		.tab-space::after {
			content: "";
			display: inline-block;
			width: 2em; /* Adjust the value as needed */
    	}
		.separator {
			border: none;
			border-top: 4px solid #ccc; /* Adjust the color and thickness as needed */
			margin: 20px 0; /* Adjust the spacing as needed */
  		}		
	</style>

	  {#each channels as ch}
		<div class="bg-white rounded-lg shadow p-4">
		  <!-- Header -->
		  <div class="flex justify-between mb-2">
			<span class="font-bold" style="font-size: 25px;">Channel {ch.id}</span>
			<span class="text-gray-500">
				{#if masterState[`c${ch.id}_USBInfo_usbType`] == 0} -
				{:else if masterState[`c${ch.id}_USBInfo_usbType`] == 2} USB 2.0
				{:else if masterState[`c${ch.id}_USBInfo_usbType`] == 3} USB 3.0
				{/if}
			</span>
		  </div>
  
		  <!-- Enumerator -->
		  <div class="mb-4">
			<label class="text-gray-600 text-sm block" style="font-size: 20px;">Enumerators:</label>			
			<div class="bg-gray-100 p-2 rounded whitespace-pre-wrap" style="font-size: 20px;">{enumText[`c${ch.id}_txt`]}</div>
			<!--<div class="bg-gray-100 p-2 rounded" style="font-size: 20px;">{masterState[`c${ch.id}_USBInfo_Dev2_Name`]}</div>-->			
		  </div>
  
		  <!-- Voltage and Current -->
		  <div class="mb-2">
			<div>Voltage:&nbsp&nbsp<span class="font-bold text-blue-600" style="font-size: 25px;">{(masterState[`c${ch.id}_meter_voltage`] / 1000).toFixed(3)} V</span></div>
			<div>Current:&nbsp&nbsp<span class="font-bold text-blue-600" style="font-size: 25px;">{Math.abs((masterState[`c${ch.id}_meter_current`] / 1000)).toFixed(3)} A</span></div>
		  </div>
		  
		  <!-- Status -->
		  <div class="mb-4">
			Status:&nbsp&nbsp&nbsp  
			<span class={
			  masterState[`c${ch.id}_BaseMCU_fault`] ? "text-red-500 font-bold" : 
			  masterState[`c${ch.id}_meter_fwdAlertSet`] || masterState[`c${ch.id}_meter_backAlertSet`] ? "text-yellow-500 font-bold" : 
			  masterState[`c${ch.id}_BaseMCU_pwr_en`] ? "text-green-500 font-bold"	: "text-gray-500 font-bold"		   			  
			}>
				{#if masterState[`c${ch.id}_BaseMCU_fault`] } Short!
				{:else if masterState[`c${ch.id}_meter_fwdAlertSet`]} Forward Overcurrent!
				{:else if masterState[`c${ch.id}_meter_backAlertSet`]} Backward Overcurrent!
				{:else if masterState[`c${ch.id}_BaseMCU_pwr_en`]} ON
				{:else } OFF
				{/if}
			</span>
			{#if masterState[`c${ch.id}_BaseMCU_fault`] || masterState[`c${ch.id}_meter_fwdAlertSet`] || masterState[`c${ch.id}_meter_backAlertSet`] }
			<button 
				class="bg-blue-500 text-white py-1 px-3 rounded hover:bg-blue-600 w-50"
				on:click={() => {clearFault(ch.id)}}
			>
				Clear Fault
			  </button>
			{/if}			  
		  </div>
		  <hr class="separator">
		  <!-- Controls -->
		  <div class="mb-4">
			
			<label class="flex items-center gap-2 mb-2">
			  <span class="tab-space">Power Control:&nbsp&nbsp</span>
			  <input 			  
				type="checkbox"
				class="toggle toggle-primary rounded-full"					
				bind:checked={prevControlVals[`c${ch.id}_pwr`]}
				on:change={() => {
					//only if cursor is over the control the toggle takes effect. This avoids an artifact that when clicking 
					// on the right side of the control it is activated.
					if(toggleFocus[`c${ch.id}_pwr`]) {
						masterState[`c${ch.id}_BaseMCU_pwr_en`] = prevControlVals[`c${ch.id}_pwr`];
						socket.sendEvent('master', masterState);
						synchEpoch = 25; //wait time to synchronize the variables
					}
				}}
				on:mouseenter={() => {toggleFocus[`c${ch.id}_pwr`] = true; }}
				on:mouseleave={() => {toggleFocus[`c${ch.id}_pwr`] = false; }}							  	
			  />
			</label>
			<label class="flex items-center gap-2">
			  <span class="tab-space">Data Control:&nbsp&nbsp&nbsp&nbsp</span>
			  <input 
				type="checkbox"
				class="toggle toggle-primary rounded-full"
				bind:checked={prevControlVals[`c${ch.id}_data`]}					
				on:change={() => {
					if(toggleFocus[`c${ch.id}_data`]) {
						masterState[`c${ch.id}_BaseMCU_data_en`] = prevControlVals[`c${ch.id}_data`];
						socket.sendEvent('master', masterState);
						synchEpoch = 20;
					}
					
				}}
				on:mouseenter={() => {toggleFocus[`c${ch.id}_data`] = true}}
				on:mouseleave={() => {toggleFocus[`c${ch.id}_data`] = false}}			  
			  />
			</label>
		  </div>
		  <hr class="separator">
		  <!-- Limits -->
		  <div class="text-sm text-gray-600 mb-2" style="font-size: 20px;">
			<span class="tab-space">Over Current Limit :</span>
		
			<input 
				type="number" 
				class="border rounded p-1" 
				bind:value={tempParams[`c${ch.id}_fwdCLim`]}
				min="100"
				max="2000"
				step="1"
				style="width: 80px;"
				on:change={(e) => validateInput(e, `c${ch.id}_fwdCLim`, 100, 2000)} 
			/> mA
			{#if ch.id == 1}  <span class="text-sm cursor-help" title={Help.CONTROL.OVER_CURRENT}>ℹ️</span> {/if}
		  </div>
		  <div class="text-sm text-gray-600 mb-2" style="font-size: 20px;">
			<span class="tab-space">Back Current Limit :</span>			 
			<input 
				type="number" 
				class="border rounded p-1" 
				bind:value={tempParams[`c${ch.id}_backCLim`]} 
				min="20"
				max="100"
				step="1"
				style="width: 80px;"
				on:change={(e) => validateInput(e, `c${ch.id}_backCLim`, 20, 100)}
			/> mA
			{#if ch.id == 1}  <span class="text-sm cursor-help" title={Help.CONTROL.BACK_CURRENT}>ℹ️</span> {/if}
		  </div>
		  <div class="text-sm text-gray-600 mb-4" style="font-size: 20px;">
			<span class="tab-space">Startup Time Delay:</span>			 
			<input 
				type="number" 
				class="border rounded p-1" 				 								
				bind:value={tempParams[`c${ch.id}_startupTime`]}
				min="1"
				max="10"				
				step="0.1"
				style="width: 80px;"
				on:change={(e) => validateInput(e, `c${ch.id}_startupTime`, 0.1, 10)}
			/> s
			{#if ch.id == 1}  <span class="text-sm cursor-help" title={Help.CONTROL.STARTUP_DELAY}>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ℹ️</span> {/if}
		  </div>
  
		  <button 
		  	class="bg-blue-500 text-white py-1 px-3 rounded hover:bg-blue-600 w-50
			  transition duration-200
			  bg-gray-400 cursor-not-allowed disabled:opacity-50 
			  enabled:bg-blue-500 enabled:hover:bg-blue-600 enabled:cursor-pointer"
		
			  disabled = {!updateLimits[`c${ch.id}`]}
			  on:click={() => {updateParams(ch.id)}}
			>
			Update
		  </button>
		</div>
	  {/each}
	</div>
  </div>