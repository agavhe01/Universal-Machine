/*
* um.c
* Name: Anesu Gavhera
* Date: April 21, 2021
* Purpose: organizes/checks valid input and calls main driver functions.
*/


#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <seq.h>



typedef struct Seg_T *Seg_T;
typedef struct Um *Um;


typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV,
} Um_opcode;


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

struct Seg_T{
    Seq_T mapped;
    Seq_T unmapped;
};


static inline int run_instructions(Seg_T segments);

/*
* Funcion name: conditional_move
* inputs : the Um struct that contains all the segments
* outputs : there are no outputs
* Purpose: calls individual instruction functions - The Um struct holds all
*         registers and the current instruction, so it is the only thing
*         that needs to be passed in for each function is a Um struct.
*/
static inline void run_next_instr(Um program);


/*
* Function : conditional_move
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose: moves $r[A] into $r[B] if $r[C] != 0
*/
static inline void conditional_move(Um program);


/*
* Function : segmented_load
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose: moves uint32_t in $m[$r[B]][$r[C]] into $r[A]
*/
static inline void segmented_load(Um program);

/*
* Function : segmented_store
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose: moves value in $r[C] into $m[$r[A]][$r[B]]
*/
static inline void segmented_store(Um program);


/*
* function : add
* inputs : the Um struct that contains all program data
* outputs : none
* purpose: moves  $r[B] + $r[C] into  $r[A]
*/
static inline void add(Um program);


/*
* Function : mult
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose : moves  $r[B] multiplied by  $r[C] into  $r[A]
*/
static inline void mult(Um program);



/*
* Function : divide
* inputs : the Um struct that contains all program data
* outputs : none
* Purpose : moves  $r[B] divided by  $r[C] into  $r[A]
*/
static inline void divide(Um program);


/*
* Function : bit_nand
* inputs : the Um struct that contains all program data
* outputs : none
* moves  $r[B] bitwise nand $r[C] into  $r[A]
*/
static inline void bit_nand(Um program);


/*
* Function: halt
* inputs : the Um struct that contains all program data
* outputs : none
* purpose halts program, frees all dynamic data structures used
*/
static inline void halt(Um program);

/*
* Function: output
* inputs : the Um struct that contains all program data
* outputs : none
* purpose: The value in $r[C] is written to the I/O. Only values from 0 to 255
* are allowed.
*/
static inline void output(Um program);

/*
* Function : input
* inputs : the Um struct that contains all program data
* outputs : none
*  Purpose: waits for input, When input arrives, $r[C] is loaded with the
*           input, which must be a value from 0 to 255. If the end of input has
*           been signaled, then $r[C] is loaded with a full 32-bit word in
*           which every bit is 1.
*/
static inline void input(Um program);

/*
* Function: load_program
* inputs : the Um struct that contains all program data
* outputs : none
* Segment $m[$r[B]] is duplicated, and the duplicate replaces $m[0], which is
* abandoned. The program counter is set to point to $m[0][$r[C]]. If $r[B] = 0,
*the load-program operation is expected to be extremely quick.
*/
static inline void load_program(Um program);

/*
*  function : load_value
*  inputs : the Um struct that contains all program data
*  outputs : none
*            Sets $r[x] to value contained in the least significant
*            25 bits of the current instruction where x is represented by
*            the 3 bits between 25 and 28 of that instruction
*/
static inline void load_value(Um program);

static inline Seg_T new_segt();

/*
  function: read_program
  purpose: this function reads all of the 32 bit words from a file into a
           segments struct.
  inuts: The only input is the FILE
  outputs: The output is an segments struct
*/
static inline Seg_T read_program(FILE *input);

/*
  function: get_instr
  purpose:  takes in a segments struct, and 2 variables to determine
            the location of an instruction, and returns the instruction at
            the given index.
  inuts: Segments struct and uint32_ts that represent both segment index and
        instruction index
  outputs: The output is an instruction
*/
static inline uint32_t get_instr(Seg_T segments, uint32_t segment, uint32_t index);

/*
  function: map_segment
  purpose: this function takes in the segments, a size, and creates a new
  segment of that size
  inputs: Segments struct, and uint32_t size
  outputs: The output is an uint32_t with the index of the mapped segment
*/
static inline uint32_t  map_segment(Seg_T segments, uint32_t size);


/*
  function: unmap_segment
  purpose: this function takes in the segments and an segment index, and
  unmaps the segment at that index.
  inputs: Segments struct, and uint32_t segmeent index
  outputs: none
*/
static inline void unmap_segment(Seg_T segments, uint32_t seg_id);


/*
  function: unmap_segment
  purpose: this function takes in a segments struct and frees all data within
  inuts: The only input is a pointer to asegments struct
  outputs: none
*/
static inline void seg_free(Seg_T *segments);


/*
  function: unmap_segment
  purpose: this function takes in the segments and an segment index, and
  returns the sequence of instructions at that index.
  inputs: Segments struct, and uint32_t segmeent index
  outputs: Sequence that holds a program
*/
static inline Seq_T Seg_get_index(Seg_T segments, uint32_t index);

//gavin
int main(int argc, char *argv[]){

    FILE *input;

    if (argc != 2){
		  fprintf(stderr, "you are missing a valid input file \n");
          return 1;
    }else{
        input = fopen(argv[1], "r");
        Seg_T seg0 = read_program(input);
        fclose(input);
        return run_instructions(seg0);

    }
}

/*
* instr.c
* HW 6 um
* Names: Luca Conetta (lconet01) & Jack Duboc (jduboc01)
* Date: April 21, 2021
* Purpose: has all of the instructions for all of the functions that are
*          called in the 32 bit words that are read in
*/







