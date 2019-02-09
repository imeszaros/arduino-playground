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
		Worried
	};

	Catris(fontDataReader fontDataReader, spriteDataReader spriteDataReader);

	void setAnimation(Anim animation);
	void setText(const char* text);
	bool update();
	void draw(canvas canvas);

private:

	fontDataReader _fontDataReader;
	spriteDataReader _spriteDataReader;

	Timer* _rainbowTimer;
	Timer* _scrollTimer;
	Timer* _animTimer;

	ScrollText* _scrollText;

	Animation* _currentAnimation;

	Animation* _happyAnimation;
	Animation* _shockedAnimation;
	Animation* _worriedAnimation;

	uint8_t _animFrame;

	void _loadAnimation(Animation* animation);
	void _playAnimation(canvas canvas);
};

#endif
