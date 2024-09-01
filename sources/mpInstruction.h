#ifndef mpINSTRUCTION_H
#define mpINSTRUCTION_H

#include <stdio.h>
#include <stdint.h>

#include "mpType.h"
#include "mpString.h"
#include "mpMemory.h"
#include "mpError.h"

#include "vector/mpVector_mpToken.h"

#define mpINSTRUCTION_MAX 26u

#define mpADD_OPCODE  0x20
#define mpADDI_OPCODE  0x08
#define mpAND_OPCODE    0x24
#define mpBEQ_OPCODE   0x04
#define mpBGTZ_OPCODE 0x07
#define mpBLEZ_OPCODE  0x06
#define mpBNE_OPCODE    0x05
#define mpDIV_OPCODE   0x1A
#define mpJ_OPCODE    0x02
#define mpJAL_OPCODE   0x03
#define mpJR_OPCODE     0x08
#define mpLUI_OPCODE    0x0F
#define mpLW_OPCODE      0x23
#define mpMFHI_OPCODE    0x10
#define mpMFLO_OPCODE     0x12
#define mpMULT_OPCODE     0x18
#define mpNOP_OPCODE      0x00
#define mpOR_OPCODE      0x25
#define mpROTR_OPCODE    0x02
#define mpSLL_OPCODE    0x00
#define mpSLT_OPCODE    0x2A
#define mpSRL_OPCODE   0x02
#define mpSUB_OPCODE   0x22
#define mpSW_OPCODE     0x2B
#define mpSYSCALL_OPCODE 0x0C
#define mpXOR_OPCODE     0x26

/**
 *
 */
typedef struct mpInstruction mpInstruction;
typedef void (*mpToMemory)(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 * Définit le nom et l'opcode d'uns instruction
 * avec ses méthodes et autres informations
 * associées.
 *
 * @pre
 *     - @mnemonic non NULL.
 *     - @opcode sur 6 bits.
 */
struct mpInstruction
{
    int        opcode;
    mpCString  mnemonic;
    mpToMemory ToMemory;

    mpCString format;
    mpCString description;
};

/**
 *
 */
void
mpRtype_ORRR(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 *
 */
void
mpRtype_ORRI(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 *
 */
void
mpRtype_ORR(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 *
 */
void
mpRtype_OR(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 *
 */
void
mpRtype_O(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 *
 */
void
mpItype_ORIBRB(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 *
 */
void
mpItype_ORRI(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 *
 */
void
mpItype_ORI(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 *
 */
void
mpJtype_OI(
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error);

/**
 * Cherche l'instruction associée à son nom.
 *
 * @param[in] mnemonic
 * @return    l'instruction, NULL sinon.
 *
 * @pre @mnemonic != NULL;
 */
mpInstruction const*
mpGetInstruction(
    mpCString mnemonic);

#endif /* mpINSTRUCTION_H */
