/*
* seg.c
* Name: Anesu Gavhera
* Date: April 21, 2021
* Purpose: holds the function declerations for the seg.c file.
*/



#ifndef SEG_H_INCLUDED
#define SEG_H_INCLUDED



typedef struct Seg_T *Seg_T;

#include <stdint.h>
#include <stdlib.h>
#include "seq.h"
#include <stdio.h>
#include <assert.h>
#include "bitpack.h"


/*
  function: new_segt
  purpose: The purpose of the function is to create and return a new segment
  inuts: there are no inputs
  outputs: returns a new instance of a segments struct.
*/
Seg_T new_segt();

/*
  function: read_program
  purpose: this function reads all of the 32 bit words from a file into a
           segments struct.
  inuts: The only input is the FILE
  outputs: The output is an segments struct
*/
Seg_T read_program(FILE *input);

/*
  function: get_instr
  purpose:  takes in a segments struct, and 2 variables to determine
            the location of an instruction, and returns the instruction at
            the given index.
  inuts: Segments struct and uint32_ts that represent both segment index and
        instruction index
  outputs: The output is an instruction
*/
uint32_t get_instr(Seg_T segments, uint32_t segment, uint32_t index);

/*
  function: map_segment
  purpose: this function takes in the segments, a size, and creates a new
  segment of that size
  inputs: Segments struct, and uint32_t size
  outputs: The output is an uint32_t with the index of the mapped segment
*/
uint32_t  map_segment(Seg_T segments, uint32_t size);


/*
  function: unmap_segment
  purpose: this function takes in the segments and an segment index, and
  unmaps the segment at that index.
  inputs: Segments struct, and uint32_t segmeent index
  outputs: none
*/
void unmap_segment(Seg_T segments, uint32_t seg_id);


/*
  function: unmap_segment
  purpose: this function takes in a segments struct and frees all data within
  inuts: The only input is a pointer to asegments struct
  outputs: none
*/
void seg_free(Seg_T *segments);


/*
  function: unmap_segment
  purpose: this function takes in the segments and an segment index, and
  returns the sequence of instructions at that index.
  inputs: Segments struct, and uint32_t segmeent index
  outputs: Sequence that holds a program
*/
Seq_T Seg_get_index(Seg_T segments, uint32_t index);


#endif
