#include "mbed.h"
#define fs  400	// 400 hertz for sampling frequency
#define abs(a) (a >= 0? a: (-1 * a))	// absolute value

BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
unsigned char table[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };
InterruptIn  Switch(SW3);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
AnalogOut Aout(DAC0_OUT);
AnalogIn Ain(A0);

void sevenSeg();	// number of digits
int CountFreq(double *A);	// how many samples to repeat
int pwrOfTen(int pwr);	// calculate 10^pwr
int NumOfDigit(double x);	/* calculate number of digit
of the number's nearest integer */
int *d;
int main() {
	double Data[40];	// store data to calculate frequency
	double freq;		// the desired frequency

	for (int t = 0; t < 0.1; t += 1 / fs) {
		Data[t] = Ain;	// sampling
		wait(1 / fs);
	}
	freq = fs / CountFreq(Data);

	/* build an array of each digit to display */
	int n = NumOfDigit(freq), f = freq;	/* define f 
	//so that we can modify f without concerning freq*/
	d = new int[n];
	for (int i = n - 1; i >= 0; i--) {
		d[n - 1 - i] = (int)f / pwrOfTen(i);
		f = f - d[i] * pwrOfTen(i);
		if (i == 0 && f >= 0.5)  d[n - 1 - i]++;
	}
	Switch.rise(&sevenSeg);
	while (1) {
		redLED = 0;
		greenLED = 1;
	}
}

void sevenSeg()
{
	greenLED = 0;
	redLED = 1;
	for (int t = 0; ; t++) {
		if (t == sizeof(d) - 1) {	// the last digit
			display = table[d[t]] + 0x80;	// add decimal point
			t -= sizeof(d);	// prepare for restart the cycle
		}
		else
			display = table[d[t]];
		wait(0.3);
	}
}
int CountFreq(double *A)
{
	bool flag = false; 
	unsigned int i;

	for (i = 1; flag == false && i < sizeof(A); i++) {
		flag = true;
		for (unsigned int j = 0; j < sizeof(A) - i; j++) {
			if (abs(A[j] - A[j + i]) > 0.1) flag = false;
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