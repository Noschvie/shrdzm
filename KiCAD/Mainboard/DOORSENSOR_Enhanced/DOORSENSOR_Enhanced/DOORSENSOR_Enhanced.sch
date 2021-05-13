EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "DOORSENSOR Enhanced"
Date "2021-03-21"
Rev "V1.0"
Comp "Erich O. Pintar"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L NDP6020P:NDP6020P Q2
U 1 1 60578BDB
P 4550 2650
F 0 "Q2" H 4980 2704 50  0000 L CNN
F 1 "NDP6020P" H 4980 2795 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Horizontal_TabDown" H 5000 2600 50  0001 L CNN
F 3 "https://www.onsemi.com/pub/Collateral/NDP6020P-D.PDF" H 5000 2500 50  0001 L CNN
F 4 "P-Channel MOSFET, 24 A, 20 V, 3-Pin TO-220 ON Semiconductor NDP6020P" H 5000 2400 50  0001 L CNN "Description"
F 5 "4.7" H 5000 2300 50  0001 L CNN "Height"
F 6 "ON Semiconductor" H 5000 2200 50  0001 L CNN "Manufacturer_Name"
F 7 "NDP6020P" H 5000 2100 50  0001 L CNN "Manufacturer_Part_Number"
F 8 "512-NDP6020P" H 5000 2000 50  0001 L CNN "Mouser Part Number"
F 9 "https://www.mouser.co.uk/ProductDetail/ON-Semiconductor-Fairchild/NDP6020P?qs=mdiO5HdF0Khl%252BuaDKGopOw%3D%3D" H 5000 1900 50  0001 L CNN "Mouser Price/Stock"
F 10 "NDP6020P" H 5000 1800 50  0001 L CNN "Arrow Part Number"
F 11 "https://www.arrow.com/en/products/ndp6020p/on-semiconductor" H 5000 1700 50  0001 L CNN "Arrow Price/Stock"
	1    4550 2650
	1    0    0    1   
$EndComp
$Comp
L 2n2222:2N2222 Q1
U 1 1 6057A176
P 3950 3450
F 0 "Q1" H 4126 3496 50  0000 L CNN
F 1 "2N2222" H 4126 3405 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 3950 3450 60  0001 C CNN
F 3 "" H 3950 3450 60  0000 C CNN
	1    3950 3450
	1    0    0    -1  
$EndComp
$Comp
L Diode:1N4148 D1
U 1 1 6057A855
P 2950 2900
F 0 "D1" H 2950 3117 50  0000 C CNN
F 1 "1N4148" H 2950 3026 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2950 2725 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 2950 2900 50  0001 C CNN
	1    2950 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 6057B49C
P 2950 3450
F 0 "R1" V 2743 3450 50  0000 C CNN
F 1 "10k" V 2834 3450 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 2880 3450 50  0001 C CNN
F 3 "~" H 2950 3450 50  0001 C CNN
	1    2950 3450
	0    1    1    0   
$EndComp
$Comp
L Device:CP C1
U 1 1 6057B838
P 1950 4200
F 0 "C1" H 2068 4246 50  0000 L CNN
F 1 "33uF" H 2068 4155 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D8.0mm_P5.00mm" H 1988 4050 50  0001 C CNN
F 3 "~" H 1950 4200 50  0001 C CNN
	1    1950 4200
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 6057BFC0
P 6600 4500
F 0 "SW2" V 6554 4648 50  0000 L CNN
F 1 "Pairing" V 6645 4648 50  0000 L CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 6600 4700 50  0001 C CNN
F 3 "~" H 6600 4700 50  0001 C CNN
	1    6600 4500
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 6057C5A7
P 4050 2250
F 0 "R2" H 4120 2296 50  0000 L CNN
F 1 "1k" H 4120 2205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 3980 2250 50  0001 C CNN
F 3 "~" H 4050 2250 50  0001 C CNN
	1    4050 2250
	1    0    0    -1  
