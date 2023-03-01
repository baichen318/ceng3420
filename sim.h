/***************************************************************/
/*                                                             */
/*                  RISCV-LC Simulator                         */
/*                                                             */
/*                     CEG3420 Lab2                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/


#ifndef __LC_SIM_H__
#define __LC_SIM_H__


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include "util.h"


/*
 * Macros
 */
#define MASK6_0(x) ((x) & 0x7F)
#define MASK7(x) (((x) & 0x80) >> 7)
#define MASK7_0(x) ((x) & 0xFF)
#define MASK11_0(x) ((x) & 0xFFF)
#define MASK11_7(x) (((x) & 0xF80) >> 7)
#define MASK11_8(x) (((x) & 0xF00) >> 8)
#define MASK15_0(x) ((x) & 0xFFFF)
#define MASK15_8(x) (((x) & 0xFF00) >> 8)
#define MASK14_12(x) (((x) & 0x7000) >> 12)
#define MASK19_12(x) (((x) & 0xFF000) >> 12)
#define MASK19_15(x) (((x) & 0xF8000) >> 15)
#define MASK20(x) (((x) & 0x100000) >> 20)
#define MASK23_16(x) (((x) & 0xFF0000) >> 16)
#define MASK24_20(x) (((x) & 0x1F00000) >> 20)
#define MASK31(x) (((x) & 0x80000000) >> 31)
#define MASK31_12(x) (((x) & 0xFFFFF000) >> 12)
#define MASK31_20(x) (((x) & 0xFFF00000) >> 20)
#define MASK30_21(x) (((x) & 0x7FE00000) >> 21)
#define MASK30_25(x) (((x) & 0x7E000000) >> 25)
#define MASK31_25(x) (((x) & 0xFE000000) >> 25)
#define MASK31_24(x) (((x) & 0xFF000000) >> 24)
/*
 * definitions of special base addresses 
 * `CODE_BASE_ADDR`: the base address to locate source codes
 * `TRAPVEC_BASE_ADDR`: exceptions & interruptions' base address
 */
#define CODE_BASE_ADDR 0x0
#define TRAPVEC_BASE_ADDR 0x400000
/*
 * byte-addressed main memory, 32 MB
 */
#define BYTES_IN_MEM 0x2000000
#define NUM_RISCV_LC_REGS 32


/*
 * the definition of the main memory
 */
unsigned char MEMORY[BYTES_IN_MEM];


/*
 * `struct_system_latches` stores the status of `PC` and registers
 */
typedef struct {
    /* program counter */
    int PC;
    /* register files  */
    int REGS[NUM_RISCV_LC_REGS];
} struct_system_latches;

struct_system_latches CURRENT_LATCHES, NEXT_LATCHES;

/*
 * `INSTRUCTION_COUNT` traces instruction counts
 */
int INSTRUCTION_COUNT;


/*
 * names of `NUM_RISCV_LC_REGS` registers
 */
const char *regs[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "fp/s0", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3",
    "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4",
    "t5", "t6"
};


/*
 * `RUN_BIT` trace the running status of the RISCV LC simulator
 * the simulator is halted if `RUN_BIT` is equal to false, otherwise,
 * the simulator is running
 */
bool RUN_BIT;


/*
 * help menu
 */
void help();


/*
 * the initialization of the main memory, initialize to zeros
 */
 void init_memory();


/*
 * load binary programs from the main memory
 */
 void load_program(char *);


/*
 * initialization function
 */
 void initialize(char *, int);


/*
 * dump contents of the main memory
 */
void mdump(FILE *, int, int);


/*
 * dump contents of all latches (registers)
 */
void rdump(FILE *);


/*
 * the interface of the RISCV LC simulator console
 */
 void get_command(FILE *);


/*
 * step by step simulation
 */
void cycle();


/*
 * run simulation until the end of the program
 */
void go();


/*
 * run simulation via pre-defined steps
 */
void run(int);


/*
 * read contents from the main memory
 */
 int read_mem(int);


/*
 * sign extended
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
 * RISCV LC simulator instructions handler
 */
 void handle_instruction();


#endif
