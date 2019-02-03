#include "tray.h"

Tray::Tray(uint8_t clsdPIPin, uint8_t openPIPin,
		uint8_t motorCtrlEnabledPin, uint8_t motorDirection1Pin, uint8_t motorDirection2Pin):

		desiredState(Off),
		controllerState(Off) {

	this->clsdPIPin = clsdPIPin;
	this->openPIPin = openPIPin;

	this->motorCtrlEnabledPin = motorCtrlEnabledPin;
	this->motorDirection1Pin = motorDirection1Pin;
	this->motorDirection2Pin = motorDirection2Pin;

	clsdPI = Bounce();
	openPI = Bounce();
}

void Tray::init() {
	clsdPI.attach(clsdPIPin, INPUT);
	openPI.attach(openPIPin, INPUT);

	pinMode(motorCtrlEnabledPin, OUTPUT);
	pinMode(motorDirection1Pin, OUTPUT);
	pinMode(motorDirection2Pin, OUTPUT);
}

Tray::State Tray::getCurrentState() {
	if (!clsdPI.read()) {
		return Closed;
	} else if (!openPI.read()) {
		return Open;
	} else {
		if (desiredState == Off) {
			return Off;
		} else if (desiredState == Open) {
			return Opening;
		} else if (desiredState == Closed) {
			return Closing;
		}
	}

	return Off;
}

Tray::State Tray::getDesiredState() {
	return desiredState;
}

void Tray::setDesiredState(State desiredState) {
	switch (desiredState) {
	case Open:
	case Opening:
		this->desiredState = Open;
		break;
	case Closed:
	case Closing:
		this->desiredState = Closed;
		break;
	case Off:
	default:
		this->desiredState = Off;
	}
}

void Tray::update() {
	clsdPI.update();
	openPI.update();

	State ctrlState = determineControllerState();

	if (controllerState != ctrlState) {
		controllerState = ctrlState;

		switch (controllerState) {
		case Opening:
			digitalWrite(motorCtrlEnabledPin, LOW);
			digitalWrite(motorDirection1Pin, HIGH);
			digitalWrite(motorDirection2Pin, LOW);
			digitalWrite(motorCtrlEnabledPin, HIGH);
			break;
		case Closing:
			digitalWrite(motorCtrlEnabledPin, LOW);
			digitalWrite(motorDirection1Pin, LOW);
			digitalWrite(motorDirection2Pin, HIGH);
			digitalWrite(motorCtrlEnabledPin, HIGH);
			break;
		case Off:
		default:
			digitalWrite(motorCtrlEnabledPin, LOW);
			digitalWrite(motorDirection1Pin, LOW);
			digitalWrite(motorDirection2Pin, LOW);
		}
	}
}

Tray::State Tray::determineControllerState() {
	if (desiredState == Open) {
		if (getCurrentState() != Open) {
			return Opening;
		}
	} else if (desiredState == Closed) {
		if (getCurrentState() != Closed) {
			return Closing;
		}
	}

	return Off;
}
