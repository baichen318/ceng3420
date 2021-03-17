  /***************************************************************/
/*                                                             */
/*                  RISCV-LC Assembler                         */
/*                                                             */
/*                     CEG3420 Lab2                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/

/*
 * Header files
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Macros
 */
#define FALSE 0
#define TRUE  1
/*
 * Masks that you would find them very useful
 */
#define MASK7_0(x) ((x) & 0xFF)
#define MASK15_8(x) (((x) & 0xFF00) >> 8)
#define MASK23_16(x) (((x) & 0xFF0000) >> 16)
#define MASK31_24(x) (((x) & 0xFF000000) >> 24)
#define MASK15_0(x) ((x) & 0xFFFF)
#define MASK6_0(x) ((x) & 0x7F)
#define MASK11_7(x) (((x) & 0xF80) >> 7)
#define MASK14_12(x) (((x) & 0x7000) >> 12)
#define MASK19_15(x) (((x) & 0xF8000) >> 15)
#define MASK24_20(x) (((x) & 0x1F00000) >> 20)
#define MASK31_25(x) (((x) & 0xFE000000) >> 25)
#define MASK31_20(x) (((x) & 0xFFF00000) >> 20)
#define MASK19_12(x) (((x) & 0xFF000) >> 12)
#define MASK20(x) (((x) & 0x100000) >> 20)
#define MASK30_21(x) (((x) & 0x7FE00000) >> 21)
#define MASK31(x) (((x) & 0x80000000) >> 31)
#define MASK7(x) (((x) & 0x80) >> 7)
#define MASK11_8(x) (((x) & 0xF00) >> 8)
#define MASK30_25(x) (((x) & 0x7E000000) >> 25)
#define MASK31_12(x) (((x) & 0xFFFFF000) >> 12)
/*
 * Special base addresses definitions
 * `CODE_BASE_ADDR`: the base address to locate source codes
 * `TRAPVEC_BASE_ADDR`: Exceptions & Interruptions' base address
 */
#define CODE_BASE_ADDR 0x0
#define TRAPVEC_BASE_ADDR 0x400000
/*
 * #num of general-purpose registers
 */
#define LC_RISCV_REGS 32
/*
 * Byte-addressed MEMORY, 32 MB
 */
#define WORDS_IN_MEM 0x2000000
unsigned char MEMORY[WORDS_IN_MEM];

/*
 * Global variables
 */
/* Flag of the run bit */
int RUN_BIT;
/*
 * Aliases for registers
 */
const char * regs[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "fp/s0", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4",
    "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
/* Data Structure for Latch */
typedef struct SystemLatchesStruct {
    /* program counter */
    int PC;
    /* register files  */
    int REGS[LC_RISCV_REGS];
} SystemLatches;
/* Definitions for Latch */
SystemLatches CURRENT_LATCHES, NEXT_LATCHES;
/* 
 * Cycle counter
 */
int INSTRUCTION_COUNT;

/*
 * Function eclarations
 */
/*
 * Help menu
 */
void help();
/*
 * Step by step simulation
 */
void cycle();
/*
 * Run simulation via pre-defined steps
 */
void run(int);
/*
 * Run simulation until the end of the program
 */
void go();
/*
 * Memory dump
 */
void mdump(FILE *, int, int);
/*
 * Registers dump
 */
void rdump(FILE *);
/*
 * Shell command
 */
 void get_command(FILE *);
/*
 * Memory initialization
 */
 void init_memory();
/*
 * Load binary programs
 */
 void load_program(char *);
/*
 * Utilities for the initialization
 */
 void initialize(char *, int);
/*
 * Read from the memory
 */
 int read_mem(int);
/*
 * Sign extended
 */
 int sext(int, int);
/*
 * addi implementation
 */
 void handle_addi(unsigned int);
/*
 * slli implementation
 */
 void handle_slli(unsigned int);
/*
 * xori implementation
 */
 void handle_xori(unsigned int);
/*
 * srli implementation
 */
 void handle_srli(unsigned int);
/*
 * srai implementation
 */
 void handle_srai(unsigned int);
/*
 * ori implementation
 */
 void handle_ori(unsigned int);
/*
 * andi implementation
 */
 void handle_andi(unsigned int);
/*
 * lui implementation
 */
 void handle_lui(unsigned int);
/*
 * add implementation
 */
 void handle_add(unsigned int);
/*
 * sub implementation
 */
 void handle_sub(unsigned int);
/*
 * sll implementation
 */
 void handle_sll(unsigned int);
/*
 * xor implementation
 */
 void handle_xor(unsigned int);
/*
 * srl implementation
 */
 void handle_srl(unsigned int);
/*
 * sra implementation
 */
 void handle_sra(unsigned int);
/*
 * or implementation
 */
 void handle_or(unsigned int);
/*
 * and implementation
 */
 void handle_and(unsigned int);
/*
 * jalr implementation
 */
 void handle_jalr(unsigned int);
/*
 * jal implementation
 */
 void handle_jal(unsigned int);
/*
 * beq implementation
 */
 void handle_beq(unsigned int);
/*
 * bne implementation
 */
 void handle_bne(unsigned int);
/*
 * blt implementation
 */
 void handle_blt(unsigned int);
/*
 * bge implementation
 */
 void handle_bge(unsigned int);
/*
 * lb implementation
 */
 void handle_lb(unsigned int);
/*
 * lh implementation
 */
 void handle_lh(unsigned int);
/*
 * lw implementation
 */
 void handle_lw(unsigned int);
/*
 * sb implementation
 */
 void handle_sb(unsigned int);
/*
 * sh implementation
 */
 void handle_sh(unsigned int);
/*
 * sw implementation
 */
 void handle_sw(unsigned int);
/*
 * RV32I instructions handler
 */
 void handle_instruction();