$EndComp
$Comp
L RF_Module:ESP-12F U1
U 1 1 6057C8AC
P 5400 3850
F 0 "U1" H 5400 4831 50  0000 C CNN
F 1 "ESP-12F" H 5400 4740 50  0000 C CNN
F 2 "RF_Module:ESP-12E" H 5400 3850 50  0001 C CNN
F 3 "http://wiki.ai-thinker.com/_media/esp8266/esp8266_series_modules_user_manual_v1.1.pdf" H 5050 3950 50  0001 C CNN
	1    5400 3850
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR01
U 1 1 6057D1FA
P 1300 1800
F 0 "#PWR01" H 1300 1650 50  0001 C CNN
F 1 "+3.3V" H 1300 2000 50  0000 C CNN
F 2 "" H 1300 1800 50  0001 C CNN
F 3 "" H 1300 1800 50  0001 C CNN
	1    1300 1800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 6057D8DC
P 1950 5100
F 0 "#PWR02" H 1950 4850 50  0001 C CNN
F 1 "GND" H 1955 4927 50  0000 C CNN
F 2 "" H 1950 5100 50  0001 C CNN
F 3 "" H 1950 5100 50  0001 C CNN
	1    1950 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 3050 5400 3050
Wire Wire Line
	4850 3050 4550 3050
Wire Wire Line
	4550 3050 4550 3450
Wire Wire Line
	4550 3450 4800 3450
Connection ~ 4850 3050
Wire Wire Line
	4850 1800 4850 2450
Wire Wire Line
	4050 2100 4050 1800
Connection ~ 4050 1800
Wire Wire Line
	4050 1800 4850 1800
Wire Wire Line
	4050 2400 4050 2650
Wire Wire Line
	4050 2650 4550 2650
Connection ~ 4050 2650
Wire Wire Line
	6000 3950 6600 3950
Wire Wire Line
	6600 3950 6600 4300
$Comp
L power:GND #PWR05
U 1 1 6058442E
P 6600 5100
F 0 "#PWR05" H 6600 4850 50  0001 C CNN
F 1 "GND" H 6605 4927 50  0000 C CNN
F 2 "" H 6600 5100 50  0001 C CNN
F 3 "" H 6600 5100 50  0001 C CNN
	1    6600 5100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 60584D1F
P 5400 5100
F 0 "#PWR04" H 5400 4850 50  0001 C CNN
F 1 "GND" H 5405 4927 50  0000 C CNN
F 2 "" H 5400 5100 50  0001 C CNN
F 3 "" H 5400 5100 50  0001 C CNN
	1    5400 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 4550 5400 4850
Wire Wire Line
	6000 4150 6250 4150
Wire Wire Line
	6250 4850 5400 4850
$Comp
L Switch:SW_Reed SW1
U 1 1 6058584E
P 1950 2150
F 0 "SW1" V 1904 2235 50  0000 L CNN
F 1 "Trigger" V 1995 2235 50  0000 L CNN
F 2 "SHRDZM:Magnetic Reed Relay" H 1950 2150 50  0001 C CNN
F 3 "~" H 1950 2150 50  0001 C CNN
	1    1950 2150
	0    1    1    0   
$EndComp
Wire Wire Line
	1950 4350 1950 5100
Wire Wire Line
	3450 2500 6250 2500
Wire Wire Line
	6250 2500 6250 4050
Wire Wire Line
	6250 4050 6000 4050
Wire Wire Line
	4050 2650 4050 3250
Wire Wire Line
	2800 3450 1950 3450
Connection ~ 1950 3450
Wire Wire Line
	1950 3450 1950 4050
Wire Wire Line
	3100 3450 3750 3450
$Comp
L power:GND #PWR03
U 1 1 6058F445
P 4050 5100
F 0 "#PWR03" H 4050 4850 50  0001 C CNN
F 1 "GND" H 4055 4927 50  0000 C CNN
F 2 "" H 4050 5100 50  0001 C CNN
F 3 "" H 4050 5100 50  0001 C CNN
	1    4050 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 3650 4050 5100
$Comp
L Switch:SW_Reed SW3
U 1 1 60590287
P 7100 4050
F 0 "SW3" V 7054 4135 50  0000 L CNN
F 1 "Open/Close" V 7145 4135 50  0000 L CNN
F 2 "SHRDZM:Magnetic Reed Relay" H 7100 4050 50  0001 C CNN
F 3 "~" H 7100 4050 50  0001 C CNN
	1    7100 4050
	0    1    1    0   
