#include "mpTranspiler.h"

/**
 *
 */
/*bool
mpTranspiler_Label (
    FILE*             file,
    mpVector_mpLabel* label,
    mpError*          error
) {

    while (!feof(file))
    {

        mpFetchToken()
        if (fetch.token.tag == mpTagLABEL)
        {

        }
        else
        {
            size_t i = label->size;
            while (i >= 1u && label->data[i - 1].row != 0u)
            {
                label->data[i - 1].row = thisVirtualRow;
            }
        }
    }
}
*/

/**
 *
 */
void
mpTranspiler_FetchToken (
    mpLine*   line,
    mpMemory* memory,
    mpError*  error
) {
    /*
     * On crée un vecteur de tokens.
     */
    mpVector_mpToken tokens;
    if (mpVectorInit_mpToken(&tokens))
    {
        bool success = true;

        /*
         * On crée une structure qui va nous permettre
         * de parcourir la ligne est de récupérer les tokens
         * un à un.
         */
        mpFetch fetch;
        mpFetchInit(&fetch, line);

        /*
         * On définit la ligne de l'erreur.
         */
        error->line = line->text;

        /*
         * On parcourt tant qu'il n'y a plus rien
         * dans la ligne.
         */
        while (mpFetchToken(&fetch, error))
        {
            /*
             * On met à jour l'erreur globale.
             */
            success &= error->success;
            if (error->success)
            {
                /*
                 * Puis on ajoute le token trouvé à la
                 * liste des tokens.
                 */
                if (!mpVectorAdd_mpToken(
                        &tokens, &fetch.token)
                ) {
                    /*
                     * Erreur interne : realloc failed.
                     */
                    success = false;
                    printf(
                        "\x1B[91m[CS900] Error:"
                        "\x1B[97m Could not append token:\n"
                        "\x1B[0m\t%s\n",
                            strerror(errno));
                }
            }
            else mpRaise(error);
        }

        /*
         * Erreur à retourner.
         */
        error->success = success;

        /*
         * S'il y a au moins un token dans la liste.
         */
        if (error->success && tokens.size > 0u)
        {
            /*
             * On regarde si le premier token est
             * une instruction.
             */
            mpToken* firstToken = tokens.data;
            if (firstToken->tag == mpTagINSTRUCTION)
            {
                /*
                 * On récupère l'instruction.
                 */
                mpInstruction* inst =
                    (mpInstruction*) firstToken->value.pointer;

                /*
                 * On vérifie qu'il y a une instruction.
                 * Mais ça ne devrait ne jamais arriver.
                 */
                if (inst != NULL)
                {
                    inst->ToMemory(
                        inst, &tokens, memory, error);

                    /*
                     * On met à jour l'erreur globale.
                     */
                    success = error->success;
                    if (!success)
                    {
                        mpRaise(error);

                        if (error->type == mpErrorMISSING_OPERAND
                         || error->type == mpErrorINVALID_FORMAT
                        ) {
                            printf(
                                "\t\x1B[96mFormat: \x1B[97m%s %s\n"
                                "\t\x1B[0m        %s\n\n",
                                    inst->mnemonic,
                                    inst->format,
                                    inst->description);
                        }
                    }
                }

                /*
                 * Ca ne devrait jamais arriver.
                 */
                else
                {
                    error->success = false;
                    error->type = mpErrorUNKNOWN;
                    error->code = 901;
                    error->line = line->text;
                    error->col  = firstToken->word;
                    mpRaise(error);
                }
            }

            /*
             * TMP :
             * Si le premier token n'est pas une operande alors
             * erreur.
             */
            else
            {
                error->success = false;
                error->type = mpErrorTMP_INSTRUCTION;
                error->code = 902;
                error->line = line->text;
                error->col  = firstToken->word;
                mpRaise(error);
            }
        }

        /* TMP
         * S'il y a 0 token dans le vecteur.
         * C'est possible quand tous les tokens extrait
         * de la ligne ont eu des erreurs.
         */
        else
        {
            error->success = false;
            /* error->type = mpErrorUNKNOWN; */
            /* error->code = 903; */
            /* error->line = line->text; */
            /* error->col  = NULL; */
            /* mpRaise(error); */
        }

        /*
         * On n'oublie pas de libérer les données du vecteur.
         */
        mpVectorFree_mpToken(&tokens);
    }

    /*
     * Erreur interne : malloc failed.
     */
    else
    {
        error->success = false;
        printf(
            "\x1B[91m[CS904] Error:"
            "\x1B[97m Could not create token vector:\n"
            "\x1B[0m\t%s\n",
                strerror(errno));
    }
}

/**
 *
 */
