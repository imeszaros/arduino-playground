#include <EEPROM.h>
#include <Bounce2.h>

#include "pmf_player.h"
#include "pmf_tune.cpp"

#define BTN_RESET 2
#define BTN_MUSIC 3
#define BTN_SOUND 4
#define BTN_ROTATE 5
#define BTN_LEFT 6
#define BTN_RIGHT 7
#define BTN_DOWN 8

#define EE_ADDR_MUSIC 0
#define EE_ADDR_SOUND EE_ADDR_MUSIC + sizeof(bool)

static pmf_player audio;

static Bounce resetButton = Bounce();
static Bounce musicButton = Bounce();
static Bounce soundButton = Bounce();
static Bounce rotateButton = Bounce();
static Bounce leftButton = Bounce();
static Bounce rightButton = Bounce();
static Bounce downButton = Bounce();

static bool music = true;
static bool sound = true;

void setup() {
	resetButton.attach(BTN_RESET, INPUT);
	resetButton.interval(5);

	musicButton.attach(BTN_MUSIC, INPUT);
	musicButton.interval(5);

	soundButton.attach(BTN_MUSIC, INPUT);
	soundButton.interval(5);

	rotateButton.attach(BTN_MUSIC, INPUT);
	rotateButton.interval(5);

	leftButton.attach(BTN_MUSIC, INPUT);
	leftButton.interval(5);

	rightButton.attach(BTN_MUSIC, INPUT);
	rightButton.interval(5);

	downButton.attach(BTN_MUSIC, INPUT);
	downButton.interval(5);

	EEPROM.get(EE_ADDR_MUSIC, music);
	EEPROM.get(EE_ADDR_SOUND, sound);

	audio.enable_output();

	playGameMusic();
}

void loop() {
	resetButton.update();
	musicButton.update();
	soundButton.update();
	rotateButton.update();
	leftButton.update();
	rightButton.update();
	downButton.update();

	if (music) {
		audio.update();
	}

	if (musicButton.rose()) {
		music = !music;
		if (music) {
			playGameMusic();
		} else {
			stopMusic();
		}
		playButtonPressSound();
		EEPROM.put(EE_ADDR_MUSIC, music);
	}

	if (soundButton.rose()) {
		sound = !sound;
		playButtonPressSound();
		EEPROM.put(EE_ADDR_SOUND, sound);
	}
}

void playMusic(const void *track) {
	if (music) {
		audio.start(track);
	}
}

void stopMusic() {
	if (music) {
		audio.stop();
	}
}

void playGameMusic() {
	playMusic(gameMusic);
}

void playButtonPressSound() {
	const uint16_t size = sizeof(buttonSound) / sizeof(buttonSound[0]);
	audio.mixin(buttonSound, size);
}
