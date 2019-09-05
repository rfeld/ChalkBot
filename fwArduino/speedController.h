// Chalkbot 2018

#include "inttypes.h" // now types like unsigned long can be used.

// Speed Controller:
// This class manages the state of the Chalk bot in terms of 
// * speed
// * acceleration
// * deceleration
class speedController
{
    private:

    const unsigned int m_baseClk_us;
    unsigned long m_interval;       // [ticks]
    unsigned long m_targetInterval; // [ticks] 1/target speed
    unsigned long m_acc;
    unsigned long m_steps;    
    unsigned long m_cumTicks;
    bool m_first;        // perform first step after go() at next call of isNextStep()
 
    public:
    
    speedController(unsigned int baseClk_us );

    // Returns the time to the next Step in ticks
    // ticks - number of ticks since last step todo handle overflow
    long timeToNextStep( unsigned long ticks ) {};

    // Returns if next step is due
    // ticks - number of ticks since last step has been performed todo handle overflow
    bool isNextStep(unsigned long ticks );

    // Inform class that step has been made
    void step() { m_steps--; };


    bool stepLeft()  
    { 
        if(m_steps > 0) return true; 
        else return false; 
        };

    // Set maximum acceleration that is usually the constant acceleration 
    // used while ramp up and ramp down
    void setMaxAcceleration( unsigned long maxAcceleration ) { m_acc = maxAcceleration; };

    // Start the Motion with ramp up
    // steps - number of steps to go
    // speed - speed in steps per second (after ramp up and before ramp down)
    //         The maximum speed, that will actually be reached after ramp up, is
    //         limited by maximum accelaration and number of steps if the ramp up can not
    //         be completed before the ramp down has to start.
    //         Allowed range 1 to 1000
    unsigned long go( unsigned long steps , unsigned long speed );

    // Stop immediately
    void stop() { m_steps = 0; };


};
