#include "tray.h"

Tray::Tray(uint8_t clsdPIPin, uint8_t openPIPin,
		uint8_t motorCtrlEnabledPin, uint8_t motorDirection1Pin, uint8_t motorDirection2Pin):

		_desiredState(Off),
		_controllerState(Off) {

	this->_clsdPIPin = clsdPIPin;
	this->_openPIPin = openPIPin;

	this->_motorCtrlEnabledPin = motorCtrlEnabledPin;
	this->_motorDirection1Pin = motorDirection1Pin;
	this->_motorDirection2Pin = motorDirection2Pin;

	_clsdPI = Bounce();
	_openPI = Bounce();
}

void Tray::init() {
	_clsdPI.attach(_clsdPIPin, INPUT);
	_openPI.attach(_openPIPin, INPUT);

	pinMode(_motorCtrlEnabledPin, OUTPUT);
	pinMode(_motorDirection1Pin, OUTPUT);
	pinMode(_motorDirection2Pin, OUTPUT);
}

Tray::State Tray::getCurrentState() {
	if (!_clsdPI.read()) {
		return Closed;
	} else if (!_openPI.read()) {
		return Open;
	} else {
		if (_desiredState == Off) {
			return Off;
		} else if (_desiredState == Open) {
			return Opening;
		} else if (_desiredState == Closed) {
			return Closing;
		}
	}

	return Off;
}

Tray::State Tray::getDesiredState() {
	return _desiredState;
}

void Tray::setDesiredState(State desiredState) {
	switch (desiredState) {
	case Open:
	case Opening:
		this->_desiredState = Open;
		break;
	case Closed:
	case Closing:
		this->_desiredState = Closed;
		break;
	case Off:
	default:
		this->_desiredState = Off;
	}
}

void Tray::update() {
	_clsdPI.update();
	_openPI.update();

	State ctrlState = _determineControllerState();

	if (_controllerState != ctrlState) {
		_controllerState = ctrlState;

		switch (_controllerState) {
		case Opening:
			digitalWrite(_motorCtrlEnabledPin, LOW);
			digitalWrite(_motorDirection1Pin, HIGH);
			digitalWrite(_motorDirection2Pin, LOW);
			digitalWrite(_motorCtrlEnabledPin, HIGH);
			break;
		case Closing:
			digitalWrite(_motorCtrlEnabledPin, LOW);
			digitalWrite(_motorDirection1Pin, LOW);
			digitalWrite(_motorDirection2Pin, HIGH);
			digitalWrite(_motorCtrlEnabledPin, HIGH);
			break;
		case Off:
		default:
			digitalWrite(_motorCtrlEnabledPin, LOW);
			digitalWrite(_motorDirection1Pin, LOW);
			digitalWrite(_motorDirection2Pin, LOW);
		}
	}
}

Tray::State Tray::_determineControllerState() {
	if (_desiredState == Open) {
		if (getCurrentState() != Open) {
			return Opening;
		}
	} else if (_desiredState == Closed) {
		if (getCurrentState() != Closed) {
			return Closing;
		}
	}

	return Off;
}
