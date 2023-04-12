/***************************************************************/
/*                                                             */
/*                   RISCV-LC System                           */
/*                                                             */
/*                     CEG3420 Lab3                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/


#ifndef __RISCV_LC_H__
#define __RISCV_LC_H__


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include "util.h"


/* bit operations */
#define MASK7_0(x) ((x) & 0xFF)
#define MASK15_8(x) (((x) & 0xFF00) >> 8)
#define MASK23_16(x) (((x) & 0xFF0000) >> 16)
#define MASK31_24(x) (((x) & 0xFF000000) >> 24)
/* relative definition of the control signals */
#define CONTROL_STATE_ROWS 128
#define INITIAL_STATE_NUMBER 0
/*
 * definitions of special base addresses 
 * `CODE_BASE_ADDR`: the base address to locate source codes
 * `TRAPVEC_BASE_ADDR`: exceptions & interruptions' base address
 */
#define CODE_BASE_ADDR 0x0
#define TRAPVEC_BASE_ADDR 0x400000


/* definition of the bit for the control signals */
enum {
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
  CONTROL_SIGNAL_BITS
} CONTROL_SIGNALS;


#define handle_get_element_of_error(x, id) error("unexpected signal offset: %d\n", (id));
#define get_element_of(x, id) ((x)[(id)])
#define get_IRD(x) get_element_of((x), IRD)
#define get_J(x) ( \
    (get_element_of((x), J6) << 6) + \
    (get_element_of((x), J5) << 5) + \
    (get_element_of((x), J4) << 4) + \
    (get_element_of((x), J3) << 3) + \
    (get_element_of((x), J2) << 2) + \
    (get_element_of((x), J1) << 1) + \
    (get_element_of((x), J0)) \
  )
#define get_LD_PC(x) get_element_of((x), LD_PC)
#define get_LD_MAR(x) get_element_of((x), LD_MAR)
#define get_LD_MDR(x) get_element_of((x), LD_MDR)
#define get_LD_IR(x) get_element_of((x), LD_IR)
#define get_LD_REG(x) get_element_of((x), LD_REG)
#define get_LD_BEN(x) get_element_of((x), LD_BEN)
#define get_GatePC(x) get_element_of((x), GatePC)
#define get_GateMAR(x) get_element_of((x), GateMAR)
#define get_GateMDR(x) get_element_of((x), GateMDR)
#define get_GateALUSHF(x) get_element_of((x), GateALUSHF)
#define get_GateRS2(x) get_element_of((x), GateRS2)
#define get_PCMUX(x) get_element_of((x), PCMUX)
#define get_ADDR1MUX(x) ( \
    (get_element_of((x), ADDR1MUX1) << 1) + \
    (get_element_of((x), ADDR1MUX0)) \
  )
#define get_ADDR2MUX(x) ( \
    (get_element_of((x), ADDR2MUX1) << 1) + \
    (get_element_of((x), ADDR2MUX0)) \
  )
#define get_MARMUX(x) get_element_of((x), MARMUX)
#define get_MDRMUX(x) get_element_of((x), MDRMUX)
#define get_RS2MUX(x) get_element_of((x), RS2MUX)
#define get_RS2En(x) get_element_of((x), RS2En)
#define get_RS1En(x) get_element_of((x), RS1En)
#define get_MIO_EN(x) get_element_of((x), MIO_EN)
#define get_WE(x) get_element_of((x), WE)
#define get_DATASIZE(x) get_element_of((x), DATASIZE)
#define get_RESET(x) get_element_of((x), RESET)
/* The control store ROM */
int CONTROL_STATE[CONTROL_STATE_ROWS][CONTROL_SIGNAL_BITS];
/* Main memory */
#define MEM_CYCLES 5
#define BYTES_IN_MEM 0x2000000
unsigned char MEMORY[BYTES_IN_MEM];


/*
 * `NUM_RISCV_LC_REGS`: number of RISCV LC architectural registers
 */
#define NUM_RISCV_LC_REGS 32


/* run bit */
int RUN_BIT;
/* names of `NUM_RISCV_LC_REGS` registers */
static const char * regs[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "fp/s0", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3",
    "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4",
    "t5", "t6"
};


/*
 * `struct_system_latches` stores the status of `PC` and other registers
 */
typedef struct {
    /* program counter */
    int PC;
    /* register file  */
    int REGS[NUM_RISCV_LC_REGS];
    /* memory data register */
    int MDR;
    /* memory address register */
    unsigned int MAR;
    /* instruction register */
    unsigned int IR;
    /* branch flag register */
    unsigned int B;
    /* memory ready bit indicator */
    int READY;
    /* micro-code / microintruction */
    int MICROINSTRUCTION[CONTROL_SIGNAL_BITS];
    /* state number: provided for computer system debugging */
    int STATE_NUMBER;
} struct_system_latches;


/* Data Structure for Latch */
struct_system_latches CURRENT_LATCHES, NEXT_LATCHES;


/* cycle counter */
int CYCLE_COUNT;


/* `MEM_VAL` saves the output of the main memory at each cycle */
int MEM_VAL;


/* input tristate drivers */
int value_of_GatePC;
int value_of_GateMAR;
int value_of_GateMDR;
int value_of_GateALUSHF;
int value_of_GateRS2;


/* value of the bus */
int BUS;


/* function.h */
unsigned int mask_val(int, int, int);
int sext_unit(int, int);
int rs2_mux(int, int, int);
int mar_mux(int, int, int);
int alu(int, int, int, int);
int addr2_mux(int, int, int, int, int);
int addr1_mux(int, int, int, int, int);
int logic_shift_20_function_unit(int);
int shift_function_unit(int, int, int, int);
int pc_mux(int, int, int);
int s_format_imm_gen_unit(int, int);
int b_format_imm_gen_unit(int, int, int, int);
int j_format_imm_gen_unit(int, int, int, int);
int mdr_mux(int, int, int);
int rs1_en(int, int);
int rs2_en(int, int);
int alu_shift_mux(int, int, int);
int blockBMUX(int, int, int);
int compare_function_unit(int, int, int, int);
int datasize_mux(unsigned int, int, int);
void write_memory(unsigned int, unsigned int, int);
int read_memory(unsigned int, unsigned int);


void cycle();
void init_control_state(char *);
void load_program(char *);
void init_memory();
void initialize(char *, char *, int);
void help();
void mdump(FILE *, int, int);
void rdump(FILE *);
void sdump(FILE *);
void go();
void run(int);
void get_command(FILE *);


/*
 * core function declarations
 */
/*
 * evaluate the address of the following state according to the
 * micro-code sequencer logic, and latch the next microinstruction
 */
void eval_micro_sequencer();
/*
 * `cycle_memory` emulates the main memory and other logics.
 * if `MIO_EN` is asserted at the fourth cycle, we need to latch `READY`
 * bit at the end of the cycle to prepare the microsequencer for the fifth cycle
 */
void cycle_memory();
/*
 * evaluate the value of tristate drivers
 */
void eval_bus_drivers();
/*
 * drive the system bus from one of the five possible tristate drivers
 */
void drive_bus();
/*
 * compute all functions that need to latch values in the data path
 * at the end of this cycle
 */
void latch_datapath_values();

#endif
