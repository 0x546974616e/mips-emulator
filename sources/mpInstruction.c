#include "mpInstruction.h"

/**
 * Liste des instructions.
 *
 * @pre
 *     /!\ Doit impérativement être triée /!\
 *         par ordre alphabétique.
 */
static
mpInstruction const s_instruction[mpINSTRUCTION_MAX] =
{
    {
        mpADD_OPCODE,
        "ADD",
        mpRtype_ORRR,
        "rd, rs, rt",
        "GPR[rd] = GPR[rs] + GPR[rt]"
    },
    {
        mpADDI_OPCODE,
        "ADDI",
        mpItype_ORRI,
        "rt, rs, immediate",
        "GPR[rt] = GPR[rs] + immediate"
    },
    {
        mpAND_OPCODE,
        "AND",
        mpRtype_ORRR,
        "rd, rs, rt",
        "GPR[rd] = GPR[rs] & GPR[rt]"
    },
    {
        mpBEQ_OPCODE,
        "BEQ",
        mpItype_ORRI,
        "rs, rt, offset",
        "if GPR[rs] == GPR[rt] then branch"
    },
    {
        mpBGTZ_OPCODE,
        "BGTZ",
        mpItype_ORI,
        "rs, offset",
        "if GPR[rs] > 0 then branch"
    },
    {
        mpBLEZ_OPCODE,
        "BLEZ",
        mpItype_ORI,
        "rs, offset",
        "if GPR[rs] <= 0 then branch"
    },
    {
        mpBNE_OPCODE,
        "BNE",
        mpItype_ORRI,
        "rs, rt, offset",
        "if GPR[rs] != GPR[rt] then branch"
    },
    {
        mpDIV_OPCODE,
        "DIV",
        mpRtype_ORR,
        "rs, rt",
        "LO = GPR[rs] / GPR[rt]; HI = GPR[rs] % GPR[rt]"
    },
    {
        mpJ_OPCODE,
        "J",
        mpJtype_OI,
        "target",
        "PC = target"
    },
    {
        mpJAL_OPCODE,
        "JAL",
        mpJtype_OI,
        "target",
        "GPR[31] = PC + 8; PC = target"
    },
    {
        mpJR_OPCODE,
        "JR",
        mpRtype_OR,
        "rs",
        "PC = GPR[rs]"
    },
    {
        mpLUI_OPCODE,
        "LUI",
        mpItype_ORI,
        "rt, immediate",
        "GPR[rt] = immediate << 16"
    },
    {
        mpLW_OPCODE,
        "LW",
        mpItype_ORIBRB,
        "rt, offset(base)",
        "GPR[rt] = memory[GPR[base] + offset]"
    },
    {
        mpMFHI_OPCODE,
        "MFHI",
        mpRtype_OR,
        "rd",
        "GPR[rd] = HI"
    },
    {
        mpMFLO_OPCODE,
        "MFLO",
        mpRtype_OR,
        "rd",
        "GPR[rd] = LO"
    },
    {
        mpMULT_OPCODE,
        "MULT",
        mpRtype_ORR,
        "rs, rt",
        "(HI, LO) = GPR[rs] * GPR[rt]"
    },
    {
        mpNOP_OPCODE,
        "NOP",
        mpRtype_O,
        "",
        "SLL $0, $0, 0"
    },
    {
        mpOR_OPCODE,
        "OR",
        mpRtype_ORRR,
        "rd, rs, rt",
        "GPR[rd] = GPR[rs] | GPR[rt]"
    },
    {
        mpROTR_OPCODE,
        "ROTR",
        mpRtype_ORRI,
        "rd, rt, sa",
        "GPR[rd] = GPR[rt] <-> (right)sa"
    },
    {
        mpSLL_OPCODE,
        "SLL",
        mpRtype_ORRI,
        "rd, rt, sa",
        "GPR[rd] = GPR[rt] << sa"
    },
    {
        mpSLT_OPCODE,
        "SLT",
        mpRtype_ORRR,
        "rd, rs, rt",
        "GPR[rd] = (GPR[rs] < GPR[rt] ? 1 : 0)"
    },
    {
        mpSRL_OPCODE,
        "SRL",
        mpRtype_ORRI,
        "rd, rt, sa",
        "GPR[rd] = GPR[rt] >> sa"
    },
    {
        mpSUB_OPCODE,
        "SUB",
        mpRtype_ORRR,
        "rd, rs, rt",
        "GPR[rd] = GPR[rs] - GPR[rt]"
    },
    {
        mpSW_OPCODE,
        "SW",
        mpItype_ORIBRB,
        "rt, offset(base)",
        "memory[GPR[base] + offset] = GPR[rt]"
    },
    {
        mpSYSCALL_OPCODE,
        "SYSCALL",
        mpRtype_O,
        "",
        "Causes a System Call exception."
    },
    {
        mpXOR_OPCODE,
        "XOR",
        mpRtype_ORRR,
        "rd, rs, rt",
        "GPR[rd] = GPR[rs] ^ GPR[rt]"
    }
};

