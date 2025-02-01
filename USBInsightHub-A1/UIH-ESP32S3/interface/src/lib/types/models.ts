export type WifiStatus = {
	status: number;
	local_ip: string;
	mac_address: string;
	rssi: number;
	ssid: string;
	bssid: string;
	channel: number;
	subnet_mask: string;
	gateway_ip: string;
	dns_ip_1: string;
	dns_ip_2?: string;
};

export type WifiSettings = {
	hostname: string;
	priority_RSSI: boolean;
	wifi_networks: KnownNetworkItem[];
};

export type KnownNetworkItem = {
	ssid: string;
	password: string;
	static_ip_config: boolean;
	local_ip?: string;
	subnet_mask?: string;
	gateway_ip?: string;
	dns_ip_1?: string;
	dns_ip_2?: string;
};

export type NetworkItem = {
	rssi: number;
	ssid: string;
	bssid: string;
	channel: number;
	encryption_type: number;
};

export type ApStatus = {
	status: number;
	ip_address: string;
	mac_address: string;
	station_num: number;
};

export type ApSettings = {
	provision_mode: number;
	ssid: string;
	password: string;
	channel: number;
	ssid_hidden: boolean;
	max_clients: number;
	local_ip: string;
	gateway_ip: string;
	subnet_mask: string;
};

export type BrokerSettings = {
	mqtt_path: string;
	name: string;
	unique_id: string;
};

export type NTPStatus = {
	status: number;
	utc_time: string;
	local_time: string;
	server: string;
	uptime: number;
};

export type NTPSettings = {
	enabled: boolean;
	server: string;
	tz_label: string;
	tz_format: string;
};

export type Analytics = {
	max_alloc_heap: number;
	psram_size: number;
	free_psram: number;
	free_heap: number;
	total_heap: number;
	min_free_heap: number;
	core_temp: number;
	fs_total: number;
	fs_used: number;
	uptime: number;
};

export type RSSI = {
	rssi: number;
	ssid: string;
};

export type Battery = {
	soc: number;
	charging: boolean;
};

export type DownloadOTA = {
	status: string;
	progress: number;
	error: string;
};

export type StaticSystemInformation = {
	esp_platform: string;
	firmware_version: string;
	cpu_freq_mhz: number;
	cpu_type: string;
	cpu_rev: number;
	cpu_cores: number;
	sketch_size: number;
	free_sketch_space: number;
	sdk_version: string;
	arduino_version: string;
	flash_chip_size: number;
	flash_chip_speed: number;
	cpu_reset_reason: string;
};

export type SystemInformation = Analytics & StaticSystemInformation;

export type MQTTStatus = {
	enabled: boolean;
	connected: boolean;
	client_id: string;
	last_error: string;
};

export type MQTTSettings = {
	enabled: boolean;
	uri: string;
	username: string;
	password: string;
	client_id: string;
	keep_alive: number;
	clean_session: boolean;
};

export type MasterState = {
	power_on: boolean;
	switch_on: boolean;

	channels: ChannelData[];

	features_conf_startUpmode: number;
	features_conf_wifi_enabled: number;
	features_conf_hubMode: number;
	features_conf_filterType: number;
	features_conf_refreshRate: number;
	features_startUpActive: boolean;
	features_pcConnected: boolean;
	features_vbusVoltage: number;
	
	screen_conf_rotation: number;
	screen_conf_brightness: number;
	
	BaseMCU_vext_cc: number;
	BaseMCU_vhost_cc: number;
	BaseMCU_vext_stat: number;
	BaseMCU_vhost_stat: number;
	BaseMCU_pwr_source: boolean;
	BaseMCU_usb3_mux_out_en: boolean;
	BaseMCU_usb3_mux_sel_pos: boolean;
	BaseMCU_base_ver: number;
};

export type ChannelData = {
	startup_counter: number;
	startup_conf_timer: number;

	meter_voltage: number;
	meter_current: number;
	meter_fwdAlertSet: boolean;
	meter_backAlertSet: boolean;
	meter_conf_fwdCLim: number;
	meter_conf_backCLim: number;

	USBInfo_numDev: number;
	USBInfo_Dev1_Name: string;
	USBInfo_Dev2_Name: string;
	USBInfo_usbType: number;

	BaseMCU_fault: boolean;
	BaseMCU_ilim: number;
	BaseMCU_data_en: boolean;
	BaseMCU_pwr_en: boolean;
};