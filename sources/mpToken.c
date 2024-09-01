#include "mpToken.h"

/**
 * A refaire : Trouver un moyen plus optimisé.
 */
static
void
mpParseRegister (
    mpFetch* fetch,
    mpError* error
) {
    mpString word = fetch->token.word;

    fetch->token.tag = mpTagNONE;
    error->success = true;

    /*
     * On regarde que ça commence bien par un '$'.
     */
    if (word[0] == '$')
    {
        /*
         * Si le second caractère est un chiffre alors
         * c'est peut-être un registre.
         */
        if (word[1] >= '0' && word[1] <= '9')
        {
            /*
             * S'il y a qu'un seul chiffre alors le registre
             * est valide.
             */
            if (word[2] == '\0')
            {
                fetch->token.tag           = mpTagREGISTER;
                fetch->token.value.integer = (int)(word[1] - '0');
            }
            /*
             * Sinon les registres plus long ne doivent pas commencer
             * par '0'.
             */
            else if (word[1] != '0')
            {
                /*
                 * Si le deuxième caractère est un chiffre et
                 * le troisième est le caractère de fin,
                 * alors c'est peut-être un registre.
                 */
                if (word[2] >= '0'
                 && word[2] <= '9'
                 && word[3] == '\0'
                ) {
                    fetch->token.value.integer =
                        (int)(word[1] - '0') * 10 +
                        (int)(word[2] - '0');

                    /*
                     * Si le registre est compris entre
                     * $0 et $31.
                     */
                    if (fetch->token.value.integer >= 0
                     && fetch->token.value.integer <= 31
                    ) {
                        fetch->token.tag = mpTagREGISTER;
                    }
                }
            }

            /*
             * Sinon erreur.
             */
            if (fetch->token.tag != mpTagREGISTER)
            {
                error->success = false;
                error->type = mpErrorUNKNOWN_REGISTER;
                error->code = 10;
                error->col  = word;
            }
        }
        /*
         * Sinon c'est peut-être un alias.
         */
        else if (word[1] != '\0')
        {
            int i = mpIsValidRegister(word + 1);

            /*
             * Si on s'est arrêté avant c'est qu'on a trouvé
             * une correspondance.
             */
            if (i >= 0)
            {
                fetch->token.tag           = mpTagREGISTER;
                fetch->token.value.integer = i;
            }
            /*
             * Sinon erreur.
             */
            else
            {
                error->success = false;
                error->type = mpErrorUNKNOWN_REGISTER;
                error->code = 11;
                error->col  = word;
            }
        }
        /*
         * Sinon c'est un registre vide.
         */
        else
        {
            error->success = false;
            error->type = mpErrorUNEXPECTED_CHAR;
            error->code = 12;
            error->col  = word;
        }
    }
    /*
     * S'il n'y a pas le signe '$' alors c'est une erreur.
     */
    else
    {
        error->success = false;
        error->type = mpErrorUNEXPECTED_CHAR;
        error->code = 13;
        error->col  = word;
    }
}

/**
 * Convertit une chaîne de caractères en nombre.
 *
 * @param[in]     base
 * @param[in-out] token
 *
 * @pre
 *     - @base > 0 && @base <= 10
 *     - Décimal : ^(-|\+)[1-9][0-9]*$
 *     - Octal   : ^(-|\+)0[0-9]*$
 */
static
void
mpParseInteger_Base (
    int base,
    mpFetch* fetch,
    mpError* error
) {
    /*
     * Valeur finale.
     */
    int value = 0;

    /*
     * On récupère le début du mot.
     */
    mpString string = fetch->token.word;

    /*
     * On sauvegarde le signe.
     */
    bool isNegative = (*string == '-');

    /*
     * On récupère le caractère max en fonction
     * de la base.
     */
    char max = (char)('0' + base - 1);

    /*
     * On met le tag à NONE.
     */
    fetch->token.tag = mpTagNONE;

    /*
     * Et le succès à vrai.
     */
    error->success = true;

    /*
     * S'il y a un signe '+' ou '-' on saute
     * au caractère suivant.
     */
    if (isNegative || *string == '+')
    {
        ++string;
    }

    /*
     * Si la suite n'est pas vide.
     */
    if (*string != '\0')
    {
        /*
         * On commence la conversion.
         */
        while (*string != '\0'
            && *string >= '0'
            && *string <= max
        ) {
            value = value * base + (int)(*string - '0');
            ++string;
        }

        /*
         * On n'oublie pas le signe.
         */
        if (isNegative)
        {
            value = -value;
        }

        /*
         * Si on est arrivé au bout du mot sans encombre,
         * alors c'est un vrai nombre.
         */
        if (*string == '\0')
        {
            fetch->token.tag           = mpTagINTEGER;
            fetch->token.value.integer = value;

            /* printf("\t\"%s\" = %d\n", token->word, value); */
        }
        /*
         * Sinon erreur.
         */
        else
        {
            error->success = false;
                 if (base == 2) error->type = mpErrorBINARY_FORMAT;
            else if (base == 8) error->type = mpErrorOCTAL_FORMAT;
            else                error->type = mpErrorINTEGER_FORMAT;
            error->code = 20;
            error->col  = string;
        }
    }
    /*
     * Sinon on a détecté une erreur.
     */
    else
    {
        error->success = false;
        error->type = mpErrorUNEXPECTED_CHAR;
        error->code = 21;
        error->col  = string - 1u;
    }
}

