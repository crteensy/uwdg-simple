#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG
  #define PRINTDEBUG(x) Serial.printf x
#else
  #define PRINTDEBUG(x)
#endif


#endif // DEBUG_H

