/***************************************************************/
/*                                                             */
/*                   RISCV-LC System                           */
/*                                                             */
/*                     CEG3420 Lab3                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/

/*
    Remove all unnecessary lines (including this one) in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name: [Full name]
    ID  : [Your Student ID]
*/

#include "lc.h"

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*                                                             */
/*         isaprogram   RISC-V machine language program file   */
/*                                                             */
/***************************************************************/

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
    printf("------------------RISC-V LC Help----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n cycles      \n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

	/*
	 *  Evaluate the address of the next state according to the
	 *  micro sequencer logic. Latch the next microinstruction
	 */
    eval_micro_sequencer();
    cycle_memory();
    eval_bus_drivers();
    drive_bus();
    latch_datapath_values();

    CURRENT_LATCHES = NEXT_LATCHES;

    CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == TRAPVEC_BASE_ADDR) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != TRAPVEC_BASE_ADDR)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address;

    printf("\nMemory content [0x%08x..0x%08x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = start; address <= stop; address += 4)
        printf("  0x%08x (%d) : 0x%02x%02x%02x%02x\n", address, address,
            MEMORY[address + 3], MEMORY[address + 2], MEMORY[address + 1], MEMORY[address]);
    printf("\n");

    /* Dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%08x..0x%08x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = start; address <= stop; address += 4)
        fprintf(dumpsim_file, " 0x%08x (%d) : 0x%02x%02x%02x%02x\n", address, address,
            MEMORY[address + 3], MEMORY[address + 2], MEMORY[address + 1], MEMORY[address]);
    fprintf(dumpsim_file, "\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%08x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%08x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%08x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%08x\n", BUS);
    printf("MDR          : 0x%08x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%08x\n", CURRENT_LATCHES.MAR);
	printf("MemOut       : 0x%08x\n", MemOut);
	printf("B            : 0x%08x\n", CURRENT_LATCHES.B);
    printf("Registers:\n");
    for (k = 0; k < LC_RISCV_REGS; k++)
        printf("%s\t[x%d]:\t0x%08x\n", regs[k], k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* Dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%08x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%08x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%08x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%08x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%08x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%08x\n", CURRENT_LATCHES.MAR);
	fprintf(dumpsim_file, "MemOut       : 0x%08x\n", MemOut);
	fprintf(dumpsim_file, "B            : 0x%08x\n", CURRENT_LATCHES.B);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_RISCV_REGS; k++)
        fprintf(dumpsim_file, "%s\t[x%d]:\t0x%08x\n", regs[k], k, CURRENT_LATCHES.REGS[k]);
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : sdump                                           */
/*                                                             */
/* Purpose   : Dump current signals to the                     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void sdump(FILE * dumpsim_file) {
    printf("Signals: \n");
    printf("  IRD: %d\t", get_IRD(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  J: %d\n", get_J(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  LD_PC: %d\t", get_LD_PC(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  LD_MAR: %d\n", get_LD_MAR(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  LD_MDR: %d\t", get_LD_MDR(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  LD_IR: %d\n", get_LD_IR(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  LD_REG: %d\t", get_LD_REG(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  LD_BEN: %d\n", get_LD_BEN(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  GatePC: %d\t", get_GatePC(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  GateMAR: %d\n", get_GateMAR(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  GateMDR: %d\t", get_GateMDR(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  GateALUSHF: %d\n", get_GateALUSHF(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  GateRS2: %d\t", get_GateRS2(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  PCMUX: %d\n", get_PCMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  ADDR1MUX: %d\t", get_ADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  ADDR2MUX: %d\n", get_ADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  MARMUX: %d\t", get_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  MDRMUX: %d\n", get_MDRMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  RS2MUX: %d\t", get_RS2MUX(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  RS2En: %d\n", get_RS2En(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  RS1En: %d\t", get_RS1En(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  MIO_EN: %d\n", get_MIO_EN(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  WE: %d\t", get_WE(CURRENT_LATCHES.MICROINSTRUCTION));
    printf("  RESET: %d\n", get_RESET(CURRENT_LATCHES.MICROINSTRUCTION));

    for(int i = 0; i < CONTROL_STORE_BITS; i++)
        fprintf(dumpsim_file, "%d", CURRENT_LATCHES.MICROINSTRUCTION[i]);
    fprintf(dumpsim_file, "\n");
    fprintf(dumpsim_file, "Signals: \n");
    fprintf(dumpsim_file, "  IRD: %d\t", get_IRD(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  J: %d\n", get_J(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  LD_PC: %d\t", get_LD_PC(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  LD_MAR: %d\n", get_LD_MAR(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  LD_MDR: %d\t", get_LD_MDR(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  LD_IR: %d\n", get_LD_IR(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  LD_REG: %d\t", get_LD_REG(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  LD_BEN: %d\n", get_LD_BEN(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  GatePC: %d\t", get_GatePC(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  GateMAR: %d\n", get_GateMAR(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  GateMDR: %d\t", get_GateMDR(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  GateALUSHF: %d\n", get_GateALUSHF(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  GateRS2: %d\t", get_GateRS2(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  PCMUX: %d\n", get_PCMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  ADDR1MUX: %d\t", get_ADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  ADDR2MUX: %d\n", get_ADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  MARMUX: %d\t", get_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  MDRMUX: %d\n", get_MDRMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  RS2MUX: %d\t", get_RS2MUX(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  RS2En: %d\n", get_RS2En(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  RS1En: %d\t", get_RS1En(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  MIO_EN: %d\n", get_MIO_EN(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  WE: %d\t", get_WE(CURRENT_LATCHES.MICROINSTRUCTION));
    fprintf(dumpsim_file, "  RESET: %d\n", get_RESET(CURRENT_LATCHES.MICROINSTRUCTION));
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-RISCV-SIM> ");

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
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;
		case 'S':
		case 's':
			sdump(dumpsim_file);
			break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
        printf("Error: Can't open micro-code file %s\n", ucode_filename);
        exit(-1);
    }

    /* Read a line for each row in the control store */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
        if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
            printf("Error: Too few lines (%d) in micro-code file: %s\n",
                    i, ucode_filename);
            exit(-1);
        }

        /* Put in bits one at a time */
        index = 0;

        for (j = 0; j < CONTROL_STORE_BITS; j++) {
            /* Needs to find enough bits in line */
            if (line[index] == '\0') {
                printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
                        ucode_filename, i);
                exit(-1);
            }
            if (line[index] != '0' && line[index] != '1') {
                printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
                        ucode_filename, i, j);
                exit(-1);
            }

            /* Set the bit in the Control Store */
            CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
            index++;
        }

        /* Warn about extra bits in line */
        if (line[index] != '\0')
            printf("[WARNING] Extra bit(s) in control store file %s. Line: %d\n",
                    ucode_filename, i);
    }
    printf("\n");
}

