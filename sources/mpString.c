#include "mpString.h"

/**
 * Convertit une lettre en minuscule.
 */
mpChar
mpToLower (
    mpChar ch
) {
    return
        ch >= 'A' && ch <= 'Z'
            ? (ch | 0x20)
            : ch;
}

/**
 * Compare deux chaînes de caractères sans prendre
 * en compte la casse.
 */
int
mpStrCiCmp (
    mpCString a,
    mpCString b
) {
    while (*a != '\0'
        && mpToLower(*a) == mpToLower(*b)
    ) {
        ++a; ++b;
    }
    return (int)(mpToLower(*a) - mpToLower(*b));
}

/**
 * Vérifie si le caractère est un white-space,
 * c'est-à-dire soit un espace ou une tabulation.
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidWhiteSpace (
    mpChar ch
) {
    return
        ch == ' '
     || ch == '\t';
}

/**
 * Vérifie si le caractère est une ponctuation,
 * c'est-à-dire toute la ponctuation usuelle sauf
 * le caractère '$' (Pour les registres) et le
 * caractère '.' (Pour les directives).
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidPunctuation (
    mpChar ch
) {
    return
        (ch >= '!' && ch <  '$')
     || (ch >  '$' && ch <  '+')
     || (ch == ',')
     || (ch == '/')
     || (ch >= ':' && ch <= '@')
     || (ch >= '[' && ch <= '`')
     || (ch >= '{' && ch <= '~');
}

/**
 * Vérifie si le caractère est affichable, c'est-à-dire
 * tous les caractères usuelles visibles et la tabulation.
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidPrintable (
    mpChar ch
) {
    return
        (ch >= ' ' && ch <= '~')
     || (ch == '\t');
}

/**
 * Vérifie si le caractère est un caractère alpha-numérique,
 * c'est-à-dire un chiffre ou une lettre.
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidAlphaNum (
    mpChar ch
) {
    return
        (ch >= '0' && ch <= '9')
     || (ch >= 'A' && ch <= 'Z')
     || (ch >= 'a' && ch <= 'z');
}

/**
 * Vérifie si le caractère est une lettre (majuscule ou minuscule).
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidAlpha (
    mpChar ch
) {
    return
        (ch >= 'A' && ch <= 'Z')
     || (ch >= 'a' && ch <= 'z');
}

/**
 * Retourne un pointeur vers le début du nom du fichier
 * dans un chemin.
 * Exemple :
 *     ./test/test.txt -> test.txt
 *            ^
 */
mpCString
mpGetFilename (
    mpCString path
) {
    mpCString filename = path;

    while (*path != '\0')
        if (*(path++) == '/')
            filename = path;

    return filename;
}

/**
 * Récupère la ligne courante du fichier.
 *
 * @param[in]  file
 * @param[out] line
 * @return     true si la ligne est vide, false sinon.
 *
 * @pre
 *     - @file != NULL.
 *     - @line != NULL.
 * @post
 *     - @line.text finit par un '\0'.
 */
bool
mpGetLine (
    FILE*    file,
    mpLine*  line,
    mpError* error
) {
    /*
     * Pour savoir si la ligne est "vide", c'est-à-dire
     * remplie uniquement d'espaces et/ou de tabulations.
     */
    bool isEmpty = true;

    /*
     * On récupère le premier caractère.
     */
    int ch = fgetc(file);

    /*
     * On récupère le début de la ligne envoyée.
     * Et on calcule la fin ("-1" pour ajouter le
     * caractère de fin '\0').
     */
    mpChar* curr = line->text;
    mpChar* stop = line->text + (mpLINE_MAX - 1u);

    /*
     * On initialise le succès.
     */
    error->success = true;

    /*
     * On parcourt la ligne courante du fichier tant que :
     *     - On n'a pas dépassé la taille maximale de
     *       la ligne.
     *     - On n'est pas arrivé à la fin du fichier.
     *     - On n'est pas arrivé à la fin de la ligne.
     *     - On ne rencontre pas de commentaire.
     */
    while (curr < stop
        && ch != EOF
        && ch != '\n'
        && ch != '\r'
        && ch != '#'
    ) {
        /*
         * On vérifie si la ligne est "vide".
         */
        isEmpty &= mpIsValidWhiteSpace((mpChar)ch);

        /*
         * On enregistre le caractère courant dans la
         * ligne.
         */
        if (ch == '\t')
        {
            /* TMP */
            *curr = ' '; ++curr;
            *curr = ' '; ++curr;
            *curr = ' '; ++curr;
            *curr = ' ';
        }
        else
        {
            *curr = (mpChar)ch;
        }
        ++curr;

        /*
         * Et on passe au caractère suivant du fichier.
         */
        ch = fgetc(file);
    }

    /*
     * On place le caractère sentinelle '\0' pour marquer
     * la fin de la chaîne.
     */
    *curr = '\0';

    /*
     * On calcule la taille de la ligne.
     */
    line->length = (size_t)(curr - line->text);

    /*
     * Si on est sortie de la boucle et que le dernier
     * caractère est toujours un caractère valide,
     * alors c'est qu'on est arrivé au bout du buffer et
     * on le retourne comme erroreur.
     */
    if (curr >= stop
        && ch != EOF
        && ch != '\n'
        && ch != '\r'
        && ch != '#'
    ) {
        error->success = false;
        error->type = mpErrorLINE_OVERFLOW;
        error->code = 1;
        error->line = line->text;
        error->col  = curr;
    }

    /*
     * Si jamais on s'est arrêté avant la fin de la ligne
     * (Voir la première boucle pourquoi), alors on continue
     * de la parcourir jusqu'à être bien arrivé au bout.
     */
    while (ch != EOF
        && ch != '\n'
        && ch != '\r'
    ) {
        ch = fgetc(file);
    }

    return isEmpty;
}
