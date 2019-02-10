#include "catris.h"

Catris::Catris(fontDataReader fontDataReader, spriteDataReader spriteDataReader):
		_fontDataReader(fontDataReader), _spriteDataReader(spriteDataReader), _buffer(NULL) {

	_rainbowTimer = new Timer(8000);
	_scrollTimer = new Timer(60);
	_animTimer = new Timer(0);

	_scrollText = new ScrollText(0, 14, 10, fontDataReader, (uint8_t*) font4x5);

	uint8_t delay = 150;
	_happyAnimation = new Animation(12);
	_happyAnimation->setFrame(0, delay, (uint8_t*) catrisHappy1LeftSprite);
	_happyAnimation->setFrame(1, delay, (uint8_t*) catrisHappy2LeftSprite);
	_happyAnimation->setFrame(2, delay, (uint8_t*) catrisHappy3LeftSprite);
	_happyAnimation->setFrame(3, delay, (uint8_t*) catrisHappy1LeftSprite);
	_happyAnimation->setFrame(4, delay, (uint8_t*) catrisHappy2LeftSprite);
	_happyAnimation->setFrame(5, delay * 5, (uint8_t*) catrisHappy3LeftSprite);
	_happyAnimation->setFrame(6, delay, (uint8_t*) catrisHappy3RightSprite);
	_happyAnimation->setFrame(7, delay, (uint8_t*) catrisHappy2RightSprite);
	_happyAnimation->setFrame(8, delay, (uint8_t*) catrisHappy1RightSprite);
	_happyAnimation->setFrame(9, delay, (uint8_t*) catrisHappy3RightSprite);
	_happyAnimation->setFrame(10, delay, (uint8_t*) catrisHappy2RightSprite);
	_happyAnimation->setFrame(11, delay * 5, (uint8_t*) catrisHappy1RightSprite);

	_shockedAnimation = new Animation(11);
	_shockedAnimation->setFrame(0, 1000, (uint8_t*) catrisShocked1Sprite);
	_shockedAnimation->setFrame(1, 50, (uint8_t*) catrisShocked2Sprite);
	_shockedAnimation->setFrame(2, 100, (uint8_t*) catrisShocked3Sprite);
	_shockedAnimation->setFrame(3, 50, (uint8_t*) catrisShocked2Sprite);
	_shockedAnimation->setFrame(4, 2000, (uint8_t*) catrisShocked1Sprite);
	_shockedAnimation->setFrame(5, 50, (uint8_t*) catrisShocked2Sprite);
	_shockedAnimation->setFrame(6, 100, (uint8_t*) catrisShocked3Sprite);
	_shockedAnimation->setFrame(7, 50, (uint8_t*) catrisShocked2Sprite);
	_shockedAnimation->setFrame(8, 50, (uint8_t*) catrisShocked1Sprite);
	_shockedAnimation->setFrame(9, 100, (uint8_t*) catrisShocked3Sprite);
	_shockedAnimation->setFrame(10, 50, (uint8_t*) catrisShocked2Sprite);

	_worriedAnimation = new Animation(11);
	_worriedAnimation->setFrame(0, delay, (uint8_t*) catrisWorried1Sprite);
	_worriedAnimation->setFrame(1, delay, (uint8_t*) catrisWorried2Sprite);
	_worriedAnimation->setFrame(2, delay, (uint8_t*) catrisWorried3Sprite);
	_worriedAnimation->setFrame(3, delay, (uint8_t*) catrisWorried4Sprite);
	_worriedAnimation->setFrame(4, delay, (uint8_t*) catrisWorried3Sprite);
	_worriedAnimation->setFrame(5, delay, (uint8_t*) catrisWorried2Sprite);
	_worriedAnimation->setFrame(6, delay, (uint8_t*) catrisWorried1Sprite);
	_worriedAnimation->setFrame(7, delay, (uint8_t*) catrisWorried4Sprite);
	_worriedAnimation->setFrame(8, delay, (uint8_t*) catrisWorried1Sprite);
	_worriedAnimation->setFrame(9, delay, (uint8_t*) catrisWorried2Sprite);
	_worriedAnimation->setFrame(10, delay, (uint8_t*) catrisWorried3Sprite);

	delay = 100;
	_inLoveAnimation = new Animation(7);
	_inLoveAnimation->setFrame(0, delay, (uint8_t*) catrisInLove1Sprite);
	_inLoveAnimation->setFrame(1, delay, (uint8_t*) catrisInLove2Sprite);
	_inLoveAnimation->setFrame(2, delay, (uint8_t*) catrisInLove3Sprite);
	_inLoveAnimation->setFrame(3, delay, (uint8_t*) catrisInLove4Sprite);
	_inLoveAnimation->setFrame(4, delay, (uint8_t*) catrisInLove3Sprite);
	_inLoveAnimation->setFrame(5, delay, (uint8_t*) catrisInLove2Sprite);
	_inLoveAnimation->setFrame(6, delay, (uint8_t*) catrisInLove1Sprite);

	setAnimation(Anim::Happy);
}

void Catris::setAnimation(Anim animation) {
	switch (animation) {
	case Anim::Happy:
		_loadAnimation(_happyAnimation);
		break;
	case Anim::Shocked:
		_loadAnimation(_shockedAnimation);
		break;
	case Anim::Worried:
		_loadAnimation(_worriedAnimation);
		break;
	case Anim::InLove:
		_loadAnimation(_inLoveAnimation);
		break;
	}
}

void Catris::setText(const char* text) {
	_scrollText->setText(text);
}

void Catris::setFormattedText(const char* format, ...) {
	if (_buffer != NULL) {
		free(_buffer);
	}

	va_list args;

	va_start(args, format);
	int len = vsnprintf(NULL, 0, format, args) + 1;
	va_end(args);

	_buffer = (char*) malloc(len);

	va_start(args, format);
	vsprintf(_buffer, format, args);
	va_end(args);

	setText(_buffer);
}

bool Catris::update() {
	if (_scrollTimer->fire()) {
		return _scrollText->scroll();
	} else {
		return false;
	}
}

void Catris::draw(canvas canvas) {
	_playAnimation(canvas);

	uint8_t color[3];
	hsv2rgb(360 * _rainbowTimer->progress(true), 1, 1, color);

	_scrollText->draw(canvas, color);
}

void Catris::_loadAnimation(Animation* animation) {
	_currentAnimation = animation;
	_animFrame = 0;
	_animTimer->reset(animation->getDuration(0));
}

void Catris::_playAnimation(canvas canvas) {
	if (_animTimer->fire()) {
		_animFrame = _animFrame == _currentAnimation->frameCount - 1 ? 0 : _animFrame + 1;
		_animTimer->reset(_currentAnimation->getDuration(_animFrame));
	}

	for (uint8_t x = 0; x < 10; ++x) {
		for (uint8_t y = 1; y < 14; ++y) {
			canvas(x, y, 0, 0, 0);
		}
	}

	drawSprite(canvas, spritePalette, _spriteDataReader, _currentAnimation->getSprite(_animFrame), 0, 1);
}
