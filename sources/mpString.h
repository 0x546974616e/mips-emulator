#ifndef mpSTRING_H
#define mpSTRING_H

#include <stdbool.h>

#include "mpType.h"
#include "mpError.h"

/**
 * Convertit une lettre en minuscule.
 */
mpChar
mpToLower(
    mpChar ch);

/**
 * Compare deux chaînes de caractères sans prendre
 * en compte la casse.
 */
int
mpStrCiCmp(
    mpCString a,
    mpCString b);

/**
 * Vérifie si le caractère est un white-space,
 * c'est-à-dire soit un espace ou une tabulation.
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidWhiteSpace(
    mpChar ch);

/**
 * Vérifie si le caractère est une ponctuation,
 * c'est-à-dire toute la ponctuation usuelle sauf
 * le caractère '$' (Pour les registres).
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidPunctuation(
    mpChar ch);

/**
 * Vérifie si le caractère est affichable, c'est-à-dire
 * tous les caractères usuelles visibles et la tabulation.
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidPrintable(
    mpChar ch);

/**
 * Vérifie si le caractère est un caractère alpha-numérique,
 * c'est-à-dire un chiffre ou une lettre.
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidAlphaNum(
    mpChar ch);

/**
 * Vérifie si le caractère est une lettre (majuscule ou minuscule).
 *
 * @param[in] ch
 * @return    ok
 */
bool
mpIsValidAlpha(
    mpChar ch);

/**
 * Retourne un pointeur vers le début du nom du fichier
 * dans un chemin.
 * Exemple :
 *     ./test/test.txt -> test.txt
 *            ^
 */
mpCString
mpGetFilename(
    mpCString path);

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
mpGetLine(
    FILE*    file,
    mpLine*  line,
    mpError* error);

#endif /* mpSTRING_H */
