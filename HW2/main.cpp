#include "mbed.h"
#define fs = 400	// 400 hertz for sampling frequency
#define abs(a) (a >= 0? a: (-1 * a))	// absolute value

BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
unsigned char table[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };
DigitalIn  Switch(SW3);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
AnalogOut Aout(DAC0_OUT);
AnalogIn Ain(A0);

void 7seg(double freq, int NumDig);	// number of digits
int CountFreq(double *A);	// how many samples to repeat
int pwrOfTen(int pwr);	// calculate 10^pwr
int NumOfDigit(double x);	/* calculate number of digit
of the number's nearest integer */

int main() {
	double Data[40];	// store data to calculate frequency
	double freq;		// the desired frequency

	for (int t = 0; t < 0.1; t += 1 / fs) {
		Data[i] = Ain;	// sampling
		wait(1 / fs);
	}
	freq = fs / CountFreq(Data);

	while (1) {
		if (Switch == 1) {
			greenLED = 0;
			redLED = 1;
			7seg(freq);
		}
		else {
			redLED = 0;
			greenLED = 1;
		}
	}
}

void 7seg(double freq)
{
	/* build an array of each digit to display */
	int n = NumOfDigit(freq);
	int d[n];
	for (int i = n - 1; i >= 0; i--) {
		d[n - 1 - i] = (int)freq / pwrOfTen(i);
		freq = freq - d[i] * pwrOfTen(i);
		if (i == 0 && freq >= 0.5)  d[n - 1 - i]++;
	}

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
int CountFreq(double *A)
{
	int flag = 0, i;

	for (i = 1; flag == 0 && i < sizeof(A); i++) {
		flag = 1;
		for (int j = 0; j < sizeof(A) - i; j++) {
			if (abs(A[j] - A[j + i]) > 0.1) flag = 0;
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
	for (i = 0; x >= pwrofTen(i); i++);
	return i;
}