/**
 *
 */
static
mpTag const s_ORIBRB[] =
{
    mpTagINSTRUCTION,
    mpTagREGISTER,
    mpTagCOMMA,
    mpTagINTEGER,
    mpTagLEFT_BRACKET,
    mpTagREGISTER,
    mpTagRIGHT_BRACKET,
    mpTagEND
};

/**
 *
 */
static
mpTag const s_ORRR[] =
{
    mpTagINSTRUCTION,
    mpTagREGISTER,
    mpTagCOMMA,
    mpTagREGISTER,
    mpTagCOMMA,
    mpTagREGISTER,
    mpTagEND
};

/**
 *
 */
static
mpTag const s_ORRI[] =
{
    mpTagINSTRUCTION,
    mpTagREGISTER,
    mpTagCOMMA,
    mpTagREGISTER,
    mpTagCOMMA,
    mpTagINTEGER,
    mpTagEND
};

/**
 *
 */
static
mpTag const s_ORR[] =
{
    mpTagINSTRUCTION,
    mpTagREGISTER,
    mpTagCOMMA,
    mpTagREGISTER,
    mpTagEND
};

/**
 *
 */
static
mpTag const s_ORI[] =
{
    mpTagINSTRUCTION,
    mpTagREGISTER,
    mpTagCOMMA,
    mpTagINTEGER,
    mpTagEND
};

/**
 *
 */
static
mpTag const s_OR[] =
{
    mpTagINSTRUCTION,
    mpTagREGISTER,
    mpTagEND
};

/**
 *
 */
static
mpTag const s_OI[] =
{
    mpTagINSTRUCTION,
    mpTagINTEGER,
    mpTagEND
};

/**
 *
 */
static
mpTag const s_O[] =
{
    mpTagINSTRUCTION,
    mpTagEND
};

/**
 * Vérifie si les tags de la liste de token
 * est égale à la liste de tags.
 *
 */
static
void
mpMatchTag (
    mpVector_mpToken* vector,
    mpTag const*      tag,
    mpError*          error
) {
    error->success = (vector->size != 0u);

    if (error->success)
    {
        /*
         * On récupère le premier et le dernier token.
         */
        mpToken* token = vector->data;
        mpToken* end   = token + vector->size;

        /*
         * On parcourt la liste de token tant que :
         *     - On est pas arrivé au dernier token.
         *     - On est pas arrivé au dernier tag.
         *     - Les tags sont égaux.
         */
        while (     token  <  end
           &&        *tag !=  mpTagEND
           &&  token->tag == *tag
        ) {
            ++token;
            ++tag;
        }

        /*
         * On retourne true si :
         *     - On est arrivé au dernier token.
         *     - On est arrivé au dernier tag.
         * Sinon c'est que :
         *     - Soit on est pas arrivé au dernier token
         *       parce qu'ils matcher pas avec les tags.
         *     - Soit il y avait plus de tokens que de tags.
         */
        error->success =
                token == end
            && *tag   == mpTagEND;

        if (!error->success)
        {
            if (token == end)
            {
                error->type = mpErrorMISSING_OPERAND;
                error->code = 100;
                error->col  = vector->data[0].word;
            }
            else
            {
                error->type = mpErrorINVALID_FORMAT;
                error->code = 101;
                error->col  = token->word;
            }
        }
    }
    else
    {
        error->success = false;
        error->type = mpErrorUNKNOWN;
        error->code = 102;
        error->col  = NULL;
    }
}

