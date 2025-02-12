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
	connection_mode: number;
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

export type LightState = {
	led_on: boolean;
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

//--------------------------------------------

	c1_startup_counter: number;
	c1_startup_conf_timer: number;

	c1_meter_voltage: number;
	c1_meter_current: number;
	c1_meter_fwdAlertSet: boolean;
	c1_meter_backAlertSet: boolean;
	c1_meter_conf_fwdCLim: number;
	c1_meter_conf_backCLim: number;

	c1_USBInfo_numDev: number;
	c1_USBInfo_Dev1_Name: string;
	c1_USBInfo_Dev2_Name: string;
	c1_USBInfo_usbType: number;

	c1_BaseMCU_fault: boolean;
	c1_BaseMCU_ilim: number;
	c1_BaseMCU_data_en: boolean;
	c1_BaseMCU_pwr_en: boolean;

//--------------------------------------------

	c2_startup_counter: number;
	c2_startup_conf_timer: number;

	c2_meter_voltage: number;
	c2_meter_current: number;
	c2_meter_fwdAlertSet: boolean;
	c2_meter_backAlertSet: boolean;
	c2_meter_conf_fwdCLim: number;
	c2_meter_conf_backCLim: number;

	c2_USBInfo_numDev: number;
	c2_USBInfo_Dev1_Name: string;
	c2_USBInfo_Dev2_Name: string;
	c2_USBInfo_usbType: number;

	c2_BaseMCU_fault: boolean;
	c2_BaseMCU_ilim: number;
	c2_BaseMCU_data_en: boolean;
	c2_BaseMCU_pwr_en: boolean;

//--------------------------------------------

	c3_startup_counter: number;
	c3_startup_conf_timer: number;

	c3_meter_voltage: number;
	c3_meter_current: number;
	c3_meter_fwdAlertSet: boolean;
	c3_meter_backAlertSet: boolean;
	c3_meter_conf_fwdCLim: number;
	c3_meter_conf_backCLim: number;

	c3_USBInfo_numDev: number;
	c3_USBInfo_Dev1_Name: string;
	c3_USBInfo_Dev2_Name: string;
	c3_USBInfo_usbType: number;

	c3_BaseMCU_fault: boolean;
	c3_BaseMCU_ilim: number;
	c3_BaseMCU_data_en: boolean;
	c3_BaseMCU_pwr_en: boolean;

};