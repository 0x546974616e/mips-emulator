#include "mpEmulator.h"

static mpChar* s_context = "Emulator\0";

/**
 *
 */
static
uint32_t
mpRotr (
    uint32_t value,
    uint32_t rotr
) {
    uint32_t i = 0;
    while (i++ < rotr)
        value = ((value & 0x1) << 31) | (value >> 1) ;
    return value;
}

/**
 *
 */
static
bool
mpRtype (
    uint32_t    inst,
    mpRegister* reg,
    mpError*    error
) {
    bool success = error->success = true;

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
            int32_t result =
                (int32_t)reg->GPR[rs] + (int32_t)reg->GPR[rt];

            reg->GPR[rd] = (uint32_t)result;
            reg->all.pc += 4u;
        } break;

        /*
         * AND rd, rs, rt
         */
        case mpAND_OPCODE :
        {
            reg->GPR[rd] = reg->GPR[rs] & reg->GPR[rt];
            reg->all.pc += 4u;
        } break;

        /*
         * DIV rs, rt
         */
        case mpDIV_OPCODE :
        {
            /*
             * Pas de condition bloquante sur le dénominateur.
             */
            if (reg->GPR[rt] != 0u)
            {
                int32_t div =
                    (int32_t)reg->GPR[rs] / (int32_t)reg->GPR[rt];

                int32_t mod =
                    (int32_t)reg->GPR[rs] % (int32_t)reg->GPR[rt];

                reg->all.lo = (uint32_t)div;
                reg->all.hi = (uint32_t)mod;
            }
            else
            {
                reg->all.lo = 0u;
                reg->all.hi = 0u;
            }
            reg->all.pc += 4u;
        } break;

        /*
         * JR rs
         */
        case mpJR_OPCODE :
        {
            uint32_t pc = reg->GPR[rs];

            if ((pc & 0x3) == 0)
            {
                reg->all.pc = pc;
            }
            else
            {
                error->success = false;
                error->type = mpErrorEMULATOR_JR_NOT_ALIGNED;
                error->code = 96;
            }

        } break;

        /*
         * MFHI rd
         */
        case mpMFHI_OPCODE :
        {
            reg->GPR[rd] = reg->all.hi;
            reg->all.pc += 4u;
        } break;

        /*
         * MFLO rd
         */
        case mpMFLO_OPCODE :
        {
            reg->GPR[rd] = reg->all.lo;
            reg->all.pc += 4u;
        } break;

        /*
         * MULT rs, rt
         */
        case mpMULT_OPCODE :
        {
            int64_t result =
                (int64_t)reg->GPR[rs] * (int64_t)reg->GPR[rt];

            reg->all.lo = (uint32_t)(result & 0xFFFFFFFF);
            reg->all.hi = (uint32_t)(result >> 32);
            reg->all.pc += 4u;
        } break;

        /*
         * OR rd, rs, rt
         */
        case mpOR_OPCODE :
        {
            reg->GPR[rd] = reg->GPR[rs] | reg->GPR[rt];
            reg->all.pc += 4u;
        } break;

        /*
         * NOP
         * SLL rd, rt, sa
         */
        case mpSLL_OPCODE :
        {
            reg->GPR[rd] = reg->GPR[rt] << sa;
            reg->all.pc += 4u;
        } break;

        /*
         * SLT rd, rs, rt
         */
        case mpSLT_OPCODE :
        {
            reg->GPR[rd] = reg->GPR[rs] < reg->GPR[rt];
            reg->all.pc += 4u;
        } break;

        /*
         * ROTR rd, rt, sa
         * SRL rd, rt, sa
         */
        case mpSRL_OPCODE :
        {
            reg->GPR[rd] =
                rs & 0x1
                    ? mpRotr(reg->GPR[rt], sa)
                    : reg->GPR[rt] >> sa;
            reg->all.pc += 4u;
        } break;

        /*
         * SUB rd, rs, rt
         */
        case mpSUB_OPCODE :
        {
            int32_t result =
                (int32_t)reg->GPR[rs] - (int32_t)reg->GPR[rt];

            reg->GPR[rd] = (uint32_t)result;
            reg->all.pc += 4u;
        } break;

        /*
         * SYSCALL
         */
        case mpSYSCALL_OPCODE :
        {
            /* TMP : code = (inst >> 6) & 0x000FFFFF*/
            reg->all.pc += 4u;
        } break;

        /*
         * XOR rd, rs, rt
         */
        case mpXOR_OPCODE :
        {
            reg->GPR[rd] = reg->GPR[rs] ^ reg->GPR[rt];
            reg->all.pc += 4u;
        } break;

        default :
            error->success = success = false;
            error->type = mpErrorEMULATOR_UNDEFINED;
            error->line = (mpString)inst;
            error->code = 99;
    }

    return success;
}

