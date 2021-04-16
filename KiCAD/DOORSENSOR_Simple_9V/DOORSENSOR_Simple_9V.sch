EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Regulator_Linear:AMS1117-3.3 U1
U 1 1 6076FCE2
P 3600 1850
F 0 "U1" H 3600 2092 50  0000 C CNN
F 1 "AMS1117-3.3" H 3600 2001 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 3600 2050 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 3700 1600 50  0001 C CNN
	1    3600 1850
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Reed SW1
U 1 1 607709A2
P 1700 1900
F 0 "SW1" V 1654 1985 50  0000 L CNN
F 1 "Trigger" V 1745 1985 50  0000 L CNN
F 2 "SHRDZM:Magnetic Reed Relay" H 1700 1900 50  0001 C CNN
F 3 "~" H 1700 1900 50  0001 C CNN
	1    1700 1900
	0    1    1    0   
$EndComp
$Comp
L Device:CP C1
U 1 1 60771802
P 1700 2600
F 0 "C1" H 1818 2646 50  0000 L CNN
F 1 "100uF" H 1818 2555 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.50mm" H 1738 2450 50  0001 C CNN
F 3 "~" H 1700 2600 50  0001 C CNN
	1    1700 2600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 60772F57
P 1000 3100
F 0 "#PWR02" H 1000 2850 50  0001 C CNN
F 1 "GND" H 1005 2927 50  0000 C CNN
F 2 "" H 1000 3100 50  0001 C CNN
F 3 "" H 1000 3100 50  0001 C CNN
	1    1000 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 1600 1700 1600
Wire Wire Line
	1700 1600 1700 1700
Wire Wire Line
	1700 2100 1700 2250
Wire Wire Line
	2300 2250 2150 2250
Connection ~ 1700 2250
Wire Wire Line
	1700 2250 1700 2450
$Comp
L power:GND #PWR03
U 1 1 60773683
P 1700 3100
F 0 "#PWR03" H 1700 2850 50  0001 C CNN
F 1 "GND" H 1705 2927 50  0000 C CNN
F 2 "" H 1700 3100 50  0001 C CNN
F 3 "" H 1700 3100 50  0001 C CNN
	1    1700 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 2750 1700 3100
$Comp
L Device:R R1
U 1 1 60773A0E
P 2150 2600
F 0 "R1" H 2220 2646 50  0000 L CNN
F 1 "33k" H 2220 2555 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2080 2600 50  0001 C CNN
F 3 "~" H 2150 2600 50  0001 C CNN
	1    2150 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 2250 2150 2450
Connection ~ 2150 2250
Wire Wire Line
	2150 2250 1700 2250
$Comp
L power:GND #PWR04
U 1 1 60774281
P 2150 3100
F 0 "#PWR04" H 2150 2850 50  0001 C CNN
F 1 "GND" H 2155 2927 50  0000 C CNN
F 2 "" H 2150 3100 50  0001 C CNN
F 3 "" H 2150 3100 50  0001 C CNN
	1    2150 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 2750 2150 3100
$Comp
L power:GND #PWR05
U 1 1 60774757
P 2800 3100
F 0 "#PWR05" H 2800 2850 50  0001 C CNN
F 1 "GND" H 2805 2927 50  0000 C CNN
F 2 "" H 2800 3100 50  0001 C CNN
F 3 "" H 2800 3100 50  0001 C CNN
	1    2800 3100
	1    0    0    -1  
$EndComp
$Comp
L RF_Module:ESP-12F U2
U 1 1 60775E1B
P 5150 2350
F 0 "U2" H 5150 3331 50  0000 C CNN
F 1 "ESP-12F" H 5150 3240 50  0000 C CNN
F 2 "RF_Module:ESP-12E" H 5150 2350 50  0001 C CNN
F 3 "http://wiki.ai-thinker.com/_media/esp8266/esp8266_series_modules_user_manual_v1.1.pdf" H 4800 2450 50  0001 C CNN
	1    5150 2350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 6077856C
