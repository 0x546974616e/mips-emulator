#ifndef mpTRANSPILE_H
#define mpTRANSPILE_H

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "mpType.h"
#include "mpString.h"
#include "mpToken.h"
#include "mpMemory.h"
#include "mpError.h"

#include "vector/mpVector_mpToken.h"

/**
 *
 */
void
mpTranspiler_FetchToken(
    mpLine*   line,
    mpMemory* memory,
    mpError*  error);

/**
 *
 */
void
mpTranspiler(
    mpCString filename,
    mpMemory* memory,
    mpError*  error);

/**
 *
 */
bool
mpTranspiler_Rtype(
    uint32_t    inst,
    mpLine*     line);

/**
 *
 */
bool
mpTranspiler_Itype(
    uint32_t    inst,
    mpLine*     line);

/**
 *
 */
bool
mpTranspiler_Jtype(
    uint32_t    inst,
    mpLine*     line);


#endif /* mpTRANSPILE_H */
