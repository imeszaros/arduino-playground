#ifndef __CATRIS_H
#define __CATRIS_H

#include "timer.h"
#include "graphics.h"
#include "font_data.h"
#include "sprite_data.h"

class Catris {

public:

	enum Anim {

		Happy,
		Shocked,
		Worried,
		InLove,
		LowBattery,
		HighScore
	};

	Catris(fontDataReader fontDataReader, spriteDataReader spriteDataReader);

	~Catris();

	Anim getAnimation();
	void setAnimation(Anim animation);
	void setText(const char* text);
	void setFormattedText(const char* format, ...);
	bool update();
	void draw(canvas canvas);

private:

	fontDataReader _fontDataReader;
	spriteDataReader _spriteDataReader;

	Timer* _rainbowTimer;
	Timer* _scrollTimer;
	Timer* _animTimer;

	ScrollText* _scrollText;
	char* _buffer;

	Anim _currentAnim;
	Animation* _currentAnimation;

	Animation* _happyAnimation;
	Animation* _shockedAnimation;
	Animation* _worriedAnimation;
	Animation* _inLoveAnimation;
	Animation* _lowBatteryAnimation;
	Animation* _highScoreAnimation;

	uint8_t _animFrame;

	void _loadAnimation(Animation* animation);
	void _playAnimation(canvas canvas);
};

#endif
