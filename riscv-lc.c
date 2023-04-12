/***************************************************************/
/*                                                             */
/*                   RISCV-LC System                           */
/*                                                             */
/*                     CEG3420 Lab3                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/


#include "riscv-lc.h"


/*
 * execute a cycle
 */
void cycle() {
    /*
     * core steps
     */
    eval_micro_sequencer();   
    cycle_memory();
    eval_bus_drivers();
    drive_bus();
    latch_datapath_values();

    CURRENT_LATCHES = NEXT_LATCHES;

    CYCLE_COUNT++;
}


void cycle_memory() {
    static int mem_cycle_cnt = 0;

    int mio_en = get_MIO_EN(CURRENT_LATCHES.MICROINSTRUCTION),
        we = get_WE(CURRENT_LATCHES.MICROINSTRUCTION);
    int W = ~mask_val(CURRENT_LATCHES.MAR, 0, 0) & we;

    MEM_VAL = 0;
    NEXT_LATCHES.READY = 0;
    info("memory cycle count = %d\n", mem_cycle_cnt);
    info("MIO_EN = %d, WE = %d, W = %d\n", mio_en, we, W);

    if (mio_en) {
        if (mem_cycle_cnt == MEM_CYCLES) {
            mem_cycle_cnt = 0;
            NEXT_LATCHES.READY = 1;
        }
        if (W) {
            /* write */
            /*
             * Lab3-2 assignment
             */
            error("Lab3-2 assignment: write to the main memory");
        } else {
            /* read */
            /*
             * Lab3-2 assignment
             * Tips: assign the read value to `MEM_VAL`
             */
            error("Lab3-2 assignment: read from the main memory");
        }
        mem_cycle_cnt++;
    } else
        mem_cycle_cnt = 0;
}


