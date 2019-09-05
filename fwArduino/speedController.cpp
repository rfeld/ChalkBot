#ifndef SPEEDCONTROLLER_H
#define SPEEDCONTROLLER_H

#include "speedController.h"


speedController::speedController(unsigned int baseClk_us)
: m_baseClk_us(baseClk_us),
  m_steps( 0 ),
  m_interval(100),
  m_targetInterval(100),
  m_acc(1000),
  m_cumTicks(0),
  m_first(true)
{  
}



bool  speedController::isNextStep(unsigned long ticks )
{
  m_cumTicks += ticks;

  m_interval = m_acc/m_cumTicks;
  m_interval = m_interval > m_targetInterval ? m_interval : m_targetInterval;  
  
  if(  (m_steps > 0) && ( (ticks>=m_interval) || m_first) )
  {
    m_first = false;
    return true;
  }
  else 
    return false;
}

unsigned long speedController::go( unsigned long steps , unsigned long speed ) 
{ 
  m_cumTicks = 0;
 
  m_steps = steps; 
  if(speed<1) m_targetInterval = 100;                          // arbitary default value if speed is not corretly set
  else        m_targetInterval = (1000000/m_baseClk_us)/speed; // one second devided by the baseClk devided by speed

  m_first = true;

  return m_targetInterval;

}

#endif // SPEEDCONTROLLER_H
