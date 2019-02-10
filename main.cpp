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
Timer rainbowTimer(5000);

// game state
uint8_t state = STATE_CATRIS_LOOP;
bool clearCanvasOnNextLoop = true;

// TODO remove
byte trayState = 0;

// catris
Catris catris(&progMemRead, &progMemRead);

void setup() {
	// power-up safety delay
    delay(500);

    // turn on serial interface in debug mode
    if (DEBUG) {
        Serial.begin(7680);
    }

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

	// initialize audio output
	EEPROM.get(EE_ADDR_MUSIC, music);
	EEPROM.get(EE_ADDR_SOUND, sound);

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

    // initialize catris
    catris.setAnimation(Catris::Anim::Happy);
    catris.setText("    *purr-purr* Hello 2sofix! I am your guide, Catris. Ready to play? Press -> to begin.");

    // initialize tetris
    Entropy.initialize();
    tetris = new Tetris(canvasWidth(), canvasHeight(), Entropy.random(), &tetrisEvent);
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

	if (clearCanvasOnNextLoop) {
		clearCanvasOnNextLoop = false;
		clrscr();
	}

	if (music) {
		audio.update();
	}

	if (soundButton.rose()) {
		sound = !sound;

		playBeepUpSound();
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

		playBeepUpSound();
		playButtonPressVibra();

		EEPROM.put(EE_ADDR_MUSIC, music);
	}

	if (isCatris()) {
		if (rightButton.rose()) {
			buttonRepeat(true);
			showTetris();
			playBeepUpSound();
			playButtonPressVibra();
		}

		bool finished = catris.update();
		if (state == STATE_CATRIS_ONCE && finished) {
			showTetris();
		}

	    if (displayTimer.fire()) {
	    	catris.draw(&setCanvas);
	    	FastLED.show();
	    }
	} else if (isTetris()) {
		if (pauseButton.rose()) {
			if (tetris->isPaused()) {
				playBeepUpSound();
				tetris->setPaused(false);
			} else {
				playBeepDownSound();
				tetris->setPaused(true);
			}

			playButtonPressVibra();
		}

		if (resetButton.rose()) {
			tetris->reset();

			playBeepUpSound();
			playButtonPressVibra();
		}

		if (rotateLeftButton.rose()) {
			if (tetris->rotateCounterClockWise()) {
				playBeepUpSound();
			} else {
				playBeepDownSound();
			}

			playButtonPressVibra();
		}

		if (rotateRightButton.rose()) {
			if (tetris->rotateClockWise()) {
				playBeepUpSound();
			} else {
				playBeepDownSound();
			}

			playButtonPressVibra();
		}

		if (leftButton.rose()) {
			buttonRepeat(true);

			if (tetris->moveLeft()) {
				playBeepUpSound();
			} else {
				playBeepDownSound();
			}

			playButtonPressVibra();
		} else if (leftButton.read() && buttonRepeat(false)) {
			tetris->moveLeft();
		}

		if (rightButton.rose()) {
			buttonRepeat(true);

			if (tetris->moveRight()) {
				playBeepUpSound();
			} else {
				playBeepDownSound();
			}

			playButtonPressVibra();
		} else if (rightButton.read() && buttonRepeat(false)) {
			tetris->moveRight();
		}

		if (downButton.rose()) {
			buttonRepeat(true);

			if (tetris->moveDown()) {
				playBeepUpSound();
			} else {
				playBeepDownSound();
			}

			playButtonPressVibra();
		} else if (downButton.read() && buttonRepeat(false)) {
			tetris->moveDown();
		}

		tetris->update();

	    if (displayTimer.fire()) {
	    	if (tetris->isGameOver()) {
	    		playGameOverVibra();
	    		catris.setAnimation(Catris::Anim::Worried);
	    		catris.setText("    Oh-no! Game over. Press -> to try again.");
				showCatris(true);
	    	} else {
		    	tetris->draw(&setCanvas);

		    	if (tetris->isPaused()) {
					showPauseSign();
				}
	    	}

	    	FastLED.show();
	    }
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
	playMusic(pmfChippy);
}

void playBeepUpSound() {
	if (sound) {
		const uint16_t size = sizeof(pcmBeepUp) / sizeof(pcmBeepUp[0]);
		audio.mixin(pcmBeepUp, size);
	}
}

void playBeepDownSound() {
	if (sound) {
		const uint16_t size = sizeof(pcmBeepDown) / sizeof(pcmBeepDown[0]);
		audio.mixin(pcmBeepDown, size);
	}
}

void playButtonPressVibra() {
	vibra.setWaveform(0, 1);
	vibra.setWaveform(1, 0);
	vibra.go();
}

void playGameOverVibra() {
	vibra.setWaveform(0, 1);
	vibra.setWaveform(1, 0);
	vibra.go();
}

void playSingleRowClearVibra() {
	vibra.setWaveform(0, 7);
	vibra.setWaveform(1, 0);
	vibra.go();
}

void playDoubleRowClearVibra() {
	vibra.setWaveform(0, 7);
	vibra.setWaveform(1, 7);
	vibra.setWaveform(2, 0);
	vibra.go();
}

void playTripleRowClearVibra() {
	vibra.setWaveform(0, 7);
	vibra.setWaveform(1, 7);
	vibra.setWaveform(2, 7);
	vibra.setWaveform(3, 0);
	vibra.go();
}

void playTetrisVibra() {
	vibra.setWaveform(0, 52);
	vibra.setWaveform(1, 52);
	vibra.setWaveform(2, 52);
	vibra.setWaveform(3, 52);
	vibra.setWaveform(4, 0);
	vibra.go();
}

void playLevelUpVibra() {
	vibra.setWaveform(0, 89);
	vibra.setWaveform(1, 89);
	vibra.setWaveform(2, 89);
	vibra.setWaveform(3, 0);
	vibra.go();
}

void setCanvas(int8_t x, int8_t y, uint8_t r, uint8_t g, uint8_t b) {
	if (x < 0 || x >= canvasWidth() || y < 0 || y >= canvasHeight()) {
		return;
	}

	leds[y * canvasWidth() + (y % 2 == 0 ? x : canvasWidth() - x - 1)].setRGB(r, g, b);
}

void tetrisEvent(TetrisEvent event, uint8_t data) {
	switch (event) {
	case TetrisEvent::LevelUp:
		playLevelUpVibra();
		catris.setAnimation(Catris::Anim::Happy);
		catris.setFormattedText("    You've reached level %d with %d points. Keep up!", data, tetris->getScores());
		showCatris(false);
		break;
	case TetrisEvent::RowsCompleted:
		// TODO check scores here
		switch (data) {
		case 1:
			playSingleRowClearVibra();
			if (tetris->getRowsCompleted() == 1) {
				catris.setAnimation(Catris::Anim::Happy);
				catris.setText("    Your first clear.. Stunning!");
				showCatris(false);
			}
			break;
		case 2:
			playDoubleRowClearVibra();
			break;
		case 3:
			playTripleRowClearVibra();
			catris.setAnimation(Catris::Anim::Happy);
			catris.setText("    Three at once! You're amazing!");
			showCatris(false);
			break;
		case 4:
			playTetrisVibra();
			catris.setAnimation(Catris::Anim::Shocked);
			catris.setText("    TETRIS! Your're the master!");
			showCatris(false);
			break;
		default:
			;
		}
		break;
	default:
		;
	}
}

bool buttonRepeat(bool reset) {
	if (reset) {
		buttonTimer.setOrigin(millis() + REPEAT_DELAY - REPEAT_INTERVAL);
		return false;
	}

	return buttonTimer.fire();
}

void showPauseSign() {
	uint8_t hue = rainbowTimer.progress(true) * 255;

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

bool isTetris() {
	return state == STATE_TETRIS;
}

void showTetris() {
	if (tetris->isGameOver()) {
		tetris->reset();
	}

	state = STATE_TETRIS;
}

bool isCatris() {
	return state == STATE_CATRIS_LOOP || state == STATE_CATRIS_ONCE;
}

void showCatris(bool loop) {
	clearCanvasOnNextLoop = true;
	state = loop ? STATE_CATRIS_LOOP : STATE_CATRIS_ONCE;
}

uint8_t progMemRead(uint8_t* addr) {
	return pgm_read_byte(addr);
}

unsigned long Timer::_millis() {
	return millis();
}