P 5150 3100
F 0 "#PWR07" H 5150 2850 50  0001 C CNN
F 1 "GND" H 5155 2927 50  0000 C CNN
F 2 "" H 5150 3100 50  0001 C CNN
F 3 "" H 5150 3100 50  0001 C CNN
	1    5150 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3050 5150 3100
$Comp
L Switch:SW_Reed SW2
U 1 1 607790E4
P 6250 2900
F 0 "SW2" V 6204 2985 50  0000 L CNN
F 1 "Open/Close" V 6295 2985 50  0000 L CNN
F 2 "SHRDZM:Magnetic Reed Relay" H 6250 2900 50  0001 C CNN
F 3 "~" H 6250 2900 50  0001 C CNN
	1    6250 2900
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR09
U 1 1 6077A1D1
P 6250 3100
F 0 "#PWR09" H 6250 2850 50  0001 C CNN
F 1 "GND" H 6255 2927 50  0000 C CNN
F 2 "" H 6250 3100 50  0001 C CNN
F 3 "" H 6250 3100 50  0001 C CNN
	1    6250 3100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 6077AD05
P 5950 2900
F 0 "R2" H 6020 2946 50  0000 L CNN
F 1 "10k" H 6020 2855 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 5880 2900 50  0001 C CNN
F 3 "~" H 5950 2900 50  0001 C CNN
	1    5950 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 2650 5950 2650
Wire Wire Line
	5950 2650 5950 2750
$Comp
L power:GND #PWR08
U 1 1 6077B6AB
P 5950 3100
F 0 "#PWR08" H 5950 2850 50  0001 C CNN
F 1 "GND" H 5955 2927 50  0000 C CNN
F 2 "" H 5950 3100 50  0001 C CNN
F 3 "" H 5950 3100 50  0001 C CNN
	1    5950 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 3050 5950 3100
