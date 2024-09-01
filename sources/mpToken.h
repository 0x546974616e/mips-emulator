#ifndef mpTOKEN_H
#define mpTOKEN_H

#include <stdbool.h>

#include "mpType.h"
#include "mpString.h"
#include "mpInstruction.h"
#include "mpRegister.h"

/**
 * @param[out] fetch
 * @param[in]  line
 */
void
mpFetchInit(
    mpFetch* fetch,
    mpLine*  line);

/**
 *
 */
bool
mpFetchToken(
    mpFetch* fetch,
    mpError* error);

#endif /* mpTOKEN_H */
