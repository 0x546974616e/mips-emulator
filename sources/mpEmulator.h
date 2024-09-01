#ifndef mpEMULATOR_H
#define mpEMULATOR_H

#include <stdio.h>
#include <string.h>

#include "mpType.h"

#include "mpMemory.h"
#include "mpRegister.h"
#include "mpInstruction.h"
#include "mpTranspiler.h"
#include "mpError.h"
#include "mpString.h"

/**
 *
 */
void
mpEmulator_Mode(
    mpMemory* mem,
    bool stepMode);

/**
 *
 */
void
mpEmulator_Interactive(void);

#endif /* mpEMULATOR_H */