$EndComp
Wire Wire Line
	6000 3750 7100 3750
Wire Wire Line
	7100 3750 7100 3850
$Comp
L power:GND #PWR06
U 1 1 60591EBE
P 7100 5100
F 0 "#PWR06" H 7100 4850 50  0001 C CNN
F 1 "GND" H 7105 4927 50  0000 C CNN
F 2 "" H 7100 5100 50  0001 C CNN
F 3 "" H 7100 5100 50  0001 C CNN
	1    7100 5100
	1    0    0    -1  
$EndComp
NoConn ~ 4800 3650
NoConn ~ 4800 3850
NoConn ~ 4800 3950
NoConn ~ 4800 4050
NoConn ~ 4800 4150
NoConn ~ 4800 4250
NoConn ~ 4800 4350
NoConn ~ 6000 3850
NoConn ~ 6000 3650
NoConn ~ 6000 3550
NoConn ~ 6000 3450
NoConn ~ 6000 3350
NoConn ~ 6000 3250
Wire Wire Line
	1300 3450 1300 1800
Connection ~ 1300 1800
$Comp
L power:GND #PWR07
U 1 1 605BC8F1
P 1300 5100
F 0 "#PWR07" H 1300 4850 50  0001 C CNN
F 1 "GND" H 1305 4927 50  0000 C CNN
F 2 "" H 1300 5100 50  0001 C CNN
F 3 "" H 1300 5100 50  0001 C CNN
	1    1300 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 2500 3450 2900
Wire Wire Line
	3450 2900 3100 2900
Wire Wire Line
	2800 2900 1950 2900
Wire Wire Line
	1950 2900 1950 3450
$Comp
L Erich:Conn_01x02_Female_Power J1
U 1 1 605D02B6
P 1100 3550
F 0 "J1" H 992 3225 50  0000 C CNN
F 1 "Power" H 992 3316 50  0000 C CNN
F 2 "Connector_JST:JST_PH_S2B-PH-K_1x02_P2.00mm_Horizontal" H 1100 3550 50  0001 C CNN
F 3 "~" H 1100 3550 50  0001 C CNN
	1    1100 3550
	-1   0    0    1   
$EndComp
Wire Wire Line
	6600 4700 6600 5100
Wire Wire Line
	7100 4250 7100 5100
Wire Wire Line
	5400 4850 5400 5100
Connection ~ 5400 4850
Wire Wire Line
	6000 4250 6000 4700
Wire Wire Line
	6000 4700 4650 4700
Wire Wire Line
	4650 4700 4650 3250
Wire Wire Line
	4650 3250 4800 3250
$Comp
L Device:R R3
U 1 1 609CFCE5
P 1950 2500
F 0 "R3" H 2020 2546 50  0000 L CNN
F 1 "10" H 2020 2455 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 1880 2500 50  0001 C CNN
F 3 "~" H 1950 2500 50  0001 C CNN
	1    1950 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 1800 1950 1800
Wire Wire Line
	1950 1950 1950 1800
Connection ~ 1950 1800
Wire Wire Line
	1950 1800 4050 1800
Wire Wire Line
	1950 2650 1950 2900
Connection ~ 1950 2900
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 609D4F4D
P 5800 2850
F 0 "#FLG0101" H 5800 2925 50  0001 C CNN
F 1 "PWR_FLAG" H 5800 3023 50  0000 C CNN
F 2 "" H 5800 2850 50  0001 C CNN
F 3 "~" H 5800 2850 50  0001 C CNN
	1    5800 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 3050 5800 3050
Wire Wire Line
	5800 3050 5800 2850
Connection ~ 5400 3050
Wire Wire Line
	1300 3550 1300 5100
$Comp
L Device:R R4
U 1 1 609D72F0
P 6250 4500
F 0 "R4" H 6320 4546 50  0000 L CNN
F 1 "10k" H 6320 4455 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 6180 4500 50  0001 C CNN
F 3 "~" H 6250 4500 50  0001 C CNN
	1    6250 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 4150 6250 4350
Wire Wire Line
	6250 4650 6250 4850
$EndSCHEMATC