static
void
mpTranspiler_FetchLine (
    FILE*     file,
    mpMemory* memory,
    mpError*  error
) {
   bool success = true;

    /*
     * On crée une ligne.
     */
    mpLine  line;

    /*
     *
     */
    size_t lineCount = 0u;

    /*
     * On initialise la ligne de l'erreur.
     */
    error->row = 0u;

    /*
     * Tant qu'on est pas arrivé au bout du fichier.
     */
    while (!feof(file))
    {
        /*
         * On augmente la ligne de l'erreur.
         */
        error->row += 1u;

        /*
         * mpGetLine return "true" si la ligne est vide.
         */
        if (!mpGetLine(file, &line, error))
        {
            ++lineCount;

            /*
             * On met à jour le succès globale.
             */
            success &= error->success;
            if (error->success)
            {
                /* printf("%2d | %s\n", error->row, line.text); */
                /*
                 * Si mpGetLine s'est déroulée sans encombre
                 * on lance la suite.
                 */
                mpTranspiler_FetchToken(
                    &line, memory, error);

                /*
                 * On met à jour l'erreur globale.
                 */
                success &= error->success;
            }
            else mpRaise(error);
        }
    }

    if (!error->row || !lineCount)
    {
        success = false;
        printf(
            "\x1B[91mError: "
            "\x1B[97m\""
            "\x1B[93m%s"
            "\x1B[97m\" is empty."
            "\x1B[0m\n",
                error->context);
    }

    error->success = success;
}

/**
 *
 */
void
mpTranspiler (
    mpCString filename,
    mpMemory* memory,
    mpError*  error
) {
    /*
     * On ouvre le fichier demandé.
     */
    FILE* source = fopen(filename, "r");

    /*
     * On teste s'il est bien ouvert.
     */
    error->success = source != NULL;
    if (error->success)
    {
        /*
         * Puis on transpile ves la mémoire.
         */
        mpTranspiler_FetchLine(
            source, memory, error);

        /*
         * Et on n'oublie pas de le fermer.
         */
        fclose(source);
    }

    /*
     * Si le fichier ne s'est pas bien ouvert.
     */
    else
    {
        printf(
            "\x1B[91mError:"
            "\x1B[97m Could not open \""
            "\x1B[93m%s"
            "\x1B[97m\":\n"
            "\x1B[0m\t%s\n",
                filename,
                strerror(errno));
    }

}



/*
 * ============================================================================
 */


/**
 * C'est moche et ça va prendre BEAUCOUP de place mais tant pis ...
 * Si j'avais eu plus de temps et moins de travaille j'aurais fait
 * tellement mieux et complet et etc............................
 */
bool
mpTranspiler_Rtype (
    uint32_t    inst,
    mpLine*     line
) {
    bool success = true;

    uint32_t op = (inst)       & 0x3Fu;
    uint32_t sa = (inst >> 6)  & 0x1Fu;
    uint32_t rd = (inst >> 11) & 0x1Fu;
    uint32_t rt = (inst >> 16) & 0x1Fu;
    uint32_t rs = (inst >> 21) & 0x1Fu;

    switch (op)
    {
        /*
         * ADD rd, rs, rt
         */
        case mpADD_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mADD "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rd),
                        mpGetRegister(rs),
                        mpGetRegister(rt));
        } break;

        /*
         * AND rd, rs, rt
         */
        case mpAND_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mAND "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rd),
                        mpGetRegister(rs),
                        mpGetRegister(rt));
        } break;

        /*
         * DIV rs, rt
         */
        case mpDIV_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mDIV "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rs),
                        mpGetRegister(rt));
        } break;

        /*
         * JR rs
         */
        case mpJR_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mJR "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rs));
        } break;

        /*
         * MFHI rd
         */
        case mpMFHI_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mMFHI "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rd));
        } break;

        /*
         * MFLO rd
         */
        case mpMFLO_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mMFLO "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rd));
        } break;

        /*
         * MULT rs, rt
         */
        case mpMULT_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mMULT "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rs),
                        mpGetRegister(rt));
        } break;

        /*
         * OR rd, rs, rt
         */
        case mpOR_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mOR "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rd),
                        mpGetRegister(rs),
                        mpGetRegister(rt));
        } break;

        /*
         * NOP
         * SLL rd, rt, sa
         */
        case mpSLL_OPCODE :
        {
            if (rd == 0 && rt == 0 && sa == 0)
            {
                line->length = (size_t)
                    snprintf(
                        line->text,
                        mpLINE_MAX,
                        "\x1B[94mNOP\x1B[0m");
            }
            else
            {
                line->length = (size_t)
                    snprintf(
                        line->text,
                        mpLINE_MAX,
                        "\x1B[94mADD "
                        "\x1B[95m$\x1B[96m%s\x1B[0m, "
                        "\x1B[95m$\x1B[96m%s\x1B[0m, "
                        "\x1B[96m%u\x1B[0m",
                            mpGetRegister(rd),
                            mpGetRegister(rs),
                            sa);
            }
        } break;

        /*
         * SLT rd, rs, rt
         */
        case mpSLT_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mSLT "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rd),
                        mpGetRegister(rs),
                        mpGetRegister(rt));
        } break;

        /*
         * ROTR rd, rt, sa
         * SRL rd, rt, sa
         */
        case mpSRL_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94m%s "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[96m%u\x1B[0m",
                        (rs & 0x1) ? "ROTR" : "SRL",
                        mpGetRegister(rd),
                        mpGetRegister(rs),
                        sa);
        } break;

        /*
         * SUB rd, rs, rt
         */
        case mpSUB_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mSUB "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rd),
                        mpGetRegister(rs),
                        mpGetRegister(rt));
        } break;

        /*
         * SYSCALL
         */
        case mpSYSCALL_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mSYSCALL \x1B[0m");
        } break;

        /*
         * XOR rd, rs, rt
         */
        case mpXOR_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mXOR "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m",
                        mpGetRegister(rd),
                        mpGetRegister(rs),
                        mpGetRegister(rt));
        } break;

        default :
            success = false;
    }

    return success;
}

