#include "tray.h"

Tray::Tray(uint8_t clsdPIPin, uint8_t openPIPin,
		uint8_t motorCtrlEnabledPin, uint8_t motorDirection1Pin, uint8_t motorDirection2Pin):

		_desiredState(Off),
		_controllerState(Off),
		_closingDelay(0),
		_openingDelay(0) {

	this->_clsdPIPin = clsdPIPin;
	this->_openPIPin = openPIPin;

	this->_motorCtrlEnabledPin = motorCtrlEnabledPin;
	this->_motorDirection1Pin = motorDirection1Pin;
	this->_motorDirection2Pin = motorDirection2Pin;

	_clsdPI = new Bounce();
	_openPI = new Bounce();

	_closingTimer = new Timer(_closingDelay);
	_openingTimer = new Timer(_openingDelay);

	_closingTimer->setEnabled(false);
	_openingTimer->setEnabled(false);
}

Tray::~Tray() {
	delete _clsdPI;
	delete _openPI;

	delete _closingTimer;
	delete _openingTimer;
}

void Tray::init() {
	_clsdPI->attach(_clsdPIPin, INPUT);
	_openPI->attach(_openPIPin, INPUT);

	pinMode(_motorCtrlEnabledPin, OUTPUT);
	pinMode(_motorDirection1Pin, OUTPUT);
	pinMode(_motorDirection2Pin, OUTPUT);
}

Tray::State Tray::getCurrentState() {
	if (!_clsdPI->read()) {
		return Closed;
	} else if (!_openPI->read()) {
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

unsigned long Tray::getClosingDelay() {
	return _closingDelay;
}

void Tray::setClosingDelay(unsigned long delay) {
	_closingDelay = delay;
}

unsigned long Tray::getOpeningDelay() {
	return _openingDelay;
}

void Tray::setOpeningDelay(unsigned long delay) {
	_openingDelay = delay;
}

void Tray::update() {
	_clsdPI->update();
	_openPI->update();

	State ctrlState = _determineControllerState();

	if (_controllerState != ctrlState) {
		State previousState = _controllerState;
		_controllerState = ctrlState;

		switch (_controllerState) {
		case Opening:
			_openingTimer->setEnabled(false);
			_closingTimer->setEnabled(false);
			_open();
			break;
		case Closing:
			_openingTimer->setEnabled(false);
			_closingTimer->setEnabled(false);
			_close();
			break;
		case Off:
		default:
			switch (previousState) {
			case Opening:
				_openingTimer->reset(_openingDelay);
				_openingTimer->setEnabled(true);
				break;
			case Closing:
				_closingTimer->reset(_closingDelay);
				_closingTimer->setEnabled(true);
				break;
			default:
				_off();
			}
		}
	}

	if (_openingTimer->fire()) {
		_openingTimer->setEnabled(false);
		_off();
	}

	if (_closingTimer->fire()) {
		_closingTimer->setEnabled(false);
		_off();
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

void Tray::_open() {
	digitalWrite(_motorCtrlEnabledPin, LOW);
	digitalWrite(_motorDirection1Pin, LOW);
	digitalWrite(_motorDirection2Pin, HIGH);
	digitalWrite(_motorCtrlEnabledPin, HIGH);
}

void Tray::_close() {
	digitalWrite(_motorCtrlEnabledPin, LOW);
	digitalWrite(_motorDirection1Pin, HIGH);
	digitalWrite(_motorDirection2Pin, LOW);
	digitalWrite(_motorCtrlEnabledPin, HIGH);
}

void Tray::_off() {
	digitalWrite(_motorCtrlEnabledPin, LOW);
	digitalWrite(_motorDirection1Pin, LOW);
	digitalWrite(_motorDirection2Pin, LOW);
}
