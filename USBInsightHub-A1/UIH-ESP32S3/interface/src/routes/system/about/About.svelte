<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { socket } from '$lib/stores/socket';

	onMount(() => {		
		socket.on('master', (data)=> void 0);		
	});

	onDestroy(() => {					
		socket.off('master');						
	});	


</script>

<div class="bg-gray-100 min-h-screen p-4 space-y-4" style="font-size: 20px;">
	<h1 class="text-4xl font-bold text-gray-800">USB Insight Hub</h1>

	<p class="text-lg text-gray-700 mb-6">
		A USB interfacing tool for developers and tech enthusiasts.
	  </p>
	
	<h2 class="text-2xl font-bold text-gray-800">Enumeration Extraction Agent</h2>
	
	<p class="text-gray-700 mb-6">
		To display on the Hub screens the OS device name of the USB device connected 
		to each port, the host computer needs a small agent running on the host computer. 
		Please download the <strong>Enumeration Extraction Agent</strong> here: 
		<a href="https://github.com/Aeriosolutions/USB-Insight-HUB/releases/latest" target="_blank" class="text-blue-600 hover:underline">
			github.com/Aeriosolutions/USB-Insight-HUB/releases/latest
		  </a>
	</p>

	<h2 class="text-2xl font-bold text-gray-800">Project source documents</h2>

	<ul class="list-disc list-inside text-blue-600">
		<li><a class="hover:underline" href="https://github.com/Aeriosolutions/USB-Insight-HUB-Software" target="_blank">Software files (Host, Firmware).</a></li>
		<li><a class="hover:underline" href="https://github.com/Aeriosolutions/USB-Insight-HUB-Hardware" target="_blank">Hardware files (Block diagram, schematics, etc.).</a></li>				
	  </ul>
  
	<h2 class="text-2xl font-bold text-gray-800">Reference Projects</h2>
  
	<p class="text-gray-700 mb-6">
		USB Insight Hub specific code is under the <strong>MIT License</strong>. 
		This project is based on or uses components of other projects: 
	</p>

	<ul class="list-disc list-inside text-blue-600">
	  <li><a class="hover:underline" href="https://github.com/theelims/ESP32-sveltekit" target="_blank">ESP32 SvelteKit</a></li>
	  <li><a class="hover:underline" href="https://github.com/Bodmer/TFT_eSPI" target="_blank">TFT-eSPI</a></li>
	  <li><a class="hover:underline" href="https://arduinojson.org/" target="_blank">ArduinoJSON</a></li>
	</ul>
  
	<h2 class="text-2xl font-bold text-gray-800">MIT License</h2>

	<p class="text-lg text-gray-700 mb-6">
		Copyright (C) 2024 - 2025 Aeriosolutions<br>
		Copyright (C) 2024 - 2025 JoDaSa		
	</p>
	
	<p class="text-lg text-gray-700 mb-6">	
		Permission is hereby granted, free of charge, to any person obtaining a copy
		of this software and associated documentation files (the "Software"), to deal
		in the Software without restriction, including without limitation the rights
		to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
		copies of the Software, and to permit persons to whom the Software is
		furnished to do so, subject to the following conditions:<br><br>
		
		The above copyright notice and this permission notice shall be included in all
		copies or substantial portions of the Software.<br>
		<br>
		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
		IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
		LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
		SOFTWARE.
	</p>


</div>