/**
 * Hexadécimale : ^(-|\+)?0(x|X)[0-9A-Fa-f]+$
 */
static
void
mpParseInteger_Hex (
    mpFetch* fetch,
    mpError* error
) {
    /*
     * Valeur finale.
     */
    int value = 0;
    int digit;

    /*
     * On récupère le début du mot.
     */
    mpString string = fetch->token.word;

    /*
     * On sauvegarde le signe.
     */
    bool isNegative = (*string == '-');

    /*
     * On met le tag à NONE.
     */
    fetch->token.tag = mpTagNONE;

    /*
     * Et le succès à "true".
     */
    error->success = true;

    /*
     * S'il y a un signe '+' ou '-' on saute
     * au caractère suivant.
     */
    if (isNegative || *string == '+')
    {
        ++string;
    }

    /*
     * Un nombre hexadécimal commence par un '0'.
     */
    if (*string == '0')
    {
        ++string;

        /*
         * Puis un 'x' ou 'X'.
         */
        if ((*string | 0x20) == 'x')
        {
            ++string;

            /*
             * Si le mot n'est pas vide.
             */
            if (*string != '\0')
            {
                /*
                 * On commence la conversion.
                 */
                while (     *string != '\0'
                       && ((*string >= '0' && *string <= '9')
                        || (*string >= 'A' && *string <= 'F')
                        || (*string >= 'a' && *string <= 'f'))
                ) {
                    digit =
                           *string >= '0'
                        && *string <= '9'
                            ? (int)(*string - '0')
                            : (int)((*string | 0x20) - 'a') + 10;

                    value = value * 16 + digit;
                    ++string;
                }

                /*
                 * On n'oublie pas le signe.
                 */
                if (isNegative)
                {
                    value = -value;
                }

                /*
                 * Si on est arrivé au bout du mot sans encombre,
                 * alors c'est un vrai nombre.
                 */
                if (*string == '\0')
                {
                    fetch->token.tag           = mpTagINTEGER;
                    fetch->token.value.integer = value;

                    /* printf("\t\"%s\" = %d\n", token->word, value); */
                }
                /*
                 * Sinon erreur.
                 */
                else
                {
                    error->success = false;
                    error->type = mpErrorHEX_FORMAT;
                    error->code = 22;
                    error->col  = string;
                }
            }
            /*
             * S'il n'y a plus de caractères, alors erreur.
             */
            else
            {
                error->success = false;
                error->type = mpErrorHEX_FORMAT;
                error->code = 23;
                error->col  = string - 1u;
            }
        }
        /*
         * S'il n'y a pas de 'x' ou 'X' alors erreur.
         */
        else
        {
            error->success = false;
            error->type = mpErrorINTEGER_FORMAT;
            error->code = 24;
            error->col  = string;
        }
    }
    /*
     * S'il n'y a pas '0' alors erreur.
     */
    else
    {
        error->success = false;
        error->type = mpErrorUNEXPECTED_CHAR;
        error->code = 25;
        error->col  = string;
    }
}

/**
 * Un châine de caractères est un nombre si :
 *      - Hexadécimale : ^(-|\+)?0(x|X)[0-9A-Fa-f]+$
 *      - Décimal      : ^(-|\+)[1-9][0-9]*$
 *      - Octal        : ^(-|\+)0[0-9]*$
 */
