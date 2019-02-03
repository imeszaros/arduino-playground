#include <Arduino.h>
#include <inttypes.h>
#include <Bounce2.h>

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

	void init();
	State getCurrentState();
	State getDesiredState();
	void setDesiredState(State desiredState);
	void update();

private:
	uint8_t clsdPIPin;
	uint8_t openPIPin;

	uint8_t motorCtrlEnabledPin;
	uint8_t motorDirection1Pin;
	uint8_t motorDirection2Pin;

	Bounce clsdPI;
	Bounce openPI;

	State desiredState;
	State controllerState;

	State determineControllerState();
};