static inline unsigned get_opcode(uint32_t instruction)
{
	//return (unsigned)Bitpack_getu(instruction, 4, 28);
  unsigned temp = instruction >> 28;
  return temp;
}

static inline void set_reg(uint32_t instruction, Um program)
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

static inline void set_val(uint32_t instruction, Um program)
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

static inline int run_instructions(Seg_T segments)
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


static inline void run_next_instr(Um program)
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

static inline void conditional_move(Um program)
{
    if(program->registers[program->rc] != 0){
        program->registers[program->ra] = program->registers[program->rb];
    }

}


static inline void segmented_load(Um program)
{
    program->registers[program->ra] = get_instr(program->segments,
            program->registers[program->rb], program->registers[program->rc]);
}


static inline void segmented_store(Um program)
{

    Seq_T segment = Seg_get_index(program->segments,
                    program->registers[program->ra]) ;


    Seq_put(segment, program->registers[program->rb],
                (void *)(uintptr_t)program->registers[program->rc]);
}


static inline void add(Um program)
{
    program->registers[program->ra] = (program->registers[program->rb] +
                                        program->registers[program->rc]);
}


static inline void mult(Um program)
{
    program->registers[program->ra] = (program->registers[program->rb] *
                                        program->registers[program->rc]);
}

static inline void divide(Um program)
{
    program->registers[program->ra] = (program->registers[program->rb] /
                                        program->registers[program->rc]);
}

static inline void bit_nand(Um program)
{
    uint32_t b, c;

    b = program->registers[program->rb];
    c = program->registers[program->rc];

    program->registers[program->ra] = ~(b & c);

}

static inline void halt(Um program)
{
  program->ra = 0;
  program->rb = 0;
  program->rc = 0;

  seg_free(&program->segments);
  free(program);
  exit(0);

}


static inline void output(Um program)
{
    if(program->rc <= 255){
        putchar(program->registers[program->rc]);
    }else{
        exit(EXIT_FAILURE);
    }


}

static inline void input(Um program)
{
    int in = getchar();

    if(in == EOF){
        program->registers[program->rc] = ~(uint32_t)0;
    }else{
        program->registers[program->rc] = (uint32_t)in;
    }
}

static inline void load_program(Um program)
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

static inline void load_value(Um program)
{
    program->registers[program->ra] = program->value;
}

/*
* seg.c
* HW 6 um
* Names: Luca Conetta (lconet01) & Jack Duboc (jduboc01)
* Date: April 21, 2021
* Purpose: implementation and interface for Seg_T data structure,
*          will use Hanson Sequence data structure to organize each individual
*          segment and overall group of segments. Seg_Ts will consist of a
*          struct with 2 sequences, one to keep track of and hold mapped
*          segments, and another to keep track of the addresses of unmapped
*          segments.
*/



//gavin sucks balls


static inline Seg_T new_segt()
{
    Seg_T segments = malloc(sizeof(struct Seg_T));
    assert(segments);

    segments->mapped = Seq_new(0);
    segments->unmapped = Seq_new(0);
    assert(segments->mapped);
    assert(segments->unmapped);

    Seq_addhi(segments->mapped, NULL);
    return segments;
}

static inline void seg_free(Seg_T *segments)
{

    assert(segments != NULL);
    Seg_T temp = *segments;

    Seq_T to_free;
    uint32_t len = Seq_length(temp->mapped);

    for (uint32_t i = 0; i < len; i++) {
        to_free = Seq_get(temp->mapped, i);
        if (to_free != NULL) {
            Seq_free(&to_free);
        }
    }

    Seq_free(&((temp)->mapped));
    Seq_free(&((temp)->unmapped));
    free(temp);
}



static inline Seg_T read_program(FILE *fp){

    Seg_T segments = new_segt();
    uint32_t word = 0;
    int temp_num;

    int c = getc(fp);

    Seq_T temp  = Seq_new(0);

    while(c != EOF){
        word = 0;
        //temp_num = a << 24;
        for(int i = 3; i >= 0; i--){
            temp_num = c << (i * 8);
            //word = (uint32_t)Bitpack_newu((uint64_t)word, 8, (i * 8),
                                                //(uint64_t)c);
            word += temp_num;
            c = getc(fp);
        }

        Seq_addhi(temp, (void *)(uintptr_t)word);

    }

    Seq_addhi(segments->mapped, temp);
    Seq_remlo(segments->mapped);

    return segments;

}


static inline uint32_t get_instr(Seg_T segments, uint32_t segment, uint32_t index)
{
    Seq_T seg = (Seq_T)Seq_get(segments->mapped, segment);
    return (uint32_t)(uintptr_t)Seq_get(seg , index);
}



static inline uint32_t  map_segment(Seg_T segments, uint32_t size)
{
    Seq_T new_segt = Seq_new(size);

    for(uint32_t i = 0; i < size; i++){
        Seq_addhi(new_segt, 0);
    }

    uint32_t index;

    if(Seq_length(segments->unmapped) != 0){
        index = (uint32_t)(uintptr_t)Seq_remhi(segments->unmapped);

        Seq_put(segments->mapped, index, new_segt);
    }
    else{
        index = Seq_length(segments->mapped);
        Seq_addhi(segments->mapped, (void *)new_segt);
    }




    return index;

}


static inline void unmap_segment(Seg_T segments, uint32_t index)
{
    Seq_T seg = Seq_get(segments->mapped, index);

    Seq_free(&seg);
    Seq_put(segments->mapped, index, NULL);
    Seq_addhi(segments->unmapped, (void *)(uintptr_t)index);
}


static inline Seq_T Seg_get_index(Seg_T segments, uint32_t index)
{
        return (Seq_T)Seq_get(segments->mapped, index);
}
