/***************************************************************/
/*                                                             */
/*                   RISCV-LC System                           */
/*                                                             */
/*                     CEG3420 Lab3                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/

#include "lc.h"

/* 
 * return the corresponding value of intst[hBit:lBit]
 */
unsigned int mask_val(int instr, int hBit, int lBit) {
    if (hBit < lBit || hBit > 31 || lBit < 0) {
        printf ("Illigal usage of function 'mask_val'\n");
        exit(1);
    }
    int i, mask = 0;
    for (i = lBit; i <= hBit; i++)
        mask += (1 << i);
    return ((mask & instr) >> lBit);
}

/*
 * sign extend the imm to 16 bits
 * 'width' is the bit width before imm is extended
 */
int blockSEXT(int imm, int width) {
    if (imm >> (width - 1)) {
        /* sign bit is 1 */
        return (0xFFFFFFFF & imm) | (0xFFFFFFFF << width);
    }
    else {
        /* sign bit is 0 */
        return imm;
    }
}

int blockRS2MUX(int rs2mux, int rs2, int imm12) {
    switch(rs2mux) {
        case 0:
            return rs2;
        case 1:
            return imm12;
        default:
            printf("Warning: unknown rs2mux = %d\n", rs2mux);
    }
    return rs2;
}

int blockMARMUX(int marmux, int offset, int operand) {
    switch (marmux) {
        case 0:
            return offset;
        case 1:
            return operand;
        default:
            printf("Warning: unknown MARMUX = %d\n", marmux);
    }
    return operand;
}

int blockALU(int funct3, int funct7, int rs1, int operand) {
    switch (funct3) {
        case 0:
            switch(funct7) {
                case 0b0000000:
                    return rs1 + operand;
                case 0b0100000:
                    return rs1 - operand;
				default:
					return rs1 + operand;
            }
        case 4:
            return rs1 ^ operand;
        case 6:
            return rs1 | operand;
        case 7:
            return rs1 & operand;
    }
    return rs1 + operand;
}

int blockADDR2MUX(int addr2mux, int zero, int imm12, int simm12, int jimm20) {
    switch (addr2mux & 0x3) {
        case 0:
            return zero;
        case 1:
            return imm12;
        case 2:
            return simm12;
        case 3:
            return jimm20;
        default:
            printf("[WARNING] unknown ADDR2MUX = %d\n", addr2mux);
    }
    return zero;
}

int blockADDR1MUX(int addr1mux, int zero, int PC, int base, int bimm12) {
    switch (addr1mux & 0x3) {
        case 0:
            return zero;
        case 1:
            return PC;
        case 2:
            return base;
        case 3:
            return bimm12;
        default:
            printf("[WARNING] unknown ADDR1MUX = %d\n", addr1mux);
    }
    return PC;
}

int blockLSHF20(int value) {
    return value << 20;
}

int blockSHF(int funct3, int funct7, int rs1, int operand) {
    switch(funct3) {
        case 1:
            return rs1 << operand;
        case 5:
            switch(funct7) {
                case 0b0000000:
                    return rs1 >> operand;
                case 0b0100000:
                    return blockSEXT(rs1 >> operand, 31 - operand);
            }
    }
    return rs1 << operand;
}

int blockPCMUX(int pcmux, int PCAdd4, int bus) {
    switch (pcmux) {
        case 0:
            return PCAdd4;
        case 1:
            return bus;
        default:
            printf("[WARNING] unknown PCMUX = %d\n", pcmux);
    }
    return PCAdd4;
}

int blockStypeImm(int imm4_0, int imm11_5) {
    return (imm11_5 << 5) + imm4_0;
}

int blockBtypeImm(int imm11, int imm4_1, int imm10_5, int imm12) {
    return (imm12 << 12) + (imm11 << 11) + (imm10_5 << 5) + (imm4_1 << 1);
}

int blockJtypeImm(int imm20, int imm10_1, int imm11, int imm19_12) {
    return (imm20 << 20) + (imm19_12 << 12) + (imm11 << 11) + (imm10_1 << 1);
}