/************************************************************/
/*                                                          */
/* Procedure : init_memory                                  */
/*                                                          */
/* Purpose   : Zero out the memory array                    */
/*                                                          */
/************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

    /* Open program file */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program */
    program_base = CODE_BASE_ADDR;

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                    program_filename, ii);
            exit(-1);
        }

        /*
         * Write the word to memory array
         * Little endian
         */
        MEMORY[program_base + ii] = MASK7_0(word);
        MEMORY[program_base + ii + 1] = MASK15_8(word);
        MEMORY[program_base + ii + 2] = MASK23_16(word);
        MEMORY[program_base + ii + 3] = MASK31_24(word);
        ii += 4;
    }

    if (CURRENT_LATCHES.PC == 0)
        CURRENT_LATCHES.PC = CODE_BASE_ADDR;

    printf("Read %d words (%d bytes) from program into memory.\n\n", ii, ii << 2);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION,
        CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int) * CONTROL_STORE_BITS);

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 3) {
        printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
                argv[0]);
        exit(1);
    }

    printf("RISC-V LC Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

    if ((dumpsim_file = fopen( "dumpsim", "w" )) == NULL) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);
}

/***************************************************************/
/*              DO NOT MODIFY THE ABOVE CODE!                  */
/***************************************************************/

/*
 * `cycle` emulates memory and the logic.
 * Keep track of which cycle of MIOEN we are dealing with.
 * If fourth, we need to latch Ready bit at the end of
 * cycle to prepare microsequencer for the fifth cycle
 */
