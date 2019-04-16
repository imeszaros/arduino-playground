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

// catris
Catris catris(
#ifdef FONTS_IN_PROGMEM
	&progMemRead
#else
	&directMemRead
#endif
,
#ifdef SPRITES_IN_PROGMEM
	&progMemRead
#else
	&directMemRead
#endif
);

// game engine
Tetris* tetris;

// low battery signal
Bounce lowBattery = Bounce();

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
Bounce highScoreButton = Bounce();

// timers
Timer buttonTimer(REPEAT_INTERVAL);
Timer rainbowTimer(5000);
Timer surpriseConfigTimer(0);
Timer highScoreClearTimer(0);
Timer batteryCheckTimer(MILLIS_BATTERY_CHECK_INTERVAL);

// persistent state variables
byte music = 0;
byte sound = true;
byte surprise = true;
uint32_t highScore = 0;

// volatile state variables
uint8_t state = STATE_CATRIS_LOOP;
bool clearCanvasOnNextLoop = true;
bool surpriseMentioned = false;
bool lowBatteryDetected = false;
uint8_t lowBatteryCounter = 0;

void setup() {
	// power-up safety delay
    delay(500);

    // turn on serial interface in debug mode
    if (DEBUG) {
        Serial.begin(7680);
    }

    // initialize random generator
    Entropy.initialize();
    srand(Entropy.random());

    // setup low battery detection
    lowBattery.attach(LBO, INPUT_PULLUP);
    lowBattery.interval(5);
    pinMode(LED_LB, OUTPUT);
    digitalWrite(LED_LB, LOW);

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

	highScoreButton.attach(BTN_HIGH_SCORE, INPUT);
	highScoreButton.interval(5);

	// initialize audio output
	EEPROM.get(EE_ADDR_MUSIC, music);
	EEPROM.get(EE_ADDR_SOUND, sound);

	audio.enable_output();
	playMusic();

	// initialize display
    FastLED.addLeds<LED_TYPE, LED_SDI, LED_SCK, COLOR_ORDER, DATA_RATE_MHZ(20)>(leds, NUM_LEDS).setCorrection(UncorrectedColor);
    FastLED.setBrightness(BRIGHTNESS);

    // initialize vibra-motor
    vibra.begin();
    vibra.selectLibrary(1);
    vibra.setMode(DRV2605_MODE_INTTRIG);

    // initialize tray control
    tray.init();
    tray.setDesiredState(tray.getCurrentState());
    EEPROM.get(EE_ADDR_SURPRISE, surprise);

    // load high score
    EEPROM.get(EE_ADDR_HIGH_SCORE, highScore);

    // initialize tetris
    tetris = new Tetris(canvasWidth(), canvasHeight(), &tetrisEvent);
    resetTetris();

    // initialize catris
    catris.setAnimation(Catris::Anim::Happy);
    catris.setFormattedText(randomText(5,
    		"    *purr-purr* Hello, 2sofix! %s",
			"    Howdy, 2sofix! %s",
			"    Feeling good, 2sofix? %s",
			"    Welcome back, 2sofix! %s",
			"    *meow-meow* Hi there, 2sofix! %s"),

    		"I am your guide, Catris. Ready to play? Press -> to begin.");
}