/**
 *
 */
static
bool
mpItype (
    uint32_t    inst,
    mpRegister* reg,
    mpMemory*   mem,
    mpError*    error
) {
    bool success = error->success = true;

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
            int32_t result =
                (int32_t)reg->GPR[rs] + (int32_t)im;

            reg->GPR[rt] = (uint32_t)result;
            reg->all.pc += 4u;
        } break;

        /*
         * BEQ rs, rt, offset
         */
        case mpBEQ_OPCODE :
        {
            reg->all.pc +=
                reg->GPR[rs] == reg->GPR[rt]
                    ? im << 2
                    : 4u;
        } break;

        /*
         * BGTZ rs, offset
         */
        case mpBGTZ_OPCODE :
        {
            reg->all.pc +=
                (int32_t)reg->GPR[rs] > 0
                    ? im << 2
                    : 4u;
        } break;

        /*
         * BLEZ rs, offset
         */
        case mpBLEZ_OPCODE :
        {
            reg->all.pc +=
                (int32_t)reg->GPR[rs] <= 0
                    ? im << 2
                    : 4u;
        } break;

        /*
         * BNE rs, rt, offset
         */
        case mpBNE_OPCODE :
        {
            reg->all.pc +=
                reg->GPR[rs] != reg->GPR[rt]
                    ? im << 2
                    : 4u;
        } break;

        /*
         * LUI rt, immediate
         */
        case mpLUI_OPCODE :
        {
            reg->GPR[rt] = im << 16;
            reg->all.pc += 4u;
        } break;

        /*
         * LW rt, offset(base)
         */
        case mpLW_OPCODE :
        {
            reg->GPR[rt] = mpLoadWord(mem, reg->GPR[rs] + im, error);

            if (error->success)
            {
                reg->all.pc += 4u;
            }
        } break;

        /*
         * SW rt, offset(base)
         */
        case mpSW_OPCODE :
        {
            mpStoreWord(mem, reg->GPR[rs] + im, reg->GPR[rt], error);

            if (error->success)
            {
                reg->all.pc += 4u;
            }
        } break;

        default :
            error->success = success = false;
            error->type = mpErrorEMULATOR_UNDEFINED;
            error->line = (mpString)inst;
            error->code = 98;
    }

    return success;
}

/**
 *
 */
static
bool
mpJtype (
    uint32_t    inst,
    mpRegister* reg,
    mpError*    error
) {
    bool success = error->success = true;

    uint32_t op = (inst >> 26) & 0x0000003Fu;
    uint32_t im = (inst)       & 0x03FFFFFFu;

    switch (op)
    {
        /*
         * J target
         */
        case mpJ_OPCODE :
        {
            reg->all.pc = (reg->all.pc & 0xF0000000) | (im << 2);
        } break;

        /*
         * JAL target
         */
        case mpJAL_OPCODE :
        {
            reg->all.ra =  reg->all.pc + 4u;
            reg->all.pc = (reg->all.pc & 0xF0000000) | (im << 2);
        } break;

        default :
            error->success = success = false;
            error->type = mpErrorEMULATOR_UNDEFINED;
            error->line = (mpString)inst;
            error->code = 97;
    }

    return success;
}

/**
 *
 */
static
void
mpEmulator_Init (
    mpMemory*   mem,
    mpRegister* reg
) {
    /*
     * On initiale tout à 0.
     */
    memset(reg, 0, sizeof(mpRegister));

    /*
     * Et certains registres aussi.
     */
    reg->all.pc   = 0u;
    reg->all.sp   = mem->maxAddress - 3u;
    reg->all.zero = 0u;
}

