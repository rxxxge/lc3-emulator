#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define MEMORY_CAP (1 << 16)	// LC-3 Memory capacity
#define REG_COUNT 8				// Number of registers R0-R7

// VM states
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

// LC-3 Machine object
typedef struct {
	emulator_state_t state;
	const char *rom_name;
	uint16_t ram[MEMORY_CAP];	// Memory with 65536 locations (128KB)
	uint16_t R[REG_COUNT];		// Data registers R0-R7
	uint16_t PC;				// Program counter
	uint16_t opcode;
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


#ifdef DEBUG
// Function to print debug info during execution of instructions
void print_debug_info(lc3_t *lc3) {
	printf("Address: 0x%04X, Opcode: 0x%04X Desc: ", lc3->PC - 1, lc3->opcode);
	switch(lc3->opcode >> 12) {
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
	lc3->opcode = lc3->ram[lc3->PC];
	lc3->PC++;

#ifdef DEBUG
	print_debug_info(lc3);
#endif

	switch(lc3->opcode >> 12) {
	case ADD:
		// TODO
		break;
	case AND:
		// TODO
		break;
	case NOT:
		// TODO
		break;
	case BR:
		// TODO
		break;
	case JMP:
		// TODO
		break;
	case JSR:
		// TODO
		break;
	case LD:
		// TODO
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