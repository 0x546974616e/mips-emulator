#ifndef mpTYPE_H
#define mpTYPE_H

#include <stddef.h>
#include <stdbool.h>

/**
 *
 */
typedef char        mpChar;
typedef char*       mpString;
typedef char const* mpCString;

/**
 * Structure décrivant une ligne.
 */
#define mpLINE_MAX 81u
typedef struct
{
    mpChar text[mpLINE_MAX];
    size_t length;
}
mpLine;

/**
 *
 */
typedef enum
{
    mpTagEND  = -1,
    mpTagNONE =  0,

    mpTagLEFT_BRACKET,
    mpTagRIGHT_BRACKET,
    mpTagCOMMA,
    mpTagCOLON,

    mpTagINSTRUCTION,
    mpTagREGISTER,
    mpTagINTEGER,
    mpTagLABEL
}
mpTag;

/**
 *
 */
typedef union
{
    char  character;
    int   integer;
    void* pointer;
}
mpValue;

/**
 * @param word Pointeur (seulement) sur le début du mot.
 *             Attention, aucun caractère de fin '\0'.
 */
typedef struct
{
    mpTag   tag;
    mpValue value;
    mpChar* word;
}
mpToken;

/**
 *
 */
typedef unsigned long mpHash;
typedef struct
{
    mpHash hash;
    size_t row;
}
mpLabel;

/**
 *
 */
typedef struct
{
    mpToken token;

    mpChar* line;
    mpChar* curr;
}
mpFetch;

/**
 * Enumération de toutes les erreurs possibles.
 */
typedef enum
{
    mpErrorOK = 0,

    mpErrorUNEXPECTED_CHAR,
    mpErrorUNEXPECTED_WORD,

    mpErrorBINARY_FORMAT,
    mpErrorOCTAL_FORMAT,
    mpErrorINTEGER_FORMAT,
    mpErrorHEX_FORMAT,

    mpErrorUNKNOWN_REGISTER,
    mpErrorMISSING_OPERAND,
    mpErrorINVALID_FORMAT,
    mpErrorLINE_OVERFLOW,
    mpErrorMEMORY_FULL,

    mpErrorUNKNOWN,
    mpErrorTMP_DIRECTIVE,
    mpErrorTMP_INSTRUCTION,

    mpErrorEMULATOR_UNDEFINED,
    mpErrorEMULATOR_JR_NOT_ALIGNED,
    mpErrorEMULATOR_LW_OUT_MEMORY,
    mpErrorEMULATOR_SW_READ_ONLY,
    mpErrorEMULATOR_SW_OUT_MEMORY,

    mpErrorEMULATOR_LINE_ZERO,
    mpErrorEMULATOR_FGETS_UNKNOWN,

    mpErrorINTEGER_OVERFLOW
}
mpErrorType;

/**
 * @param type Type de l'erreur.
 * @param code Code d'erreur à afficher.
 * @param line Pointeur sur une ligne.
 * @param col  Pointeur sur le caractère où occure l'erreur.
 * @param row  Numéro de la ligne.
 *
 * @pre Seul @col peut être NULL.
 */
typedef int mpErrorCode;
typedef struct
{
    bool        success;

    mpErrorType type;
    mpErrorCode code;

    mpString    context;
    mpString    line;
    mpChar*     col;
    size_t      row;
}
mpError;

#endif /* mpTYPE_H */