/**
 *
 */
static
bool
mpEmulator_Step (
    mpMemory*   mem,
    mpRegister* reg,
    mpError*    error
) {
    bool loop = (reg->all.pc < mem->minAddress);

    error->success = true;

    if (loop)
    {
        /*
         * On récupère l'instruction dans le IR (Instruction Register).
         */
        reg->all.ir = mpLoadWord(mem, reg->all.pc, error);

        /*
         *
         */
        if (error->success)
        {
            if ((reg->all.ir >> 26) == 0)
            {
                mpRtype(reg->all.ir, reg, error);
            }
            else if (!mpItype(reg->all.ir, reg, mem, error))
            {
                mpJtype(reg->all.ir, reg, error);
            }
        }

        /*
         * On remet le registre zéro à 0 au cas où.
         */
        reg->all.zero = 0u;
    }

    return loop;
}

/**
 *
 */
static
void
mpEmulator_Display (
    mpMemory*   mem,
    mpRegister* reg,
    bool        showInstruction
) {
    /*
     * On efface la console et on positionne le cursor
     * tout en haut à gauche.
     */
    printf("\x1B[2J\x1B[1;1H\n");

    /*
     * On affiche la mémoire et les registres.
     */
    mpPrintMemory(3u, 2u, reg->all.pc, "Text", mem);
    mpPrintMemory(3u, 8u, reg->all.sp, "Stack", mem);
    mpPrintRegister(3u, 14u, 65u, reg);

    /*
     *
     */
    if (showInstruction)
    {
        mpLine line;
        mpError error;

        uint32_t ir = mpLoadWord(mem, reg->all.pc, &error);

        if ((ir >> 26) == 0)
        {
            mpTranspiler_Rtype(ir, &line);
        }
        else if (!mpTranspiler_Itype(ir, &line))
        {
            if (!mpTranspiler_Jtype(ir, &line))
            {
                line.text[0] = '\\';
                line.text[1] = '\0';
            }
        }

        printf(
            "\x1B[24;3H"
            "\x1B[97mText"
            "\x1B[37m["
            "\x1B[96mpc\x1B[0m"
            "\x1B[37m] "
            "\x1B[97m=\x1B[0m %s",
            line.text);
    }
}

/**
 *
 */
static
void
mpEmulator_GetLine (
    mpLine*  line,
    mpError* error
) {
    error->success = true;

    /*
     * On récupère une ligne.
     */
    if (fgets(line->text, mpLINE_MAX, stdin) != NULL)
    {
        char* string = line->text;

        /*
         * On recherche le caractère de saut de ligne.
         */
        while (*string != '\0'
            && *string != '\n'
            && *string != '\r'
        ) {
            ++string;
        }

        /*
         * Si le caractère est présent alors on
         * marque la fin de la ligne.
         */
        if (*string == '\n' || *string == '\r')
        {
            *string = '\0';

            /*
             * On calcule la taille de la ligne.
             */
            line->length = (size_t)(string - line->text);

            if (line->length == 0u)
            {
                error->success = false;
                error->type = mpErrorEMULATOR_LINE_ZERO;
                error->code = 94;
                error->line = NULL;
            }
        }

        /*
         * Si le caractère n'est pas présent c'est que
         * la ligne est trop grande par rapport à la taille
         * fournie, et donc qu'il reste des caractères dans le
         * buffer d'entrée.
         */
        else
        {
            /*
             * On vide le buffer d'entrée.
             */
            int ch;
               do  ch = getchar();
            while (ch != EOF && ch != '\n');

            error->success = false;
            error->type = mpErrorLINE_OVERFLOW;
            error->line = line->text;
            error->col  = line->text + 39u;
            error->code = 93;
        }
    }
    else
    {
        error->success = false;
        error->type = mpErrorEMULATOR_FGETS_UNKNOWN;
        error->code = 92;
    }
}

/**
 *
 */
