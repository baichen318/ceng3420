/***************************************************************/
/*                                                             */
/*                  RISCV-LC Assembler                         */
/*                                                             */
/*                     CEG3420 Lab2                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/


#include "asm.h"


int reg_to_num(char *reg, int line_no) {
    if (is_reg(reg) == MISMATCH)
        handle_err(7, line_no);
    size_t len_of_regs_code = sizeof(arch_regs_encode) / sizeof(arch_regs_encode[0]);
    for (int i = 0; i < len_of_regs_code; i++) {
        if (cmp_str(arch_regs[i], reg))
            return arch_regs_encode[i];
    }
}


int is_imm_in_range(int val, int width) {
    /*
     * given a bit width, check whether it can hold the signed value
     */
    // return (val <= ((1 << width) - 1) && val >= -(1 << width));
    return val <= ((1 << (width - 1)) - 1) && (val >= (-(1 << (width - 1))));
}


int handle_label_or_imm(int line_no, char *val, struct_label_table *label_table, int number_of_labels) {
    int _val, ret;
    if ((ret = is_imm(val)) == MISMATCH) {
        // label
        return label_to_num(line_no, val, 20, label_table, number_of_labels);
    } else {
        // immediate value
        _val = (int)strtol(val, NULL, (ret == HEX) ? 16 : 10);
        if (is_imm_in_range(_val, 20))
            return _val;
        else
            handle_err(5, line_no);
    }
}


int validate_imm(char *imm, int width, int line_no) {
    /*
     * do the base conversion or given a bit width,
     * validate whether it can hold the immediate value
     */
    int val, ret;
    if ((ret = is_imm(imm)) == MISMATCH)
        handle_err(7, line_no);
    else
        val = (int)strtol(imm, NULL, (ret == HEX) ? 16 : 10);
    if (width != -1) {
        if (is_imm_in_range(val, width))
            return val;
        else
            handle_err(3, line_no);
    } else
        return val;
}


int lower5bit(char *imm, int line_no) {
    int val = 0, ret;
    if ((ret = is_imm(imm)) == MISMATCH)
        handle_err(7, line_no);
    else
        val = ((int)strtol(imm, NULL, (ret == HEX) ? 16 : 10) & 0x1F);
    if (is_imm_in_range(val, 5))
        return val;
    else
        handle_err(5, line_no);
}


int label_to_num(
    int line_no,
    char *label,
    int width,
    struct_label_table *label_table,
    int number_of_labels
) {
    if (is_label(label) != VALID_LABEL)
        handle_err(10, line_no);
    for (int i = 0; i < number_of_labels; i++) {
        if(cmp_str(label_table[i].label, label)) {
            int address = label_table[i].address;
            if (!is_imm_in_range(address, width))
                handle_err(8, line_no);
            return address;
        }
    }
    handle_err(1, line_no);
}


struct_regs_indirect_addr* parse_regs_indirect_addr(char *arg2, int line_no) {
    struct_regs_indirect_addr* ret;
    char *reg, *imm;
    char temp[MAX_LINE_LENGTH + 1];
    int i = 0;
    if ((ret = (struct_regs_indirect_addr *)malloc(sizeof(ret))) == NULL)
        handle_err(11, line_no);
    while (arg2[i] != '\0') {
        temp[i] = arg2[i];
        i++;
    }
    temp[i] = '\0';
    imm = strtok(temp, "( ");
    ret->imm = validate_imm(imm, 12, line_no);
    reg = strtok(NULL, ")");
    copy_str(ret->reg, reg);
    return ret;
}


void fill_to_binary(FILE* output_file, char *arg1, int line_no) {
    fprintf(output_file, "0x%08x\n", validate_imm(arg1, 31, line_no));
}


void halt_to_binary(FILE *output_file) {
    /*
     * HALT is mapped to the fixed binary
     */
    fprintf(output_file, "0x%08x\n", 0x0000707f);
}


void la_to_binary(
    FILE *output_file,
    int line_no,
    char *opcode,
    char *arg1,
    char *arg2,
    char *arg3,
    struct_label_table *label_table,
    int number_of_labels,
    int addr
) {
    /*
     * LA is translated to lui and addi.
    */
    char temp[MAX_LINE_LENGTH + 1];
    fprintf(
        output_file,
        "0x%08x\n",
        inst_to_binary(
            line_no, "lui", arg1, arg2, arg3, label_table, number_of_labels, addr
        )
     );
    sprintf(
        temp,
        "%d",
        (handle_label_or_imm(
            line_no,
            arg2,
            label_table,
            number_of_labels
        )) & 0xFFF
    );
    fprintf(
        output_file,
        "0x%08x\n",
        inst_to_binary(
            line_no, "addi", arg1, arg1, temp, label_table, number_of_labels, addr
        )
    );
}