static
void
mpParseInteger (
    mpFetch* fetch,
    mpError* error
) {
    /*
     * On va vérifier de quel format est le nombre.
     * (Octal, Décimal, Hexadécimal)
     */
    mpCString number = fetch->token.word;

    fetch->token.tag = mpTagNONE;

    /*
     * On saute le '+' et le '-'.
     */
    if (*number == '-' || *number == '+')
    {
        ++number;
    }

    /*
     * Si le nombre commence par un 0, il est soit
     * écrit en base 8 soit en base 16.
     */
    if (*number == '0')
    {
        ++number;

        /*
         * Si le '0' est suivant d'un 'x', alors
         * il est en base hexadécimal.
         */
        if ((*number | 0x20) == 'x')
        {
            mpParseInteger_Hex(fetch, error);
        }
        else if ((*number | 0x20) == 'b')
        {
            /*
             * On sauvegarde le début du mot.
             */
            mpString word = fetch->token.word;

            /*
             * On saute pour arriver après le 'b'.
             */
            fetch->token.word += 2;

            mpParseInteger_Base(2, fetch, error);

            /*
             * On remet correctement le début du mot.
             */
            fetch->token.word = word;
        }
        /*
         * Sinon il est en base octal.
         */
        else
        {
            mpParseInteger_Base(8, fetch, error);
        }
    }
    /*
     * Sinon en base 10.
     */
    else
    {
        mpParseInteger_Base(10, fetch, error);
    }
}

/**
 *
 */
static
void
mpParseInstruction (
    mpFetch* fetch,
    mpError* error
) {
    /*
     * On récupère l'instruction associée à son nom.
     */
     mpInstruction const* instruction =
        mpGetInstruction(fetch->token.word);

    error->success = (instruction != NULL);

    /*
     * On regarde si l'opérande envoyée existe vraiment.
     */
    if (error->success)
    {
        fetch->token.tag           = mpTagINSTRUCTION;
        fetch->token.value.pointer = (void*) instruction;

        /* printf("\x1B[95m%s\x1B[0m\n", instruction->mnemonic); */
    }
    else
    {
        fetch->token.tag = mpTagNONE;
    }
}

/**
 * Vérifie si le mot peut être un label, c'est-à-dire :
 *     - [A-Za-z][A-Za-z0-9]*
 */
static
void
mpParseLabel (
    mpFetch* fetch,
    mpError* error
) {
    mpString word = fetch->token.word;

    fetch->token.tag = mpTagNONE;

    error->success = true;

    /*
     * On vérifie que le premier caractère soit une lettre.
     */
    if (mpIsValidAlpha(*word))
    {
        ++word;

        /*
         * On parcourt le mot tant que il y a des
         * chiffres et des lettres.
         */
        while (*word != '\0'
            && mpIsValidAlphaNum(*word)
        ) {
            ++word;
        }

        /*
         * Si on est arrivé au bout de la chaîne sans
         * encombre.
         */
        if (*word == '\0')
        {
            fetch->token.tag           = mpTagLABEL;
            fetch->token.value.pointer = (void*) fetch->token.word;
        }
        /*
         * Sinon erreur.
         */
        else
        {
            error->success = false;
            error->type = mpErrorUNEXPECTED_CHAR;
            error->code = 30;
            error->col  = word;
        }
    }
    /*
     * Si le premier caractère n'est pas une lettre,
     * alors erreur.
     */
    else
    {
        error->success = false;
        error->type = mpErrorUNEXPECTED_CHAR;
        error->code = 31;
        error->col  = word;
    }
}

/**
 *
 */
static
void
mpFindTag_String (
    mpFetch* fetch,
    mpError* error
) {
    mpChar firstChar = *fetch->token.word;

    error->success = true;

    fetch->token.tag = mpTagNONE;
    fetch->token.value.integer = 0u;

    /*
     * Si le premier caractère est '\0',
     * alors le token n'a pas de sens.
     * (Ce cas n'arrivera jamais).
     */
    if (firstChar == '\0')
    {
        error->success = false;
        error->type = mpErrorUNEXPECTED_CHAR;
        error->code = 40;
        error->col  = fetch->token.word;
    }

    /*
     * 1) Les registres commencent par un '$'.
     */
    else if (firstChar == '$')
    {
        mpParseRegister(fetch, error);
    }

    /*
     * 2) Les directives commencent par un '.'.
     */
    else if (firstChar == '.')
    {
        /* TMP : Les directives ne sont pas encore implémentées */
        error->success = false;
        error->type =
            fetch->token.word[1] == '\0'
                ? mpErrorUNEXPECTED_CHAR
                : mpErrorTMP_DIRECTIVE;
        error->code = 41;
        error->col  = fetch->token.word;
    }

    /*
     * 3) Les nombres commencent soit par un chiffre
     * soit par le signe '-' ou '+'.
     */
    else if ((firstChar >= '0' && firstChar <= '9')
           || firstChar == '-'
           || firstChar == '+'
    ) {
        mpParseInteger(fetch, error);
    }

    else
    {
        /*
         * 4) On regarde si c'est une opérande connue.
         */
        mpParseInstruction(fetch, error);

        if (!error->success)
        {
            /*
             * 5) Sinon c'est peut-être un label valide.
             */
            mpParseLabel(fetch, error);
        }
    }
}

/**
 * Regarde si le premier caractère du token
 * est une poncutatio valide.
 */
