/*
* instr.c
* Name: Anesu Gavhera
* Date: April 21, 2021
* Purpose: has all of the instructions for all of the functions that are
*          called in the 32 bit words that are read in
*/

#include "instr.h"
#include <stdlib.h>
#include <seq.h>
#include <stdio.h>
#include <assert.h>
#include <bitpack.h>
#include "seg.h"



struct Um{
    uint32_t registers[8];
    Seg_T segments;
    unsigned ra;
    unsigned rb;
    unsigned rc;
    unsigned op;
    uint32_t next_instr;
    uint32_t counter;
    uint32_t len;
    uint32_t value;

};


typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV,
} Um_opcode;


unsigned get_opcode(uint32_t instruction)
{
	//return (unsigned)Bitpack_getu(instruction, 4, 28);
  unsigned temp = instruction >> 28;
  return temp;
}

void set_reg(uint32_t instruction, Um program)
{
    uint32_t temp = instruction;
    temp = temp << 23;
    temp = temp >> 29;
    program->ra = (unsigned)temp;
    temp = instruction;
    temp = temp << 26;
    temp = temp >> 29;
    program->rb = (unsigned)temp;
    temp = instruction;
    temp = temp  << 29;
    temp = temp >> 29;
    program->rc = (unsigned)temp;

}

void set_val(uint32_t instruction, Um program)
{
    uint32_t temp = instruction;
    temp = temp << 4;
    temp = temp >> 29;
    program->ra = (unsigned)temp;
    temp = instruction;
    temp = temp << 7;
    temp = temp >> 7;
    program->value = (unsigned)temp;
}

int run_instructions(Seg_T segments)
{
    Um program = malloc(sizeof(struct Um));

    program->segments = segments;

    for (int i = 0; i < 8; i++) {
        program->registers[i] = 0;
    }

    Seq_T seg0 = Seg_get_index(segments, 0);

    uint32_t test = get_instr(segments, 0, 0);

    (void) test;
    program->len = Seq_length(seg0);

    program->counter = 0;

    if(program->counter > program->len){
        exit(EXIT_FAILURE);
    }

    while(program->counter < program->len){
        program->next_instr = (uint32_t)(uintptr_t)
                    get_instr(program->segments, 0, program->counter);

        program->counter += 1;

        run_next_instr(program);
    }

    halt(program);

    return 1;
}


void run_next_instr(Um program)
{
    switch (get_opcode(program->next_instr)) {

        case CMOV:
            set_reg(program->next_instr, program);
            conditional_move(program);
            break;

        case SLOAD:
            set_reg(program->next_instr, program);

            program->registers[program->ra] = get_instr(program->segments,
                    program->registers[program->rb],
                    program->registers[program->rc]);
            break;

        case SSTORE:
            set_reg(program->next_instr, program);

            Seq_T segment = Seg_get_index(program->segments,
                            program->registers[program->ra]);

            Seq_put(segment, program->registers[program->rb],
                        (void *)(uintptr_t)program->registers[program->rc]);
            break;



        case ADD:
            set_reg(program->next_instr, program);
            add(program);
            break;

        case MUL:
            set_reg(program->next_instr, program);
            mult(program);
            break;

        case DIV:
            set_reg(program->next_instr, program);
            divide(program);
            break;

        case NAND:
            set_reg(program->next_instr, program);
            bit_nand(program);
            break;

        case HALT:
            halt(program);
            exit(EXIT_SUCCESS);
            break;

        case MAP:
            set_reg(program->next_instr, program);
            program->registers[program->rb] = map_segment(program->segments,
                                            program->registers[program->rc]);
            break;

        case UNMAP:
            set_reg(program->next_instr, program);
            if(program->registers[program->rc] != 0){
                unmap_segment(program->segments, program->registers
                  [program->rc]);
            }else{
                exit(EXIT_FAILURE);
            }
            break;

        case OUT:
            set_reg(program->next_instr, program);
            output(program);
            break;

        case IN:
            set_reg(program->next_instr, program);
            input(program);
            break;

        case LOADP:
            set_reg(program->next_instr, program);
            load_program(program);
            break;

        case LV:
            set_val(program->next_instr, program);
            load_value(program);
            break;

        default :
    		fprintf(stderr, "Improper instruction\n");
    		halt(program);
    	}


}

void conditional_move(Um program)
{
    if(program->registers[program->rc] != 0){
        program->registers[program->ra] = program->registers[program->rb];
    }

}


void segmented_load(Um program)
{
    program->registers[program->ra] = get_instr(program->segments,
            program->registers[program->rb], program->registers[program->rc]);
}


void segmented_store(Um program)
{

    Seq_T segment = Seg_get_index(program->segments,
                    program->registers[program->ra]) ;


    Seq_put(segment, program->registers[program->rb],
                (void *)(uintptr_t)program->registers[program->rc]);
}


void add(Um program)
{
    program->registers[program->ra] = (program->registers[program->rb] +
                                        program->registers[program->rc]);
}


void mult(Um program)
{
    program->registers[program->ra] = (program->registers[program->rb] *
                                        program->registers[program->rc]);
}

void divide(Um program)
{
    program->registers[program->ra] = (program->registers[program->rb] /
                                        program->registers[program->rc]);
}

void bit_nand(Um program)
{
    uint32_t b, c;

    b = program->registers[program->rb];
    c = program->registers[program->rc];

    program->registers[program->ra] = ~(b & c);

}

void halt(Um program)
{
  program->ra = 0;
  program->rb = 0;
  program->rc = 0;

  seg_free(&program->segments);
  free(program);
  exit(0);

}


void output(Um program)
{
    if(program->rc <= 255){
        putchar(program->registers[program->rc]);
    }else{
        exit(EXIT_FAILURE);
    }


}

void input(Um program)
{
    int in = getchar();

    if(in == EOF){
        program->registers[program->rc] = ~(uint32_t)0;
    }else{
        program->registers[program->rc] = (uint32_t)in;
    }
}

void load_program(Um program)
{
    Seq_T current;
    uint32_t len, index;

    if(program->registers[program->rb] != 0){
        index = program->registers[program->rb];

        current = Seg_get_index(program->segments, index);

        len = Seq_length(current);

        unmap_segment(program->segments, 0);

        map_segment(program->segments, len);

        for(uint32_t i = 0; i < len; i ++){
            Seq_put(Seg_get_index(program->segments, 0), i,
                                            Seq_get(current, i));
        }
        program->len = len;
    }
    program->counter = program->registers[program->rc];

}

void load_value(Um program)
{
    program->registers[program->ra] = program->value;
}
