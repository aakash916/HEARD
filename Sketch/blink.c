#include <stdio.h>
#include <wiringPi.h>

int main(void)
{
	wiringPiSetup();

	pinMode(0, OUTPUT);

	while (1) {
		digitalWrite(0, HIGH);
		delay(1000);
		digitalWrite(0, LOW);
		delay(1000);
	}
	return 0;
}