int inst_to_binary(
    int line_no,
    char *opcode,
    char *arg1,
    char *arg2,
    char *arg3,
    struct_label_table *label_table,
    int number_of_labels,
    int addr
) {
    /* save the translated machine binary */
    int binary = 0;

    // Integer Register-Immediate Instructions
    if (is_opcode(opcode) == ADDI) {
        binary = (0x04 << 2) + 0x03;
        binary += (reg_to_num(arg1, line_no) << 7);
        binary += (reg_to_num(arg2, line_no) << 15);
        binary += (MASK11_0(validate_imm(arg3, 12, line_no)) << 20);
    } else if (is_opcode(opcode) == SLLI) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: SLLI instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == XORI) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: XORI instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == SRLI) {
        /*
         * Lab2-1 assignment
         * tip: you may need the function `lower5bit`
         */
        warn("Lab2-1 assignment: SRLI instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == SRAI) {
        /*
         * Lab2-1 assignment
         * tip: you may need the function `lower5bit`
         */
        warn("Lab2-1 assignment: SRAI instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == ORI) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: ORI instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == ANDI) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: ADDI instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == LUI) {
        binary = (0x0D << 2) + 0x03;
        binary += (reg_to_num(arg1, line_no) << 7);
        binary += handle_label_or_imm(
                line_no,
                arg2,
                label_table,
                number_of_labels
            ) & 0xFFFFF000;
    }

    // Integer Register-Register Operations
    else if (is_opcode(opcode) == ADD) {
        binary = (0x0C << 2) + 0x03;
        binary += (reg_to_num(arg1, line_no) << 7);
        binary += (reg_to_num(arg2, line_no) << 15);
        binary += (reg_to_num(arg3, line_no) << 20);
        binary += (0x0 << 25);
    } else if (is_opcode(opcode) == SUB) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: SUB instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == SLL) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: SLL instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == XOR) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: XOR instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == SRL) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: SRL instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == SRA) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: SRA instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == OR) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: OR instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == AND) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: AND instruction\n");
        exit(EXIT_FAILURE);
    }


    // Unconditional Jumps
    else if (is_opcode(opcode) == JALR) {
        /*
         * Lab2-1 assignment
         * tip: you may need the function `parse_regs_indirect_addr`
         * e.g., parse_regs_indirect_addr(arg2, line_no)
         */
        warn("Lab2-1 assignment: JALR instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == JAL) {
        /*
         * Lab2-1 assignment
         * tip: you may need the function `handle_label_or_imm`
         * e.g., handle_label_or_imm(arg2, label_table, cmd_no, line_no)
         */
        warn("Lab2-1 assignment: JAL instruction\n");
        exit(EXIT_FAILURE);
    }

    // Conditional Branches
    else if (is_opcode(opcode) == BEQ) {
        binary = (0x18 << 2) + 0x03;
        binary += (reg_to_num(arg1, line_no) << 15);
        binary += (reg_to_num(arg2, line_no) << 20);
        int val = label_to_num(
            line_no, arg3, 12, label_table, number_of_labels
        ), offset = val - addr;
        // imm[11]
        binary += ((offset & 0x800) >> 4);
        // imm[4:1]
        binary += ((offset & 0x1E) << 7);
        // imm[10:5]
        binary += ((offset & 0x7E0) << 20);
        // imm[12]
        binary += ((offset & 0x1000) << 19);
    } else if (is_opcode(opcode) == BNE) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: BNE instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == BLT) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: BLT instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == BGE) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: BGE instruction\n");
        exit(EXIT_FAILURE);
    }

    // Load and Store Instructions
    else if (is_opcode(opcode) == LB) {
        binary = 0x03;
        binary += (reg_to_num(arg1, line_no) << 7);
        struct_regs_indirect_addr* ret = parse_regs_indirect_addr(arg2, line_no);
        binary += (reg_to_num(ret->reg, line_no) << 15);
        binary += (MASK11_0(ret->imm) << 20);
    } else if (is_opcode(opcode) == LH) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: LH instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == LW) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: LW instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == SB) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: SB instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == SH) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: SH instruction\n");
        exit(EXIT_FAILURE);
    } else if (is_opcode(opcode) == SW) {
        /* Lab2-1 assignment */
        warn("Lab2-1 assignment: SW instruction\n");
        exit(EXIT_FAILURE);
    }
    return binary;
}


