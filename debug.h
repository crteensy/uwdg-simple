#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG_UWDG
  #define PRINTDEBUG(x) Serial.printf x
#else
  #define PRINTDEBUG(x)
#endif // dEBUG_UWDG


#endif // DEBUG_H
