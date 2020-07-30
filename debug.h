#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"

#ifdef DEBUG
  #define LOG Serial.print
#else
  #define LOG
#endif

#endif /* DEBUG_H */
