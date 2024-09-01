#include "mpRegister.h"

/**
 *
 */
static
mpCString
s_register[mpUSUAL_REGISTER + mpSPECIAL_REGISTER] =
{
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9",
    "k0", "k1",
    "gp", "sp", "fp", "ra",

    "pc", "ir", "hi", "lo"
};

static
mpCString
s_zero = "0";

/**
 * Pas ouf mais pas mieux.
 */
int
mpIsValidRegister (
    mpCString word
) {
    size_t i = 0;

    while (i < mpUSUAL_REGISTER
        && mpStrCiCmp(word, s_register[i]) != 0
    ) {
        ++i;
    }

    return i < mpUSUAL_REGISTER ? (int)i : -1;
}

/*
 *
 */
mpCString
mpGetRegister(
    uint32_t reg
) {
    mpCString word = s_zero;

    if (reg >= 1u && reg < mpUSUAL_REGISTER + mpSPECIAL_REGISTER)
    {
        word = s_register[reg];
    }

    return word;
}

/**
 * Affiche les registres dans une boîte.
 * @param[in] x, y Position supérieur gauche de la boîte.
 * @param[in] w    Largeur maximale de la boîte.
 */
void
mpPrintRegister (
    size_t x,
    size_t y,
    size_t w,
    mpRegister* reg
) {
    size_t ox = x;
    size_t i = 0u;
    while (i < mpUSUAL_REGISTER + mpSPECIAL_REGISTER)
    {
        if (i == mpUSUAL_REGISTER && x!=ox)
        {
            y += 1u;
            x = ox;
        }

        printf(
            "\x1B[%d;%dH"
            "\x1B[95m$%d%s "
            "\x1B[96m%s\x1B[0m "
            "\x1B[37m["
            "\x1B[%dm%08X\x1B[0m"
            "\x1B[37m]\x1B[0m",
                y, x,
                i, i <= 9 ? " "  : "",
                   i == 0 ? "00" : s_register[i],
                   i == 29 || i == 32 ? 94 : 97,
                reg->GPR[i]);

        x += 19u;
        if (x >= ox + w)
        {
            x = ox;
            y += 1u;
        }
        ++i;
    }
}
