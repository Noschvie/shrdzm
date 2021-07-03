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
L RF_Module:ESP-12F U?
U 1 1 60593876
P 3150 2550
F 0 "U?" H 3150 3531 50  0000 C CNN
F 1 "ESP-12F" H 3150 3440 50  0000 C CNN
F 2 "RF_Module:ESP-12E" H 3150 2550 50  0001 C CNN
F 3 "http://wiki.ai-thinker.com/_media/esp8266/esp8266_series_modules_user_manual_v1.1.pdf" H 2800 2650 50  0001 C CNN
	1    3150 2550
	1    0    0    -1  
$EndComp
$Comp
L RF_Module:ESP-12F U?
U 1 1 60593F62
P 3150 4650
F 0 "U?" H 3150 5631 50  0000 C CNN
F 1 "ESP-12F" H 3150 5540 50  0000 C CNN
F 2 "RF_Module:ESP-12E" H 3150 4650 50  0001 C CNN
F 3 "http://wiki.ai-thinker.com/_media/esp8266/esp8266_series_modules_user_manual_v1.1.pdf" H 2800 4750 50  0001 C CNN
	1    3150 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 5050 4000 5050
Wire Wire Line
	4000 5050 4000 3500
Wire Wire Line
	4000 3500 2000 3500
Wire Wire Line
	2000 3500 2000 1950
Wire Wire Line
	2000 1950 2550 1950
$Comp
L power:+3.3V #PWR?
U 1 1 605967AD
P 1500 1700
F 0 "#PWR?" H 1500 1550 50  0001 C CNN
F 1 "+3.3V" H 1515 1873 50  0000 C CNN
F 2 "" H 1500 1700 50  0001 C CNN
F 3 "" H 1500 1700 50  0001 C CNN
	1    1500 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 1750 1500 1750
Wire Wire Line
	1500 1750 1500 1700
Wire Wire Line
	2550 2150 1500 2150
Wire Wire Line
	1500 2150 1500 1750
Connection ~ 1500 1750
Wire Wire Line
	2550 4250 1500 4250
Wire Wire Line
	1500 4250 1500 3850
Connection ~ 1500 2150
Wire Wire Line
	3150 3850 1500 3850
Connection ~ 1500 3850
Wire Wire Line
	1500 3850 1500 2150
$Comp
L power:GND #PWR?
U 1 1 605975D5
P 3150 5700
F 0 "#PWR?" H 3150 5450 50  0001 C CNN
F 1 "GND" H 3155 5527 50  0000 C CNN
F 2 "" H 3150 5700 50  0001 C CNN
F 3 "" H 3150 5700 50  0001 C CNN
	1    3150 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 3250 4250 3250
Wire Wire Line
	4250 3250 4250 4950
Wire Wire Line
	4250 5350 3150 5350
Wire Wire Line
	3150 5350 3150 5600
Connection ~ 3150 5350
Wire Wire Line
	3750 4950 4250 4950
Connection ~ 4250 4950
Wire Wire Line
	4250 4950 4250 5350
Wire Wire Line
	3750 2850 4250 2850
Wire Wire Line
	4250 2850 4250 3250
Connection ~ 4250 3250
Wire Wire Line
	3750 4850 4500 4850
Wire Wire Line
	4500 4850 4500 2050
Wire Wire Line
	4500 2050 3750 2050
Wire Wire Line
	3750 4650 4400 4650
Wire Wire Line
	4400 4650 4400 2250
Wire Wire Line
	4400 2250 3750 2250
$Comp
L Switch:SW_Push SW?
U 1 1 60599FD0
P 3950 5600
F 0 "SW?" H 3950 5885 50  0000 C CNN
F 1 "SW_Push" H 3950 5794 50  0000 C CNN
F 2 "" H 3950 5800 50  0001 C CNN
F 3 "~" H 3950 5800 50  0001 C CNN
	1    3950 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 4750 4400 4750
Wire Wire Line
	4400 4750 4400 5600
Wire Wire Line
	4400 5600 4150 5600
Wire Wire Line
	3750 5600 3150 5600
Connection ~ 3150 5600
Wire Wire Line
	3150 5600 3150 5700
$EndSCHEMATC
