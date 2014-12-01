/* 
 CUPBOT
 
 Program to draw dick on a plastic cup
 Tested with arduino Mega on Serial2 with other Mega who run modified MakerBlock XY Plotter firmware (modified to run on Serial2).
 Created 27 November 2014 : V.0
 by Flo Gales.

 License Rien à branler.
 License Do What The Fuck You Want.
 */

char* SYMBOLES[36][15]={
{"0",
"G0 X0 Y23.333",
"G1 Z-1",
"G2 X13.333 Y23.333 I6.667 J0",
"G1 X13.333 Y6.667",
"G2 X0 Y6.667 I-6.667 J0",
"G1 X0 Y23.333"},

{"1",
"G0 X0 Y23.333",
"G1 Z-1",
"G1 X6.667 Y30",
"G1 X6.667 Y0"},

{"2",
"G0 X0 Y23.333",
"G1 Z-1",
"G2 X12.327 Y19.811 I6.667 J0",
"G1 X0 Y0",
"G1 X13.333 Y0"},

{"3",
"G0 X0 Y30",
"G1 Z-1",
"G1 X8.333 Y30",
"G2 X13.333 Y25 I0 J-5",
"G1 X13.333 Y21.667",
"G2 X8.333 Y16.667 I-5 J0",
"G1 X3.333 Y16.667",
"G0 Z2",
"G0 X8.333 Y16.667",
"G1 Z-1",
"G2 X13.333 Y11.667 I0 J-5",
"G1 X13.333 Y5",
"G2 X8.333 Y0 I-5 J0",
"G1 X0 Y0"},

{"4",
"G0 X6.667 Y30",
"G1 Z-1",
"G1 X0 Y6.667",
"G1 X13.333 Y6.667",
"G0 Z2",
"G0 X11.667 Y0",
"G1 Z-1",
"G1 X11.667 Y13.333"},

{"5",
"G0 X0 Y0",
"G1 Z-1",
"G1 X8.333 Y0",
"G3 X13.333 Y5 I0 J5",
"G1 X13.333 Y11.667",
"G3 X8.333 Y16.667 I-5 J0",
"G1 X0 Y16.667",
"G1 X0 Y30",
"G1 X13.333 Y30"},

{"6",
"G0 X0 Y10",
"G1 Z-1",
"G2 X13.333 Y10 I6.667 J0",
"G1 X13.333 Y6.667",
"G2 X0 Y6.667 I-6.667 J0",
"G1 X0 Y10",
"G2 X10 Y30 I25 J0"},

{"7",
"G0 X0 Y30",
"G1 Z-1",
"G1 X13.333 Y30",
"G1 X5 Y0",
"G0 Z2",
"G0 X6.667 Y16.667",
"G1 Z-1",
"G1 X13.333 Y16.667"},

{"8",
"G0 X6.667 Y16.667",
"G1 Z-1",
"G3 X6.667 Y30 I0 J6.667",
"G3 X6.667 Y16.667 I0 J-6.667",
"G2 X13.333 Y10 I0 J-6.667",
"G1 X13.333 Y6.667",
"G2 X0 Y6.667 I-6.667 J0",
"G1 X0 Y10",
"G2 X6.667 Y16.667 I6.667 J0"},

{"9",
"G0 X3.333 Y0",
"G1 Z-1",
"G3 X13.333 Y20 I-15 J20",
"G1 X13.333 Y23.333",
"G3 X0 Y23.333 I-6.667 J0",
"G1 X0 Y20",
"G3 X13.333 Y20 I6.667 J0"},

{"A",
"G0 X0 Y0",
"G1 Z-1",
"G1 X10 Y30",
"G1 X20 Y0",
"G0 Z2",
"G0 X17.222 Y8.333",
"G1 Z-1",
"G1 X2.778 Y8.333"},

{"B",
"G0 X0 Y16.667",
"G1 Z-1",
"G1 X8.333 Y16.667",
"G3 X8.333 Y30 I0 J6.667",
"G1 X0 Y30",
"G1 X0 Y0",
"G1 X8.333 Y0",
"G3 X8.333 Y16.667 I0 J8.333"},

{"C",
"G0 X13.333 Y0",
"G1 Z-1",
"G1 X8.333 Y0",
"G2 X0 Y8.333 I0 J8.333",
"G1 X0 Y21.667",
"G2 X8.333 Y30 I8.333 J0",
"G1 X13.333 Y30"},

{"D",
"G0 X0 Y0",
"G1 Z-1",
"G1 X0 Y30",
"G1 X8.333 Y30",
"G2 X16.667 Y21.667 I0 J-8.333",
"G1 X16.667 Y8.333",
"G2 X8.333 Y0 I-8.333 J0",
"G1 X0 Y0"},

{"E",
"G0 X13.333 Y0",
"G1 Z-1",
"G1 X0 Y0",
"G1 X0 Y16.667",
"G1 X10 Y16.667",
"G0 Z2",
"G0 X0 Y16.667",
"G1 Z-1",
"G1 X0 Y30",
"G1 X13.333 Y30"},

{"F",
"G0 X0 Y0",
"G1 Z-1",
"G1 X0 Y16.667",
"G1 X13.333 Y16.667",
"G0 Z2",
"G0 X0 Y16.667",
"G1 Z-1",
"G1 X0 Y30",
"G1 X13.333 Y30"},

{"G",
"G0 X13.333 Y16.667",
"G1 Z-1",
"G1 X16.667 Y16.667",
"G1 X16.667 Y0",
"G1 X8.333 Y0",
"G2 X0 Y8.333 I0 J8.333",
"G1 X0 Y21.667",
"G2 X8.333 Y30 I8.333 J0",
"G1 X16.667 Y30"},

{"H",
"G0 X0 Y0",
"G1 Z-1",
"G1 X0 Y30",
"G0 Z2",
"G0 X0 Y16.667",
"G1 Z-1",
"G1 X16.667 Y16.667",
"G0 Z2",
"G0 X16.667 Y0",
"G1 Z-1",
"G1 X16.667 Y30"},

{"I",
"G0 X1.667 Y0",
"G1 Z-1",
"G1 X1.667 Y30"},

{"J",
"G0 X0 Y0",
"G1 Z-1",
"G1 X1.667 Y0",
"G3 X10 Y8.333 I0 J8.333",
"G1 X10 Y30"},

{"K",
"G0 X0 Y0",
"G1 Z-1",
"G1 X0 Y30",
"G0 Z2",
"G0 X0 Y13.333",
"G1 Z-1",
"G1 X16.667 Y30",
"G0 Z2",
"G0 X3.333 Y16.667",
"G1 Z-1",
"G1 X16.667 Y0"},

{"L",
"G0 X0 Y30",
"G1 Z-1",
"G1 X0 Y0",
"G1 X13.333 Y0"},

{"M",
"G0 X0 Y0",
"G1 Z-1",
"G1 X0 Y30",
"G1 X10 Y13.333",
"G1 X20 Y30",
"G1 X20 Y0"},

{"N",
"G0 X0 Y0",
"G1 Z-1",
"G1 X0 Y30",
"G1 X16.667 Y0",
"G1 X16.667 Y30"},

{"O",
"G0 X0 Y21.667",
"G1 Z-1",
"G2 X16.667 Y21.667 I8.333 J0",
"G1 X16.667 Y8.333",
"G2 X0 Y8.333 I-8.333 J0",
"G1 X0 Y21.667"},

{"P",
"G0 X0 Y0",
"G1 Z-1",
"G1 X0 Y30",
"G1 X8.333 Y30",
"G2 X8.333 Y13.333 I0 J-8.333",
"G1 X0 Y13.333"},

{"Q",
"G0 X0 Y21.667",
"G1 Z-1",
"G2 X16.667 Y21.667 I8.333 J0",
"G1 X16.667 Y8.333",
"G2 X0 Y8.333 I-8.333 J0",
"G1 X0 Y21.667",
"G0 Z2",
"G0 X10 Y6.667",
"G1 Z-1",
"G1 X20 Y0"},

{"R",
"G0 X0 Y0",
"G1 Z-1",
"G1 X0 Y30",
"G1 X8.333 Y30",
"G2 X8.333 Y13.333 I0 J-8.333",
"G1 X0 Y13.333",
"G0 Z2",
"G0 X8.333 Y13.333",
"G1 Z-1",
"G1 X16.667 Y0"},

{"S",
"G0 X0 Y8.333",
"G1 Z-1",
"G3 X8.333 Y16.667 I8.333 J0",
"G2 X15 Y23.333 I0 J6.667"},

{"T",
"G0 X0 Y30",
"G1 Z-1",
"G1 X16.667 Y30",
"G0 Z2",
"G0 X8.333 Y30",
"G1 Z-1",
"G1 X8.333 Y0"},

{"U",
"G0 X0 Y30",
"G1 Z-1",
"G1 X0 Y8.333",
"G3 X16.667 Y8.333 I8.333 J0",
"G1 X16.667 Y30"},

{"V",
"G0 X0 Y30",
"G1 Z-1",
"G1 X10 Y0",
"G1 X20 Y30"},

{"W",
"G0 X0 Y30",
"G1 Z-1",
"G1 X6.667 Y0",
"G1 X13.333 Y20",
"G1 X20 Y0",
"G1 X26.667 Y30"},

{"X",
"G0 X0 Y0",
"G1 Z-1",
"G1 X20 Y30",
"G0 Z2",
"G0 X0 Y30",
"G1 Z-1",
"G1 X20 Y0"},

{"Y",
"G0 X0 Y30",
"G1 Z-1",
"G1 X10 Y16.667",
"G1 X10 Y0",
"G0 Z2",
"G0 X10 Y16.667",
"G1 Z-1",
"G1 X20 Y30"},

{"Z",
"G0 X0 Y30",
"G1 Z-1",
"G1 X20 Y30",
"G1 X0 Y0",
"G1 X20 Y0"}};