void loop() {
	lowBattery.update();

	pauseButton.update();
	resetButton.update();
	musicButton.update();
	soundButton.update();
	rotateLeftButton.update();
	rotateRightButton.update();
	leftButton.update();
	rightButton.update();
	downButton.update();
	highScoreButton.update();

	if (clearCanvasOnNextLoop) {
		clearCanvasOnNextLoop = false;
		clrscr();
	}

	if (music) {
		audio.update();
	}

	if (soundButton.rose()) {
		sound = !sound;

		EEPROM.put(EE_ADDR_SOUND, sound);

		playBeepUpSound();
		playVibra(buttonPressVibra);
	}

	if (musicButton.rose()) {
		music = music == 4 ? 0 : music + 1;

		EEPROM.put(EE_ADDR_MUSIC, music);

		playMusic();

		playBeepUpSound();
		playVibra(buttonPressVibra);
	}

	if (resetButton.rose()) {
		surpriseConfigTimer.setOriginToNow();
	} else if (resetButton.fell()) {
		unsigned long elapsed = surpriseConfigTimer.elapsed();

		if (elapsed >= MILLIS_SURPRISE_STATE && elapsed < MILLIS_SURPRISE_TOGGLE) {
			catris.setAnimation(Catris::Anim::Happy);
			catris.setFormattedText("    Surprise state currently: %s", surprise ? "YES" : "NO");
			showCatris(true);
		} else if (elapsed >= MILLIS_SURPRISE_TOGGLE && elapsed < MILLIS_TRAY_TOGGLE) {
			surprise = !surprise;
			EEPROM.put(EE_ADDR_SURPRISE, surprise);

			catris.setAnimation(Catris::Anim::Happy);
			catris.setFormattedText("    Surprise changed to: %s", surprise ? "YES" : "NO");
			showCatris(true);
		} else if (elapsed >= MILLIS_TRAY_TOGGLE) {
			tray.setDesiredState(tray.getDesiredState() == Tray::Closed ? Tray::Open : Tray::Closed);
		}
	}

	if (highScore > 0) {
		if (highScoreButton.rose()) {
			highScoreClearTimer.setOriginToNow();

			playBeepUpSound();
			playVibra(buttonPressVibra);

			catris.setAnimation(Catris::Anim::HighScore);
			catris.setFormattedText("    The current high score is %" PRIu32 " points. Go and beat it!", highScore);
			showCatris(false);
		} else if (highScoreButton.fell()) {
			if (highScoreClearTimer.elapsed() >= MILLIS_CLEAR_HIGH_SCORE) {
				highScore = 0;
				EEPROM.put(EE_ADDR_HIGH_SCORE, highScore);

				playBeepUpSound();
				playVibra(buttonPressVibra);

				catris.setAnimation(Catris::Anim::Happy);
				catris.setText("    As you wish. High score has been cleared.");
				showCatris(false);
			}
		}
	}

	// low battery detection
	if (lowBattery.read()) {
		lowBatteryDetected = false;
		lowBatteryCounter = 0;
	    digitalWrite(LED_LB, LOW);
	} else {
		if (batteryCheckTimer.fire()) {
			if (lowBatteryCounter >= LOW_BAT_DETECTION_LIMIT) {
				lowBatteryCounter = 0;
				lowBatteryDetected = true;
			    digitalWrite(LED_LB, HIGH);

				if (!isCatris() || catris.getAnimation() != Catris::Anim::LowBattery) {
					catris.setAnimation(Catris::Anim::LowBattery);
					catris.setText("    The battery is low! Please connect a charger! Press -> to continue.");
					showCatris(true);
				}
			} else {
				lowBatteryCounter++;
			}
		}
	}

	if (isCatris()) {
		if (rightButton.rose()) {
			buttonRepeat(true);

			playBeepUpSound();
			playVibra(buttonPressVibra);

			showTetris();
		} else if (catris.getAnimation() == Catris::Anim::LowBattery && !lowBatteryDetected) {
			playBeepUpSound();
			playVibra(buttonPressVibra);

			showTetris();
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

			playVibra(buttonPressVibra);
		}

		if (resetButton.rose()) {
			resetTetris();

			playBeepUpSound();
			playVibra(buttonPressVibra);
		}

		if (rotateLeftButton.rose()) {
			if (tetris->rotateCounterClockWise()) {
				playBeepUpSound();
			} else {
				playBeepDownSound();
			}

			playVibra(buttonPressVibra);
		}

		if (rotateRightButton.rose()) {
			if (tetris->rotateClockWise()) {
				playBeepUpSound();
			} else {
				playBeepDownSound();
			}

			playVibra(buttonPressVibra);
		}

		if (leftButton.rose()) {
			buttonRepeat(true);

			if (tetris->moveLeft()) {
				playBeepUpSound();
			} else {
				playBeepDownSound();
			}

			playVibra(buttonPressVibra);
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

			playVibra(buttonPressVibra);
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

			playVibra(buttonPressVibra);
		} else if (downButton.read() && buttonRepeat(false)) {
			tetris->moveDown();
		}

		tetris->update();

	    if (displayTimer.fire()) {
	    	if (tetris->isGameOver()) {
	    		playVibra(gameOverVibra);

	    		uint32_t scores = tetris->getScores();

	    		if (scores > highScore) {
	    			playSuccessSound();

	    			highScore = scores;
	    			EEPROM.put(EE_ADDR_HIGH_SCORE, highScore);

		    		catris.setAnimation(Catris::Anim::HighScore);
		    		catris.setFormattedText(randomText(5,
		    				"    Congratulations! New high score! You were at level %" PRIu8 " with %" PRIu32 " points. %s",
							"    Amazing! You've just beaten the high score! You were at level %" PRIu8 " with %" PRIu32 " points. %s",
							"    That's it! New high score! You were at level %" PRIu8 " with %" PRIu32 " points. %s",
							"    New high score! Nicely done. You were at level %" PRIu8 " with %" PRIu32 " points. %s",
							"    Cool, new high score. You were at level %" PRIu8 " with %" PRIu32 " points. %s"),

		    				tetris->getLevel(),
							scores,
		    				"Press -> to play again.");
	    		} else {
		    		playBeepDownSound();

		    		catris.setAnimation(Catris::Anim::Worried);
		    		catris.setFormattedText(randomText(5,
		    				"    Oh-no! Game over. You were at level %" PRIu8 " with %" PRIu32 " points. %s",
							"    Damn.. Game over. You were at level %" PRIu8 " with %" PRIu32 " points. %s",
							"    Isn't so easy, right? You were at level %" PRIu8 " with %" PRIu32 " points. %s",
							"    Better luck next time, maybe! You were at level %" PRIu8 " with %" PRIu32 " points. %s",
							"    Don't be sad, it's just a game.. You were at level %" PRIu8 " with %" PRIu32 " points. %s"),

		    				tetris->getLevel(),
							scores,
		    				"Press -> to try again.");
	    		}

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

	tray.update();
}

void playMusic() {
	switch (music) {
	case 0:
		audio.stop();
		break;
	case 1:
		audio.start(pmfTetris);
		break;
	case 2:
		audio.start(pmfLoveYa);
		break;
	case 3:
		audio.start(pmfLoveInMyDreams);
		break;
	case 4:
		audio.start(pmfChippy);
		break;
	}
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

void playSuccessSound() {
	if (sound) {
		const uint16_t size = sizeof(pcmSuccess) / sizeof(pcmSuccess[0]);
		audio.mixin(pcmSuccess, size);
	}
}

void playVibra(const uint8_t* pattern) {
	uint8_t effects = pgm_read_byte(pattern);
	uint8_t i = 0;

	for (; i < effects; ++i) {
		vibra.setWaveform(i, pgm_read_byte(pattern + i + 1));
	}

	vibra.setWaveform(i, 0);
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
		playSuccessSound();
		playVibra(levelUpVibra);

		catris.setAnimation(Catris::Anim::Happy);
		catris.setFormattedText(randomText(5,
				"    You've reached level %" PRIu8 " with %" PRIu32 " points. Keep up!",
				"    It's level %" PRIu8 " already! You have %" PRIu32 " points. You rock!",
				"    Level %" PRIu8 " is here! Precious %" PRIu32 " points are now yours.",
				"    There's no stopping. Let's see level %" PRIu8 ". %" PRIu32 " points earned so far.",
				"    What a march! Here comes level %" PRIu8 "! Mighty %" PRIu32 " points for you."), data, tetris->getScores());

		showCatris(false);
		break;
	case TetrisEvent::RowsCompleted:
		playSuccessSound();

		if (surprise) {
			uint32_t scores = tetris->getScores();

			if (scores >= SCORES_SURPRISE_REVEAL) {
				surprise = false;
				EEPROM.put(EE_ADDR_SURPRISE, surprise);
				tray.setDesiredState(Tray::Open);

				playVibra(surpriseVibra);

				catris.setAnimation(Catris::Anim::InLove);
				catris.setText("    Oh my god 2sofix! Maci has a question for you: Will you marry him?");
				showCatris(true);

				return;
			}

			if (!surpriseMentioned && tetris->getScores() >= SCORES_SURPRISE_TEASER) {
				surpriseMentioned = true;

				playVibra(tetrisVibra);

				catris.setAnimation(Catris::Anim::Shocked);
				catris.setFormattedText("    %" PRIu32 " points already! There is a surprise waiting for you at %d points. Press -> to go for it!", scores, SCORES_SURPRISE_REVEAL);
				showCatris(true);

				return;
			}
		}

		switch (data) {
		case 1:
			playVibra(singleRowClearVibra);

			if (tetris->getRowsCompleted() == 1) {
				catris.setAnimation(Catris::Anim::Happy);
				catris.setText(randomText(5,
						"    Your first clear.. Stunning!",
						"    So adorable! The first complete line!",
						"    I can feel the power in you!",
						"    That's how professionals begin!"
						"    Tetrominoes are all afraid of you now!"));

				showCatris(false);
			}
			break;
		case 2:
			playVibra(doubleRowClearVibra);
			break;
		case 3:
			playVibra(tripleRowClearVibra);

			catris.setAnimation(Catris::Anim::Happy);
			catris.setText(randomText(5,
					"    Three at once! You're amazing!",
					"    Three lines by one move.. delicious!",
					"    Three at once? Marvellous!",
					"    Playing fancy? 3 by 1.",
					"    I'm starting to take you seriously. 3 by 1."));

			showCatris(false);
			break;
		case 4:
			playVibra(tetrisVibra);

			catris.setAnimation(Catris::Anim::Shocked);
			catris.setText(randomText(5,
					"    TETRIS! Your're the master!",
					"    TETRIS! All could learn from you!",
					"    TETRIS! Are you on steroids?",
					"    TETRIS! Tetrominoes, obey 2sofix!",
					"    TETRIS! It's just crazy!"));

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

void resetTetris() {
	tetris->reset();
	surpriseMentioned = false;
}

bool isTetris() {
	return state == STATE_TETRIS;
}

void showTetris() {
	if (tetris->isGameOver()) {
		resetTetris();
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

uint8_t directMemRead(uint8_t* addr) {
	return *addr;
}

const char* randomText(uint8_t count, ...) {
	int r = rand() % count;
	const char* ret = NULL;

	va_list args;
	va_start(args, count);

	for (uint8_t i = 0; i <= r; ++i) {
		ret = va_arg(args, const char*);
	}

	va_end(args);

	return ret;
}

unsigned long Timer::_millis() {
	return millis();
}
