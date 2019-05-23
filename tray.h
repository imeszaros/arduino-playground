#ifndef __TRAY_H
#define __TRAY_H

#include <Arduino.h>
#include <inttypes.h>
#include <Bounce2.h>
#include "timer.h"

class Tray {

public:

	enum State {
		Open,
		Closed,
		Opening,
		Closing,
		Off
	};

	Tray(uint8_t clsdPIPin, uint8_t openPIPin,
			uint8_t motorCtrlEnabledPin, uint8_t motorDirection1Pin, uint8_t motorDirection2Pin);

	~Tray();

	void init();
	State getCurrentState();
	State getDesiredState();
	void setDesiredState(State desiredState);
	unsigned long getClosingDelay();
	void setClosingDelay(unsigned long delay);
	unsigned long getOpeningDelay();
	void setOpeningDelay(unsigned long delay);
	void update();

private:
	uint8_t _clsdPIPin;
	uint8_t _openPIPin;

	uint8_t _motorCtrlEnabledPin;
	uint8_t _motorDirection1Pin;
	uint8_t _motorDirection2Pin;

	Bounce* _clsdPI;
	Bounce* _openPI;

	State _desiredState;
	State _controllerState;

	Timer* _closingTimer;
	Timer* _openingTimer;

	unsigned long _closingDelay;
	unsigned long _openingDelay;

	State _determineControllerState();

	void _open();
	void _close();
	void _off();
};

#endif
