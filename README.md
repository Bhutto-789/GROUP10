# GROUP10
EMBEDDED PROJECT:GRID FREQUENCY MEASUREMENT

PROBLEM STATEMENT:
Designing a real-time grid frequency measurement system using the Tiva C Series microcontroller. This system should be capable of accurately measuring the frequency of an AC power grid of 50 Hz with minimal delay and high accuracy. 

SOLUTION:
1)	By using a LV25P sensor and zero-crossing detection circuit to convert the AC grid signal to a pulse train, which marks each cycle’s start and allows safe measurement. 
2)	By configuring the Tiva C microcontroller’s timer to measure the interval between zero-crossing pulses and calculate frequency as the reciprocal of this period for real-time accuracy. 
3)	Indicating the frequency data on LED, with an alert system to signal any deviations from the expected range, helping to monitor grid stability


![GRID FREQUENCY MEASUREMENT docx - Word 07-11-2024 22_13_43](https://github.com/user-attachments/assets/e0e25a4c-7312-42fb-abd3-60f8dd885ee1)
BLOCK DIAGRAM:

![GRID FREQUENCY MEASUREMENT docx - Word 07-11-2024 23_01_43](https://github.com/user-attachments/assets/b86338cb-74ed-4b49-baa1-eace0b720d8f)
FLOWCHART:
