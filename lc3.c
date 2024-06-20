#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define MEMORY_CAP 0xFFFF		// LC-3 Memory capacity
#define REG_COUNT 8				// Number of registers R0-R7

// LC-3 states
typedef enum {
	QUIT,
	RUNNING,
} emulator_state_t;

enum
{
    BR = 0, /* branch */
    ADD,    /* add  */
    LD,     /* load */
    ST,     /* store */
    JSR,    /* jump register */
    AND,    /* bitwise and */
    LDR,    /* load register */
    STR,    /* store register */
    RTI,    /* unused */
    NOT,    /* bitwise not */
    LDI,    /* load indirect */
    STI,    /* store indirect */
    JMP,    /* jump */
    RES,    /* reserved (unused) */
    LEA,    /* load effective address */
    TRAP    /* execute trap */
};

typedef struct {
	uint16_t DR;			// Destination register. One of R0-R7
	uint16_t imm5;			// A 5-bit immediate value
	uint16_t SR, SR1, SR2;	// Source register. one of R0..R7
	uint16_t PCoffset9;		// A 9-bit value. Used with PC+offset addressing mode
	uint16_t PCoffset11;	// 11-bit value. used with the JSR opcode to compute the target address of a subroutine call
	uint16_t baseR;			// Base Register. Used with 6-bit offset to compute Base+offset addresses.
} parameters_t;

// LC-3 Machine object
typedef struct {
	emulator_state_t state;
	const char *rom_name;
	uint16_t ram[MEMORY_CAP];	// Memory with 65536 locations (128KB)
	uint16_t R[REG_COUNT];		// Data registers R0-R7
	uint16_t PC;				// Program counter
	uint8_t N, Z, P;			// Condition codes
	uint16_t inst;				// Instruction
	parameters_t param;			// Parameters inside instruction
	uint16_t opcode;			// Opcode
} lc3_t;


bool init_lc3(lc3_t *lc3, const char *rom_name) {
	const uint16_t entry_point = 0x3000;

	FILE *rom = fopen(rom_name, "rb");
	if (!rom) {
		fprintf(stderr, "Unknown/invalid file: %s.\n", rom_name);
		return false;
	}


	// Check size of input file 
	fseek(rom, 0, SEEK_END);
	const size_t rom_size = ftell(rom);
	const size_t max_size = sizeof lc3->ram - entry_point;
	rewind(rom);

	if (rom_size > max_size) {
		fprintf(stderr, "Rom file %s is too big. Rom file size: %lu. Max size allowed: %lu.\n",
				rom_name, (unsigned long)rom_size, (unsigned long)max_size);
		return false;
	}

	if (fread(&lc3->ram[entry_point], rom_size, 1, rom) != 1) {
		fprintf(stderr, "Could not read Rom file %s into LC-3 memory.\n", rom_name);
		return false;
	}

	fclose(rom);

	lc3->state = RUNNING;
	lc3->PC = entry_point;
	lc3->rom_name = rom_name;

	return true;
}

void handle_input(lc3_t *lc3) {
	(void)lc3;
	return;
}

// Extend sign
uint16_t sign_extend(uint16_t value, uint16_t bit_count) {
	// Check if "value" is negative. Left most bit is 1. 
	//	Extend with 1's if so, otherwise with zeros.
	if ((value >> (bit_count - 1)) & 1) 
		value |= 0xFFFF << bit_count;
	return value;
}

// Set condition codes
void setcc(lc3_t *lc3) {
	if (lc3->R[lc3->param.DR] == 0) {
		lc3->N = 0;
		lc3->Z = 1;
		lc3->P = 0;
	} else if (lc3->R[lc3->param.DR] >> 15) { // left most bit indicates negative
		lc3->N = 1;
		lc3->Z = 0;
		lc3->P = 0;
	} else {
		lc3->N = 0;
		lc3->Z = 0;
		lc3->P = 1;
	}
}


#ifdef DEBUG
// Function to print debug info during execution of instructions
void print_debug_info(lc3_t *lc3) {
	printf("Address: 0x%04X, Opcode: 0x%04X Desc: ", lc3->PC - 1, lc3->opcode);
	switch(lc3->opcode) {
	case ADD:
		printf("Execute ADD instruction\n");
		break;
	case AND:
		printf("Execute AND instruction\n");
		break;
	case NOT:
		printf("Execute NOT instruction\n");
		break;
	case BR:
		printf("Execute BR instruction\n");
		break;
	case JMP:
		printf("Execute JPM instruction\n");
		break;
	case JSR:
		printf("Execute JSR instruction\n");
		break;
	case LD:
		printf("Execute LD instruction\n");
		break;
	case LDI:
		printf("Execute LDI instruction\n");
		break;
	case LDR:
		printf("Execute LDR instruction\n");
		break;
	case LEA:
		printf("Execute LEA instruction\n");
		break;
	case ST:
		printf("Execute ST instruction\n");
		break;
	case STI:
		printf("Execute STI instruction\n");
		break;
	case STR:
		printf("Execute STR instruction\n");
		break;
	case TRAP:
		printf("Execute TRAP instruction\n");
		break;
	case RES:
	case RTI:
	default:
		printf("Invalid/unsuported opcode\n");
		break;
	}
}

#endif


