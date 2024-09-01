#include "mpError.h"

#define mpRED     91
#define mpCYAN    96
#define mpMAGENTA 95
#define mpARROW   '^'

#define mpCHAR    0x1
#define mpWORD    0x0

#define mpToStrI(v) #v
#define mpToStrD(v) mpToStrI(v)

/*
#define mpERROR_MAX 7u
static size_t s_count = 0u;
*/

/**
 * Affiche un mot et échape les caractères
 * non affichables.
 */
static
void
mpPrintWORD (
    mpCString word,
    bool      one
) {
    /* Pas ouf mais disons que c'est temporaire. */
    for (;one||*word;++word,one=0)
        if (*word != '\t'
          && mpIsValidPrintable(*word))
             printf("%c",   *word);
        else printf("0x%X", *word);
}

/**
 *
 */
static
void
mpPrintUNEXPECTED_WORD (
    mpError*  error,
    mpColor   color,
    mpCString title,
    mpCString description1,
    mpCString description2,
    bool      one
) {

    /*
     * Si le pointeur vers l'erreur n'est pas NULL.
     */
    if (error->col == NULL)
    {
        printf(
            "\x1B[93m%s\x1B[0m:("
            "\x1B[92m%d\x1B[0m,"
            "\x1B[92m?\x1B[0m): "

            "\x1B[%dm[CS%03d] %s: "
            "\x1B[97m%s"
            "\x1B[%dm'?'"
            "\x1B[0m%s.\n"
            "\x1B[37m%s\x1B[0m\n\n",

                error->context,
                error->row,

                color,
                error->code,
                title,
                description1,
                color,
                description2,
                error->line);
    }

    /*
     * Si le pointeur n'est pas égale à NULL.
     */
    else
    {
        mpChar charWord;
        mpChar charEnd;

        /*
         * On calcule le numéro de la colonne.
         */
        size_t col = (size_t)(error->col - error->line);

        /*
         * On récupère le début du mot.
         */
        mpChar* word = error->col;

        /*
         * Et sa fin.
         */
        mpChar* end  = word;
            if (mpIsValidPunctuation(*word)) one = mpCHAR;
            if (one) ++end;
            else
                while (*end != '\0'
                    &&  mpIsValidPrintable(*end)
                    && !mpIsValidWhiteSpace(*end)
                    && !mpIsValidPunctuation(*end)
                ) ++end;

        /*
         * On sauvegarde les caractères de début et de fin.
         */
        charWord = *word;
        charEnd  = *end;

        /*
         * On affiche l'en-tête avec le mot, le titre et
         * les descriptions.
         */
        *end = '\0';
        printf(
            "\x1B[93m%s\x1B[0m:("
            "\x1B[92m%d\x1B[0m,"
            "\x1B[92m%d\x1B[0m): "

            "\x1B[%dm[CS%03d] %s: "
            "\x1B[97m%s"
            "\x1B[%dm'",

                error->context,
                error->row,
                col + 1u,

                color,
                error->code,
                title,
                description1,
                color);

        mpPrintWORD(word, one);

        printf(
            "'"
            "\x1B[97m%s.\n",
                description2);

        /*
         * On affiche le début de la ligne.
         */
        *word = '\0';
         printf(
            "\x1B[37m%s"
            "\x1B[%dm",
                error->line,
                color);

        /*
         * Puis le mot en rouge.
         */
        *word = charWord;
        mpPrintWORD(word, one);

        /*
         * Et la fin de la ligne.
         */
        *end = charEnd;
        printf(
            "\x1B[37m%s\n"
            "\x1B[%dm%*c",
                end,
                color,
                col + 1u,
                mpARROW);

        /*
         * Pour finir on "souligne" le mot en rouge.
         */
        while (--end > word)
        {
            printf("~");
        }

        /*
         * On termine proprement.
         */
        printf("\x1B[0m\n");
    }
}

/**
 *
 */
static
void
mpPrintSIMPLE (
    mpError*  error,
    mpColor   color,
    mpCString title,
    mpCString description
) {
    if (error->line == NULL)
    {
        printf(
            "\x1B[93m%s: "
            "\x1B[%dm[CS%03d] %s: "
            "\x1B[97m%s.\x1B[0m"
            "\n\n",
                error->context,
                color,
                error->code,
                title,
                description);
    }
    else
    {
        printf(
            "\x1B[93m%s: "
            "\x1B[%dm[CS%03d] %s: "
            "\x1B[97m%s.\n"
            "\x1B[37m    %s\x1B[0m\n\n",
                error->context,
                color,
                error->code,
                title,
                description,
                error->line);
    }
}

/**
 *
 */