/**
 *
 */
static
void
mpWriteRtype (
    mpMemory* memory,
    int op,
    int sa,
    int rd,
    int rs,
    int rt,
    mpError* error
) {
    uint32_t hex = 0;

    hex |= (uint32_t)((op & 0x3F));
    hex |= (uint32_t)((sa & 0x1F) << 6);
    hex |= (uint32_t)((rd & 0x1F) << 11);
    hex |= (uint32_t)((rt & 0x1F) << 16);
    hex |= (uint32_t)((rs & 0x1F) << 21);

    /* printf("\t\x1B[97m%08X\x1B[0m\n", hex); */
    /* fprintf(file, "%08X\n", hex); */
    mpWriteInstruction(memory, hex, error);
}

/**
 *
 */
static
void
mpWriteItype (
    mpMemory* memory,
    int op,
    int rs,
    int rt,
    int im,
    mpError* error
) {
    uint32_t hex = 0;

    hex |= (uint32_t)((im & 0xFFFF));
    hex |= (uint32_t)((rt & 0x1F) << 16);
    hex |= (uint32_t)((rs & 0x1F) << 21);
    hex |= (uint32_t)((op & 0x3F) << 26);

    /* printf("\t\x1B[97m%08X\x1B[0m\n", hex); */
    /* fprintf(file, "%08X\n", hex); */
    mpWriteInstruction(memory, hex, error);
}

/**
 *
 */
static
void
mpWriteJtype (
    mpMemory* memory,
    int op,
    int im,
    mpError* error
) {
    uint32_t hex = 0;

    hex |= (uint32_t)((im & 0x3FFFFFF));
    hex |= (uint32_t)((op & 0x3F) << 26);

    /* printf("\t\x1B[97m%08X\x1B[0m\n", hex); */
    /* fprintf(file, "%08X\n", hex); */
    mpWriteInstruction(memory, hex, error);
}

/**
 *
 */
static
void
mpWarningINTEGER_OVERFLOW (
    size_t bits,
    mpToken* token,
    mpError* error
) {
    /*
     * On calcule la valeur max.
     */
    bool inside = true;

    int integer = token->value.integer;

    if (integer >= 0)
    {
        int max = 1 << (bits);
        inside = (integer <= max);
    }
    else
    {
        int max = -(1 << (bits - 1));
        inside = (integer >= max);
    }

    if (!inside)
    {
        error->success = false;
        error->type = mpErrorINTEGER_OVERFLOW;
        error->code = (int)bits;
        error->col  = token->word;
    }
}

/**
 *
 */
void
mpRtype_ORRR (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_ORRR, error);

    if (error->success)
    {
        mpWriteRtype(memory,
            /* op */ inst->opcode,
            /* sa */ 0x0,
            /* rd */ tokens->data[1].value.integer,
            /* rs */ tokens->data[3].value.integer,
            /* rt */ tokens->data[5].value.integer,
                     error);
    }
}

/**
 *
 */
void
mpRtype_ORRI (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_ORRI, error);

    if (error->success)
    {
        int rs = 0;

        if ((inst->opcode == mpROTR_OPCODE)
         && (*tokens->data[0].word | 0x20) == 'r')
            { rs = 0x1; }

        mpWarningINTEGER_OVERFLOW(5, tokens->data + 5, error);

        if (error->success)
        {
            mpWriteRtype(memory,
                /* op */ inst->opcode,
                /* sa */ tokens->data[5].value.integer,
                /* rd */ tokens->data[1].value.integer,
                /* rs */ rs,
                /* rt */ tokens->data[3].value.integer,
                         error);
        }
    }
}

/**
 *
 */
void
mpRtype_ORR (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_ORR, error);

    if (error->success)
    {
        mpWriteRtype(memory,
            /* op */ inst->opcode,
            /* sa */ 0x0,
            /* rd */ 0x0,
            /* rs */ tokens->data[1].value.integer,
            /* rt */ tokens->data[3].value.integer,
                     error);
    }
}