void emulate_instruction(lc3_t *lc3) {
	// Get next opcode from RAM
	// First fout bits are the instruction to execute 
	// 		rest are parameters
	lc3->inst = lc3->ram[lc3->PC];
	lc3->opcode = lc3->inst >> 12;
	lc3->PC++;

#ifdef DEBUG
	print_debug_info(lc3);
#endif

	switch(lc3->opcode) {
	case ADD:
		// The second source operand and the contents of SR1 are
		// 		bitwise ADDed and result is stored in DR 
		lc3->param.DR = (lc3->inst >> 9) & 0x7;
		lc3->param.SR1 = (lc3->inst >> 6) & 0x7;
		// Check whether bit[5] on or off
		if ((lc3->inst >> 5) & 1) {
			lc3->param.imm5 = lc3->inst & 0x1F; 
			lc3->R[lc3->param.DR] = lc3->R[lc3->param.SR1] + sign_extend(lc3->param.imm5, 5);
		} else {
			lc3->param.SR2 = lc3->inst & 0x7;
			lc3->R[lc3->param.DR] = lc3->R[lc3->param.SR1] + lc3->R[lc3->param.SR2];
		}
		setcc(lc3);

		break;
	case AND:
		// The second source operand and the contents of SR1 are
		// 		bitwise ANDed and result is stored in DR 
		lc3->param.DR = (lc3->inst >> 9) & 0x7;
		lc3->param.SR1 = (lc3->inst >> 6) & 0x7;
		// Check whether bit[5] on or off
		if ((lc3->inst >> 5) & 1) {
			lc3->param.imm5 = lc3->inst & 0x1F; 
			lc3->R[lc3->param.DR] = lc3->R[lc3->param.SR1] & sign_extend(lc3->param.imm5, 5);
		} else {
			lc3->param.SR2 = lc3->inst & 0x7;
			lc3->R[lc3->param.DR] = lc3->R[lc3->param.SR1] & lc3->R[lc3->param.SR2];
		}
		setcc(lc3);

		break;
	case NOT:
		// The bit-wise complement of the contents of SR is stored in DR
		lc3->param.DR = (lc3->inst >> 9) & 0x7;
		lc3->param.SR = (lc3->inst >> 6) & 0x7;
		lc3->R[lc3->param.DR] = ~lc3->R[lc3->param.SR];
		setcc(lc3);

		break;
	case BR:
		uint8_t n = (lc3->inst >> 11) & 1; // Extract 11th bit value from instruction
		uint8_t z = (lc3->inst >> 11) & 1; // ...
		uint8_t p = (lc3->inst >> 11) & 1; // ...
		lc3->param.PCoffset9 = lc3->inst & 0x1FF;
		// If any of the condition codes tested is set, the program branches to the location
		// 		specified by adding the sign-extended PCoffset9 field to the incremented PC.
		if ((n && lc3->N) || (z && lc3->Z) || (p && lc3->P))
			lc3->R[lc3->PC] += lc3->param.PCoffset9;

		break;
	case JMP:
		// The program unconditionally jumps to the location specified by the contents of
		//		the base register
		lc3->param.baseR = (lc3->inst >> 6) & 0x7;
		lc3->PC = lc3->param.baseR;

		break;
	case JSR:
		// First, the incremented PC is saved in R7. This is the linkage back to the calling
		// 		routine. Then the PC is loaded with the address of the first instruction of the
		// 		subroutine, causing an unconditional jump to that address.
		lc3->param.PCoffset11 = lc3->inst & 0x7FF;
		lc3->param.baseR = (lc3->inst >> 6) & 0x7; 

		lc3->R[7] = lc3->PC;
		if ((lc3->inst >> 11) & 1)
			lc3->R[lc3->PC] += sign_extend(lc3->param.PCoffset11, 11);
		else
			lc3->PC = lc3->param.baseR;

		break;
	case LD:
		// The contents of memory at address PC + SEXT[PCoffset9]
		// 		are loaded into DR
		lc3->param.DR = (lc3->inst >> 9) & 0x7;
		lc3->param.PCoffset9 = lc3->inst & 0x1FF;

		lc3->R[lc3->param.DR] = lc3->ram[lc3->PC + sign_extend(lc3->param.PCoffset9, 9)];
		setcc(lc3);

		break;
	case LDI:
		// TODO
		break;
	case LDR:
		// TODO
		break;
	case LEA:
		// TODO
		break;
	case ST:
		// TODO
		break;
	case STI:
		// TODO
		break;
	case STR:
		// TODO
		break;
	case TRAP:
		// TODO
		break;
	case RES:
	case RTI:
	default:
		// Invalid opcode
		fprintf(stderr, "Invalid/unsuported opcode. (%i)\n", lc3->opcode);
		break;
	}

}

int main(int argc, char *argv[]) {
	// Default usage message
	if (argc < 2) {
		fprintf(stderr, "Usage %s <rom_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Initialize LC-3 Machine
	lc3_t lc3 = {0};
	const char *rom_name = argv[1];
	if(!init_lc3(&lc3, rom_name)) exit(EXIT_FAILURE);

	while (lc3.state != QUIT) {
		// Handle user input
		handle_input(&lc3);

		// Emulate LC-3 Instructions
		emulate_instruction(&lc3);

	}

	exit(EXIT_SUCCESS);
}