int is_label(char *ptr) {
    if (*ptr == '\0')
        return MISMATCH;

    char *label = ptr;
    if (
        is_opcode(label) == MISMATCH && \
        is_reg(ptr) == MISMATCH && \
        is_imm(ptr) == MISMATCH && \
        is_regs_indirect_addr(ptr) == MISMATCH
    ) {
        if (isdigit(*label))
            return INVALID_LABEL;
        else {
            label++;
            while (label != NULL && *label != '\0') {
                /* all characters have been converted to lower-case */
                if (isalnum(*label) || *label == '_') {
                    label++;
                    continue;
                }
                else
                    return INVALID_LABEL;
            }
        }
        return VALID_LABEL;
    } else
        return INVALID_LABEL;
}


int is_opcode(char *ptr) {
    if (*ptr == '\0')
        return MISMATCH;

    size_t len_of_opcode_table = sizeof(opcode_table) / sizeof(opcode_table[0]);
    for (int i = 0; i < (int)len_of_opcode_table; i++) {
        if (cmp_str(opcode_table[i], ptr)) {
            /* i is equal to the enumeration of opcodes */
            return i;
        }
    }
    /* if the opcode is invalid */
    return MISMATCH;
}


int is_reg(char *ptr) {
    if (*ptr == '\0')
        return MISMATCH;

    size_t len_of_arch_regs = sizeof(arch_regs) / sizeof(arch_regs[0]);
    for (int i = 0; i < (int)len_of_arch_regs; i++) {
        if (cmp_str(arch_regs[i], ptr)) {
            /* it is an architecture register */
            return ARCH_REGS;
        }
    }
    return MISMATCH;
}


int is_imm(char *ptr) {
    /* -0x7, 0x7, -7, 7 */
    if (*ptr == '\0')
        return MISMATCH;

    int i = 0, l = strlen(ptr);
    bool hex = false;
    if (l > 3 && ptr[0] == '-' && ptr[1] == '0' && (tolower(ptr[2]) == 'x')) {
        // negative hexadecimal
        i += 3;
        hex = true;
    } else if (l > 2 && ptr[0] == '0' && (tolower(ptr[1]) == 'x')) {
        // positive hexadecimal
        i += 2;
        hex = true;
    } else if (l > 1 && ptr[0] == '-') {
        // negative number
        i += 1;
    }
    while (i < l && isxdigit(ptr[i]))
        i++;
    if (i == l) {
        if (hex)
            return HEX;
        else
            return DEC;
    } else
        return MISMATCH;
}


int is_regs_indirect_addr(char *ptr) {
    if (*ptr == '\0')
        return MISMATCH;

    char *imm = NULL, *reg = NULL, *_ptr = NULL;
    int i = 0;
    if (!(imm = (char *)malloc((strlen(ptr) + 1) * sizeof(char))))
        error("malloc is failed in 'is_regs_indirect_addr'.\n");
    if (!(reg = (char *)malloc((strlen(ptr) + 1) * sizeof(char))))
        error("malloc is failed in 'is_regs_indirect_addr'.\n");
    memset(imm, '\0', strlen(ptr));
    memset(reg, '\0', strlen(ptr));
    _ptr = ptr;
    /* parse imm. */
    while (*_ptr != '\0' && *_ptr != '(') {
        if (*_ptr != ' ' && *_ptr != '\t')
            imm[i++] = *_ptr;
        _ptr++;
    }
    imm[i] = '\0';
    if (*_ptr == '\0') {
        free(imm);
        free(reg);
        return MISMATCH;
    }
    _ptr++;
    i = 0;
    /* parse reg. */
    while (*_ptr != '\0' && *_ptr != ')') {
        if (*_ptr != ' ' && *_ptr != '\t')
            reg[i++] = *_ptr;
        _ptr++;
    }
    reg[i] = '\0';
    if (is_imm(imm) != MISMATCH && is_reg(reg) != MISMATCH) {
        free(imm);
        free(reg);
        return REGS_INDIRECT_ADDR;
    }
    else {
        free(imm);
        free(reg);
        return MISMATCH;
    }
}


