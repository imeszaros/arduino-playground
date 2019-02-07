#include "main.h"

// display
CRGB leds[NUM_LEDS];
unsigned long lastLedUpdate = millis();

// vibra-motor
Adafruit_DRV2605 vibra;

// tray control
Tray tray(PI_CLSD, PI_OPEN, M_ENABLE, M_CTRL_1, M_CTRL_2);

// audio output
pmf_player audio;

// game engine
Tetris tetris(LEDS_PER_ROW, NUM_LEDS / LEDS_PER_ROW, millis()); // TODO analogRead

// buttons
Bounce resetButton = Bounce();
Bounce musicButton = Bounce();
Bounce soundButton = Bounce();
Bounce rotateButton = Bounce();
Bounce leftButton = Bounce();
Bounce rightButton = Bounce();
Bounce downButton = Bounce();

unsigned long lastButtonRepeat;

// music enabled
byte music = true;

// sound enabled
byte sound = true;

// TODO remove
byte trayState = 0;

void setup() {
	// power-up safety delay
    delay(500);
    //Serial.begin(7680);

    // initialize buttons
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

	// initialize audio output
	audio.enable_output();
	playGameMusic();

	// initialize display
    FastLED.addLeds<LED_TYPE, LED_SDI, LED_SCK, COLOR_ORDER, DATA_RATE_MHZ(20)>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    //FastLED.setMaxPowerInVoltsAndMilliamps(5, 100); // TODO
    FastLED.setBrightness(5); // TODO

    // initialize vibra-motor
    vibra.begin();
    vibra.selectLibrary(1);
    vibra.setMode(DRV2605_MODE_INTTRIG);

    // initialize tray control
    tray.init();

    // initialize tetris
    tetris.reset();
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

	if (resetButton.rose()) {
		tetris.reset();
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (rotateButton.rose()) {
		tetris.rotateClockWise();
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (leftButton.rose()) {
		buttonRepeat(true);
		tetris.moveLeft();
		playButtonPressSound();
		playButtonPressVibra();
	} else if (leftButton.read() && buttonRepeat(false)) {
		tetris.moveLeft();
	}

	if (rightButton.rose()) {
		buttonRepeat(true);
		tetris.moveRight();
		playButtonPressSound();
		playButtonPressVibra();
	} else if (rightButton.read() && buttonRepeat(false)) {
		tetris.moveRight();
	}

	if (downButton.rose()) {
		buttonRepeat(true);
		tetris.moveDown();
		playButtonPressSound();
		playButtonPressVibra();
	} else if (downButton.read() && buttonRepeat(false)) {
		tetris.moveDown();
	}

    unsigned long now = millis();
    if (lastLedUpdate - now > 100) {
    	lastLedUpdate = now;
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

	tetris.update();
	tetris.draw(&setLedColor);
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
	unsigned long now = millis();

	if (reset) {
		lastButtonRepeat = now + REPEAT_DELAY - REPEAT_INTERVAL;
		return false;
	}

	if (now > lastButtonRepeat && now - lastButtonRepeat > REPEAT_INTERVAL) {
		lastButtonRepeat = now;
		return true;
	} else {
		return false;
	}
}
