#ifndef mpMEMORY_H
#define mpMEMORY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "mpType.h"

#define mpBIG_ENDIAN 1

#define mpMEMORY_MAX 1024 /* ou 1Mo */

/**
 * L'intervalle [@minAddress, @maxAddress] désigne
 * l'ensemble des octets accessible en Lecture/Écriture.
 * Par exemple :
 *     Les octets en-dessous de @minAddress seront les
 *     instructions.
 */
typedef struct
{
    uint8_t  octet[mpMEMORY_MAX];
    uint32_t minAddress;
    uint32_t maxAddress;
}
mpMemory;

/**
 *
 */
void
mpMemory_Init(
    mpMemory* memory);

/**
 * Retourne les octets d'un mot de 32 bits.
 */
uint32_t
mpReverseWord(
    uint32_t h);

/**
 * Ajoute une instruction dans la mémoire à l'adresse
 * pointée par "minAddress" puis l'augmente.
 *
 * @param[in-out] memory Mémoire déjà initialisée.
 * @param[in]     inst   Instruction codée en BigEndian.
 */
void
mpWriteInstruction(
    mpMemory* memory,
    uint32_t  inst,
    mpError*  error);

/**
 *
 */
void
mpPrintMemory(
    size_t x, size_t y,
    uint32_t address,
    mpCString title,
    mpMemory* memory);

/**
 * Charge un entier de 32 bits dans la mémoire.
 *
 * La valeur sera stockée en big-endian si le flag
 * mpBIG_ENDIAN est à 1.
 *
 * @param[in-out] memory
 * @param[in]     address
 * @param[in]     value
 *
 * @pre
 *     - @memory != NULL.
 */
void
mpStoreWord(
    mpMemory* memory,
    uint32_t  address,
    uint32_t  value,
    mpError*  error);

/**
 * Recupère un entier 32 bits de la mémoire.
 *
 * @param[in-out] memory
 * @param[in]     address
 * @return        value
 *
 * @pre
 *     - @memory != NULL.
 */
uint32_t
mpLoadWord(
    mpMemory* memory,
    uint32_t  address,
    mpError*  error);

#endif /* mpMEMORY_H */