int is_arg(char *ptr) {
    if (*ptr == '\0')
        return MISMATCH;

    if (is_label(ptr) == VALID_LABEL)
        return VALID_LABEL;
    if (is_reg(ptr) != MISMATCH)
        return ARCH_REGS;
    else if (is_imm(ptr) != MISMATCH)
        return IMM;
    else if (is_regs_indirect_addr(ptr) != MISMATCH)
        return REGS_INDIRECT_ADDR;
    else
        return REGS_INDIRECT_ADDR;
}


void calc_la(FILE *input_file) {
    /*
     * each pseudo-instruction LA is translated to Lui and addi,
     * i.e., two instructions, so we should count them first to
     * correctly know the offset of other instructions.
     */
    char line[MAX_LINE_LENGTH + 1];
    char *ptr;
    int line_cnt = 0, k = 0;

    /* re-read the input file */
    rewind(input_file);
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        for (int i = 0; i < strlen(line); i++)
            line[i] = tolower(line[i]);
        ptr = line;
        while (*ptr != '#' && *ptr != '\0' && *ptr != '\n' && *ptr != '\r')
            ptr++;
        *ptr = '\0';
        if (!(ptr = strtok(line, "\t\n ,")))
            continue;
        if (is_opcode(ptr) == LA)
            la_inst[line_cnt] = ++k;
        else {
            if (!(ptr = strtok(NULL, "\t\n ,")))
                continue;
            if (is_opcode(ptr) == LA)
                la_inst[line_cnt] = ++k;
            else
                la_inst[line_cnt] = k;
        }
        line_cnt++;
    }
    la_cnt = k;
}


