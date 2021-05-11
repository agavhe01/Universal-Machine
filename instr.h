/*
* instr.c
* Names: Anesu Gavhera
* Purpose: holds all of the function declerations for instr.h
*/

#ifndef INSTR_INCLUDED
#define INSTR_INCLUDED

#include "seg.h"


typedef struct Um *Um;

/*
* Funcion name: run_instructions
* inputs : Segments struct containing original program.
* outputs : there are no outputs
* Purpose: reads Seg_T into Um data structure, and loops through the program,
*          calling run_next_instr for every instruction
*/
int run_instructions(Seg_T segments);

/*
* Funcion name: conditional_move
* inputs : the Um struct that contains all the segments
* outputs : there are no outputs
* Purpose: calls individual instruction functions - The Um struct holds all
*         registers and the current instruction, so it is the only thing
*         that needs to be passed in for each function is a Um struct.
*/
void run_next_instr(Um program);


/*
* Function : conditional_move
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose: moves $r[A] into $r[B] if $r[C] != 0
*/
void conditional_move(Um program);


/*
* Function : segmented_load
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose: moves uint32_t in $m[$r[B]][$r[C]] into $r[A]
*/
void segmented_load(Um program);

/*
* Function : segmented_store
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose: moves value in $r[C] into $m[$r[A]][$r[B]]
*/
void segmented_store(Um program);


/*
* function : add
* inputs : the Um struct that contains all program data
* outputs : none
* purpose: moves  $r[B] + $r[C] into  $r[A]
*/
void add(Um program);


/*
* Function : mult
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose : moves  $r[B] multiplied by  $r[C] into  $r[A]
*/
void mult(Um program);



/*
* Function : divide
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose : moves  $r[B] divided by  $r[C] into  $r[A]
*/
void divide(Um program);


/*
* Function : bit_nand
* inputs : the Um struct that contains all program data
* outputs : none
* moves  $r[B] bitwise nand $r[C] into  $r[A]
*/
void bit_nand(Um program);


/*
* Function: halt
* inputs : the Um struct that contains all program data
* outputs : none
* purpose halts program, frees all dynamic data structures used
*/
void halt(Um program);

/*
* Function: output
* inputs : the Um struct that contains all program data
* outputs : none
* purpose: The value in $r[C] is written to the I/O. Only values from 0 to 255
* are allowed.
*/
void output(Um program);

/*
* Function : input
* inputs : the Um struct that contains all program data
* outputs : none
*  Purpose: waits for input, When input arrives, $r[C] is loaded with the
*           input, which must be a value from 0 to 255. If the end of input has
*           been signaled, then $r[C] is loaded with a full 32-bit word in
*           which every bit is 1.
*/
void input(Um program);

/*
* Function: load_program
* inputs : the Um struct that contains all program data
* outputs : none
* Segment $m[$r[B]] is duplicated, and the duplicate replaces $m[0], which is
* abandoned. The program counter is set to point to $m[0][$r[C]]. If $r[B] = 0,
*the load-program operation is expected to be extremely quick.
*/
void load_program(Um program);

/*
*  function : load_value
*  inputs : the Um struct that contains all program data
*  outputs : none
*            Sets $r[x] to value contained in the least significant
*            25 bits of the current instruction where x is represented by
*            the 3 bits between 25 and 28 of that instruction
*/
void load_value(Um program);

#endif
