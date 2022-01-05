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
L Diode:BAT46 D2
U 1 1 6177BBC3
P 6500 2350
F 0 "D2" H 6500 2567 50  0000 C CNN
F 1 "BAT46W" H 6500 2476 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123" H 6500 2175 50  0001 C CNN
F 3 "" H 6500 2350 50  0001 C CNN
	1    6500 2350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 6177D771
P 4100 2500
F 0 "C1" H 4215 2546 50  0000 L CNN
F 1 "1uF" H 4215 2455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 4138 2350 50  0001 C CNN
F 3 "~" H 4100 2500 50  0001 C CNN
	1    4100 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 6177DCDB
P 4800 2500
F 0 "R4" H 4870 2546 50  0000 L CNN
F 1 "13K" H 4870 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 4730 2500 50  0001 C CNN
F 3 "~" H 4800 2500 50  0001 C CNN
	1    4800 2500
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC817 T3
U 1 1 6177E445
P 4700 3450
F 0 "T3" H 4891 3496 50  0000 L CNN
F 1 "BC817" H 4891 3405 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4900 3375 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/BC818-D.pdf" H 4700 3450 50  0001 L CNN
	1    4700 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 61780400
P 3900 3650
F 0 "R1" H 3970 3696 50  0000 L CNN
F 1 "13K" H 3970 3605 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 3830 3650 50  0001 C CNN
F 3 "~" H 3900 3650 50  0001 C CNN
	1    3900 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2350 3900 2900
Connection ~ 4100 2350
Wire Wire Line
	4100 2350 3900 2350
Wire Wire Line
	4800 2350 4100 2350
Wire Wire Line
	3900 3300 3900 3450
Wire Wire Line
	4500 3450 3900 3450
Connection ~ 3900 3450
Wire Wire Line
	3900 3450 3900 3500
$Comp
L power:GND #PWR02
U 1 1 617817CE
P 3900 3800
F 0 "#PWR02" H 3900 3550 50  0001 C CNN
F 1 "GND" H 3905 3627 50  0000 C CNN
F 2 "" H 3900 3800 50  0001 C CNN
F 3 "" H 3900 3800 50  0001 C CNN
	1    3900 3800
	1    0    0    -1  
$EndComp
$Comp
L Connector:RJ12 J1
U 1 1 61783447
P 7850 4050
F 0 "J1" H 7520 4054 50  0000 R CNN
F 1 "RJ12" H 7520 4145 50  0000 R CNN
F 2 "ErichCollection:RJ12_Amphenol_54601_Reduced" V 7850 4075 50  0001 C CNN
F 3 "~" V 7850 4075 50  0001 C CNN
	1    7850 4050
	-1   0    0    1   
$EndComp
Wire Wire Line
	7450 3850 7300 3850
Wire Wire Line
	7300 3850 7300 2350
Wire Wire Line
	7300 2350 6650 2350
$Comp
L power:PWR_FLAG #FLG01
U 1 1 6178500F
P 7300 2350
F 0 "#FLG01" H 7300 2425 50  0001 C CNN
F 1 "PWR_FLAG" H 7300 2523 50  0000 C CNN
F 2 "" H 7300 2350 50  0001 C CNN
F 3 "~" H 7300 2350 50  0001 C CNN
	1    7300 2350
	1    0    0    -1  
$EndComp
Connection ~ 7300 2350
$Comp
L power:GND #PWR08
U 1 1 61785706
P 6950 4500
F 0 "#PWR08" H 6950 4250 50  0001 C CNN
F 1 "GND" H 6955 4327 50  0000 C CNN
F 2 "" H 6950 4500 50  0001 C CNN
F 3 "" H 6950 4500 50  0001 C CNN
	1    6950 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR09
U 1 1 61786001
P 7450 4450
F 0 "#PWR09" H 7450 4250 50  0001 C CNN
F 1 "GNDPWR" H 7454 4296 50  0000 C CNN
F 2 "" H 7450 4400 50  0001 C CNN
F 3 "" H 7450 4400 50  0001 C CNN
	1    7450 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 4450 7450 4350
Connection ~ 7450 4350
Wire Wire Line
	7450 4250 6750 4250
Wire Wire Line
	6750 4250 6750 3150
Wire Wire Line
	6750 3150 6350 3150
