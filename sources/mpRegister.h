#ifndef mpREGISTER_H
#define mpREGISTER_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "mpType.h"
#include "mpString.h"

#define mpUSUAL_REGISTER   32u
#define mpSPECIAL_REGISTER 4u

/**
 * Structure des registres nommées.
 */
typedef struct
{
    /* Registres */
    uint32_t zero;
    uint32_t at;
    uint32_t v0, v1;
    uint32_t a0, a1, a2, a3;
    uint32_t t0, t1, t2, t3, t4, t5, t6, t7;
    uint32_t s0, s1, s2, s3, s4, s5, s6, s7;
    uint32_t t8, t9;
    uint32_t k0, k1;
    uint32_t gp, sp, fp, ra;
    /* Registres spéciaux */
    uint32_t pc, ir;
    uint32_t hi, lo;
}
mpRegister_;

/**
 * Union réunissant un tableau de registres avec
 * la structure ci-dessus.
 */
typedef union
{
    uint32_t    GPR[mpUSUAL_REGISTER + mpSPECIAL_REGISTER];
    mpRegister_ all;
}
mpRegister;

/**
 *
 */
int
mpIsValidRegister(
    mpCString word);

/*
 *
 */
mpCString
mpGetRegister(
    uint32_t reg);

/*
 *
 */
void
mpPrintRegister(
    size_t x,
    size_t y,
    size_t w,
    mpRegister* reg);

#endif /* mpREGISTER_H */