static
void
mpEmulator_PressEnter (
    bool stepMode
) {
    int flush;

    printf(stepMode ? "\x1B[24;49H" : "\n");

    /*
     * Puis un message.
     */
    printf("\x1B[37mPress [\x1B[97mENTER\x1B[37m] to continue...\x1B[0m");

    /*
     * On attend que l'utilisateur ait appuyé
     * sur la touche ENTREE.
     */
       do  flush = getchar();
    while (flush != EOF && flush != '\n');
}

/**
 *
 */
static
bool
mpEmulator_Exit (
    mpLine* line
) {
    bool success = false;

    /*
        1) aller jusqu'au prochain mot.
        2) Aller a la fin du mot.
        3) Regarder si c'est exit.
        4) Regarder si le reste est vide.
    */

    static mpCString s_exit = "exit\0";

    mpCString word = line->text;
    mpCString exit = s_exit;

    while (*word != '\0' && mpIsValidWhiteSpace(*word))
        ++word;

    while (*exit != '\0' && mpToLower(*word) == *exit)
    {
        ++word;
        ++exit;
    }

    if (*exit == '\0')
    {
        while (*word != '\0' && mpIsValidWhiteSpace(*word))
            ++word;

        if (*word == '\0')
        {
            success = true;
        }
    }
    return success;
}

/**
 *
 */
void
mpEmulator_Mode (
    mpMemory* mem,
    bool      stepMode
) {
    /*
     * On crée nos registres.
     */
    mpRegister reg;

    /*
     * Et une erreur.
     */
    mpError error;
    error.context = s_context;

    /*
     * On initialise.
     */
    mpEmulator_Init(mem, &reg);

    /*
     * On parcourt toutes les instructions tant qu'on
     * est pas arrivé à la dernière.
     */
    do
    {
        /*
         * Si le mode pas à pas est activé.
         */
        if (stepMode)
        {
            /*
             * On affiche la mémoire.
             */
            mpEmulator_Display(mem, &reg, true);

            mpEmulator_PressEnter(true);
        }
    }
    /*
     * On boucle tant qu'il y a des instructions
     * à exécuter.
     */
    while (mpEmulator_Step(mem, &reg, &error) && error.success);

    /*
     * On affiche l'état de la mémoire et des registres.
     */
    mpEmulator_Display(mem, &reg, !error.success);

    /*
     * On fait remonter une erreur s'il y en a une.
     */
    if (!error.success)
    {
        printf("\n\n");
        mpRaise(&error);
    }
    else
    {
        printf("\n\n\t\x1B[92mEmulation successfully completed.\x1B[0m\n");
    }
}

/**
 *
 */
void
mpEmulator_Interactive (void)
{
    /*
     * On crée nos registres et la mémoire.
     */
    mpMemory   mem;
    mpRegister reg;

    mpLine line;

    uint32_t pc;

    bool exit = false;

    /*
     *
     */
    mpError error;
    error.context = s_context;
    error.success = true;
    error.row = 1u;

    /*
     * On initialise.
     */
    mpMemory_Init(&mem);
    mpEmulator_Init(&mem, &reg);

    pc = reg.all.pc;

    do {
        reg.all.pc = pc;

            if (!error.success)
            {
                mpRaise(&error);
                mpEmulator_PressEnter(false);
            }

        do {
            mpEmulator_Display(&mem, &reg, false);

            printf("\n\n\x1B[92memul-mips \x1B[97m>\x1B[0m ");
            /*
             * On récupère une ligne.
             */
            mpEmulator_GetLine(&line, &error); printf("\n");
            if (error.success)
            {
                exit = mpEmulator_Exit(&line);

                if (!exit)
                {
                    /*
                     * On parse la ligne.
                     */
                    mpTranspiler_FetchToken(&line, &mem, &error);
                    if (!error.success)
                    {
                        mpEmulator_PressEnter(false);
                    }
                }
            }
            else
            {
                mpRaise(&error);
                mpEmulator_PressEnter(false);
            }
        }
        while (!error.success);

        pc = reg.all.pc + 4u;
    }
    while (!exit && mpEmulator_Step(&mem, &reg, &error));

    mpEmulator_Display(&mem, &reg, false);
    printf("\n");
}
