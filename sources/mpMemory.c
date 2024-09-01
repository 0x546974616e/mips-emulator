#include "mpMemory.h"

/**
 *
 */
void
mpMemory_Init (
    mpMemory* memory
) {
    memset(memory, 0, sizeof(mpMemory));

    memory->minAddress = 0u;
    memory->maxAddress = mpMEMORY_MAX - 1u;
}

/**
 * Retourne les octets d'un mot de 32 bits.
 */
uint32_t
mpReverseWord (
    uint32_t word
) {
    return
        ((word & 0x000000FF) << 24) |
        ((word & 0x0000FF00) << 8)  |
        ((word & 0x00FF0000) >> 8)  |
        ((word & 0xFF000000) >> 24);
}

/**
 * Ajoute une instruction dans la mémoire à l'adresse
 * pointée par "minAddress" puis l'augmente.
 *
 * @param[in-out] memory Mémoire déjà initialisée.
 * @param[in]     inst   Instruction codée en BigEndian.
 */
void
mpWriteInstruction (
    mpMemory* memory,
    uint32_t  inst,
    mpError*  error
) {
    error->success = (memory->minAddress < mpMEMORY_MAX - 4u);

    if (error->success)
    {
        /*
         * On récupère un pointeur de 32bits.
         */
        uint32_t* ptr =
            (uint32_t*) (memory->octet + memory->minAddress);

        /*
         * On écrit dans la mémoire.
         */
        #if mpBIG_ENDIAN == 1
            *ptr = mpReverseWord(inst);
        #else
            *ptr = inst;
        #endif

        /*
         * On augmente l'adresse minimal de 4 octets.
         */
        memory->minAddress += 4u;
    }
    else
    {
        error->type = mpErrorMEMORY_FULL;
        error->code = 199;
    }
}

/**
 *
 */
void
mpPrintMemory (
    size_t x,
    size_t y,
    uint32_t address,
    mpCString title,
    mpMemory* memory
) {
    size_t i;

    /* TMP
     * On affiche l'adresse demandée dans une plage de
     * 0x40 adresses.
     */
    uint32_t from, to;

    if (address < 0x10)
    {
        from = address;
    }
    else if (address + 0x30 >= mpMEMORY_MAX)
    {
        from = mpMEMORY_MAX - 0x40;
    }
    else
    {
        from = address - 0x10;
    }

    /*
     * On aligne les adresses sur 16 octets.
     */
    from -= from % 16u;
    to    = from + 0x40; /* += 16u - (to % 16u); */

    /*
     * Titre
     */
    printf("\x1B[%d;%dH\x1B[37m%s\x1B[0m", y, x, title);

    /*
     * On affiche la première ligne d'offset (0 -> F).
     */
    printf("\x1B[%d;%dH\x1B[96m", y++, x + 9);
    for (i = 0; i < 16; ++i)
    {
        printf(" %X ", i);
    }
    printf("\x1B[0m");

    /*
     * On parcourt chaque ligne (de 16 octets).
     */
    while (from < to && from < mpMEMORY_MAX)
    {
        /*
         * On affiche l'adresse de la ligne.
         */
        printf(
            "\x1B[%d;%dH"
            "\x1B[95m0x\x1B[96m%04X \x1B[37m|\x1B[0m ",
                y++, x, from);

        printf("\x1B[97m");

        /*
         * On affiche chaque octet sous leur forme hexadécimale.
         */
        for (i = 0; (i < 16) && (from + i < mpMEMORY_MAX); ++i)
        {
            bool color =
                (from + i >= address) &&
                (from + i <  address + 4u);

            if (color) printf("\x1B[94m");

            printf("%02X ", memory->octet[from + i]);

            if (color) printf("\x1B[97m");
        }

        printf("\x1B[37m");

        /* printf("\x1B[96m|\x1B[0m "); */

        /*
         * Puis chaque octet sous leur format ASCII (sauf les caractères
         * non affichables).
         */
        for (i = 0; (i < 16) && (from + i < mpMEMORY_MAX); ++i)
        {
            char c = (char)memory->octet[from + i];
            printf("%c", isprint(c) ? c : '.');
        }

        printf("\x1B[0m");

        /*
         * On passe à la ligne suivante.
         */
        from += 16u;
    }
}

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
mpStoreWord (
    mpMemory* memory,
    uint32_t  address,
    uint32_t  value,
    mpError*  error
) {
    /*
    error->success =
        address >= memory->minAddress &&
        address <= memory->maxAddress - 3u;
    */

    if (address < memory->minAddress)
    {
        error->success = false;
        error->type = mpErrorEMULATOR_SW_READ_ONLY;
        error->code = 198;
    }
    else if (address > memory->maxAddress - 3u)
    {
        error->success = false;
        error->type = mpErrorEMULATOR_SW_OUT_MEMORY;
        error->code = 198;
    }
    else
    {
        error->success = true;
    }

    /* TMP */
    if (error->success)
    {
        uint32_t* ptr =
            (uint32_t*) (memory->octet + address);

        #if mpBIG_ENDIAN == 1
            *ptr = mpReverseWord(value);
        #else
            *ptr = value;
        #endif
    }
}

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
mpLoadWord (
    mpMemory* memory,
    uint32_t  address,
    mpError*  error
) {
    uint32_t value = 0u;

    error->success = address < mpMEMORY_MAX;

    if (error->success)
    {
        uint32_t* ptr =
            (uint32_t*) (memory->octet + address);

        #if mpBIG_ENDIAN == 1
            value = mpReverseWord(*ptr);
        #else
            value = *ptr;
        #endif
    }
    else
    {
        error->type = mpErrorEMULATOR_LW_OUT_MEMORY;
        error->code = 197;
    }

    return value;
}
