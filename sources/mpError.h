#ifndef mpERROR_H
#define mpERROR_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mpType.h"
#include "mpString.h"

#include "mpMemory.h"

typedef int mpColor;

/**
 * Fait remonter une erreur à l'utilisateur,
 * c'est-à-dire l'affiche sur la console.
 *
 * @param error
 * @pre  @error != NULL.
 */
void
mpRaise(
    mpError* error);

#endif /* mpERROR_H */