/**
 *
 */
bool
mpTranspiler_Itype (
    uint32_t    inst,
    mpLine*     line
) {
    bool success = true;

    uint32_t op = (inst >> 26) & 0x003Fu;
    uint32_t rs = (inst >> 21) & 0x001Fu;
    uint32_t rt = (inst >> 16) & 0x001Fu;
    uint32_t im = (uint32_t)(int16_t)(inst & 0xFFFFu);

    switch (op)
    {
        /*
         * ADDI rt, rs, im
         */
        case mpADDI_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mADDI "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[96m%d\x1B[0m",
                        mpGetRegister(rt),
                        mpGetRegister(rs),
                        im);
        } break;

        /*
         * BEQ rs, rt, offset
         */
        case mpBEQ_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mBEQ "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[96m%d\x1B[0m",
                        mpGetRegister(rs),
                        mpGetRegister(rt),
                        im);
        } break;

        /*
         * BGTZ rs, offset
         */
        case mpBGTZ_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mBGTZ "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[96m%d\x1B[0m",
                        mpGetRegister(rs),
                        im);
        } break;

        /*
         * BLEZ rs, offset
         */
        case mpBLEZ_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mBLEZ "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[96m%d\x1B[0m",
                        mpGetRegister(rs),
                        im);
        } break;

        /*
         * BNE rs, rt, offset
         */
        case mpBNE_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mBNE "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[96m%d\x1B[0m",
                        mpGetRegister(rs),
                        mpGetRegister(rt),
                        im);
        } break;

        /*
         * LUI rt, immediate
         */
        case mpLUI_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mLUI "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[96m0x%X\x1B[0m # %u",
                        mpGetRegister(rt),
                        im & 0xFFFF, im);
        } break;

        /*
         * LW rt, offset(base)
         */
        case mpLW_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mLW "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m%d\x1B[0m("
                    "\x1B[96m%s\x1B[0m)",
                        mpGetRegister(rt),
                        im, mpGetRegister(rs));
        } break;

        /*
         * SW rt, offset(base)
         */
        case mpSW_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mSW "
                    "\x1B[95m$\x1B[96m%s\x1B[0m, "
                    "\x1B[95m%d\x1B[0m("
                    "\x1B[96m%s\x1B[0m)",
                        mpGetRegister(rt),
                        im, mpGetRegister(rs));
        } break;

        default :
            success = false;
    }

    return success;
}

/**
 *
 */
bool
mpTranspiler_Jtype (
    uint32_t    inst,
    mpLine*     line
) {
    bool success = true;

    uint32_t op = (inst >> 26) & 0x0000003Fu;
    uint32_t im = (inst)       & 0x03FFFFFFu;

    switch (op)
    {
        /*
         * J target
         */
        case mpJ_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mJ "
                    "\x1B[96m0x%X\x1B[0m # %u",
                        im, im);
        } break;

        /*
         * JAL target
         */
        case mpJAL_OPCODE :
        {
            line->length = (size_t)
                snprintf(
                    line->text,
                    mpLINE_MAX,
                    "\x1B[94mJ "
                    "\x1B[96m0x%X\x1B[0m # %u",
                        im, im);
        } break;

        default :
            success = false;
    }

    return success;
}