void cycle_memory() {
    static int MemCycleCnt = 0;

    int mio_en = get_MIO_EN(CURRENT_LATCHES.MICROINSTRUCTION),
        we = get_WE(CURRENT_LATCHES.MICROINSTRUCTION);

    int W = ~mask_val(CURRENT_LATCHES.MAR, 0, 0) & we;

    MemOut = 0;
    NEXT_LATCHES.READY = 0;
    printf("MemCycleCnt = %d\n", MemCycleCnt);
    printf("MIO_EN = %d, WE = %d, W = %d\n", mio_en, we, W);

    if (mio_en) {
        if (MemCycleCnt >= 0) {
            if (MemCycleCnt < 3) {
                MemCycleCnt++;
            } else if (MemCycleCnt == 3) {
                MemCycleCnt++;
                NEXT_LATCHES.READY = 1;
            } else {
                MemCycleCnt = 0;
            }
        }
        if (W) {
            /* Write */
            /*
             * Lab3-2 assignment
             */
            printf("Lab3-2 read_memory");
            exit(1);
        } else {
            /* Read */
            /*
             * Lab3-2 assignment
             */
            printf("Lab3-2 read_memory");
            exit(1);
        }
        MemCycleCnt++;
    } else {
        MemCycleCnt = 0;
    }
}


/*
 * Datapath routine emulating operations before driving the bus.
 * Evaluate the input of tristate drivers
 */
void eval_bus_drivers() {
    /*
     *  Lab3-2 assignment
     */
    printf("Lab3-2 assignment: eval_bus_drivers\n");
    exit(1);
}

/*
 * Datapath routine for driving the bus from one of
 * the 5 possible tristate drivers
 */
void drive_bus() {
    /*
     *  Lab3-2 assignment
     */
    printf("Lab3-2 assignment: drive_bus\n");
    exit(1);
}

/*
 * Datapath routine for computing all functions that need to latch
 * values in the data path at the end of this cycle. Some values
 * require sourcing the bus; therefore, this routine has to come
 * after drive_bus.
 */
void latch_datapath_values() {
    /* LD.MDR */
    if (get_LD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.MDR = blockMDRMUX(
            get_MDRMUX(CURRENT_LATCHES.MICROINSTRUCTION),
            MemOut,
            BUS
        );
    }
    /* LD.BEN */
    if (get_LD_BEN(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.B = blockCMP(
            mask_val(CURRENT_LATCHES.IR, 14, 12),
            blockRS1En(
                get_RS1En(CURRENT_LATCHES.MICROINSTRUCTION),
                0,
                CURRENT_LATCHES.REGS[mask_val(CURRENT_LATCHES.IR, 19, 15)]
			),
            blockRS2En(
                get_RS2En(CURRENT_LATCHES.MICROINSTRUCTION),
                0,
                CURRENT_LATCHES.REGS[mask_val(CURRENT_LATCHES.IR, 24, 20)]
			),
            0
        );
    }
    /* LD.REG */
    if (get_LD_REG(CURRENT_LATCHES.MICROINSTRUCTION)) {
        /*
         *  Lab3-2 assignment
         */
        printf("Lab3-2 LD_REG");
        exit(1);
    }
    /* LD.MAR */
    if (get_LD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        /*
         *  Lab3-2 assignment
         */
        printf("Lab3-2 LD_MDR");
        exit(1);
    }
    /* LD.IR */
    if (get_LD_IR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        /*
         *  Lab3-2 assignment
         */
        printf("Lab3-2 LD_IR");
        exit(1);
    }
    /* LD.PC */
    if (get_LD_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
        /*
         *  Lab3-2 assignment
         */
        printf("Lab3-2 LD_PC");
        exit(1);
    }
	/* RESET */
	if (get_RESET(CURRENT_LATCHES.MICROINSTRUCTION))
		NEXT_LATCHES.PC = TRAPVEC_BASE_ADDR;
}
