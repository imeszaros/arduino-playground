#ifndef __TIMER_H
#define __TIMER_H

class Timer {

public:

	Timer(unsigned long interval);

	void setOrigin(unsigned long origin);
	void setOriginToNow();
	void reset(unsigned long interval);
	float progress(bool constrain);
	unsigned long elapsed();
	bool fire();

private:

	unsigned long _origin;
	unsigned long _interval;

	unsigned long _millis();
};

#endif
