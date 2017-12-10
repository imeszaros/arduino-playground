#include <EEPROM.h>
#include <Bounce2.h>

#include "pmf_player.h"
#include "pmf_tune.cpp"

#define BTN_RESET 8
#define BTN_MUSIC 7
#define BTN_SOUND 6
#define BTN_ROTATE 5
#define BTN_LEFT 2
#define BTN_RIGHT 3
#define BTN_DOWN 4

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

static byte music = true;
static byte sound = true;

void setup() {
	resetButton.attach(BTN_RESET, INPUT);
	resetButton.interval(5);

	musicButton.attach(BTN_MUSIC, INPUT);
	musicButton.interval(5);

	soundButton.attach(BTN_SOUND, INPUT);
	soundButton.interval(5);

	rotateButton.attach(BTN_ROTATE, INPUT);
	rotateButton.interval(5);

	leftButton.attach(BTN_LEFT, INPUT);
	leftButton.interval(5);

	rightButton.attach(BTN_RIGHT, INPUT);
	rightButton.interval(5);

	downButton.attach(BTN_DOWN, INPUT);
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

	if (resetButton.rose()) {
		playButtonPressSound();
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
	audio.stop();
}

void playGameMusic() {
	playMusic(gameMusic);
}

void playButtonPressSound() {
	if (sound) {
		const uint16_t size = sizeof(buttonSound) / sizeof(buttonSound[0]);
		audio.mixin(buttonSound, size);
	}
}
