#ifndef SPEEDCONTROLLER_H
#define SPEEDCONTROLLER_H

#include "speedController.h"

speedController::speedController(unsigned int baseClk_us)
: m_baseClk_us(baseClk_us),
  m_steps( 0 ),
  m_interval(100),
  m_first(true)
{  
}



bool  speedController::isNextStep(uint32_t ticks )
{
  if(  (m_steps > 0) && ( (ticks>=m_interval) || m_first) )
  {
    m_first = false;
    return true;
  }
  else 
    return false;
}


void speedController::go( uint32_t steps , uint32_t speed ) 
{ 
  m_steps = steps; 
  if(speed<1) m_interval = 100;                          // arbitary default value if speed is not corretly set
  else        m_interval = (1000000/m_baseClk_us)/speed; // one second devided by the baseClk devided by speed
  
  m_first = true;

}

#endif // SPEEDCONTROLLER_H