$Comp
L eec:NTR4003NT1G Q1
U 1 1 60784E62
P 2200 2250
F 0 "Q1" H 2918 2196 50  0000 L CNN
F 1 "NTR4003NT1G" H 2918 2105 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2200 2750 50  0001 L CNN
F 3 "https://www.onsemi.com/pub/Collateral/NTR4003N-D.PDF" H 2200 2850 50  0001 L CNN
F 4 "Yes" H 2200 2950 50  0001 L CNN "automotive"
F 5 "Grade 0" H 2200 3050 50  0001 L CNN "automotive grade"
F 6 "Trans" H 2200 3150 50  0001 L CNN "category"
F 7 "830106" H 2200 3250 50  0001 L CNN "ciiva ids"
F 8 "0.5A" H 2200 3350 50  0001 L CNN "continuous drain current"
F 9 "False" H 2200 3450 50  0001 L CNN "depletion mode"
F 10 "Discrete Semiconductors" H 2200 3550 50  0001 L CNN "device class L1"
F 11 "Transistors" H 2200 3650 50  0001 L CNN "device class L2"
F 12 "MOSFETs" H 2200 3750 50  0001 L CNN "device class L3"
F 13 "MOSFET N-CH 30V 500MA SOT-23" H 2200 3850 50  0001 L CNN "digikey description"
F 14 "NTR4003NT1GOSCT-ND" H 2200 3950 50  0001 L CNN "digikey part number"
F 15 "30V" H 2200 4050 50  0001 L CNN "drain to source breakdown voltage"
F 16 "1Ω" H 2200 4150 50  0001 L CNN "drain to source resistance"
F 17 "30V" H 2200 4250 50  0001 L CNN "drain to source voltage"
F 18 "https://www.onsemi.com/pub/Collateral/318-08.PDF" H 2200 4350 50  0001 L CNN "footprint url"
F 19 "1.15nC @ 5V" H 2200 4450 50  0001 L CNN "gate charge at vgs"
F 20 "20V" H 2200 4550 50  0001 L CNN "gate to source voltage"
F 21 "1.1mm" H 2200 4650 50  0001 L CNN "height"
F 22 "21pF @ 5V" H 2200 4750 50  0001 L CNN "input capacitace at vds"
F 23 "Yes" H 2200 4850 50  0001 L CNN "lead free"
F 24 "939a4efbd895d123" H 2200 4950 50  0001 L CNN "library id"
F 25 "On Semiconductor" H 2200 5050 50  0001 L CNN "manufacturer"
F 26 "0.7V" H 2200 5150 50  0001 L CNN "max forward diode voltage"
F 27 "+150°C" H 2200 5250 50  0001 L CNN "max junction temp"
F 28 "MOSFET NFET 30V .56A 1500M\\n\\n" H 2200 5350 50  0001 L CNN "mouser description"
F 29 "863-NTR4003NT1G" H 2200 5450 50  0001 L CNN "mouser part number"
F 30 "1" H 2200 5550 50  0001 L CNN "number of N channels"
F 31 "1" H 2200 5650 50  0001 L CNN "number of channels"
F 32 "SOT23-3" H 2200 5750 50  0001 L CNN "package"
F 33 "0.69W" H 2200 5850 50  0001 L CNN "power dissipation"
F 34 "1.7A" H 2200 5950 50  0001 L CNN "pulse drain current"
F 35 "1407165756" H 2200 6050 50  0001 L CNN "release date"
F 36 "14ns" H 2200 6150 50  0001 L CNN "reverse recovery time"
F 37 "Yes" H 2200 6250 50  0001 L CNN "rohs"
F 38 "300°C/W" H 2200 6350 50  0001 L CNN "rthja max"
F 39 "0.01mm" H 2200 6450 50  0001 L CNN "standoff height"
F 40 "+150°C" H 2200 6550 50  0001 L CNN "temperature range high"
F 41 "-55°C" H 2200 6650 50  0001 L CNN "temperature range low"
F 42 "1.4V" H 2200 6750 50  0001 L CNN "threshold vgs max"
F 43 "0.8V" H 2200 6850 50  0001 L CNN "threshold vgs min"
F 44 "0.33S" H 2200 6950 50  0001 L CNN "transconductance"
F 45 "65.1ns" H 2200 7050 50  0001 L CNN "turn off delay time"
F 46 "16.7ns" H 2200 7150 50  0001 L CNN "turn on delay time"
F 47 "0.65V" H 2200 7250 50  0001 L CNN "typ forward diode voltage"
	1    2200 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2650 2800 3100
Wire Wire Line
	1700 1600 2800 1600
Wire Wire Line
	3150 1600 3150 1850
Wire Wire Line
	3150 1850 3300 1850
Connection ~ 1700 1600
Wire Wire Line
	3600 2150 3150 2150
Wire Wire Line
	3150 2150 3150 2050
Wire Wire Line
	3150 2050 2900 2050
$Comp
L Device:C C2
U 1 1 6078DB4C
P 3900 2650
F 0 "C2" H 4015 2696 50  0000 L CNN
F 1 "22uF" H 4015 2605 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 3938 2500 50  0001 C CNN
F 3 "~" H 3900 2650 50  0001 C CNN
	1    3900 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 1850 3900 2500
$Comp
L power:GND #PWR06
U 1 1 6078E715
P 3900 3100
F 0 "#PWR06" H 3900 2850 50  0001 C CNN
F 1 "GND" H 3905 2927 50  0000 C CNN
F 2 "" H 3900 3100 50  0001 C CNN
F 3 "" H 3900 3100 50  0001 C CNN
	1    3900 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2800 3900 3100
Wire Wire Line
	3900 1850 4150 1850
Wire Wire Line
	4150 1850 4150 1550
Wire Wire Line
	4150 1550 5150 1550
Connection ~ 3900 1850
Wire Wire Line
	4150 1850 4150 1950
Wire Wire Line
	4150 1950 4550 1950
Connection ~ 4150 1850
Wire Wire Line
	6250 2250 5750 2250
Wire Wire Line
	6250 2250 6250 2700
Wire Wire Line
	5750 2750 5750 3100
Wire Wire Line
	4350 3100 4350 1750
Wire Wire Line
	4350 1750 4550 1750