int blockMDRMUX(int mdrmux, int memout, int bus) {
    switch(mdrmux) {
        case 0:
            return memout;
        case 1:
            return bus;
        default:
            printf("[WARNING] unknown MIO_EN = %d\n", mdrmux);
    }
    return memout;
}

int blockRS1En(int rs1_en, int zero, int rs1) {
    switch(rs1_en) {
        case 0:
            return zero;
        case 1:
            return rs1;
        default:
            printf("[WARNING] unknown RS1En = %d\n", rs1_en);
    }
    return zero;
}

int blockRS2En(int rs2_en, int zero, int rs2) {
    switch(rs2_en) {
        case 0:
            return zero;
        case 1:
            return rs2;
        default:
            printf("[WARNING] unknown RS2En = %d\n", rs2_en);
    }
    return zero;
}

int blockALUSHFMUX(int funct3, int valueOfALU, int valueOfSHF) {
    switch(funct3) {
        case 0:
        case 4:
        case 6:
        case 7:
            return valueOfALU;
        case 1:
        case 5:
            return valueOfSHF;
    }
    return valueOfALU;
}

int blockBMUX(int bmux, int zero, int B) {
	switch(bmux) {
		case 0:
			return zero;
		case 1:
			return B;
	}
	return zero;
}

int blockCMP(int funct3, int rs1, int rs2, int zero) {
    int val = rs1 - rs2;

    switch(funct3) {
        case 0:
            return (val == zero);
        case 1:
            return (val != zero);
        case 4:
            return (val < zero);
        case 5:
            return (val >= zero);
    }
    return (val == zero);
}

int blockDATASIZEMUX(unsigned int data_size, int funct3, int zero) {
	if (data_size) {
		switch(data_size) {
			case 0:
				return zero;
			case 1:
			 return ~(funct3 & 0x3);
		}
	} else
		return zero;
}

void write_memory(unsigned int data_size, unsigned int MAR, int MDR) {
    /*
     * Lab3-2 assignment
     */
	switch(data_size & 0x3) {
		case 0:
		case 1:
            MEMORY[CURRENT_LATCHES.MAR] = MASK7_0(CURRENT_LATCHES.MDR);
            MEMORY[CURRENT_LATCHES.MAR + 1] = MASK15_8(CURRENT_LATCHES.MDR);
            MEMORY[CURRENT_LATCHES.MAR + 2] = MASK23_16(CURRENT_LATCHES.MDR);
            MEMORY[CURRENT_LATCHES.MAR + 3] = MASK31_24(CURRENT_LATCHES.MDR);
			break;
		case 2:
            MEMORY[CURRENT_LATCHES.MAR] = MASK7_0(CURRENT_LATCHES.MDR);
            MEMORY[CURRENT_LATCHES.MAR + 1] = MASK15_8(CURRENT_LATCHES.MDR);
			break;
		case 3:
            MEMORY[CURRENT_LATCHES.MAR] = MASK7_0(CURRENT_LATCHES.MDR);
			break;
	}
}

int read_memory(unsigned int data_size, unsigned int MAR) {
    /*
     * Lab3-2 assignment
     */
	int MemOut = 0;

	switch(data_size & 0x3) {
		case 0:
		case 1:
            MemOut = (MEMORY[CURRENT_LATCHES.MAR + 3] << 24) + \
                (MEMORY[CURRENT_LATCHES.MAR + 2] << 16) + \
                (MEMORY[CURRENT_LATCHES.MAR + 1] << 8) + \
                MEMORY[CURRENT_LATCHES.MAR];
			break;
		case 2:
            MemOut = blockSEXT((MEMORY[CURRENT_LATCHES.MAR + 1] << 8) + \
                MEMORY[CURRENT_LATCHES.MAR], 16);
			break;
		case 3:
            MemOut = blockSEXT(MEMORY[CURRENT_LATCHES.MAR], 8);
			break;
	}
	return MemOut;
}
