#include <Arduino.h>
#include "GearedMotor.h"

GearedMotor::GearedMotor(uint8_t pin_EN, uint8_t pin1, uint8_t pin2)
{
  _enablePin = pin_EN;
  _pin_1A = pin1;
  _pin_2A = pin2;
  _stepsToTarget = 0;
  _backlashSteps = 300;
  _backlashFlag = 1;
  _direction = DIRECTION_CW;
  _lastDirection = DIRECTION_CW;
  _targetPosition = 0;
  _stepInterval = 1000;
  _currentPosition = 0;
  _speed = 255; // default: run at max speed
  pinMode(pin_EN, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
}

void GearedMotor::move_CW()
{
    digitalWrite(_pin_1A, 1);
    digitalWrite(_pin_2A, 0);
	if (_speed < 255)
	  analogWrite(_enablePin, _speed);
	else
	  digitalWrite(_enablePin, 1);
}

void GearedMotor::move_CCW()
{
    digitalWrite(_pin_1A, 0);
    digitalWrite(_pin_2A, 1);
	if (_speed < 255)
	  analogWrite(_enablePin, _speed);
	else
	  digitalWrite(_enablePin, 1);

}

void GearedMotor::setBacklashFlag(boolean value)
{
	_backlashFlag = value;
}

void GearedMotor::setBacklashSteps(long value)
{
	_backlashSteps = value;
}

boolean GearedMotor::backlashComp()
// note: if pwm is used, set speed here to maxSpeed; want to run backlash
// compensation at maximum speed.
{
	long stepCounter = _backlashSteps;
	unsigned long time;
	if (!_backlashFlag)
	return false;
	if (_direction == DIRECTION_CW)
	{
		move_CW();
		while(stepCounter>0)
		{
			time = micros();
			if ((time - _lastStepTime) >= _stepInterval)
			{
				stepCounter -= 1;
				_lastStepTime = time;
			}
		}
		return true;
	}
	else
	{
		move_CCW();
		while(stepCounter>0)
		{
			time = micros();
			if ((time - _lastStepTime) >= _stepInterval)
			{
				stepCounter -= 1;
				_lastStepTime = time;
			}
		}
		return true;
	}
}

void GearedMotor::disableOutputs(){
	digitalWrite(_enablePin, 0);
}

void GearedMotor::enableOutputs() {
	// this won't start the motor
	digitalWrite(_pin_1A, 0);
	digitalWrite(_pin_2A, 0);
	if (_speed < 255)
		analogWrite(_enablePin, _speed);
	else
		digitalWrite(_enablePin, 1);
	// this one last to keep the motor from starting.
}

long GearedMotor::stepsToGo(){
	_stepsToTarget = _targetPosition - _currentPosition;
	if (_stepsToTarget > 0)
	_direction = DIRECTION_CW;
	else
	_direction = DIRECTION_CCW;
	return _stepsToTarget;
}

long GearedMotor::currentPosition()
{
	return _currentPosition;
}

long GearedMotor::targetPosition()
{
	return _targetPosition;
}


void GearedMotor::setTargetPosition(long value) {
	_targetPosition = value;
}

void GearedMotor::setCurrentPosition(long value) {
	_currentPosition = value;
}

void GearedMotor::setSpeed(uint8_t value) {
	_speed = value;
}

void GearedMotor::moveTo(long value)
{
    if (_targetPosition != value)
    {
	_targetPosition = value;
    }
}

boolean GearedMotor::run()
/*
* Check the need for a move
* Check the direction for this move
* enable the digital outputs
*/
{
  unsigned long time = micros();
  if (stepsToGo() != 0)
 // if (_stepsToTarget != 0)  // more movement needed
  {

    if((time - _lastStepTime) >= _stepInterval)
	{   // time to make a move
      if (_direction != _lastDirection)  // direction changed, add backlash compensation
      {
	      backlashComp();
	      _lastDirection = _direction;
      }

      if (_direction == DIRECTION_CW)
      {
        move_CW();
		_currentPosition +=1;
      }
      else
      {
        move_CCW();
		_currentPosition -=1;
      }
      _lastStepTime = time;
      return true;
    }
	else return false;
  }
  else
  {
    _currentPosition = _targetPosition;
    _lastDirection = _direction;
    return false;
  }
}
