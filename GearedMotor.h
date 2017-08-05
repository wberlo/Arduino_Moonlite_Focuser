#ifndef GearedMotor_h
#define GearedMotor_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

class GearedMotor
{
  public:
// constructor takes 3 pin numbers as arguments
// first pin is enable, second and third are input/direction pins
// see table for explanation
    GearedMotor(uint8_t pinEN = 9, uint8_t pin1 = 8, uint8_t pin2 = 7);
// will execute a backlash compensation by moving the motor in _direction
// direction for _backlashSteps steps if _backlashFlag is set
	void setBacklashFlag(boolean value);
    void setBacklashSteps (long value);
    boolean backlashComp();

    void disableOutputs();
    void enableOutputs();
	
    long stepsToGo();
    long currentPosition();
    long targetPosition();
    void setTargetPosition(long value);
    void setCurrentPosition(long value);
	void setSpeed(uint8_t value);
    void moveTo(long value);
    boolean run();

  protected:
// direction indicator, symbolic names. Maybe not used
    typedef enum
    {
        DIRECTION_CCW = 0,
        DIRECTION_CW = 1
    } Direction;
    void move_CW();   // set up motion clockwise
    void move_CCW();  // set up motion counter clockwise

  private:
    long _stepInterval = 1000;
    unsigned long _lastStepTime = 0;
    long _stepsToTarget;
// next will be used for calculation of stepsToTarget, and direction
    long _targetPosition;
    long _currentPosition;
    long _backlashSteps;
    boolean _direction;  // 1 == CW
    boolean _lastDirection;
	boolean _backlashFlag;
    uint8_t _enablePin, _pin_1A, _pin_2A, _speed;
};


#endif