/**
 *
 */
void
mpRtype_OR (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_OR, error);

    if (error->success)
    {
        int rs = tokens->data[1].value.integer;
        int rd = 0x0;

        if (inst->opcode == mpMFHI_OPCODE
         || inst->opcode == mpMFLO_OPCODE)
            { rd = rs; rs = 0x0; }

        mpWriteRtype(memory,
            /* op */ inst->opcode,
            /* sa */ 0x0,
            /* rd */ rd,
            /* rs */ rs,
            /* rt */ 0x0,
                     error);
    }
}

/**
 *
 */
void
mpRtype_O (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_O, error);

    if (error->success)
    {
        mpWriteRtype(memory,
            /* op */ inst->opcode,
            /* sa */ 0x0,
            /* rd */ 0x0,
            /* rs */ 0x0,
            /* rt */ 0x0,
                     error);
    }
}

/**
 *
 */
void
mpItype_ORIBRB (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_ORIBRB, error);

    if (error->success)
    {
        mpWarningINTEGER_OVERFLOW(16, tokens->data + 3, error);

        if (error->success)
        {
            mpWriteItype(memory,
                /* op */ inst->opcode,
                /* rs */ tokens->data[5].value.integer,
                /* rt */ tokens->data[1].value.integer,
                /* im */ tokens->data[3].value.integer,
                         error);
        }
    }
}

/**
 *
 */
void
mpItype_ORRI (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_ORRI, error);

    if (error->success)
    {
        int a, b;

        if (inst->opcode == mpADDI_OPCODE)
            { a = 3; b = 1; }
        else
            { a = 1; b = 3; }

        mpWarningINTEGER_OVERFLOW(16, tokens->data + 5, error);

        if (error->success)
        {
            mpWriteItype(memory,
                /* op */ inst->opcode,
                /* rs */ tokens->data[a].value.integer,
                /* rt */ tokens->data[b].value.integer,
                /* im */ tokens->data[5].value.integer,
                         error);
        }
    }
}

/**
 *
 */
void
mpItype_ORI (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_ORI, error);

    if (error->success)
    {
        int rs = tokens->data[1].value.integer;
        int rt = 0x0;

        if (inst->opcode == mpLUI_OPCODE)
            { rt = rs; rs = 0x0; }

        mpWarningINTEGER_OVERFLOW(16, tokens->data + 3, error);

        if (error->success)
        {
            mpWriteItype(memory,
                /* op */ inst->opcode,
                /* rs */ rs,
                /* rt */ rt,
                /* im */ tokens->data[3].value.integer,
                         error);
        }
    }
}

/**
 *
 */
void
mpJtype_OI (
    mpInstruction*    inst,
    mpVector_mpToken* tokens,
    mpMemory*         memory,
    mpError*          error
) {
    mpMatchTag(tokens, s_OI, error);

    if (error->success)
    {
        mpWarningINTEGER_OVERFLOW(26, tokens->data + 1, error);

        if (error->success)
        {
            mpWriteJtype(memory,
                /* op */ inst->opcode,
                /* im */ tokens->data[1].value.integer,
                         error);
        }
    }
}

/**
 * Cherche l'instruction associée à son nom.
 *
 * @param[in] mnemonic
 * @return    l'instruction, NULL sinon.
 *
 * @pre
 *     - @mnemonic != NULL;
 *     - @s_instruction doit contenir au moins
 *       un élément.
 */
mpInstruction const*
mpGetInstruction (
    mpCString mnemonic
) {
    /*
     * On réalise une recherche dichtomique.
     */
    int L = 0;
    int R = mpINSTRUCTION_MAX - 1;
    int m, cmp;

    do
    {
        m = (L + R) / 2;

        cmp = mpStrCiCmp(
            s_instruction[m].mnemonic,
            mnemonic);

        if (cmp < 0)
        {
            L = m + 1;
        }
        else if (cmp > 0)
        {
            R = m - 1;
        }
    }
    while (cmp != 0 && L <= R);

    /*
     * Si une instruction a été trouvée
     * alors on la retourne sinon NULL.
     */
    return
        cmp == 0
            ? &s_instruction[m]
            : NULL;
}
