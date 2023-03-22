/***************************************************************/
/*                                                             */
/*                   RISCV-LC System                           */
/*                                                             */
/*                     CEG3420 Lab3                            */
/*                 cbai@cse.cuhk.edu.hk                        */
/*           The Chinese University of Hong Kong               */
/*                                                             */
/***************************************************************/


#include "functions.h"


/* 
 * acquire the corresponding value of inst[high_bit:low_bit]
 */
unsigned int mask_val(int inst, int high_bit, int low_bit) {
    if (high_bit < low_bit || high_bit > 31 || low_bit < 0)
        error("illegal usage of function 'mask_val'\n");
    int i, mask = 0;
    for (i = low_bit; i <= high_bit; i++)
        mask += (1 << i);
    return ((mask & inst) >> low_bit);
}


/*
 * sign extend the `imm` to 32 bits
 * `width` is the bit width before `imm` is extended
 * Example:
 *      if we want to do the sign extension with a 12-bit
 *      immediate value `imm`, you need to call `sext` with
 *      sext(imm, 12);
 *
 */
int sext_unit(int imm, int width) {
    if (imm >> (width - 1)) {
        /* sign bit is 1 */
        return (0xFFFFFFFF & imm) | (0xFFFFFFFF << width);
    }
    else {
        /* sign bit is 0 */
        return imm;
    }
}


int rs2_mux(int rs2mux, int rs2, int imm12) {
    switch(rs2mux) {
        case 0:
            return rs2;
        case 1:
            return imm12;
        default:
            error("unknown RS2MUX = %d\n", rs2mux);
    }
    CANNOT_TOUCH;
}


int mar_mux(int marmux, int offset, int operand) {
    switch (marmux) {
        case 0:
            return offset;
        case 1:
            return operand;
        default:
            error("unknown MARMUX = %d\n", marmux);
    }
    CANNOT_TOUCH;
}


int alu(int funct3, int funct7, int rs1, int operand) {
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


int addr2_mux(int addr2mux, int zero, int imm12, int simm12, int jimm20) {
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
            error("unknown ADDR2MUX = %d\n", addr2mux);
    }
    CANNOT_TOUCH;
}


int addr1_mux(int addr1mux, int zero, int PC, int base, int bimm12) {
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
            error("unknown ADDR1MUX = %d\n", addr1mux);
    }
    CANNOT_TOUCH;
}


int logic_shift_20_function_unit(int value) {
    return value << 20;
}


int shift_function_unit(int funct3, int funct7, int rs1, int operand) {
    switch(funct3) {
        case 1:
            return rs1 << operand;
        case 5:
            switch(funct7) {
                case 0b0000000:
                    return rs1 >> operand;
                case 0b0100000:
                    return sext_unit(rs1 >> operand, 31 - operand);
            }
    }
    return rs1 << operand;
}


int pc_mux(int pcmux, int PCAdd4, int bus) {
    switch (pcmux) {
        case 0:
            return PCAdd4;
        case 1:
            return bus;
        default:
            error("unknown PCMUX = %d\n", pcmux);
    }
    CANNOT_TOUCH;
}


int s_format_imm_gen_unit(int imm4_0, int imm11_5) {
    return (imm11_5 << 5) + imm4_0;
}


int b_format_imm_gen_unit(int imm11, int imm4_1, int imm10_5, int imm12) {
    return (imm12 << 12) + (imm11 << 11) + (imm10_5 << 5) + (imm4_1 << 1);
}


int j_format_imm_gen_unit(int imm20, int imm10_1, int imm11, int imm19_12) {
    return (imm20 << 20) + (imm19_12 << 12) + (imm11 << 11) + (imm10_1 << 1);
}


int mdr_mux(int mdrmux, int mem_val, int bus) {
    switch(mdrmux) {
        case 0:
            return mem_val;
        case 1:
            return bus;
        default:
            error("unknown MIO_EN = %d\n", mdrmux);
    }
    CANNOT_TOUCH;
}


int rs1_en(int rs1_en, int rs1) {
    if (rs1_en == 1) {
        return rs1;
    } else {
        return 0;
    }
}


int rs2_en(int rs2_en, int rs2) {
    if (rs2_en == 1) {
        return rs2;
    } else {
        return 0;
    }
}


int alu_shift_mux(int funct3, int value_of_alu, int value_of_shift_function_unit) {
    switch(funct3) {
        case 0:
        case 4:
        case 6:
        case 7:
            return value_of_alu;
        case 1:
        case 5:
            return value_of_shift_function_unit;
    }
    return value_of_alu;
}


int blockBMUX(int bmux, int zero, int B) {
	switch(bmux) {
		case 0:
			return zero;
		case 1:
			return B;
        default:
            error("unknown LD_BEN = %d\n", bmux);
	}
	CANNOT_TOUCH;
}


int compare_function_unit(int funct3, int rs1, int rs2, int zero) {
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
    // RISC-V LC does not support bltu & bgeu
    CANNOT_TOUCH;
}


int datasize_mux(unsigned int data_size, int funct3, int zero) {
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
