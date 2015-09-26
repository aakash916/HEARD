#include <iostream>
#include <string>

#include <cstdio>
#include <wiringPi.h>
#include <ArduiPi_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <nfc/nfc.h>

#define MAIN_BUTTON_PIN 07
#define MODE_PIN 24
//#define EXIT_BUTTON_PIN 29

// Diaplay
Adafruit_SSD1306 display;

// State flags
bool cardDetected = false;
bool audioPlaying = false;

// RFID data
nfc_device *pnd = NULL;
nfc_context *context = NULL;
const nfc_modulation nmMifare = { NMT_ISO14443A, NBR_106, };
uint8_t cardUID[4] = {0x00};

void showStartScreen(void)
{
	// Starting screen
	display.begin();
	display.setTextSize(3);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.print("HEARD\n");
	display.display();
	display.clearDisplay();
}

void showRecordScreen(void)
{
	display.begin();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.print("Recording...\n");
	display.display();
	display.clearDisplay();
}

void showPlayScreen(void)
{
	display.begin();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.print("Playing...\n");
	display.display();
	display.clearDisplay();
}

bool checkButton(int btn)
{
	if (digitalRead(btn) == LOW)
		delay(10);
		if (digitalRead(btn) == LOW)
			return true;
	return false;
}

static void print_hex(const uint8_t *pbtData, const size_t szBytes)
{
	size_t  szPos;
	for (szPos = 0; szPos < szBytes; szPos++) {
		printf("%02x  ", pbtData[szPos]);
	}
	printf("\n");
}

void getCardUID(nfc_device *pnd, nfc_target *nt, uint8_t uid[])
{
	if (nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, nt) > 0) {
		memcpy(uid, nt->nti.nai.abtUid, 4);
	}
}

char cmdstr[128];
void playAudio(uint8_t uid[4])
{
	sprintf(cmdstr, "aplay /home/pi/Development/heard/bin/%02x%02x%02x%02x.wav", uid[0], uid[1], uid[2], uid[3]);
	system(cmdstr);
}
/*void stopRec(void)
{
	while(digitalRead(MODE_PIN) == HIGH)
	{
	}
	sprintf(cmdstr, "pkill -9 arecord");
	system(cmdstr);

}*/
void recAudio(uint8_t uid[4])
{
	sprintf(cmdstr, "arecord -D plughw:0 --duration=3 -f cd  /home/pi/Development/heard/bin/%02x%02x%02x%02x.wav", uid[0], uid[1], uid[2], uid[3]);
//	sprintf(cmdstr, "ps");
	system(cmdstr);
	//stopRec();
}
void quit() 
{
	display.begin();
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.setTextSize(2);
	display.setCursor(0, 0);
	display.print("HEARD\n");
	display.print("stopped.\n");
	display.display();
	display.close();
	nfc_close(pnd);
	nfc_exit(context);
	exit(0);
}

int main(void)
{
	// GPIO setup
	wiringPiSetup();

	// Button setup
	pinMode(7, INPUT);
	pullUpDnControl(7, PUD_UP);
	//digitalWrite(MODE_PIN, HIGH);
//	pinMode(EXIT_BUTTON_PIN, INPUT);
//	digitalWrite(EXIT_BUTTON_PIN, HIGH);
    pinMode(MODE_PIN, INPUT);
    pullUpDnControl(MODE_PIN, PUD_UP);
	//digitalWrite(MAIN_BUTTON_PIN, HIGH);
    
	// NFC setup
	nfc_target nt;
	nfc_init(&context);
	if (context == NULL) {
		std::cerr << "Unable to init libnfc." << std::endl;
		exit(EXIT_FAILURE);
	}
	pnd = nfc_open(context, NULL);
	if (pnd == NULL) {
		std::cerr << "Unable to open NFC device." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (nfc_initiator_init(pnd) < 0) {
		nfc_perror(pnd, "nfc_initiator_init");
		exit(EXIT_FAILURE);
	}
	printf("NFC reader: %s opened\n", nfc_device_get_name(pnd));

	// Display setup
	if (!display.init(OLED_I2C_RESET, 2)) {
		std::cerr << "Display initialization failure." << std::endl;
		exit(EXIT_FAILURE);
	}
	display.clearDisplay();

	// UI setup
	showStartScreen();
	std::cout << "HEARD started." << std::endl;

	while (1) {
		getCardUID(pnd, &nt, cardUID);	
		print_hex(cardUID, 4);

		// Exit Button
	//	if (checkButton(EXIT_BUTTON_PIN)) {
	//		quit();
	//	}
		if (checkButton(7)) {
			if (digitalRead(MODE_PIN)){
				showPlayScreen();
				playAudio(cardUID);
				showStartScreen();
			}
			else  {

				showRecordScreen();
				recAudio(cardUID);
				showStartScreen();
		
				}
		}
	}

	return 0;
}
