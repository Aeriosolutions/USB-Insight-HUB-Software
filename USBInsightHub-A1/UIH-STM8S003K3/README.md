# USB Insight Hub – STM8S003K3
## 1. Development Environment
The STM8S003K3 is programmed directly in C using [ST Visual Develop (4.3.12)]( https://www.st.com/en/development-tools/stvd-stm8.html) with STM8 Cosmic compiler (is necessary to [register]( https://www.cosmicsoftware.com/download_stm8_free.php) for free use). 
The code is implemented barebone and is based on included STM8 examples. 
## 2. Programming
ST Visual Programmer (software) and ST-Link-V2 programmer are used to program the STM8S003K3. A cable adapter is necessary to connect the programmer to J2 header in the BASER board. 

Check the [UIH Programing Guide](https://github.com/Aeriosolutions/USB-Insight-HUB-Software/tree/main/USBInsightHub-A1/)

## 3. I2C Protocol

### Register Map
|     Name         |     Type    |     Register Address    |     Default     |     Description                                        |
|------------------|-------------|-------------------------|-----------------|--------------------------------------------------------|
|     WHOAMI       |     R       |     10                  |                 |     Who I'm I ID                                       |
|     VERSION      |     R       |     12                  |                 |     FW Version                                         |
|     CH1REG       |     R/W     |     20                  |     xm00mm0x    |     CH1 Fault, Power   enable and current settings.    |
|     CH2REG       |     R/W     |     21                  |     xm00mm0x    |     CH2 Fault, Power   enable and current settings.    |
|     CH3REG       |     R/W     |     22                  |     xm00mm0x    |     CH3 Fault, Power   enable and current settings.    |
|     CCSUM        |     R       |     23                  |     00000000    |     VEXT and VOST CC   lines summary                   |
|     AUXREG       |     R       |     24                  |     00000000    |     USB3 and Power   Muxer values                      |
|     MUXOECTR     |     R/W     |     26                  |     00000001    |     MUXOE Control                                      |
|     VEXTCC1L     |     R       |     30                  |     00000000    |     VEXT CC1 voltage   reading                         |
|     VEXTCC1H     |     R       |     31                  |     00000000    |     ""                                                 |
|     VEXTCC2L     |     R       |     32                  |     00000000    |     VEXT CC2 voltage   reading                         |
|     VEXTCC2H     |     R       |     33                  |     00000000    |     ""                                                 |
|     VHOSTCC1L    |     R       |     34                  |     00000000    |     VHOST CC1 voltage   reading                        |
|     VHOSTCC1H    |     R       |     35                  |     00000000    |     ""                                                 |
|     VHOSTCC2L    |     R       |     36                  |     00000000    |     VHOST CC2 voltage   reading                        |
|     VHOSTCC2H    |     R       |     37                  |     00000000    |     ""                                                 |

### Register description

**<span style="color:blue">WHOAMI (10h)</span>**

Read Only register with a fixed value of 35h

**<span style="color:blue">VERSION (12h)</span>**

Firmware version in hex (number)

**<span style="color:blue">CHxREG (20h, 21h, 22h)</span>**

CHx Fault, Power Enable and current settings

|     bit    	|                  	|                            Description                          	|                        R                      	|                           W                          	|
|:----------:	|:----------------:	|:---------------------------------------------------------------:	|:---------------------------------------------:	|:----------------------------------------------------:	|
|     0      	|     CHxILIMIG    	|     Ignore   or set CHx ILIM values when writing to register    	|     0                                         	|     0 - Ignore      1 - Set                          	|
|     1      	|     CHxILIMH     	|     Set or   read the current limit configuration resistors     	|     CHxILIML                                  	|     See   table 1                                    	|
|     2      	|     CHxILIML     	|                                                                 	|     CHxILIMH                                  	|     See   Table 1                                    	|
|     3      	|     PWRENxIG     	|     Ignore   or set CHx PWREN when writing register             	|     0                                         	|     0 - Ignore      1 - Set                          	|
|     4      	|     PWRENx       	|     CHx   PWREN value.                                          	|     PWRENx                                    	|     0 -   Power disabled     1 -   Power enabled     	|
|     5      	|     DATAENxIG    	|     Ignore   or set CHx DATAEN when writing register            	|     0                                         	|     0 - Ignore      1 - Set                          	|
|     6      	|     DATAENx      	|     CHx DATA Enable   value                                     	|     DATAENx                                   	|     0 -   Data disabled     1 -   Data enabled       	|
|     7      	|     CHxFAULT     	|     CHx   Fault flag from the power switch                      	|     0 - No   fault     1 -   Fault present    	|     Read   Only                                      	|

**Table 1** - Current limit settings

|     CHxILIML    	|     CHxILIMH    	|     Current   limit    	|
|-----------------	|-----------------	|------------------------	|
|     0           	|     0           	|     0.566 A            	|
|     0           	|     1           	|     1.075 A            	|
|     1           	|     0           	|     1.544 A            	|
|     1           	|     1           	|     2.022 A            	|


**<span style="color:blue">CCSUM (23h)</span>**

VEXT and VOST CC lines summary. Provides the state of the CC lines without reading the raw analog voltage values.

|     bit    	|     7              	|     6              	|     5           	|     4           	|     3             	|     2             	|     1          	|     0           	|
|------------	|--------------------	|--------------------	|-----------------	|-----------------	|-------------------	|-------------------	|----------------	|-----------------	|
|            	|     HOSTCCSTAT1    	|     HOSTCCSTAT0    	|     VHOSTCC1    	|     VHOSTCC0    	|     EXTCCSTAT1    	|     EXTCSTATC0    	|     VEXTCC1    	|     VEXTCC10    	|

**Table 2** - [CC current capacity](https://hackaday.com/2023/01/04/all-about-usb-c-resistors-and-emarkers/)

|     VzCC1    	|     VzCC0    	|     Range               	|     Pull-up Rp     4.75-5.5 V    	|     Current   Capacity        	|     Dec values for a   10 bit ADC     V ref = 3.3V    	|
|--------------	|--------------	|-------------------------	|----------------------------------	|-------------------------------	|-------------------------------------------------------	|
|     0        	|     0        	|     -0.25V   - 0.15V    	|                                  	|     Unknown   - No pull up    	|     0 -47                                             	|
|     0        	|     1        	|     0.25V -   0.61V     	|     56k +/- 20%                  	|     Default   USB Power       	|     78 - 189                                          	|
|     1        	|     0        	|     0.70V -   1.16V     	|     22k +/- 5%                   	|     1.5 A                     	|     217 - 360                                         	|
|     1        	|     1        	|     1.31V -   2.04V     	|     10k +/- 5%                   	|     3.0 A                     	|     406 - 632                                         	|

z = HOST or EXT

**Table 3** - CCSTAT

|     VzCCSTAT1    	|     VzCCSTAT0    	|                                                                                                	|
|------------------	|------------------	|------------------------------------------------------------------------------------------------	|
|     0            	|     0            	|     CC1 nor CC2 has   pull-up                                                                  	|
|     0            	|     1            	|     CC1 has the   pull-up                                                                      	|
|     1            	|     0            	|     CC2 has the   pull-up                                                                      	|
|     1            	|     1            	|     CC1 and CC2 are   pulled up at the same time     CC1 or CC2   voltages are out of range    	|


**<span style="color:blue">AUXREG (24h)</span>**

USB3 and Power Muxer values

|     bit    	|                  	|     Description                              	|     R                                             	|
|------------	|------------------	|----------------------------------------------	|---------------------------------------------------	|
|     0      	|     VHOSTP       	|     Host   used as power source              	|     0 -   VHOST Not used,     1 -   VHOST Used     	|
|     1      	|     VEXTP        	|     External   power used as power source    	|     0 -   VEXT Not used,     1 -   VEXT Used       	|
|     2      	|     U3MUXOE      	|     USB 3   muxer output enable              	|     0 -   Disabled,     1 -   Enabled              	|
|     3      	|     U3MUXSEL     	|     USB 3   muxer selection                  	|     0 -   Position 1,     1 -   Position 2         	|
|     4      	|     FIRSTBOOT    	|     First boot flag                          	|     0 - After first read of this register,     1 -   First boot    	|

**<span style="color:blue">MUXOECTR(26h)</span>**

CC pins voltage readings in raw 10 bits 0-1023. ADC reference 3.3V

|     MSB                 	|     LSB                	|
|-------------------------	|------------------------	|
|     VEXTCC1H (31h)      	|     VEXTCC1L (30h)     	|
|     VEXTCC2H (33h)      	|     VEXTCC2L (32h)     	|
|     VHOSTCC1H (35h)     	|     VHOSTCC1L (34h)    	|
|     VHOSTCC2LH (37h)    	|     VHOSTCC2L (36h)    	|