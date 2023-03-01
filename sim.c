/***************************************************************/
/*                                                             */
/*                  RISCV-LC Simulator                         */
/*                                                             */
/*                     CEG3420 Lab2                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/


#include "sim.h"


void help() {
    printf("--------------------- RISCV LC SIM Help ----------------------\n");
    printf("go             -  run a program till the end                  \n");
    printf("run n          -  execute a program for n instructions        \n");
    printf("mdump low high -  dump memory from low address to high address\n");
    printf("rdump          -  dump the register & bus values              \n");
    printf("?/h            -  display the help menu                       \n");
    printf("quit           -  exit the simulator                        \n\n");
}


void init_memory() {
    int i;

    for (i = 0; i < BYTES_IN_MEM; i++)
        MEMORY[i] = 0;
}


void load_program(char *program_filename) {
    FILE *prog;
    int i, word, program_base;

    /* open program file. */
    prog = open(program_filename, "r");

    /* read the program. */
    program_base = CODE_BASE_ADDR;

    i = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* make sure the program fit the memory */
        if (program_base + i >= BYTES_IN_MEM) {
            error(
            	"program file %s is too long to fit in memory at the address: %x\n",
            	program_filename, i
            );
        }

        /*
         * write the word to memory array.
         * little endian
         */
        MEMORY[program_base + i] = MASK7_0(word);
        MEMORY[program_base + i + 1] = MASK15_8(word);
        MEMORY[program_base + i + 2] = MASK23_16(word);
        MEMORY[program_base + i + 3] = MASK31_24(word);
        i += 4;
    }

    if (CURRENT_LATCHES.PC == 0)
        CURRENT_LATCHES.PC = CODE_BASE_ADDR;

    info("read %d words (%d bytes) from program into memory.\n\n", i, i << 2);
}


void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for (i = 0; i < num_prog_files; i++) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = true;
}


void mdump(FILE *dumpsim_file, int start, int stop) {
	/* this is a byte address */
    int address;

    printf("\nmemory content [0x%08x..0x%08x]:\n", start, stop);
    printf("-------------------------------------\n");
    for (address = start; address <= stop; address += 4)
        printf(
        	"  0x%08x (%d) : 0x%02x%02x%02x%02x\n",
        	address,
        	address,
        	MEMORY[address + 3],
        	MEMORY[address + 2],
        	MEMORY[address + 1],
        	MEMORY[address]
        );
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nmemory content [0x%08x..0x%08x]:\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = start; address <= stop; address += 4)
        fprintf(
        	dumpsim_file,
        	" 0x%08x (%d) : 0x%02x%02x%02x%02x\n",
        	address,
        	address,
            MEMORY[address + 3],
            MEMORY[address + 2],
            MEMORY[address + 1],
            MEMORY[address]
        );
    fprintf(dumpsim_file, "\n");
}