$Comp
L Erich:74LVC1G17DBVR IC1
U 1 1 61787CD4
P 6050 5150
F 0 "IC1" H 6300 4400 50  0000 L CNN
F 1 "74LVC1G17DBVR" H 6150 4250 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 5500 5900 50  0001 C CNN
F 3 "https://www.ti.com/lit/ds/symlink/sn74lvc1g17.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1635086720658&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Fsn74lvc1g17" H 5500 5900 50  0001 C CNN
	1    6050 5150
	-1   0    0    -1  
$EndComp
Text GLabel 6050 4750 1    50   Input ~ 0
PWR
Wire Wire Line
	6050 4750 6050 4850
$Comp
L Transistor_BJT:BC807 T2
U 1 1 61789D3E
P 3950 5150
F 0 "T2" H 4141 5104 50  0000 L CNN
F 1 "BC807-40" H 4141 5195 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4150 5075 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/BC808-D.pdf" H 3950 5150 50  0001 L CNN
	1    3950 5150
	-1   0    0    1   
$EndComp
Wire Wire Line
	6050 4850 4900 4850
Wire Wire Line
	3850 4850 3850 4950
$Comp
L Device:C C2
U 1 1 6178BC1C
P 4900 5000
F 0 "C2" H 5015 5046 50  0000 L CNN
F 1 "1uF" H 5015 4955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 4938 4850 50  0001 C CNN
F 3 "~" H 4900 5000 50  0001 C CNN
	1    4900 5000
	1    0    0    -1  
$EndComp
Connection ~ 4900 4850
Wire Wire Line
	4900 4850 3850 4850
$Comp
L power:GND #PWR04
U 1 1 6178CF42
P 4900 5150
F 0 "#PWR04" H 4900 4900 50  0001 C CNN
F 1 "GND" H 4905 4977 50  0000 C CNN
F 2 "" H 4900 5150 50  0001 C CNN
F 3 "" H 4900 5150 50  0001 C CNN
	1    4900 5150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 6178D2EF
P 5100 5700
F 0 "R3" V 4893 5700 50  0000 C CNN
F 1 "13K" V 4984 5700 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 5030 5700 50  0001 C CNN
F 3 "~" H 5100 5700 50  0001 C CNN
	1    5100 5700
	0    1    1    0   
$EndComp
Wire Wire Line
	5750 5700 5250 5700
Wire Wire Line
	4950 5700 4350 5700
Wire Wire Line
	4350 5700 4350 5150
Wire Wire Line
	4350 5150 4150 5150
$Comp
L power:GND #PWR07
U 1 1 6178EA7C
P 6050 6150
F 0 "#PWR07" H 6050 5900 50  0001 C CNN
F 1 "GND" H 6055 5977 50  0000 C CNN
F 2 "" H 6050 6150 50  0001 C CNN
F 3 "" H 6050 6150 50  0001 C CNN
	1    6050 6150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 6178EF7D
P 3850 6000
F 0 "R2" H 3780 5954 50  0000 R CNN
F 1 "13K" H 3780 6045 50  0000 R CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 3780 6000 50  0001 C CNN
F 3 "~" H 3850 6000 50  0001 C CNN
	1    3850 6000
	-1   0    0    1   
$EndComp
Wire Wire Line
	3850 5350 3850 5450
$Comp
L sfh309-4_19:SFH309-4_19 T1
U 1 1 61793D38
P 3800 3100
F 0 "T1" H 3944 3146 50  0000 L CNN
F 1 "SFH309FA-4" H 3944 3055 50  0000 L CNN
F 2 "LED_THT:LED_D3.0mm" H 3800 3100 50  0001 C CNN
F 3 "" H 3800 3100 50  0001 C CNN
	1    3800 3100
	1    0    0    -1  
$EndComp
$Comp
L cn304_26:CN304_26 D1
U 1 1 61794D73
P 3850 5600
F 0 "D1" H 3953 5621 50  0000 L CNN
F 1 "SFH487-2" H 3953 5530 50  0000 L CNN
F 2 "LED_THT:LED_D3.0mm" H 3850 5600 50  0001 C CNN
F 3 "" H 3850 5600 50  0001 C CNN
	1    3850 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 5800 3850 5850
Wire Wire Line
	6950 4350 6950 4500
Wire Wire Line
	6950 4350 7450 4350
$Comp
L power:GND #PWR01
U 1 1 6179881F
P 3850 6150
F 0 "#PWR01" H 3850 5900 50  0001 C CNN
F 1 "GND" H 3855 5977 50  0000 C CNN
F 2 "" H 3850 6150 50  0001 C CNN
F 3 "" H 3850 6150 50  0001 C CNN
	1    3850 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 5350 6050 4850
