#include "mbed.h"
#define abs(a) (a >= 0? a: (-1 * a))	// absolute value

BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
unsigned char table[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };
DigitalIn  Switch(SW3);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
AnalogOut Aout(DAC0_OUT);
AnalogIn Ain(A0);
const double fs = 400;	// 400 hertz sampling frequency
double freq;		// frequency of input sine wave
int *d; // array for digits of the frequency

void sevenSeg();	// number of digits
int CountPeriod(double *A);	// how many samples to repeat
int pwrOfTen(int pwr);	// calculate 10^pwr
int NumOfDigit(double x);	/* calculate number of digits
of the nearest integer to frequency*/
void showWave(); // output the waveform to picoscope

int main() {
	double Data[40];	// store data to calculate frequency
	int n;		// number of digits of freq
	double f; // a copy of freq for modification

	for (int t = 0; t < 40; t += 1) {
		Data[t] = Ain;	// sampling
		wait(1 / fs);
	}
	if (CountPeriod(Data) == 0) {
		redLED = 1;	// forever lighting up redLED
		while(1) {}
	}
	freq = fs / CountPeriod(Data);

	/* build an array of each digit to display */
	n = NumOfDigit(freq);
	f = freq;	
	d = new int[n];
	for (int i = n - 1; i >= 0; i--) {
		d[n - 1 - i] = (int)f / pwrOfTen(i);
		f = f - d[n - 1 - i] * pwrOfTen(i);
		if (i == 0 && f >= 0.5)  d[n - 1 - i]++;
	}

	while (1) {
		if (Switch == 1) {
			greenLED = 0;
			redLED = 1;
			sevenSeg();
		}
		else {
			redLED = 0;
			greenLED = 1;
			showWave();
		}
	}
}

void sevenSeg()
{
	int n = sizeof(d);
	for (int t = 0; Switch == 1; t++) {
		if (t == n - 1) {	// the last digit
			display = table[d[t]] + 0x80;	// add decimal point
			t -= n;	// prepare for restart the cycle
		}
		else
			display = table[d[t]];
		wait(0.3);
	}
}
int CountPeriod(double *A)
{
	bool flag = 0;
	unsigned int i;

	for (i = 1; flag == 0 && i < sizeof(A); i++) {
		flag = 1;
		for (unsigned int j = 0; j < sizeof(A) - i; j++) {
			if (abs(A[j] - A[j + i]) > 0.2) flag = 0;
		}
	}

	if (i == sizeof(A))	return 0;	// unperiodic
	return i - 1;
}
int pwrOfTen(int pwr)
{
	int output = 1;
	while (pwr--) output *= 10;
	return output;
}
int NumOfDigit(double x)
{
	int i;
	for (i = 0; x >= pwrOfTen(i); i++);
	return i;
}
void showWave()
{
	for (double t = 0; t < 1 / freq; t += 0.1 / freq) {
		Aout = 1.0 + 1.0 * sin(2 * 3.14159 * freq * t);
		wait(0.1 / freq);
	}
}