/*
void
mpPrintErrorCount ()
{
    if (s_count > mpERROR_MAX)
    {
        size_t sub = s_count - mpERROR_MAX;

        printf(
            "... (\x1B[91m+%d\x1B[0m error%s)\n",
            sub, sub == 1 ? "" : "s");
    }
}
*/

/**
 * Fait remonter une erreur à l'utilisateur,
 * c'est-à-dire l'affiche sur la console.
 *
 * @param error
 * @pre  @error != NULL.
 */
void
mpRaise (
    mpError* error
) {
    /*
    if (s_count++ >= mpERROR_MAX)
        return;
    */

    switch (error->type)
    {

        case mpErrorUNEXPECTED_CHAR :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Error",
                "Unexpected ", " character",
                mpCHAR);
             break;

        case mpErrorBINARY_FORMAT :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Invalid binary integer",
                "Unexpected ", " character",
                 mpCHAR);
             break;

        case mpErrorOCTAL_FORMAT :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Invalid octal integer",
                "Unexpected ", " character",
                 mpCHAR);
             break;

        case mpErrorINTEGER_FORMAT :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Invalid integer",
                "Unexpected ", " character",
                 mpCHAR);
             break;

        case mpErrorHEX_FORMAT :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Invalid hex format",
                "Unexpected ", " character",
                 mpCHAR);
             break;

        case mpErrorUNKNOWN :
             error->col = NULL;
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Internal error ",
                "", "",
                mpCHAR);
             break;

        case mpErrorUNKNOWN_REGISTER :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Error",
                "Unknown ", " register",
                mpWORD);
             break;

        case mpErrorMISSING_OPERAND :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Format error",
                "Missing operand at ", "",
                mpWORD);
             break;

        case mpErrorINVALID_FORMAT :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Format error",
                "", " was not expected here",
                mpWORD);
             break;

        case mpErrorLINE_OVERFLOW :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Internal error",
                "Line buffer too small (", ")",
                mpCHAR);
             break;

        case mpErrorMEMORY_FULL :
             mpPrintSIMPLE(
                error, mpCYAN,
                "Warning",
                "Memory is full (max is \x1B[96m"
                    mpToStrD(mpMEMORY_MAX) "\x1B[0m bytes)."
                "\n\tCannot write instruction to memory");
             break;

        case mpErrorTMP_DIRECTIVE :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "\n\t"
                "Error",
                "Unknown ", " directive."
                "\n\t"
                "\x1B[96mNote:"
                "\x1B[97m  Directives are not implemented yet",
                mpWORD);
             break;

        case mpErrorTMP_INSTRUCTION :
             mpPrintUNEXPECTED_WORD(
                error, mpRED,
                "Error",
                "Unexpected ",
                ".\n\t"
                "\x1B[96mNote:"
                "\x1B[97m Only instruction are supported so far",
                mpWORD);
             break;

        case mpErrorEMULATOR_UNDEFINED :
        {
            error->line = NULL;
            mpPrintSIMPLE(
                error, mpRED,
                "Instruction error",
                "Undefined opcode");
        } break;

        case mpErrorEMULATOR_JR_NOT_ALIGNED :
        {
            error->line = NULL;
            mpPrintSIMPLE(
                error, mpRED,
                "JR error",
                "Register value is not aligned");
        } break;

        case mpErrorEMULATOR_LW_OUT_MEMORY :
        {
            error->line = NULL;
            mpPrintSIMPLE(
                error, mpRED,
                "Load Word error",
                "Out of memory");
        } break;

        case mpErrorEMULATOR_SW_READ_ONLY :
        {
            error->line = NULL;
            mpPrintSIMPLE(
                error, mpRED,
                "Store Word error",
                "Read-only memory");
        } break;

        case mpErrorEMULATOR_SW_OUT_MEMORY :
        {
            error->line = NULL;
            mpPrintSIMPLE(
                error, mpRED,
                "Store Word error",
                "Out of memory");
        } break;

        /*
         * Le code de @error.code correspond ici au
         * nombre de bits max.
         */
        case mpErrorINTEGER_OVERFLOW :
        {
            char description2[20] = { 0 };

            snprintf(
                description2, 20,
                " must be on %d bits",
                error->code);

            mpPrintUNEXPECTED_WORD(
                error, mpCYAN,
                "Warning",
                "Integer ",
                description2,
                mpWORD);
        } break;

        case mpErrorEMULATOR_LINE_ZERO :
             mpPrintSIMPLE(
                error, mpCYAN,
                "Warning",
                "Line is empty");
             break;

        default:
            printf(
                "\x1B[91m[CS%03d] Internal Error: "
                "\x1B[97m Error type "
                "\x1B[91m'%d'"
                "\x1B[97m is undefined."
                "\x1B[0m\n\n",
                    error->code,
                    error->type);
            break;
    }
}