void rdump(FILE *dumpsim_file) {
    int k;

    printf("\ncurrent register/bus values:\n");
    printf("-------------------------------------\n");
    printf("instruction count: %d\n", INSTRUCTION_COUNT);
    printf("PC               : 0x%08x\n", CURRENT_LATCHES.PC);
    printf("registers:\n");
    for (k = 0; k < NUM_RISCV_LC_REGS; k++)
        printf("%s\t[x%d]:\t0x%08x\n", regs[k], k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\ncurrent register/bus values:\n"); 
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "instruction count: %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC               : 0x%04x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "registers:\n");
    for (k = 0; k < NUM_RISCV_LC_REGS; k++)
        fprintf(dumpsim_file, "%s\t[x%d]:\t0x%04x\n", regs[k], k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
}


void get_command(FILE *dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("RISCV LC SIM > ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;
        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;
        case '?':
        case 'h':
        case 'H':
            help();
            break;
        case 'Q':
        case 'q':
            printf("bye.\n");
            exit(EXIT_SUCCESS);
        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;
        default:
            printf("invalid command\n");
            break;
    }
}


void cycle() {
    handle_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}


void go() {
    if (RUN_BIT == false) {
        error("RISCV LC cannot simulate, and the simulator is halted.\n\n");
        return;
    }

    info("simulating...\n\n");
    while (CURRENT_LATCHES.PC != TRAPVEC_BASE_ADDR)
        cycle();
    RUN_BIT = false;
    info("RISCV LC is halted.\n\n");
}


void run(int num_cycles) {
    int i;

    if (RUN_BIT == false) {
        error("RISCV LC cannot simulate, and the simulator is halted.\n\n");
        return;
    }

    info("simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == TRAPVEC_BASE_ADDR) {
            RUN_BIT = false;
            info("RISCV LC is halted.\n\n");
            break;
        }
        cycle();
    }
}


/* return the word content start from `start_addr` */
int read_mem(int start_addr) {
    if (start_addr % 2 || start_addr >= BYTES_IN_MEM)
        error("read the un-aligned address, or the address is out of boundary.\n");

    return (MEMORY[start_addr + 3] << 24) + \
        (MEMORY[start_addr + 2] << 16) + \
        (MEMORY[start_addr + 1] << 8) + \
        MEMORY[start_addr];
}


/*
 * sign extend the `imm` to 32 bits
 * `width` is the bit width before `imm` is extended
 * Example:
 *		if we want to do the sign extension with a 12-bit
 * 		immediate value `imm`, you need to call `sext` with
 *		sext(imm, 12);
 *
 */
int sext(int imm, int width) {
    if (imm >> (width - 1)) {
        /* sign bit is 1 */
		return (0xFFFFFFFF & imm) | (0xFFFFFFFF << width);
    }
    else {
        /* sign bit is 0 */
        return imm;
    }
}


void handle_addi(unsigned int cur_inst) {
    unsigned int rd = MASK11_7(cur_inst), rs1 = MASK19_15(cur_inst);
    int imm12 = sext(MASK31_20(cur_inst), 12);
    NEXT_LATCHES.REGS[rd] = CURRENT_LATCHES.REGS[rs1] + imm12;
}



void handle_slli(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SLLI\n");
    exit(EXIT_FAILURE);
}



void handle_xori(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: XORI\n");
    exit(EXIT_FAILURE);
}


void handle_srli(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SRLI\n");
    exit(EXIT_FAILURE);
}


void handle_srai(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SRAI\n");
    exit(EXIT_FAILURE);
}


void handle_ori(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: ORI\n");
    exit(EXIT_FAILURE);
}


void handle_andi(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: ANDI\n");
    exit(EXIT_FAILURE);
}


void handle_lui(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: LUI\n");
    exit(EXIT_FAILURE);
}


void handle_add(unsigned int cur_inst) {
    unsigned int rd = MASK11_7(cur_inst),
		rs1 = MASK19_15(cur_inst),
        rs2 = MASK24_20(cur_inst);
    NEXT_LATCHES.REGS[rd] = CURRENT_LATCHES.REGS[rs1] + CURRENT_LATCHES.REGS[rs2];
}


void handle_sub(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SUB\n");
    exit(EXIT_FAILURE);
}


void handle_sll(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SLL\n");
    exit(EXIT_FAILURE);
}


void handle_xor(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: XOR\n");
    exit(EXIT_FAILURE);
}


void handle_srl(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SRL\n");
    exit(EXIT_FAILURE);
}


void handle_sra(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SRA\n");
    exit(EXIT_FAILURE);
}


void handle_or(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: OR\n");
    exit(EXIT_FAILURE);
}


void handle_and(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: AND\n");
    exit(EXIT_FAILURE);
}


void handle_jalr(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: JALR\n");
    exit(EXIT_FAILURE);
}


void handle_jal(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: JAL\n");
    exit(EXIT_FAILURE);
}


void handle_beq(unsigned int cur_inst) {
    unsigned int rs1 = MASK19_15(cur_inst), rs2 = MASK24_20(cur_inst);
    int imm12 = (MASK31(cur_inst) << 12) + \
            (MASK7(cur_inst) << 11) + \
            (MASK30_25(cur_inst) << 5) + \
            (MASK11_8(cur_inst) << 1);
    if (CURRENT_LATCHES.REGS[rs1] == CURRENT_LATCHES.REGS[rs2])
        NEXT_LATCHES.PC = sext(imm12, 12) + NEXT_LATCHES.PC;
}


void handle_bne(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: BNE\n");
    exit(EXIT_FAILURE);
}


void handle_blt(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: BLT\n");
    exit(EXIT_FAILURE);
}


void handle_bge(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: BGE\n");
    exit(EXIT_FAILURE);
}


void handle_lb(unsigned int cur_inst) {
    unsigned int rd = MASK11_7(cur_inst), rs1 = MASK19_15(cur_inst);
    int imm12 = MASK31_20(cur_inst);
    NEXT_LATCHES.REGS[rd] = sext(MASK7_0(MEMORY[sext(imm12, 12) + CURRENT_LATCHES.REGS[rs1]]), 8);
}


void handle_lh(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: LH\n");
    exit(EXIT_FAILURE);
}

void handle_lw(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: LW\n");
    exit(EXIT_FAILURE);
}


void handle_sb(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SB\n");
    exit(EXIT_FAILURE);
}


void handle_sh(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SH\n");
    exit(EXIT_FAILURE);
}


void handle_sw(unsigned int cur_inst) {
    /*
     * Lab2-2 assignment
     */
    warn("Lab2-2 assignment: SW\n");
    exit(EXIT_FAILURE);
}


void handle_halt(unsigned int cur_inst) {
	unsigned int s0 = 8;
	NEXT_LATCHES.REGS[s0] = CURRENT_LATCHES.PC;
	NEXT_LATCHES.PC = TRAPVEC_BASE_ADDR;
}


/*
 *  process one instruction at a time  
 *  - fetch one instruction
 *  - decode 
 *  - execute
 *  - write back
 */     
void handle_instruction() {
    unsigned int cur_inst = read_mem(CURRENT_LATCHES.PC);
    unsigned int opcode = MASK6_0(cur_inst), funct3 = MASK14_12(cur_inst);
    info("cur_inst = 0x%08x\n", cur_inst);

	CURRENT_LATCHES.REGS[0] = 0;
    NEXT_LATCHES = CURRENT_LATCHES;
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 4;

    /* opcode */
    switch(opcode) {
        case (0x04 << 2) + 0x03:
            /*
             * Integer Register-Immediate Instructions
             */
            switch(funct3) {
                    case 0:
                        handle_addi(cur_inst);
                        break;
                    case 1:
                        handle_slli(cur_inst);
                        break;
                    case 4:
                        handle_xori(cur_inst);
                        break;
                    case 5:
                        if (MASK31_25(cur_inst) == 0)
                            handle_srli(cur_inst);
                        else
                            handle_srai(cur_inst);
                        break;
                    case 6:
                        handle_ori(cur_inst);
                        break;
                    case 7:
                        handle_andi(cur_inst);
                        break;
                    default:
                        error("unknown opcode 0x%08x is captured.\n", cur_inst);
                }
            break;
        case (0x1F << 2) + 0x03:
            /*
             * Handle halt instruction
             */
            if (funct3 == 0x07)
                handle_halt(cur_inst);
            else
                error("unknown opcode 0x%08x is captured.\n", cur_inst);
            break;
        /*
         * Lab2-2 assignment: Decode other types of RV32I instructions
         */
        default:
            error("unknown instruction 0x%08x is captured.\n", cur_inst);
    }
}


int main(int argc, char *argv[]) {
    FILE *dumpsim_file = open("dumpsim", "w");

    /* Error Checking */
    if (argc != 2) {
        error("Usage: %s <*.bin>\n", argv[0]);
    }

    info("Welcome to the RISCV LC Simulator\n\n");

    initialize(argv[1], argc - 1);

    while (1)
        get_command(dumpsim_file);
}
