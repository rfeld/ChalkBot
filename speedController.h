// Speed Controller:
// This class manages the state of the Chalk bot in terms of 
// * speed
// * acceleration
// * deceleration

// ToDo: Das sollte auf PC und Arduino der selbe Datentyp sein (32Bit unsigned)
typedef unsigned long int uint32_t;
typedef          long int  int32_t;

class speedController
{
    private:

    uint32_t m_steps;

    public:
    
    speedController( );

    // Returns the time to the next Step in ticks
    // ticks - number of Microseconds since last step todo handle overflow
    int32_t timeToNextStep( uint32_t ticks );

    // Inform class that step has been made
    void step() { m_steps--; };


    bool stepLeft()  
    { 
        if(m_steps > 0) return true; 
        else return false; 
        };

    // Set maximum acceleration that is usualle the constant acceleration 
    // used while ramp up and ramp down
    void setMaxAcceleration( uint32_t maxAcceleration );

    // Start the Motion with ramp up
    // steps - number of steps to go
    // speed - speed in steps per second (after ramp up and before ramp down)
    //         The maximum speed, that will actually be reached after ramp up, is
    //         limited by maximum accelaration and number of steps if the ramp up can not
    //         be completed before the ramp down has to start.
    void go( uint32_t steps , uint32_t speed ) { m_steps = steps; };

    // Stop immediately
    void stop() { m_steps = 0; };


};