int parse_inst(
    FILE *input_file,
    char **label,
    char **opcode,
    char **arg1,
    char **arg2,
    char **arg3
) {
    char *ptr;
    int i;

    char *line, *_line;
    if (!(line = malloc(sizeof(char) * (MAX_LINE_LENGTH + 1))))
        error("malloc is failed in 'parse_inst'.\n");

    if (!fgets(line, MAX_LINE_LENGTH, input_file))
        return(END_OF_FILE);

    /* convert the entire line to lowercase characters */
    for (i = 0; i < strlen(line); i++)
        line[i] = tolower(line[i]);

    /* make the pointers point to the address of the end, i.e., '\0' */
    *label = *opcode = *arg1 = *arg2 = *arg3 = line + strlen(line);

    /* ignore the comments */
    ptr = strstr(line, "#");
    if (ptr)
        *ptr = '\0';
    ptr = line;
    while (*ptr == ' ' || *ptr == '\n' || *ptr == '\r')
        ptr++;

    if (!strlen(ptr))
        return(COMMENT_LINE);

    if (!(_line = (char *)malloc((strlen(line) + 1) * sizeof(char))))
        error("malloc is failed in 'parse_inst'.\n");
    copy_str(_line, line);
    /* skip blank spaces at the beginning of the line */
    if (!(ptr = strtok(_line, "\t\n ,")))
        return(DONE);
    i = 1;
    while (strtok(NULL, delimiters))
        i++;
    switch(i) {
        case 1:
                /*
                 * 1. halt
                 */
                ptr = strtok(line, "\t\n ,");
                assert(
                    cmp_str(opcode_table[30], ptr),
                    "halt is expected in the input asm source code.\n"
                );
                *opcode = ptr;
                return TYPE1;
        case 2:
                /*
                 * 1. a label with a halt, e.g., End HALT
                 */
                ptr = strtok(line, delimiters);
                assert(
                    is_label(ptr) == VALID_LABEL,
                    "a valid label is expected in the input asm source code.\n"
                );
                *label = ptr;
                ptr = strtok(NULL, delimiters);
                assert(
                    cmp_str(opcode_table[30], ptr),
                    "halt is expected in the input asm source code.\n"
                );
                *opcode = ptr;
                return TYPE2;
        case 3:
                /*
                 * 1. label opcode argument, e.g., a .fill 10
                 * 2. opcode, arg1, arg2, e.g., li t0, 1
                 */
                ptr = strtok(line, delimiters);
                if (is_label(ptr) == VALID_LABEL) {
                    *label = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_opcode(ptr) != MISMATCH,
                        "a valid label is expected in the input asm source code.\n"
                    );
                    *opcode = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_arg(ptr) != MISMATCH,
                        "opcode is expected in the input asm source code.\n"
                    );
                    *arg1 = ptr;
                    return TYPE3;
                } else {
                    assert(
                        is_opcode(ptr) != MISMATCH,
                        "opcode is expected in the input asm source code.\n"
                    );
                    *opcode = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_arg(ptr) != MISMATCH,
                        "argument is expected in the input asm source code.\n"
                    );
                    *arg1 = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_arg(ptr) != MISMATCH,
                        "argument is expected in the input asm source code.\n"
                    );
                    *arg2 = ptr;
                    return TYPE4;
                }
        case 4:
                /*
                 * 1. label opcode, arg1, arg2, e.g., label li t0, 1
                 * 2. opcode, arg1, arg2, arg3, e.g., addi t0, t0, 1
                 */
                ptr = strtok(line, delimiters);
                if (is_label(ptr) == VALID_LABEL) {
                    *label = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_opcode(ptr) != MISMATCH,
                        "a valid label is expected in the input asm source code.\n"
                    );
                    *opcode = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_arg(ptr) != MISMATCH,
                        "opcode is expected in the input asm source code.\n"
                    );
                    *arg1 = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_arg(ptr) != MISMATCH,
                        "argument is expected in the input asm source code.\n"
                    );
                    *arg2 = ptr;
                    return TYPE5;
                } else {
                    assert(
                        is_opcode(ptr) != MISMATCH,
                        "opcode is expected in the input asm source code.\n"
                    );
                    *opcode = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_arg(ptr) != MISMATCH,
                        "opcode is expected in the input asm source code.\n"
                    );
                    *arg1 = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_arg(ptr) != MISMATCH,
                        "argument is expected in the input asm source code.\n"
                    );
                    *arg2 = ptr;
                    ptr = strtok(NULL, delimiters);
                    assert(
                        is_arg(ptr) != MISMATCH,
                        "argument is expected in the input asm source code.\n"
                    );
                    *arg3 = ptr;
                    return TYPE6;
                }
        case 5:
                /*
                 * 1. label opcode, arg1, arg2, arg3, e.g., label addi, t0, t0, 1
                 */
                ptr = strtok(line, delimiters);
                assert(
                    is_label(ptr) == VALID_LABEL,
                    "a valid label is expected in the input asm source code.\n"
                );
                *label = ptr;
                ptr = strtok(NULL, delimiters);
                assert(
                    is_opcode(ptr) != MISMATCH,
                    "a valid label is expected in the input asm source code.\n"
                );
                *opcode = ptr;
                ptr = strtok(NULL, delimiters);
                assert(
                    is_arg(ptr) != MISMATCH,
                    "opcode is expected in the input asm source code.\n"
                );
                *arg1 = ptr;
                ptr = strtok(NULL, delimiters);
                assert(
                    is_arg(ptr) != MISMATCH,
                    "argument is expected in the input asm source code.\n"
                );
                *arg2 = ptr;
                ptr = strtok(NULL, delimiters);
                assert(
                    is_arg(ptr) != MISMATCH,
                    "argument is expected in the input asm source code.\n"
                );
                *arg3 = ptr;
                return TYPE7;
        default:
                error("source codes contain errors.\n");
    }
    return(DONE);
}


void handle_err(int err_no, int line_no) {
    switch (err_no) {
        case 1:
            error("<error no: 1> undefeind label in the line %d.\n", line_no + 1);
            exit(1);
        case 2:
            error("<error no: 2> invalid opcode in the line %d.\n", line_no + 1);
            exit(2);
        case 3:
            error("<error no: 3> invalid constant in the line %d.\n", line_no + 1);
            exit(3);
        case 4:
            error("<error no: 4> invalid register operand in the line %d.\n", line_no + 1);
            exit(4);
        case 5:
            error("<error no: 5> wrong number of operands in the line %d.\n", line_no + 1);
            exit(5);
        case 6:
            error("<error no: 6> unexpected error occurs in the line %d.\n", line_no + 1);
            exit(6);
        case 7:
            error("<error no: 7> unexpected operand in the line %d.\n", line_no + 1);
            exit(7);
        case 8:
            error("<error no: 8> the address of the label in the line %d cannot be fetched.\n", line_no + 1);
            exit(8);
        case 9:
            error("<error no: 9> the memory address in the line %d is invalid to load the program.\n", line_no + 1);
            exit(9);
        case 10:
            error("<error no: 10> the label name in line %d is invalid.\n", line_no + 1);
            exit(10);
        /* reserved for other Error Code */
        case 11:
            error("<error no: 11> malloc is failed in the line %d.\n", line_no + 1);
            exit(11);
        default:
            printf("<error no: 12> unexpected error cannot be handled.\n");
            exit(12);
    }
}