void latch_datapath_values() {
    /* LD.MDR */
    if (get_LD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.MDR = mdr_mux(
            get_MDRMUX(CURRENT_LATCHES.MICROINSTRUCTION),
            MEM_VAL,
            BUS
        );
    }
    /* LD.BEN */
    if (get_LD_BEN(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.B = compare_function_unit(
            mask_val(CURRENT_LATCHES.IR, 14, 12),
            rs1_en(
                get_RS1En(CURRENT_LATCHES.MICROINSTRUCTION),
                CURRENT_LATCHES.REGS[mask_val(CURRENT_LATCHES.IR, 19, 15)]
            ),
            rs2_en(
                get_RS2En(CURRENT_LATCHES.MICROINSTRUCTION),
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
        error("Lab3-2 assignment: handle LD_REG");
    }
    /* LD.MAR */
    if (get_LD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        /*
         *  Lab3-2 assignment
         */
        error("Lab3-2 assignment: handle LD_MAR");
    }
    /* LD.IR */
    if (get_LD_IR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        /*
         *  Lab3-2 assignment
         */
        error("Lab3-2 assignment: handle LD_IR");
    }
    /* LD.PC */
    if (get_LD_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
        /*
         *  Lab3-2 assignment
         */
        error("Lab3-2 assignment: handle LD_PC");
    }
    /* RESET */
    if (get_RESET(CURRENT_LATCHES.MICROINSTRUCTION))
        NEXT_LATCHES.PC = TRAPVEC_BASE_ADDR;
}


/*
 * load the micro-code into the ROM control unit
 */
void init_control_state(char *ucode_filename) {
    FILE *ucode = open(ucode_filename, "r");
    int i, j, index;
    char line[200];

    info("load the micro: %s\n", ucode_filename);

    /* Read a line for each row in the control store */
    for(i = 0; i < CONTROL_STATE_ROWS; i++) {
        if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
            error(
                "unexpected end of the file at line %d in the micro-code: %s\n",
                i + 1, ucode_filename
            );
        }

        /* Put in bits one at a time */
        index = 0;
        for (j = 0; j < CONTROL_SIGNAL_BITS; j++) {
            /* Needs to find enough bits in line */
            if (line[index] == '\0') {
                error(
                    "unexpected end of control bits at line %d in the micro-code: %s\n",
                    i + 1, ucode_filename
                );
            }
            if (line[index] != '0' && line[index] != '1') {
                error(
                    "unknown control bit %d at line %d in the micro-code: %s\n",
                    j + 1, i + 1, ucode_filename
                );
            }
            /* set the bit in the ROM control unit */
            CONTROL_STATE[i][j] = (line[index] == '0') ? 0:1;
            index++;
        }
        /* warn about extra bits in line */
        if (line[index] != '\0') {
            warn(
                "extra bit(s) at line %d in the micro-code %s\n",
                i + 1, ucode_filename
            );
        }
    }
}


/*
 * load the binary program into the main memory
 */
void load_program(char *program_filename) {
    FILE *prog;
    int i, word, program_base;

    /* open program file */
    prog = open(program_filename, "r");

    /* Read in the program */
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

    info("read %d bytes from the program into the memory.\n\n", i);
}


/*
 * zero out the memory array
 */
void init_memory() {
    int i;

    for (i = 0; i < BYTES_IN_MEM; i++)
        MEMORY[i] = 0;
}


/*
 * Load a micro-code and a binary file, and set up the initial state of the machine.
 */
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_state(ucode_filename);

    init_memory();
    for (i = 0; i < num_prog_files; i++) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(
        CURRENT_LATCHES.MICROINSTRUCTION,
        CONTROL_STATE[INITIAL_STATE_NUMBER],
        sizeof(int) * CONTROL_SIGNAL_BITS
    );

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = true;
}


/*
 * print the help menu
 */
void help() {
    printf("--------------------- RISCV LC SIM Help ----------------------\n");
    printf("go             -  run a program till the end                  \n");
    printf("run n          -  execute a program for n instructions        \n");
    printf("mdump low high -  dump memory from low address to high address\n");
    printf("rdump          -  dump the registers & bus values             \n");
    printf("?/h            -  display the help menu                       \n");
    printf("quit           -  exit the simulator                        \n\n");
}


/*
 * dump a word-aligned region of the main memory to the output file.
 */
void mdump(FILE *dumpsim_file, int start, int stop) {
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
    fprintf(dumpsim_file, "\nMemory content [0x%08x..0x%08x]:\n", start, stop);
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


/*
 * dump current registers and bus values to the output file
 */
void rdump(FILE *dumpsim_file) {
    int k;

    printf("\ncurrent register/bus values:\n");
    printf("-------------------------------------\n");
    printf("cycle count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%08x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%08x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%08x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%08x\n", BUS);
    printf("MDR          : 0x%08x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%08x\n", CURRENT_LATCHES.MAR);
    printf("Mem. content : 0x%08x\n", MEM_VAL);
    printf("B            : 0x%08x\n", CURRENT_LATCHES.B);
    printf("READY        : 0x%08x\n", CURRENT_LATCHES.READY);
    printf("registers:\n");
    for (k = 0; k < NUM_RISCV_LC_REGS; k++)
        printf("%s\t[x%d]:\t0x%08x\n", regs[k], k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    fprintf(dumpsim_file, "\ncurrent register/bus values:\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "cycle count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%08x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%08x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%08x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%08x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%08x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%08x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "Mem. content : 0x%08x\n", MEM_VAL);
    fprintf(dumpsim_file, "B            : 0x%08x\n", CURRENT_LATCHES.B);
    fprintf(dumpsim_file, "READY        : 0x%08x\n", CURRENT_LATCHES.READY);
    fprintf(dumpsim_file, "registers:\n");
    for (k = 0; k < NUM_RISCV_LC_REGS; k++)
        fprintf(dumpsim_file, "%s\t[x%d]:\t0x%08x\n", regs[k], k, CURRENT_LATCHES.REGS[k]);
    printf("\n");
}


/*
 * dump current signals to the output file
 */
void sdump(FILE *dumpsim_file) {
    printf("signals: \n");
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

    for(int i = 0; i < CONTROL_SIGNAL_BITS; i++)
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


/*
 * simulate the RISCV LC with benchmarks until it is halted
 */
void go() {
    if (RUN_BIT == false)
        error("RISCV LC cannot simulate, and the simulator is halted.\n\n");

    info("simulating...\n\n");
    while (CURRENT_LATCHES.PC != TRAPVEC_BASE_ADDR)
        cycle();
    RUN_BIT = false;
    info("RISCV LC is halted.\n\n");
}


/*
 * simulate the RISCV LC with benchmarks for n cycles
 */
void run(int num_cycles) {
    int i;

    if (RUN_BIT == false)
        error("RISCV LC cannot simulate, and the simulator is halted.\n\n");

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


/*
 * read a command from the standard input.
 */
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
        case 'S':
        case 's':
            sdump(dumpsim_file);
            break;
        default:
            printf("invalid command\n");
            break;
    }
}


int main(int argc, char *argv[]) {
    FILE *dumpsim_file = open("dumpsim", "w");

    /* Error Checking */
    if (argc != 3) {
        error("Usage: %s <ucode> <*.bin>\n", argv[0]);
    }

    info("Welcome to the RISCV LC Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

    while (1)
        get_command(dumpsim_file);
}
