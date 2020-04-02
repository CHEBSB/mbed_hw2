#include "mbed.h"

Serial pc( USBTX, USBRX );
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
char table[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };
DigitalIn  Switch(SW3);
DigitalOut redLED(LED1), greenLED(LED2);
AnalogOut Aout(DAC0_OUT);
AnalogIn Ain(A0);
const double fs = 400.0;	// 400hz sampling frequency
double freq;		// frequency of input sine wave
int *d;	// array for digits of the nearest integer to freq

int CountPeriod(double *A, int size); // how many samples to repeat
int pwrOfTen(int pwr);	// calculate 10^pwr
int NumOfDigit(int n);	/* calculate number of digits
of the nearest integer to frequency */

int main() {
	double Data[100];	// store data to calculate frequency
	int n;		// number of digits of freq

	for (int i = 0; i < 100; i++) {
		Data[i] = Ain;	// sampling
		wait(1 / fs);
	}
	for (int i = 0; i < 100; i++){
	// output data to Python for plotting
   	 	pc.printf("%1.3lf\r\n", Data[i]);
   	 	wait(0.01);
  	}	
	freq = fs / CountPeriod(Data, 100);

	int f = (int)freq;	// integer part of freq
	if (freq - f >= 0.5) f++; // round off
	n = NumOfDigit(f);
	d = new int[n];
	for (int i = n - 1; i >= 0; i--) {
		d[n - 1 - i] = f / pwrOfTen(i);
		f = f - d[n - 1 - i] * pwrOfTen(i);
	}

	while (1) {
		redLED = Switch;
		greenLED = !redLED;
		if (Switch == 0) {
			for (int t = 0; Switch == 0; t++) {
				if (t == n - 1) {	// the last digit
					// add decimal point
					display = table[d[t]] + 0x80;	
					t -= n;	// restart the cycle
				}
				else
					display = table[d[t]];
				wait(0.3);
			}
		}
		else {
			display = 0x00;
			for (double t = 0; t < (1 / freq); t += (0.04 / freq)) {
				Aout = 0.5 + 0.5 * sin(2 * 3.14159 * freq * t);
				wait(0.04 / freq);
			}	
		}
		
	}
}

int CountPeriod(double *A, int size)
{
	bool flag = 0;
	int i;

	for (i = 1; flag == 0 && i <= size / 2; i++) {	
	/* test every i until size / 2 
	(even if we can get a period > size / 2,
	 we can't assure its correctness ) */
		flag = 1;	// for each i, first set flag = 1
		for (int j = 0; j < size - i; j++) 
			if ((A[j] - A[j + i] > 0.1) || (A[j + i] - A[j] > 0.1)) 
			// if any 2 elements do not match the period
				flag = 0;
	}
	if (i > size / 2)	return 0;	// aperiodic
	return i - 1;
}
int pwrOfTen(int pwr)
{
	int output = 1;
	while (pwr--) output *= 10;
	return output;
}
int NumOfDigit(int n)
{
	int i = 0;
	for (; n >= pwrOfTen(i); i++);
	return i;
}