#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpType.h"
#include "mpMemory.h"
#include "mpError.h"

#include "mpTranspiler.h"
#include "mpEmulator.h"

/**
 *
 */
typedef enum
{
    mpSTEP,
    mpNORMAL,
    mpINTERACTIVE
}
mpMode;

/**
 *
 */
typedef struct
{
    mpMode    mode;
    mpCString filename;
}
mpParameter;

/**
 *
 */
static
bool
mpArgument (
    int          argc,
    mpCString    argv[],
    mpParameter* param
) {
    static mpCString s_step = "-step\0";

    bool success = true;

    if (argc == 1)
    {
        /*
         * Mode interactif.
         */
        param->mode     = mpINTERACTIVE;
        param->filename = NULL;
    }

    /*
     * Mode normal
     */
    else if (argc == 2)
    {
        if (strcmp(argv[1], s_step) != 0)
        {
            param->mode     = mpNORMAL;
            param->filename = argv[1];
        }
        else
        {
            printf(
                "\x1B[91mError: "
                "\x1B[97mFilename is missing.\n"
                "\t"
                "\x1B[93m%s [path/to/filename] [-step]\n"
                "\x1B[0m",
                    mpGetFilename(argv[0]));

            success = false;
        }
    }

    /*
     * Mode normal + pas à pas
     */
    else if (argc == 3)
    {
        int i = 1, j = 0;
        int count = 0;

        while (i <= 2)
        {
            if (strcmp(argv[i], s_step) != 0)
            {
                ++count;
                j = i;
            }
            ++i;
        }

        if (count == 1)
        {
            param->mode     = mpSTEP;
            param->filename = argv[j];
        }
        else
        {
            printf(
                "\x1B[91mError: "
                "\x1B[97mExcepted format is:\n"
                "\t"
                "\x1B[93m%s [path/to/filename] [-step]\n"
                "\x1B[0m",
                    mpGetFilename(argv[0]));

            success = false;
        }
    }

    /*
     * Argument non attendu.
     */
    else if (argc > 3)
    {
        int i = 3;

        printf(
            "\x1B[91mError: "
            "\x1B[97mToo many arguments.\n"
            "\t\"\x1B[93m%s %s %s\x1B[91m",
                mpGetFilename(argv[0]),
                argv[1], argv[2]);

        while (i < argc)
            printf(" %s", argv[i++]);

        printf("\x1B[0m\"\n");

        success = false;
    }

    /*
     * Ca ne devrait jamais arriver.
     */
    else
    {
        success = false;
    }

    return success;
}


int main(int argc, mpCString argv[])
{
    mpParameter param;

    bool success;

    printf("\n");

    success = mpArgument (argc, argv, &param);

    /*
     * Si les bons arguments sont passés.
     */
    if (success)
    {
        /*
         *
         */
        if (param.mode == mpINTERACTIVE)
        {
            mpEmulator_Interactive();
        }
        else
        {
            /*
             * On crée une structure pour faire remonter les erreurs
             * et la mémoire dans laquelle iront les instructions.
             */
            mpError error;
            mpMemory memory;

            mpMemory_Init(&memory);
            error.context = (mpString)
                mpGetFilename(param.filename);

            /*
             * On décode les instructions.
             */
            mpTranspiler(param.filename, &memory, &error);

            /*
             * On récupère le succès.
             */
            success = error.success;

            /*
             * Si tout ok est on lance la simulation.
             */
            if (success)
            {
                mpEmulator_Mode(&memory, param.mode == mpSTEP);
            }
        }
    }

    /* printf("\n\n%s\n", success ? "OK" : "KO"); */
    printf("\n");

    /*
     * On retourne 0 si tout s'est bien passé, sinon 1.
     */
    return !success;
}