void initialize_label_table(struct_label_table* head) {
    for (int i = 0; i < MAX_LABELS; i++, head++) {
        head->label[0] = '\0';
    }
}


bool if_no_duplicated_label(struct_label_table* label_table, char* label, int line_no) {
    for (int i = 0; i < MAX_LABELS; i++) {
        if (cmp_str(label_table[i].label, label)) {
            /* a conflict occurs with duplicated labels */
            error(
                "line: %d, label '%s' has a conflict with other labels in line %d.\n",
                line_no, label, label_table[i].line_no
            );
        }
    }
    return true;
}


void insert_a_label(
    struct_label_table* label_table,
    struct_label_table** head_of_label_table,
    char* label,
    int line_no,
    int addr
) {
    if (if_no_duplicated_label(label_table, label, line_no)) {
        copy_str((*head_of_label_table)->label, label);
        (*head_of_label_table)->address = addr;
        (*head_of_label_table)->line_no = line_no;
        (*head_of_label_table)++;
    }
}


void assemble(char *input_file_name, char *output_file_name) {
    FILE *input_file = open(input_file_name, "r");
    FILE *output_file = open(output_file_name, "w");
    char *label, *opcode, *arg1, *arg2, *arg3;
    struct_label_table label_table[MAX_LABELS], *head_of_label_table = label_table;
    int addr = 0x0, line_no = 0, number_of_labels = 0;
    int ret;

    initialize_label_table(head_of_label_table);

    info("Processing input file: %s\n", input_file_name);
    info("Writing result to output file: %s\n", output_file_name);

    // handle labels
    while ((ret = parse_inst(input_file, &label, &opcode, &arg1, &arg2, &arg3)) \
        != END_OF_FILE
    ) {
        line_no++;
        // `addr += 0x4` is not applied to `COMMENT_LINE`
        if (ret == TYPE1 || ret == TYPE4 || ret == TYPE6) {
            // instructions wo. a label
            addr += 0x4;
        } else if (ret == TYPE2 || ret == TYPE3 || ret == TYPE5 || ret == TYPE7) {
            // instructions w. a label
            insert_a_label(label_table, &head_of_label_table, label, line_no, addr);
            number_of_labels++;
            addr += 0x4;
        }
        if (is_opcode(opcode) == LA) {
            // `la` pseudo instruciton
            addr += 0x4;
        }
    }

    #ifdef DEBUG
        for (int i = 0; i < number_of_labels; i++) {
            test(
                "label: %s, address: %d, line number: %d\n",
                label_table[i].label,
                label_table[i].address,
                label_table[i].line_no
            );
        }
    #endif

    // assemble the instruction
    rewind(input_file);
    addr = line_no = 0x0;
    while ((ret = parse_inst(input_file, &label, &opcode, &arg1, &arg2, &arg3)) \
        != END_OF_FILE
    ) {
        line_no++;
        if (ret == TYPE1 || ret == TYPE2) {
            halt_to_binary(output_file);
            addr += 0x4;
        } else if (ret == TYPE3) {
            fill_to_binary(output_file, arg1, line_no);
            addr += 0x4;
        } else if (ret == TYPE4 || ret == TYPE5) {
            if (is_opcode(opcode) != LA) {
                fprintf(
                    output_file,
                    "0x%08x\n",
                    inst_to_binary(
                        line_no,
                        opcode,
                        arg1,
                        arg2,
                        arg3,
                        label_table,
                        number_of_labels,
                        addr
                    )
                );
            } else {
                la_to_binary(
                    output_file,
                    line_no,
                    opcode,
                    arg1,
                    arg2,
                    arg3,
                    label_table,
                    number_of_labels,
                    addr
                );
            }
            addr += 0x4;
        } else if (ret == TYPE6 || ret == TYPE7) {
            fprintf(
                output_file,
                "0x%08x\n",
                inst_to_binary(
                    line_no,
                    opcode,
                    arg1,
                    arg2,
                    arg3,
                    label_table,
                    number_of_labels,
                    addr
                )
            );
            addr += 0x4;
        }
        if (is_opcode(opcode) == LA) {
            // `la` pseudo instruciton
            addr += 0x4;
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        warn("Usage: asm <path to *.asm> <path to *.bin>\n");
        error("%s: error: lack of input file or output file.\n", argv[0]);
    }

    assemble(argv[1], argv[2]);

    return 0;
}
