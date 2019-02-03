#include "main.h"

extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
Adafruit_DRV2605 vibra;
Tray tray(PI_CLSD, PI_OPEN, M_ENABLE, M_CTRL_1, M_CTRL_2);

pmf_player audio;

Bounce resetButton = Bounce();
Bounce musicButton = Bounce();
Bounce soundButton = Bounce();
Bounce rotateButton = Bounce();
Bounce leftButton = Bounce();
Bounce rightButton = Bounce();
Bounce downButton = Bounce();

byte music = true;
byte sound = true;

unsigned long ts = millis();

//TODO temp
byte trayState = 0;

void setup() {
    delay(3000); // power-up safety delay

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

	//
    FastLED.addLeds<LED_TYPE, LED_SDI, LED_SCK, COLOR_ORDER, DATA_RATE_MHZ(20)>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setMaxPowerInVoltsAndMilliamps(5,100);
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
	//

    vibra.begin();
    vibra.selectLibrary(1);
    vibra.setMode(DRV2605_MODE_INTTRIG);

    tray.init();
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
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (rotateButton.rose()) {
		playButtonPressSound();
		playButtonPressVibra();
		switch (trayState) {
		case 0:
			trayState = 1;
			break;
		case 1:
			trayState = 2;
			break;
		case 2:
			trayState = 0;
			break;
		}
	}

	if (leftButton.rose()) {
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (rightButton.rose()) {
		playButtonPressSound();
		playButtonPressVibra();
	}

	if (downButton.rose()) {
		playButtonPressSound();
		playButtonPressVibra();
	}

	//
    ChangePalettePeriodically();

    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors( startIndex);

    unsigned long x = millis();
    if (ts - x > 100) {
    	ts = x;
    	FastLED.show();
    }

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


//

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;

    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;

    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;

    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};
