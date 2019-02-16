#include "timer.h"

Timer::Timer(unsigned long interval):
		_origin(_millis()), _interval(interval) {}

unsigned long Timer::getInterval() {
	return _interval;
}

void Timer::setOrigin(unsigned long origin) {
	_origin = origin;
}

void Timer::setOriginToNow() {
	_origin = _millis();
}

void Timer::reset(unsigned long interval) {
	_origin = _millis();
	_interval = interval;
}

float Timer::progress(bool constrain) {
	float progress = (_millis() - _origin) / (double) _interval;

	return constrain ? progress - (long) progress : progress;
}

unsigned long Timer::elapsed() {
	return _millis() - _origin;
}

bool Timer::fire() {
	unsigned long now = _millis();

	if (now > _origin && now - _origin > _interval) {
		_origin = now;
		return true;
	} else {
		return false;
	}
}
