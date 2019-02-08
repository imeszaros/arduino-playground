#include "main.h"

// display
CRGB leds[NUM_LEDS];
Timer displayTimer(LED_FPS);

// vibra-motor
Adafruit_DRV2605 vibra;

// tray control
Tray tray(PI_CLSD, PI_OPEN, M_ENABLE, M_CTRL_1, M_CTRL_2);

// audio output
pmf_player audio;

// game engine
Tetris* tetris;

// buttons
Bounce pauseButton = Bounce();
Bounce resetButton = Bounce();
Bounce musicButton = Bounce();
Bounce soundButton = Bounce();
Bounce rotateLeftButton = Bounce();
Bounce rotateRightButton = Bounce();
Bounce leftButton = Bounce();
Bounce rightButton = Bounce();
Bounce downButton = Bounce();
Timer buttonTimer(REPEAT_INTERVAL);

// music enabled
byte music = true;

// sound enabled
byte sound = true;

// timers for effects
Timer pauseSignTimer(5000);

// TODO remove
byte trayState = 0;

void setup() {
	// power-up safety delay
    delay(500);
    // TODO remove Serial.begin(7680);

    // initialize buttons
    pauseButton.attach(BTN_PAUSE, INPUT);
    pauseButton.interval(5);

	resetButton.attach(BTN_RESET, INPUT);
	resetButton.interval(5);

	musicButton.attach(BTN_MUSIC, INPUT);
	musicButton.interval(5);

	soundButton.attach(BTN_SOUND, INPUT);
	soundButton.interval(5);

	rotateLeftButton.attach(BTN_ROTATE_LEFT, INPUT);
	rotateLeftButton.interval(5);

	rotateRightButton.attach(BTN_ROTATE_RIGHT, INPUT);
	rotateRightButton.interval(5);

	leftButton.attach(BTN_LEFT, INPUT);
	leftButton.interval(5);

	rightButton.attach(BTN_RIGHT, INPUT);
	rightButton.interval(5);

	downButton.attach(BTN_DOWN, INPUT);
	downButton.interval(5);

	EEPROM.get(EE_ADDR_MUSIC, music);
	EEPROM.get(EE_ADDR_SOUND, sound);

	// initialize audio output
	audio.enable_output();
	playGameMusic();

	// initialize display
    FastLED.addLeds<LED_TYPE, LED_SDI, LED_SCK, COLOR_ORDER, DATA_RATE_MHZ(20)>(leds, NUM_LEDS).setCorrection(UncorrectedColor);
    FastLED.setBrightness(BRIGHTNESS);

    // initialize vibra-motor
    vibra.begin();
    vibra.selectLibrary(1);
    vibra.setMode(DRV2605_MODE_INTTRIG);

    // initialize tray control
    tray.init();

    // initialize tetris
    Entropy.initialize();
    tetris = new Tetris(LEDS_PER_ROW, NUM_LEDS / LEDS_PER_ROW, Entropy.random());
    tetris->reset();
}

void loop() {
	pauseButton.update();
	resetButton.update();
	musicButton.update();
	soundButton.update();
	rotateLeftButton.update();
	rotateRightButton.update();
	leftButton.update();
	rightButton.update();
	downButton.update();

	if (music) {
		audio.update();
	}

	if (pauseButton.rose()) {
		tetris->setPaused(!tetris->isPaused());
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (resetButton.rose()) {
		tetris->reset();
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (soundButton.rose()) {
		sound = !sound;
		playButtonPressSound();
		playButtonPressVibra();
		EEPROM.put(EE_ADDR_SOUND, sound);
	}

	if (musicButton.rose()) {
		music = !music;
		if (music) {
			playGameMusic();
		} else {
			stopMusic();
		}
		playButtonPressSound();
		playButtonPressVibra();
		EEPROM.put(EE_ADDR_MUSIC, music);
	}

	if (rotateLeftButton.rose()) {
		tetris->rotateCounterClockWise();
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (rotateRightButton.rose()) {
		tetris->rotateClockWise();
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (leftButton.rose()) {
		buttonRepeat(true);
		tetris->moveLeft();
		playButtonPressSound();
		playButtonPressVibra();
	} else if (leftButton.read() && buttonRepeat(false)) {
		tetris->moveLeft();
	}

	if (rightButton.rose()) {
		buttonRepeat(true);
		tetris->moveRight();
		playButtonPressSound();
		playButtonPressVibra();
	} else if (rightButton.read() && buttonRepeat(false)) {
		tetris->moveRight();
	}

	if (downButton.rose()) {
		buttonRepeat(true);
		tetris->moveDown();
		playButtonPressSound();
		playButtonPressVibra();
	} else if (downButton.read() && buttonRepeat(false)) {
		tetris->moveDown();
	}

	tetris->update();

    if (displayTimer.fire()) {
    	tetris->draw(&setLedColor);

    	if (tetris->isPaused()) {
    		showPauseSign();
    	} else if (tetris->isGameOver()) {
    		// TODO
    	}

    	FastLED.show();
    }

    // TODO remove
	switch (trayState) {
	case 0:
		tray.setDesiredState(Tray::State::Off);
		break;
	case 1:
		tray.setDesiredState(Tray::State::Open);
		break;
	case 2:
		tray.setDesiredState(Tray::State::Closed);
		break;
	}

	tray.update();
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

void playButtonPressVibra() {
	vibra.setWaveform(0, 1);
	vibra.setWaveform(1, 0);
	vibra.go();
}

void setLedColor(int8_t x, int8_t y, uint8_t r, uint8_t g, uint8_t b) {
	if (x < 0 || x >= LEDS_PER_ROW || y < 0 || y >= (NUM_LEDS / LEDS_PER_ROW)) {
		return;
	}

	leds[y * LEDS_PER_ROW + (y % 2 == 0 ? x : LEDS_PER_ROW - x - 1)].setRGB(r, g, b);
}

bool buttonRepeat(bool reset) {
	if (reset) {
		buttonTimer.setOrigin(millis() + REPEAT_DELAY - REPEAT_INTERVAL);
		return false;
	}

	return buttonTimer.fire();
}

void showPauseSign() {
	uint8_t hue = pauseSignTimer.progress(true) * 255;

	for (uint8_t i = 61; i < 69; ++i) {
		leds[i] = CRGB::Black;
	}

	for (uint8_t i = 70; i < 130; i+=10) {
		leds[i + 1] = CRGB::Black;
		leds[i + 2].setHSV(hue, 255, 255);
		leds[i + 3].setHSV(hue, 255, 255);
		leds[i + 4] = CRGB::Black;
		leds[i + 5] = CRGB::Black;
		leds[i + 6].setHSV(hue, 255, 255);
		leds[i + 7].setHSV(hue, 255, 255);
		leds[i + 8] = CRGB::Black;
	}

	for (uint8_t i = 131; i < 139; ++i) {
		leds[i] = CRGB::Black;
	}
}

unsigned long Timer::_millis() {
	return millis();
}
