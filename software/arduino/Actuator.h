#ifndef Actuator_H
#define Actuator_H

#include "Arduino.h"

class Actuator{
  protected:
    // Wrapper functions
    virtual float set(float value) = 0;
    virtual bool init() = 0;
    //Has begin() been performed by THIS object?
    bool ready = false;
  public:
    /**
     * Hardware ID.
    */
    String id;
    /**
     * Meaningful, readable name of the actuator.
    */
    String name;
    /**
     * Preformatted (lowercase and underscored) name of the environment variable the actuator controls.
    */
    String evname;
    Actuator(String name, String id, String evname);
    //Calls set()
    void setTarget(float target);
    //Sets ready in accordance with init()
    bool begin();
};

#endif 