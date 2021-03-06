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
L Connector:Conn_01x02_Male J1
U 1 1 6099AC49
P 2400 3000
F 0 "J1" H 2508 3181 50  0000 C CNN
F 1 "Conn_01x02_Male" H 2250 3100 50  0000 C CNN
F 2 "" H 2400 3000 50  0001 C CNN
F 3 "~" H 2400 3000 50  0001 C CNN
	1    2400 3000
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC547 Q1
U 1 1 6099B46A
P 3800 3000
F 0 "Q1" H 3991 3046 50  0000 L CNN
F 1 "BC547" H 3991 2955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 4000 2925 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/BC550-D.pdf" H 3800 3000 50  0001 L CNN
	1    3800 3000
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC547 Q2
U 1 1 6099BB46
P 5300 3000
F 0 "Q2" H 5491 3046 50  0000 L CNN
F 1 "BC547" H 5491 2955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 5500 2925 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/BC550-D.pdf" H 5300 3000 50  0001 L CNN
	1    5300 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT1
U 1 1 6099C3BF
P 1300 3050
F 0 "BT1" H 1418 3146 50  0000 L CNN
F 1 "9V" H 1418 3055 50  0000 L CNN
F 2 "" V 1300 3110 50  0001 C CNN
F 3 "~" V 1300 3110 50  0001 C CNN
	1    1300 3050
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 6099CBD9
P 3350 2550
F 0 "C1" V 3098 2550 50  0000 C CNN
F 1 "100n" V 3189 2550 50  0000 C CNN
F 2 "" H 3388 2400 50  0001 C CNN
F 3 "~" H 3350 2550 50  0001 C CNN
	1    3350 2550
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 6099D592
P 3000 3450
F 0 "R1" H 3070 3496 50  0000 L CNN
F 1 "1M" H 3070 3405 50  0000 L CNN
F 2 "" V 2930 3450 50  0001 C CNN
F 3 "~" H 3000 3450 50  0001 C CNN
	1    3000 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 6099E393
P 3900 1950
F 0 "R2" H 3970 1996 50  0000 L CNN
F 1 "330k" H 3970 1905 50  0000 L CNN
F 2 "" V 3830 1950 50  0001 C CNN
F 3 "~" H 3900 1950 50  0001 C CNN
	1    3900 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 6099E8D1
P 5400 1950
F 0 "R4" H 5470 1996 50  0000 L CNN
F 1 "4.7k" H 5470 1905 50  0000 L CNN
F 2 "" V 5330 1950 50  0001 C CNN
F 3 "~" H 5400 1950 50  0001 C CNN
	1    5400 1950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 6099EF7B
P 4850 2550
F 0 "R3" V 4643 2550 50  0000 C CNN
F 1 "1M" V 4734 2550 50  0000 C CNN
F 2 "" V 4780 2550 50  0001 C CNN
F 3 "~" H 4850 2550 50  0001 C CNN
	1    4850 2550
	0    1    1    0   
$EndComp
$Comp
L Device:LED D1
U 1 1 6099FE3A
P 6250 2800
F 0 "D1" V 6289 2682 50  0000 R CNN
F 1 "LED" V 6198 2682 50  0000 R CNN
F 2 "" H 6250 2800 50  0001 C CNN
F 3 "~" H 6250 2800 50  0001 C CNN
	1    6250 2800
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D2
U 1 1 609A0CEB
P 6250 3100
F 0 "D2" V 6289 2982 50  0000 R CNN
F 1 "LED" V 6198 2982 50  0000 R CNN
F 2 "" H 6250 3100 50  0001 C CNN
F 3 "~" H 6250 3100 50  0001 C CNN
	1    6250 3100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1300 2850 1300 1550
Wire Wire Line
	1300 1550 3900 1550
Wire Wire Line
	5400 1550 5400 1800
Wire Wire Line
	5400 2100 5400 2550
Wire Wire Line
	5000 2550 5400 2550
Connection ~ 5400 2550
Wire Wire Line
	5400 2550 5400 2800
Wire Wire Line
	5400 2550 6250 2550
Wire Wire Line
	6250 2550 6250 2650
Wire Wire Line
	3600 3000 3000 3000
Wire Wire Line
	3000 3000 3000 3300
Wire Wire Line
	3000 3000 3000 2550
Wire Wire Line
	3000 2550 3200 2550
Connection ~ 3000 3000
Wire Wire Line
	3500 2550 4700 2550
Wire Wire Line
	3900 2100 3900 2250
Wire Wire Line
	3900 1800 3900 1550
Connection ~ 3900 1550
Wire Wire Line
	3900 1550 5400 1550
Wire Wire Line
	1300 3150 1300 3750
Wire Wire Line
	1300 3750 2600 3750
Wire Wire Line
	6250 3750 6250 3250
Wire Wire Line
	5400 3200 5400 3750
Connection ~ 5400 3750
Wire Wire Line
	5400 3750 6250 3750
Wire Wire Line
	3900 3200 3900 3750
Connection ~ 3900 3750
Wire Wire Line
	3900 3750 5400 3750
Wire Wire Line
	3000 3600 3000 3750
Connection ~ 3000 3750
Wire Wire Line
	3000 3750 3900 3750
Wire Wire Line
	2600 3100 2600 3750
Connection ~ 2600 3750
Wire Wire Line
	2600 3750 3000 3750
Wire Wire Line
	2600 3000 2600 2250
Wire Wire Line
	2600 2250 3900 2250
Connection ~ 3900 2250
Wire Wire Line
	3900 2250 3900 2800
Wire Wire Line
	3900 2250 4550 2250
Wire Wire Line
	4550 2250 4550 3000
Wire Wire Line
	4550 3000 5100 3000
$EndSCHEMATC