Wire Wire Line
	4350 3100 5750 3100
$Comp
L Switch:SW_Push SW3
U 1 1 60791BF0
P 6950 2750
F 0 "SW3" V 6904 2898 50  0000 L CNN
F 1 "Pairing" V 6995 2898 50  0000 L CNN
F 2 "Button_Switch_SMD:SW_SPST_CK_RS282G05A3" H 6950 2950 50  0001 C CNN
F 3 "~" H 6950 2950 50  0001 C CNN
	1    6950 2750
	0    1    1    0   
$EndComp
Wire Wire Line
	5750 2450 6950 2450
$Comp
L power:GND #PWR010
U 1 1 60793010
P 6950 3100
F 0 "#PWR010" H 6950 2850 50  0001 C CNN
F 1 "GND" H 6955 2927 50  0000 C CNN
F 2 "" H 6950 3100 50  0001 C CNN
F 3 "" H 6950 3100 50  0001 C CNN
	1    6950 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 2450 6950 2550
Wire Wire Line
	6950 2950 6950 3100
NoConn ~ 4550 2850
NoConn ~ 4550 2750
NoConn ~ 4550 2650
NoConn ~ 4550 2550
NoConn ~ 4550 2450
NoConn ~ 4550 2350
NoConn ~ 4550 2150
NoConn ~ 5750 2150
NoConn ~ 5750 2050
NoConn ~ 5750 1950
NoConn ~ 5750 1850
NoConn ~ 5750 1750
NoConn ~ 5750 2350
NoConn ~ 5750 2550
$Comp
L power:+3V3 #PWR011
U 1 1 607998E4
P 4150 1400
F 0 "#PWR011" H 4150 1250 50  0001 C CNN
F 1 "+3V3" H 4165 1573 50  0000 C CNN
F 2 "" H 4150 1400 50  0001 C CNN
F 3 "" H 4150 1400 50  0001 C CNN
	1    4150 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 1400 4150 1550
Connection ~ 4150 1550
Wire Wire Line
	750  3000 1000 3000
Connection ~ 1000 3000
Wire Wire Line
	1000 3000 1000 3100
$Comp
L power:PWR_FLAG #FLG01
U 1 1 6079C017
P 2800 1450
F 0 "#FLG01" H 2800 1525 50  0001 C CNN
F 1 "PWR_FLAG" H 2800 1623 50  0000 C CNN
F 2 "" H 2800 1450 50  0001 C CNN
F 3 "~" H 2800 1450 50  0001 C CNN
	1    2800 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 1450 2800 1600
Connection ~ 2800 1600
Wire Wire Line
	2800 1600 3150 1600
$Comp
L power:PWR_FLAG #FLG02
U 1 1 6079D795
P 2900 1950
F 0 "#FLG02" H 2900 2025 50  0001 C CNN
F 1 "PWR_FLAG" H 2900 2123 50  0000 C CNN
F 2 "" H 2900 1950 50  0001 C CNN
F 3 "~" H 2900 1950 50  0001 C CNN
	1    2900 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 1950 2900 2050
Connection ~ 2900 2050
Wire Wire Line
	2900 2050 2800 2050
$Comp
L power:PWR_FLAG #FLG03
U 1 1 6079EFEF
P 750 2950
F 0 "#FLG03" H 750 3025 50  0001 C CNN
F 1 "PWR_FLAG" H 750 3123 50  0000 C CNN
F 2 "" H 750 2950 50  0001 C CNN
F 3 "~" H 750 2950 50  0001 C CNN
	1    750  2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	750  3000 750  2950
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 607A3688
P 800 2250
F 0 "J1" H 692 1925 50  0000 C CNN
F 1 "9V" H 692 2016 50  0000 C CNN
F 2 "Connector_JST:JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal" H 800 2250 50  0001 C CNN
F 3 "~" H 800 2250 50  0001 C CNN
	1    800  2250
	-1   0    0    1   
$EndComp
Wire Wire Line
	1000 2250 1000 3000
Wire Wire Line
	1000 1600 1000 2150
$EndSCHEMATC
