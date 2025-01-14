import { c as create_ssr_component, b as spread, d as escape_object, a as subscribe, o as onDestroy, v as validate_component, i as is_promise, n as noop, f as escape, h as add_attribute, e as each } from "../../../../chunks/ssr.js";
import { S as SettingsCard } from "../../../../chunks/SettingsCard.js";
import { C as Collapsible } from "../../../../chunks/Collapsible.js";
import { S as Spinner } from "../../../../chunks/Spinner.js";
import { u as user } from "../../../../chunks/user.js";
import { p as page } from "../../../../chunks/stores.js";
import { C as Clock_check } from "../../../../chunks/clock-check.js";
import { _ as _24_hours } from "../../../../chunks/24-hours.js";
const TIME_ZONES = {
  "Africa/Abidjan": "GMT0",
  "Africa/Accra": "GMT0",
  "Africa/Addis_Ababa": "EAT-3",
  "Africa/Algiers": "CET-1",
  "Africa/Asmara": "EAT-3",
  "Africa/Bamako": "GMT0",
  "Africa/Bangui": "WAT-1",
  "Africa/Banjul": "GMT0",
  "Africa/Bissau": "GMT0",
  "Africa/Blantyre": "CAT-2",
  "Africa/Brazzaville": "WAT-1",
  "Africa/Bujumbura": "CAT-2",
  "Africa/Cairo": "EET-2",
  "Africa/Casablanca": "UNK-1",
  "Africa/Ceuta": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Africa/Conakry": "GMT0",
  "Africa/Dakar": "GMT0",
  "Africa/Dar_es_Salaam": "EAT-3",
  "Africa/Djibouti": "EAT-3",
  "Africa/Douala": "WAT-1",
  "Africa/El_Aaiun": "UNK-1",
  "Africa/Freetown": "GMT0",
  "Africa/Gaborone": "CAT-2",
  "Africa/Harare": "CAT-2",
  "Africa/Johannesburg": "SAST-2",
  "Africa/Juba": "EAT-3",
  "Africa/Kampala": "EAT-3",
  "Africa/Khartoum": "CAT-2",
  "Africa/Kigali": "CAT-2",
  "Africa/Kinshasa": "WAT-1",
  "Africa/Lagos": "WAT-1",
  "Africa/Libreville": "WAT-1",
  "Africa/Lome": "GMT0",
  "Africa/Luanda": "WAT-1",
  "Africa/Lubumbashi": "CAT-2",
  "Africa/Lusaka": "CAT-2",
  "Africa/Malabo": "WAT-1",
  "Africa/Maputo": "CAT-2",
  "Africa/Maseru": "SAST-2",
  "Africa/Mbabane": "SAST-2",
  "Africa/Mogadishu": "EAT-3",
  "Africa/Monrovia": "GMT0",
  "Africa/Nairobi": "EAT-3",
  "Africa/Ndjamena": "WAT-1",
  "Africa/Niamey": "WAT-1",
  "Africa/Nouakchott": "GMT0",
  "Africa/Ouagadougou": "GMT0",
  "Africa/Porto-Novo": "WAT-1",
  "Africa/Sao_Tome": "GMT0",
  "Africa/Tripoli": "EET-2",
  "Africa/Tunis": "CET-1",
  "Africa/Windhoek": "CAT-2",
  "America/Adak": "HST10HDT,M3.2.0,M11.1.0",
  "America/Anchorage": "AKST9AKDT,M3.2.0,M11.1.0",
  "America/Anguilla": "AST4",
  "America/Antigua": "AST4",
  "America/Araguaina": "UNK3",
  "America/Argentina/Buenos_Aires": "UNK3",
  "America/Argentina/Catamarca": "UNK3",
  "America/Argentina/Cordoba": "UNK3",
  "America/Argentina/Jujuy": "UNK3",
  "America/Argentina/La_Rioja": "UNK3",
  "America/Argentina/Mendoza": "UNK3",
  "America/Argentina/Rio_Gallegos": "UNK3",
  "America/Argentina/Salta": "UNK3",
  "America/Argentina/San_Juan": "UNK3",
  "America/Argentina/San_Luis": "UNK3",
  "America/Argentina/Tucuman": "UNK3",
  "America/Argentina/Ushuaia": "UNK3",
  "America/Aruba": "AST4",
  "America/Asuncion": "UNK4UNK,M10.1.0/0,M3.4.0/0",
  "America/Atikokan": "EST5",
  "America/Bahia": "UNK3",
  "America/Bahia_Banderas": "CST6CDT,M4.1.0,M10.5.0",
  "America/Barbados": "AST4",
  "America/Belem": "UNK3",
  "America/Belize": "CST6",
  "America/Blanc-Sablon": "AST4",
  "America/Boa_Vista": "UNK4",
  "America/Bogota": "UNK5",
  "America/Boise": "MST7MDT,M3.2.0,M11.1.0",
  "America/Cambridge_Bay": "MST7MDT,M3.2.0,M11.1.0",
  "America/Campo_Grande": "UNK4",
  "America/Cancun": "EST5",
  "America/Caracas": "UNK4",
  "America/Cayenne": "UNK3",
  "America/Cayman": "EST5",
  "America/Chicago": "CST6CDT,M3.2.0,M11.1.0",
  "America/Chihuahua": "MST7MDT,M4.1.0,M10.5.0",
  "America/Costa_Rica": "CST6",
  "America/Creston": "MST7",
  "America/Cuiaba": "UNK4",
  "America/Curacao": "AST4",
  "America/Danmarkshavn": "GMT0",
  "America/Dawson": "MST7",
  "America/Dawson_Creek": "MST7",
  "America/Denver": "MST7MDT,M3.2.0,M11.1.0",
  "America/Detroit": "EST5EDT,M3.2.0,M11.1.0",
  "America/Dominica": "AST4",
  "America/Edmonton": "MST7MDT,M3.2.0,M11.1.0",
  "America/Eirunepe": "UNK5",
  "America/El_Salvador": "CST6",
  "America/Fort_Nelson": "MST7",
  "America/Fortaleza": "UNK3",
  "America/Glace_Bay": "AST4ADT,M3.2.0,M11.1.0",
  "America/Godthab": "UNK3UNK,M3.5.0/-2,M10.5.0/-1",
  "America/Goose_Bay": "AST4ADT,M3.2.0,M11.1.0",
  "America/Grand_Turk": "EST5EDT,M3.2.0,M11.1.0",
  "America/Grenada": "AST4",
  "America/Guadeloupe": "AST4",
  "America/Guatemala": "CST6",
  "America/Guayaquil": "UNK5",
  "America/Guyana": "UNK4",
  "America/Halifax": "AST4ADT,M3.2.0,M11.1.0",
  "America/Havana": "CST5CDT,M3.2.0/0,M11.1.0/1",
  "America/Hermosillo": "MST7",
  "America/Indiana/Indianapolis": "EST5EDT,M3.2.0,M11.1.0",
  "America/Indiana/Knox": "CST6CDT,M3.2.0,M11.1.0",
  "America/Indiana/Marengo": "EST5EDT,M3.2.0,M11.1.0",
  "America/Indiana/Petersburg": "EST5EDT,M3.2.0,M11.1.0",
  "America/Indiana/Tell_City": "CST6CDT,M3.2.0,M11.1.0",
  "America/Indiana/Vevay": "EST5EDT,M3.2.0,M11.1.0",
  "America/Indiana/Vincennes": "EST5EDT,M3.2.0,M11.1.0",
  "America/Indiana/Winamac": "EST5EDT,M3.2.0,M11.1.0",
  "America/Inuvik": "MST7MDT,M3.2.0,M11.1.0",
  "America/Iqaluit": "EST5EDT,M3.2.0,M11.1.0",
  "America/Jamaica": "EST5",
  "America/Juneau": "AKST9AKDT,M3.2.0,M11.1.0",
  "America/Kentucky/Louisville": "EST5EDT,M3.2.0,M11.1.0",
  "America/Kentucky/Monticello": "EST5EDT,M3.2.0,M11.1.0",
  "America/Kralendijk": "AST4",
  "America/La_Paz": "UNK4",
  "America/Lima": "UNK5",
  "America/Los_Angeles": "PST8PDT,M3.2.0,M11.1.0",
  "America/Lower_Princes": "AST4",
  "America/Maceio": "UNK3",
  "America/Managua": "CST6",
  "America/Manaus": "UNK4",
  "America/Marigot": "AST4",
  "America/Martinique": "AST4",
  "America/Matamoros": "CST6CDT,M3.2.0,M11.1.0",
  "America/Mazatlan": "MST7MDT,M4.1.0,M10.5.0",
  "America/Menominee": "CST6CDT,M3.2.0,M11.1.0",
  "America/Merida": "CST6CDT,M4.1.0,M10.5.0",
  "America/Metlakatla": "AKST9AKDT,M3.2.0,M11.1.0",
  "America/Mexico_City": "CST6CDT,M4.1.0,M10.5.0",
  "America/Miquelon": "UNK3UNK,M3.2.0,M11.1.0",
  "America/Moncton": "AST4ADT,M3.2.0,M11.1.0",
  "America/Monterrey": "CST6CDT,M4.1.0,M10.5.0",
  "America/Montevideo": "UNK3",
  "America/Montreal": "EST5EDT,M3.2.0,M11.1.0",
  "America/Montserrat": "AST4",
  "America/Nassau": "EST5EDT,M3.2.0,M11.1.0",
  "America/New_York": "EST5EDT,M3.2.0,M11.1.0",
  "America/Nipigon": "EST5EDT,M3.2.0,M11.1.0",
  "America/Nome": "AKST9AKDT,M3.2.0,M11.1.0",
  "America/Noronha": "UNK2",
  "America/North_Dakota/Beulah": "CST6CDT,M3.2.0,M11.1.0",
  "America/North_Dakota/Center": "CST6CDT,M3.2.0,M11.1.0",
  "America/North_Dakota/New_Salem": "CST6CDT,M3.2.0,M11.1.0",
  "America/Ojinaga": "MST7MDT,M3.2.0,M11.1.0",
  "America/Panama": "EST5",
  "America/Pangnirtung": "EST5EDT,M3.2.0,M11.1.0",
  "America/Paramaribo": "UNK3",
  "America/Phoenix": "MST7",
  "America/Port-au-Prince": "EST5EDT,M3.2.0,M11.1.0",
  "America/Port_of_Spain": "AST4",
  "America/Porto_Velho": "UNK4",
  "America/Puerto_Rico": "AST4",
  "America/Punta_Arenas": "UNK3",
  "America/Rainy_River": "CST6CDT,M3.2.0,M11.1.0",
  "America/Rankin_Inlet": "CST6CDT,M3.2.0,M11.1.0",
  "America/Recife": "UNK3",
  "America/Regina": "CST6",
  "America/Resolute": "CST6CDT,M3.2.0,M11.1.0",
  "America/Rio_Branco": "UNK5",
  "America/Santarem": "UNK3",
  "America/Santiago": "UNK4UNK,M9.1.6/24,M4.1.6/24",
  "America/Santo_Domingo": "AST4",
  "America/Sao_Paulo": "UNK3",
  "America/Scoresbysund": "UNK1UNK,M3.5.0/0,M10.5.0/1",
  "America/Sitka": "AKST9AKDT,M3.2.0,M11.1.0",
  "America/St_Barthelemy": "AST4",
  "America/St_Johns": "NST3:30NDT,M3.2.0,M11.1.0",
  "America/St_Kitts": "AST4",
  "America/St_Lucia": "AST4",
  "America/St_Thomas": "AST4",
  "America/St_Vincent": "AST4",
  "America/Swift_Current": "CST6",
  "America/Tegucigalpa": "CST6",
  "America/Thule": "AST4ADT,M3.2.0,M11.1.0",
  "America/Thunder_Bay": "EST5EDT,M3.2.0,M11.1.0",
  "America/Tijuana": "PST8PDT,M3.2.0,M11.1.0",
  "America/Toronto": "EST5EDT,M3.2.0,M11.1.0",
  "America/Tortola": "AST4",
  "America/Vancouver": "PST8PDT,M3.2.0,M11.1.0",
  "America/Whitehorse": "MST7",
  "America/Winnipeg": "CST6CDT,M3.2.0,M11.1.0",
  "America/Yakutat": "AKST9AKDT,M3.2.0,M11.1.0",
  "America/Yellowknife": "MST7MDT,M3.2.0,M11.1.0",
  "Antarctica/Casey": "UNK-8",
  "Antarctica/Davis": "UNK-7",
  "Antarctica/DumontDUrville": "UNK-10",
  "Antarctica/Macquarie": "UNK-11",
  "Antarctica/Mawson": "UNK-5",
  "Antarctica/McMurdo": "NZST-12NZDT,M9.5.0,M4.1.0/3",
  "Antarctica/Palmer": "UNK3",
  "Antarctica/Rothera": "UNK3",
  "Antarctica/Syowa": "UNK-3",
  "Antarctica/Troll": "UNK0UNK-2,M3.5.0/1,M10.5.0/3",
  "Antarctica/Vostok": "UNK-6",
  "Arctic/Longyearbyen": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Asia/Aden": "UNK-3",
  "Asia/Almaty": "UNK-6",
  "Asia/Amman": "EET-2EEST,M3.5.4/24,M10.5.5/1",
  "Asia/Anadyr": "UNK-12",
  "Asia/Aqtau": "UNK-5",
  "Asia/Aqtobe": "UNK-5",
  "Asia/Ashgabat": "UNK-5",
  "Asia/Atyrau": "UNK-5",
  "Asia/Baghdad": "UNK-3",
  "Asia/Bahrain": "UNK-3",
  "Asia/Baku": "UNK-4",
  "Asia/Bangkok": "UNK-7",
  "Asia/Barnaul": "UNK-7",
  "Asia/Beirut": "EET-2EEST,M3.5.0/0,M10.5.0/0",
  "Asia/Bishkek": "UNK-6",
  "Asia/Brunei": "UNK-8",
  "Asia/Chita": "UNK-9",
  "Asia/Choibalsan": "UNK-8",
  "Asia/Colombo": "UNK-5:30",
  "Asia/Damascus": "EET-2EEST,M3.5.5/0,M10.5.5/0",
  "Asia/Dhaka": "UNK-6",
  "Asia/Dili": "UNK-9",
  "Asia/Dubai": "UNK-4",
  "Asia/Dushanbe": "UNK-5",
  "Asia/Famagusta": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Asia/Gaza": "EET-2EEST,M3.5.5/0,M10.5.6/1",
  "Asia/Hebron": "EET-2EEST,M3.5.5/0,M10.5.6/1",
  "Asia/Ho_Chi_Minh": "UNK-7",
  "Asia/Hong_Kong": "HKT-8",
  "Asia/Hovd": "UNK-7",
  "Asia/Irkutsk": "UNK-8",
  "Asia/Jakarta": "WIB-7",
  "Asia/Jayapura": "WIT-9",
  "Asia/Jerusalem": "IST-2IDT,M3.4.4/26,M10.5.0",
  "Asia/Kabul": "UNK-4:30",
  "Asia/Kamchatka": "UNK-12",
  "Asia/Karachi": "PKT-5",
  "Asia/Kathmandu": "UNK-5:45",
  "Asia/Khandyga": "UNK-9",
  "Asia/Kolkata": "IST-5:30",
  "Asia/Krasnoyarsk": "UNK-7",
  "Asia/Kuala_Lumpur": "UNK-8",
  "Asia/Kuching": "UNK-8",
  "Asia/Kuwait": "UNK-3",
  "Asia/Macau": "CST-8",
  "Asia/Magadan": "UNK-11",
  "Asia/Makassar": "WITA-8",
  "Asia/Manila": "PST-8",
  "Asia/Muscat": "UNK-4",
  "Asia/Nicosia": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Asia/Novokuznetsk": "UNK-7",
  "Asia/Novosibirsk": "UNK-7",
  "Asia/Omsk": "UNK-6",
  "Asia/Oral": "UNK-5",
  "Asia/Phnom_Penh": "UNK-7",
  "Asia/Pontianak": "WIB-7",
  "Asia/Pyongyang": "KST-9",
  "Asia/Qatar": "UNK-3",
  "Asia/Qyzylorda": "UNK-5",
  "Asia/Riyadh": "UNK-3",
  "Asia/Sakhalin": "UNK-11",
  "Asia/Samarkand": "UNK-5",
  "Asia/Seoul": "KST-9",
  "Asia/Shanghai": "CST-8",
  "Asia/Singapore": "UNK-8",
  "Asia/Srednekolymsk": "UNK-11",
  "Asia/Taipei": "CST-8",
  "Asia/Tashkent": "UNK-5",
  "Asia/Tbilisi": "UNK-4",
  "Asia/Tehran": "UNK-3:30UNK,J79/24,J263/24",
  "Asia/Thimphu": "UNK-6",
  "Asia/Tokyo": "JST-9",
  "Asia/Tomsk": "UNK-7",
  "Asia/Ulaanbaatar": "UNK-8",
  "Asia/Urumqi": "UNK-6",
  "Asia/Ust-Nera": "UNK-10",
  "Asia/Vientiane": "UNK-7",
  "Asia/Vladivostok": "UNK-10",
  "Asia/Yakutsk": "UNK-9",
  "Asia/Yangon": "UNK-6:30",
  "Asia/Yekaterinburg": "UNK-5",
  "Asia/Yerevan": "UNK-4",
  "Atlantic/Azores": "UNK1UNK,M3.5.0/0,M10.5.0/1",
  "Atlantic/Bermuda": "AST4ADT,M3.2.0,M11.1.0",
  "Atlantic/Canary": "WET0WEST,M3.5.0/1,M10.5.0",
  "Atlantic/Cape_Verde": "UNK1",
  "Atlantic/Faroe": "WET0WEST,M3.5.0/1,M10.5.0",
  "Atlantic/Madeira": "WET0WEST,M3.5.0/1,M10.5.0",
  "Atlantic/Reykjavik": "GMT0",
  "Atlantic/South_Georgia": "UNK2",
  "Atlantic/St_Helena": "GMT0",
  "Atlantic/Stanley": "UNK3",
  "Australia/Adelaide": "ACST-9:30ACDT,M10.1.0,M4.1.0/3",
  "Australia/Brisbane": "AEST-10",
  "Australia/Broken_Hill": "ACST-9:30ACDT,M10.1.0,M4.1.0/3",
  "Australia/Currie": "AEST-10AEDT,M10.1.0,M4.1.0/3",
  "Australia/Darwin": "ACST-9:30",
  "Australia/Eucla": "UNK-8:45",
  "Australia/Hobart": "AEST-10AEDT,M10.1.0,M4.1.0/3",
  "Australia/Lindeman": "AEST-10",
  "Australia/Lord_Howe": "UNK-10:30UNK-11,M10.1.0,M4.1.0",
  "Australia/Melbourne": "AEST-10AEDT,M10.1.0,M4.1.0/3",
  "Australia/Perth": "AWST-8",
  "Australia/Sydney": "AEST-10AEDT,M10.1.0,M4.1.0/3",
  "Etc/GMT": "GMT0",
  "Etc/GMT+0": "GMT0",
  "Etc/GMT+1": "UNK1",
  "Etc/GMT+10": "UNK10",
  "Etc/GMT+11": "UNK11",
  "Etc/GMT+12": "UNK12",
  "Etc/GMT+2": "UNK2",
  "Etc/GMT+3": "UNK3",
  "Etc/GMT+4": "UNK4",
  "Etc/GMT+5": "UNK5",
  "Etc/GMT+6": "UNK6",
  "Etc/GMT+7": "UNK7",
  "Etc/GMT+8": "UNK8",
  "Etc/GMT+9": "UNK9",
  "Etc/GMT-0": "GMT0",
  "Etc/GMT-1": "UNK-1",
  "Etc/GMT-10": "UNK-10",
  "Etc/GMT-11": "UNK-11",
  "Etc/GMT-12": "UNK-12",
  "Etc/GMT-13": "UNK-13",
  "Etc/GMT-14": "UNK-14",
  "Etc/GMT-2": "UNK-2",
  "Etc/GMT-3": "UNK-3",
  "Etc/GMT-4": "UNK-4",
  "Etc/GMT-5": "UNK-5",
  "Etc/GMT-6": "UNK-6",
  "Etc/GMT-7": "UNK-7",
  "Etc/GMT-8": "UNK-8",
  "Etc/GMT-9": "UNK-9",
  "Etc/GMT0": "GMT0",
  "Etc/Greenwich": "GMT0",
  "Etc/UCT": "UTC0",
  "Etc/UTC": "UTC0",
  "Etc/Universal": "UTC0",
  "Etc/Zulu": "UTC0",
  "Europe/Amsterdam": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Andorra": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Astrakhan": "UNK-4",
  "Europe/Athens": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Belgrade": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Berlin": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Bratislava": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Brussels": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Bucharest": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Budapest": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Busingen": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Chisinau": "EET-2EEST,M3.5.0,M10.5.0/3",
  "Europe/Copenhagen": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Dublin": "IST-1GMT0,M10.5.0,M3.5.0/1",
  "Europe/Gibraltar": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Guernsey": "GMT0BST,M3.5.0/1,M10.5.0",
  "Europe/Helsinki": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Isle_of_Man": "GMT0BST,M3.5.0/1,M10.5.0",
  "Europe/Istanbul": "UNK-3",
  "Europe/Jersey": "GMT0BST,M3.5.0/1,M10.5.0",
  "Europe/Kaliningrad": "EET-2",
  "Europe/Kiev": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Kirov": "UNK-3",
  "Europe/Lisbon": "WET0WEST,M3.5.0/1,M10.5.0",
  "Europe/Ljubljana": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/London": "GMT0BST,M3.5.0/1,M10.5.0",
  "Europe/Luxembourg": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Madrid": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Malta": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Mariehamn": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Minsk": "UNK-3",
  "Europe/Monaco": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Moscow": "MSK-3",
  "Europe/Oslo": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Paris": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Podgorica": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Prague": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Riga": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Rome": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Samara": "UNK-4",
  "Europe/San_Marino": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Sarajevo": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Saratov": "UNK-4",
  "Europe/Simferopol": "MSK-3",
  "Europe/Skopje": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Sofia": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Stockholm": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Tallinn": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Tirane": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Ulyanovsk": "UNK-4",
  "Europe/Uzhgorod": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Vaduz": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Vatican": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Vienna": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Vilnius": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Volgograd": "UNK-4",
  "Europe/Warsaw": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Zagreb": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Europe/Zaporozhye": "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "Europe/Zurich": "CET-1CEST,M3.5.0,M10.5.0/3",
  "Indian/Antananarivo": "EAT-3",
  "Indian/Chagos": "UNK-6",
  "Indian/Christmas": "UNK-7",
  "Indian/Cocos": "UNK-6:30",
  "Indian/Comoro": "EAT-3",
  "Indian/Kerguelen": "UNK-5",
  "Indian/Mahe": "UNK-4",
  "Indian/Maldives": "UNK-5",
  "Indian/Mauritius": "UNK-4",
  "Indian/Mayotte": "EAT-3",
  "Indian/Reunion": "UNK-4",
  "Pacific/Apia": "UNK-13UNK,M9.5.0/3,M4.1.0/4",
  "Pacific/Auckland": "NZST-12NZDT,M9.5.0,M4.1.0/3",
  "Pacific/Bougainville": "UNK-11",
  "Pacific/Chatham": "UNK-12:45UNK,M9.5.0/2:45,M4.1.0/3:45",
  "Pacific/Chuuk": "UNK-10",
  "Pacific/Easter": "UNK6UNK,M9.1.6/22,M4.1.6/22",
  "Pacific/Efate": "UNK-11",
  "Pacific/Enderbury": "UNK-13",
  "Pacific/Fakaofo": "UNK-13",
  "Pacific/Fiji": "UNK-12UNK,M11.2.0,M1.2.3/99",
  "Pacific/Funafuti": "UNK-12",
  "Pacific/Galapagos": "UNK6",
  "Pacific/Gambier": "UNK9",
  "Pacific/Guadalcanal": "UNK-11",
  "Pacific/Guam": "ChST-10",
  "Pacific/Honolulu": "HST10",
  "Pacific/Kiritimati": "UNK-14",
  "Pacific/Kosrae": "UNK-11",
  "Pacific/Kwajalein": "UNK-12",
  "Pacific/Majuro": "UNK-12",
  "Pacific/Marquesas": "UNK9:30",
  "Pacific/Midway": "SST11",
  "Pacific/Nauru": "UNK-12",
  "Pacific/Niue": "UNK11",
  "Pacific/Norfolk": "UNK-11UNK,M10.1.0,M4.1.0/3",
  "Pacific/Noumea": "UNK-11",
  "Pacific/Pago_Pago": "SST11",
  "Pacific/Palau": "UNK-9",
  "Pacific/Pitcairn": "UNK8",
  "Pacific/Pohnpei": "UNK-11",
  "Pacific/Port_Moresby": "UNK-10",
  "Pacific/Rarotonga": "UNK10",
  "Pacific/Saipan": "ChST-10",
  "Pacific/Tahiti": "UNK10",
  "Pacific/Tarawa": "UNK-12",
  "Pacific/Tongatapu": "UNK-13",
  "Pacific/Wake": "UNK-12",
  "Pacific/Wallis": "UNK-12"
};
const Server = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<path fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 7a3 3 0 0 1 3-3h12a3 3 0 0 1 3 3v2a3 3 0 0 1-3 3H6a3 3 0 0 1-3-3zm0 8a3 3 0 0 1 3-3h12a3 3 0 0 1 3 3v2a3 3 0 0 1-3 3H6a3 3 0 0 1-3-3zm4-7v.01M7 16v.01"/>`}<!-- HTML_TAG_END --></svg>`;
});
const Clock = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M3 12a9 9 0 1 0 18 0a9 9 0 0 0-18 0"/><path d="M12 7v5l3 3"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
const Clock_pin = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  return `<svg${spread(
    [
      { viewBox: "0 0 24 24" },
      { width: "1.2em" },
      { height: "1.2em" },
      escape_object($$props)
    ],
    {}
  )}><!-- HTML_TAG_START -->${`<g fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"><path d="M20.971 11.278a9 9 0 1 0-8.313 9.698"/><path d="M12 7v5l1.5 1.5m7.621 6.621a3 3 0 1 0-4.242 0c.418.419 1.125 1.045 2.121 1.879c1.051-.89 1.759-1.516 2.121-1.879zM19 18v.01"/></g>`}<!-- HTML_TAG_END --></svg>`;
});
function convertSeconds(seconds) {
  let minutes = Math.floor(seconds / 60);
  let hours = Math.floor(minutes / 60);
  let days = Math.floor(hours / 24);
  hours = hours % 24;
  minutes = minutes % 60;
  seconds = seconds % 60;
  let result = "";
  if (days > 0) {
    result += days + " day" + (days > 1 ? "s" : "") + " ";
  }
  if (hours > 0) {
    result += hours + " hour" + (hours > 1 ? "s" : "") + " ";
  }
  if (minutes > 0) {
    result += minutes + " minute" + (minutes > 1 ? "s" : "") + " ";
  }
  result += seconds + " second" + (seconds > 1 ? "s" : "");
  return result;
}
const NTP_1 = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let $user, $$unsubscribe_user;
  let $page, $$unsubscribe_page;
  $$unsubscribe_user = subscribe(user, (value) => $user = value);
  $$unsubscribe_page = subscribe(page, (value) => $page = value);
  let ntpSettings;
  let ntpStatus;
  async function getNTPStatus() {
    try {
      const response = await fetch("/rest/ntpStatus", {
        method: "GET",
        headers: {
          Authorization: $page.data.features.security ? "Bearer " + $user.bearer_token : "Basic",
          "Content-Type": "application/json"
        }
      });
      ntpStatus = await response.json();
    } catch (error) {
      console.error("Error:", error);
    }
    return;
  }
  const interval = setInterval(
    async () => {
      getNTPStatus();
    },
    5e3
  );
  onDestroy(() => clearInterval(interval));
  let formField;
  $$unsubscribe_user();
  $$unsubscribe_page();
  return `${validate_component(SettingsCard, "SettingsCard").$$render($$result, { collapsible: false }, {}, {
    title: () => {
      return `<span slot="title" data-svelte-h="svelte-hj6ecs">Network Time</span>`;
    },
    icon: () => {
      return `${validate_component(Clock, "Clock").$$render(
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
      return `<div class="w-full overflow-x-auto">${function(__value) {
        if (is_promise(__value)) {
          __value.then(null, noop);
          return ` ${validate_component(Spinner, "Spinner").$$render($$result, {}, {}, {})} `;
        }
        return function(nothing) {
          return ` <div class="flex w-full flex-col space-y-1"><div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="${"mask mask-hexagon h-auto w-10 " + escape(ntpStatus.status === 1 ? "bg-success" : "bg-error", true)}">${validate_component(Clock_check, "NTP").$$render(
            $$result,
            {
              class: "h-auto w-full scale-75 " + (ntpStatus.status === 1 ? "text-success-content" : "text-error-content")
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-yizsky">Status</div> <div class="text-sm opacity-75">${escape(ntpStatus.status === 1 ? "Active" : "Inactive")}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Server, "Server").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-1ruhky7">NTP Server</div> <div class="text-sm opacity-75">${escape(ntpStatus.server)}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Clock, "Clock").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-1scjaqw">Local Time</div> <div class="text-sm opacity-75">${escape(new Intl.DateTimeFormat("en-GB", { dateStyle: "long", timeStyle: "long" }).format(new Date(ntpStatus.local_time)))}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(Clock_pin, "UTC").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-djnpl1">UTC Time</div> <div class="text-sm opacity-75">${escape(new Intl.DateTimeFormat(
            "en-GB",
            {
              dateStyle: "long",
              timeStyle: "long",
              timeZone: "UTC"
            }
          ).format(new Date(ntpStatus.utc_time)))}</div></div></div> <div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2"><div class="mask mask-hexagon bg-primary h-auto w-10">${validate_component(_24_hours, "Stopwatch").$$render(
            $$result,
            {
              class: "text-primary-content h-auto w-full scale-75"
            },
            {},
            {}
          )}</div> <div><div class="font-bold" data-svelte-h="svelte-1pnqi1s">Uptime</div> <div class="text-sm opacity-75">${escape(convertSeconds(ntpStatus.uptime))}</div></div></div></div> `;
        }();
      }(getNTPStatus())}</div> ${!$page.data.features.security || $user.admin ? `${validate_component(Collapsible, "Collapsible").$$render($$result, { open: false, class: "shadow-lg" }, {}, {
        title: () => {
          return `<span slot="title" data-svelte-h="svelte-1wzwrhq">Change NTP Settings</span>`;
        },
        default: () => {
          return `<form class="form-control w-full" novalidate${add_attribute("this", formField, 0)}><label class="label cursor-pointer justify-start gap-4"><input type="checkbox" class="checkbox checkbox-primary"${add_attribute("checked", ntpSettings.enabled, 1)}> <span class="" data-svelte-h="svelte-ym05am">Enable NTP</span></label> <label class="label" for="server" data-svelte-h="svelte-btu987"><span class="label-text text-md">Server</span></label> <input type="text" min="3" max="64" class="${"input input-bordered invalid:border-error w-full invalid:border-2 " + escape("", true)}" id="server" required${add_attribute("value", ntpSettings.server, 0)}> <label class="label" for="subnet"><span class="${"label-text-alt text-error " + escape("hidden", true)}">Must be a valid IPv4 address or URL</span></label> <label class="label" for="tz" data-svelte-h="svelte-gml4zz"><span class="label-text text-md">Pick Time Zone</span></label> <select class="select select-bordered" id="tz">${each(Object.entries(TIME_ZONES), ([tz_label, tz_format]) => {
            return `<option${add_attribute("value", tz_label, 0)}>${escape(tz_label)}</option>`;
          })}</select> <div class="mt-6 place-self-end" data-svelte-h="svelte-1q4yi5x"><button class="btn btn-primary" type="submit">Apply Settings</button></div></form>`;
        }
      })}` : ``}`;
    }
  })}`;
});
const Page = create_ssr_component(($$result, $$props, $$bindings, slots) => {
  let { data } = $$props;
  if ($$props.data === void 0 && $$bindings.data && data !== void 0)
    $$bindings.data(data);
  return `<div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8">${validate_component(NTP_1, "NTP").$$render($$result, {}, {}, {})}</div>`;
});
export {
  Page as default
};
