/***************************************************************/
/*                                                             */
/*                   RISCV-LC System                           */
/*                                                             */
/*                     CEG3420 Lab3                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/

#ifndef LC_H
#define LC_H

/*
 * Header files
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Basic macros
 */
/* A couple of useful definitions */
#define FALSE 0
#define TRUE  1
/* Bit operations */
#define MASK7_0(x) ((x) & 0xFF)
#define MASK15_8(x) (((x) & 0xFF00) >> 8)
#define MASK23_16(x) (((x) & 0xFF0000) >> 16)
#define MASK31_24(x) (((x) & 0xFF000000) >> 24)
/* Definition of the control store layout */
#define CONTROL_STORE_ROWS 128
#define INITIAL_STATE_NUMBER 0
/* Definition of the code layout */
#define CODE_BASE_ADDR 0x0
#define TRAPVEC_BASE_ADDR 0x400000

/*
 * Core function declarations
 */
void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/*
 * Declarations & Definitions
 */
/* Definition of the bit order in the control store word */
enum CONTROL_BITS {
  IRD, // 1
  J6, J5, J4, J3, J2, J1, J0, // 2-8
  LD_PC, // 9
  LD_MAR, // 10
  LD_MDR, // 11
  LD_IR, // 12
  LD_REG, // 13
  LD_BEN, // 14
  GatePC, // 15
  GateMAR, // 16
  GateMDR, // 17
  GateALUSHF, // 18
  GateRS2, // 19
  PCMUX, // 20
  ADDR1MUX1, ADDR1MUX0, // 21-22
  ADDR2MUX1, ADDR2MUX0, // 23-24
  MARMUX, // 25
  MDRMUX, // 26
  RS2MUX, // 27
  RS2En, // 28
  RS1En, // 29
  MIO_EN, // 30
  WE, // 31
  DATASIZE, // 32
  RESET, // 33
  CONTROL_STORE_BITS
} CONTROL_BITS;
/* Functions to get at the control bits */
#define handle_get_element_of_error(x, id) printf("[ERROR]: id: %d\n", (id));
#define get_element_of(x, id) ((x)[(id)])
#define get_IRD(x) get_element_of(x, IRD)
#define get_J(x) ( \
    (get_element_of(x, J6) << 6) + \
    (get_element_of(x, J5) << 5) + \
    (get_element_of(x, J4) << 4) + \
    (get_element_of(x, J3) << 3) + \
    (get_element_of(x, J2) << 2) + \
    (get_element_of(x, J1) << 1) + \
    (get_element_of(x, J0)) \
  )
#define get_LD_PC(x) get_element_of(x, LD_PC)
#define get_LD_MAR(x) get_element_of(x, LD_MAR)
#define get_LD_MDR(x) get_element_of(x, LD_MDR)
#define get_LD_IR(x) get_element_of(x, LD_IR)
#define get_LD_REG(x) get_element_of(x, LD_REG)
#define get_LD_BEN(x) get_element_of(x, LD_BEN)
#define get_GatePC(x) get_element_of(x, GatePC)
#define get_GateMAR(x) get_element_of(x, GateMAR)
#define get_GateMDR(x) get_element_of(x, GateMDR)
#define get_GateALUSHF(x) get_element_of(x, GateALUSHF)
#define get_GateRS2(x) get_element_of(x, GateRS2)
#define get_PCMUX(x) get_element_of(x, PCMUX)
#define get_ADDR1MUX(x) ( \
    (get_element_of(x, ADDR1MUX1) << 1) + \
    (get_element_of(x, ADDR1MUX0)) \
  )
#define get_ADDR2MUX(x) ( \
    (get_element_of(x, ADDR2MUX1) << 1) + \
    (get_element_of(x, ADDR2MUX0)) \
  )
#define get_MARMUX(x) get_element_of(x, MARMUX)
#define get_MDRMUX(x) get_element_of(x, MDRMUX)
#define get_RS2MUX(x) get_element_of(x, RS2MUX)
#define get_RS2En(x) get_element_of(x, RS2En)
#define get_RS1En(x) get_element_of(x, RS1En)
#define get_MIO_EN(x) get_element_of(x, MIO_EN)
#define get_WE(x) get_element_of(x, WE)
#define get_DATASIZE(x) get_element_of(x, DATASIZE)
#define get_RESET(x) get_element_of(x, RESET)
/* The control store ROM */
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];
/* Main memory */
#define MEM_CYCLES 5
#define WORDS_IN_MEM 0x2000000
unsigned char MEMORY[WORDS_IN_MEM];
/*
 * RISCV-LC State infomation
 */
#define LC_RISCV_REGS 32
/* run bit */
int RUN_BIT;
/* aliases for general-purpose registers */
static const char * regs[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "fp/s0", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4",
    "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

typedef struct SystemLatchesStruct {
    /* program counter */
    int PC;
    /* register file.  */
    int REGS[LC_RISCV_REGS];
    /* memory data register */
    int MDR;
    /* memory address register */
    unsigned int MAR;
    /* instruction register */
    unsigned int IR;
    /* B identifier */
    unsigned int B;
    /* ready bit */
    int READY;
    /* The microintruction */
    int MICROINSTRUCTION[CONTROL_STORE_BITS];
    /* Current State Number - Provided for debugging */
    int STATE_NUMBER;
} SystemLatches;
/* Data Structure for Latch */
SystemLatches CURRENT_LATCHES, NEXT_LATCHES;
/* A cycle counter */
int CYCLE_COUNT;
/* `MemOut` saves the direct output of MEMORY each cycle */
int MemOut;
/* Five input tristate drivers */
int valueOfGatePC;
int valueOfGateMAR;
int valueOfGateMDR;
int valueOfGateALUSHF;
int valueOfGateRS2;
/* value of the bus */
int BUS;

/* 
 * Functions declarations
 */
unsigned int mask_val(int, int, int);
int blockSEXT(int, int);
int blockRS2MUX(int, int, int);
int blockMARMUX(int, int, int);
int blockALU(int, int, int, int);
int blockADDR2MUX(int, int, int, int, int);
int blockADDR1MUX(int, int, int, int, int);
int blockLSHF20(int);
int blockSHF(int, int, int, int);
int blockPCMUX(int, int, int);
int blockStypeImm(int, int);
int blockBtypeImm(int, int, int, int);
int blockJtypeImm(int, int, int, int);
int blockMDRMUX(int, int, int);
int blockRS1En(int, int, int);
int blockRS2En(int, int, int);
int blockALUSHFMUX(int, int, int);
int blockBMUX(int, int, int);
int blockCMP(int, int, int, int);
int blockDATASIZEMUX(unsigned int, int, int);
void write_memory(unsigned int, unsigned int, int);
int read_memory(unsigned int, unsigned int);
void help();
void cycle();
void run(int);
void go();
void mdump(FILE *, int, int);
void rdump(FILE *);
void sdump(FILE *);
void get_command(FILE *);
void init_control_store(char *);
void init_memory();
void load_program(char *);
void initialize(char *, char *, int);
void cycle_memory();
void eval_micro_sequencer();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

#endif