Connection ~ 6050 4850
Wire Wire Line
	7450 4150 6550 4150
Wire Wire Line
	6550 4150 6550 5600
Wire Wire Line
	6550 5600 6350 5600
Wire Wire Line
	6050 6050 6050 6150
NoConn ~ 7450 3950
NoConn ~ 7450 4050
$Comp
L power:GND #PWR0101
U 1 1 6179DAB8
P 4100 2650
F 0 "#PWR0101" H 4100 2400 50  0001 C CNN
F 1 "GND" H 4105 2477 50  0000 C CNN
F 2 "" H 4100 2650 50  0001 C CNN
F 3 "" H 4100 2650 50  0001 C CNN
	1    4100 2650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 6179FE55
P 4800 3800
F 0 "#PWR0102" H 4800 3550 50  0001 C CNN
F 1 "GND" H 4805 3627 50  0000 C CNN
F 2 "" H 4800 3800 50  0001 C CNN
F 3 "" H 4800 3800 50  0001 C CNN
	1    4800 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 3650 4800 3700
$Comp
L power:PWR_FLAG #FLG03
U 1 1 617A1282
P 4550 3700
F 0 "#FLG03" H 4550 3775 50  0001 C CNN
F 1 "PWR_FLAG" H 4550 3650 50  0000 C CNN
F 2 "" H 4550 3700 50  0001 C CNN
F 3 "~" H 4550 3700 50  0001 C CNN
	1    4550 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 3700 4800 3700
Connection ~ 4800 3700
Wire Wire Line
	4800 3700 4800 3800
Connection ~ 4800 3050
Wire Wire Line
	4800 3050 4800 3250
Wire Wire Line
	4800 2650 4800 3050
Connection ~ 4800 2350
$Comp
L power:PWR_FLAG #FLG02
U 1 1 6179C024
P 6050 2350
F 0 "#FLG02" H 6050 2425 50  0001 C CNN
F 1 "PWR_FLAG" H 6050 2523 50  0000 C CNN
F 2 "" H 6050 2350 50  0001 C CNN
F 3 "~" H 6050 2350 50  0001 C CNN
	1    6050 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 2350 6050 2350
Connection ~ 6250 2350
Wire Wire Line
	6250 2350 6250 2500
Text GLabel 6250 2500 3    50   Input ~ 0
PWR
Wire Wire Line
	6050 3500 6050 3800
Wire Wire Line
	6050 2350 6050 2800
Wire Wire Line
	5750 3050 4800 3050
$Comp
L power:GND #PWR05
U 1 1 61782498
P 5300 2650
F 0 "#PWR05" H 5300 2400 50  0001 C CNN
F 1 "GND" H 5305 2477 50  0000 C CNN
F 2 "" H 5300 2650 50  0001 C CNN
F 3 "" H 5300 2650 50  0001 C CNN
	1    5300 2650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 61782010
P 6050 3800
F 0 "#PWR06" H 6050 3550 50  0001 C CNN
F 1 "GND" H 6055 3627 50  0000 C CNN
F 2 "" H 6050 3800 50  0001 C CNN
F 3 "" H 6050 3800 50  0001 C CNN
	1    6050 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 2350 5300 2350
Connection ~ 6050 2350
Wire Wire Line
	5300 2350 4800 2350
Connection ~ 5300 2350
Wire Wire Line
	6350 2350 6250 2350
$Comp
L Device:C C3
U 1 1 6177D25E
P 5300 2500
F 0 "C3" H 5415 2546 50  0000 L CNN
F 1 "1uF" H 5415 2455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 5338 2350 50  0001 C CNN
F 3 "~" H 5300 2500 50  0001 C CNN
	1    5300 2500
	1    0    0    -1  
$EndComp
$Comp
L Erich:74LVC1G17DBVR IC2
U 1 1 6177C6B9
P 6050 2600
F 0 "IC2" H 6300 2250 50  0000 L CNN
F 1 "74LVC1G17DBVR" H 6100 2350 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 5500 3350 50  0001 C CNN
F 3 "https://www.ti.com/lit/ds/symlink/sn74lvc1g17.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1635086720658&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Fsn74lvc1g17" H 5500 3350 50  0001 C CNN
	1    6050 2600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
