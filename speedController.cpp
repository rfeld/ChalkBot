#ifndef SPEEDCONTROLLER_H
#define SPEEDCONTROLLER_H

#include "speedController.h"

speedController::speedController()
: m_steps( 0 )
{  
}


int32_t speedController::timeToNextStep( uint32_t ticks ) 
{
  if(m_steps>0) return 0; 
  else return 42;
}


#endif // SPEEDCONTROLLER_H