static
void
mpFindTag_Char (
    mpFetch* fetch,
    mpError* error
) {
    /*
     * On initialise le succès.
     */
    error->success = true;

    /*
     * Aucune valeur spécifique pour les caractères seuls.
     */
    fetch->token.value.integer = 0;

    /*
     * On vérifie si ce caractère fait partie de la
     * ponctuation, sinon erreur.
     */
    switch (*fetch->token.word)
    {
        case '(' :
            fetch->token.tag = mpTagLEFT_BRACKET;
            break;

        case ')' :
            fetch->token.tag = mpTagRIGHT_BRACKET;
            break;

        case ',' :
            fetch->token.tag = mpTagCOMMA;
            break;

        case ':' :
            fetch->token.tag = mpTagCOLON;
            break;

        default :
            fetch->token.tag = mpTagNONE;

            error->success = false;
            error->type = mpErrorUNEXPECTED_CHAR;
            error->code = 42;
            error->col  = fetch->token.word;
    }
}


/*
 * ============================================================================
 */

/**
 * @param[out] fetch
 * @param[in]  line
 */
void
mpFetchInit (
    mpFetch* fetch,
    mpLine*  line
) {
    fetch->line = line->text;
    fetch->curr = fetch->line;
}

/**
 * @return Plus rien
 * fetch.token.word = NULL si rien trouvé.
 */
bool
mpFetchToken (
    mpFetch* fetch,
    mpError* error
) {
    /*
     * Mot à trouver.
     */
    mpChar* word = NULL;

    /*
     * On initialise le succès.
     */
    error->success = true;

    /*
     * Si le caractère courant n'est pas une ponctuation
     * et qu'on est pas arrivé au bout de la ligne.
     */
    if (*fetch->curr != '\0'
     && !mpIsValidPunctuation(*fetch->curr)
    ) {
        /*
         * On s'avance sur la ligne jusqu'au prochaine mot.
         * C'est-à-dire tant que le caractère est un
         * white-space (' ' ou '\t').
         */
        while (*fetch->curr != '\0'
             && mpIsValidWhiteSpace(*fetch->curr)
        ) {
            ++fetch->curr;
        }

        /*
         * On récupère le début du mot.
         */
        word = fetch->curr;

        /*
         * On continue à parcourir le mot tant que :
         *     - C'est un caractère affichable.
         *     - Ce n'est pas un white-space (' ' ou '\t').
         *     - Ce n'est pas une ponctuation ('$' n'en fait
         *       pas partie et '.' aussi).
         */
        while (*fetch->curr != '\0'
            &&  mpIsValidPrintable(*fetch->curr)
            && !mpIsValidWhiteSpace(*fetch->curr)
            && !mpIsValidPunctuation(*fetch->curr)
        ) {
            ++fetch->curr;
        }

        /*
         * Si le dernier caractère n'est pas affichable
         * alors c'est un caractère inattendu, donc une
         * erreur.
         */
        if (*fetch->curr != '\0'
         && !mpIsValidPrintable(*fetch->curr)
        ) {
            error->success = false;
            error->type = mpErrorUNEXPECTED_CHAR;
            error->code = 43;
            error->col  = fetch->curr;

            ++fetch->curr;
        }

        /*
         * Si ce n'est pas un mot vide.
         */
        else if (word != fetch->curr)
        {
            /*
             * On sauvegarde le dernier caractère pour y marquer
             * temporairement la fin du mot.
             */
            mpChar backup = *fetch->curr;
            *fetch->curr = '\0';

            /*
             * On marque le début du mot du token.
             * Attention il n'y a pas de caratère de fin '\0'.
             */
            fetch->token.word = word;

            /*
             * On cherche l'étiquette du mot.
             */
            mpFindTag_String(fetch, error);

            /* printf("'\x1B[93m%s\x1B[0m'\n", word); */

            /*
             * On remet le dernier caractère.
             */
            *fetch->curr = backup;
        }

        /*
         * Sinon aucun mot trouvé.
         */
        else
        {
            word = NULL;
        }
    }

    /*
     * Si aucun mot n'a été trouvé et que le caractère courant
     * est une ponctuation.
     */
    if (word == NULL
     && mpIsValidPunctuation(*fetch->curr)
    ) {
        /* printf("'\x1B[96m%c\x1B[0m'\n", *fetch->curr); */

        /*
         * On marque le début du mot du token.
         * Attention il n'y a pas de caratère de fin '\0'.
         */
        fetch->token.word = word = fetch->curr;

        /*
         * On cherche l'étiquette de la lettre.
         */
        mpFindTag_Char(fetch, error);

        /*
         * On passe au caractère suivant.
         */
        ++fetch->curr;
    }

    return *fetch->line != '\0' && word != NULL;
}
