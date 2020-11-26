#ifndef PIT_H
#define PIT_H

#include "i8259.h"
#include "lib.h"
extern void initialize_pit();
extern void pit_handler();

#endif