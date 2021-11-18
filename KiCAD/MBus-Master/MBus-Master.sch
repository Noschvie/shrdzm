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
L Transistor_BJT:BD136 Q?
U 1 1 618ED24C
P 4150 2500
F 0 "Q?" H 4342 2454 50  0000 L CNN
F 1 "BD136" H 4342 2545 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-126-3_Vertical" H 4350 2425 50  0001 L CIN
F 3 "http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/CD00001225.pdf" H 4150 2500 50  0001 L CNN
	1    4150 2500
	1    0    0    1   
$EndComp
$Comp
L Transistor_BJT:BC337 Q?
U 1 1 618EDC6C
P 2950 3200
F 0 "Q?" H 3141 3246 50  0000 L CNN
F 1 "BC337" H 3141 3155 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 3150 3125 50  0001 L CIN
F 3 "https://diotec.com/tl_files/diotec/files/pdf/datasheets/bc337.pdf" H 2950 3200 50  0001 L CNN
	1    2950 3200
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 618EE32A
P 4250 2000
F 0 "R?" H 4320 2046 50  0000 L CNN
F 1 "220k" H 4320 1955 50  0000 L CNN
F 2 "" V 4180 2000 50  0001 C CNN
F 3 "~" H 4250 2000 50  0001 C CNN
	1    4250 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 618EE7BB
P 3500 2250
F 0 "R?" H 3570 2296 50  0000 L CNN
F 1 "2.7k" H 3570 2205 50  0000 L CNN
F 2 "" V 3430 2250 50  0001 C CNN
F 3 "~" H 3500 2250 50  0001 C CNN
	1    3500 2250
	1    0    0    -1  
$EndComp
$Comp
L Isolator:EL817 U?
U 1 1 618EED22
P 1650 3900
F 0 "U?" H 1650 4225 50  0000 C CNN
F 1 "EL817" H 1650 4134 50  0000 C CNN
F 2 "Package_DIP:DIP-4_W7.62mm" H 1450 3700 50  0001 L CIN
F 3 "http://www.everlight.com/file/ProductFile/EL817.pdf" H 1650 3900 50  0001 L CNN
	1    1650 3900
	1    0    0    -1  
$EndComp
$Comp
L Isolator:EL817 U?
U 1 1 618EF48F
P 1650 3100
F 0 "U?" H 1650 3425 50  0000 C CNN
F 1 "EL817" H 1650 3334 50  0000 C CNN
F 2 "Package_DIP:DIP-4_W7.62mm" H 1450 2900 50  0001 L CIN
F 3 "http://www.everlight.com/file/ProductFile/EL817.pdf" H 1650 3100 50  0001 L CNN
	1    1650 3100
	1    0    0    -1  
$EndComp
$Comp
L power:+36V #PWR?
U 1 1 618F0E09
P 2050 1700
F 0 "#PWR?" H 2050 1550 50  0001 C CNN
F 1 "+36V" H 2065 1873 50  0000 C CNN
F 2 "" H 2050 1700 50  0001 C CNN
F 3 "" H 2050 1700 50  0001 C CNN
	1    2050 1700
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J?
U 1 1 618F17A1
P 5650 1950
F 0 "J?" H 5678 1926 50  0000 L CNN
F 1 "Conn_01x02_Female" H 5678 1835 50  0000 L CNN
F 2 "" H 5650 1950 50  0001 C CNN
F 3 "~" H 5650 1950 50  0001 C CNN
	1    5650 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 1700 5000 1950
Wire Wire Line
	5000 1950 5450 1950
Wire Wire Line
	4250 2150 4250 2200
Wire Wire Line
	2050 1700 4250 1700
Wire Wire Line
	4250 1850 4250 1700
Connection ~ 4250 1700
Wire Wire Line
	4250 1700 5000 1700
Wire Wire Line
	4250 2200 5000 2200
Wire Wire Line
	5000 2200 5000 2050
Wire Wire Line
	5000 2050 5450 2050
Connection ~ 4250 2200
Wire Wire Line
	4250 2200 4250 2300
$EndSCHEMATC
