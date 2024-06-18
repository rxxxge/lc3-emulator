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

// typedef enum {
// 	BR = 0,
// 	ADD,
// 	LD,
// 	ST,
// 	JSR, 
// 	AND, 
// 	LDR, 
// 	STR, 
// 	RTI, 
// 	NOT, 
// 	LDI, 
// 	STI, 
// 	JMP, 
// 	RES, 
// 	LEA, 
// 	TRAP,
// } instruction_name_t;

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

// LC-3 Instruction type
typedef struct {
	uint16_t opcode;		
	// uint16_t BR = 0;		// Break
	// uint16_t ADD = 1;		// Add
	// uint16_t LD = 2; 		// Load
	// uint16_t ST = 3;		// Store
	// uint16_t JSR = 4;		// Jump register
	// uint16_t AND = 5;		// Bitwise and
	// uint16_t LDR = 6;		// Load register
	// uint16_t STR = 7;		// Store register
	// uint16_t RTI = 8;		// Unused
	// uint16_t NOT = 9;		// Bitwise not
	// uint16_t LDI = 10;		// Load indirect
	// uint16_t STI = 11;		// Store indirect
	// uint16_t JMP = 12;		// Jump
	// uint16_t RES = 13;		// Reserved (unused)
	// uint16_t LEA = 14;		// Load effective address
	// uint16_t TRAP = 15;	// Execute trap
} instruction_t;

// LC-3 Machine object
typedef struct {
	emulator_state_t state;
	uint16_t ram[MEMORY_CAP];	// Memory with 65536 locations (128KB)
	uint16_t R[REG_COUNT];		// Data registers R0-R7
	uint16_t PC;				// Program counter
	instruction_t inst;			// Currently executing instruction
} lc3_t;


bool init_lc3(lc3_t *lc3) {
	const uint16_t entry_point = 0x3000;

	// Check size of input file 
	// TODO
	// =====================

	lc3->state = RUNNING;
	lc3->PC = entry_point;

	return true;
}


void emulate_instruction(lc3_t *lc3) {
	// Get next opcode from RAM
	// First fout bits are the instruction to execute 
	// 		rest are parameters
	lc3->inst.opcode = lc3->ram[lc3->PC];
	lc3->PC++;

	switch(lc3->inst.opcode >> 12) {
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

int main(void) {


	// Initialize LC-3 Machine
	lc3_t lc3 = {0};
	if(!init_lc3(&lc3)) exit(EXIT_FAILURE);


	while (lc3.state != QUIT) {
		emulate_instruction(&lc3);
	}

	exit(EXIT_SUCCESS);
}