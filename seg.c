/*
* seg.c
* Name: Anesu Gavhera
* Date: April 21, 2021
* Purpose: implementation and interface for Seg_T data structure,
*          will use Hanson Sequence data structure to organize each individual
*          segment and overall group of segments. Seg_Ts will consist of a
*          struct with 2 sequences, one to keep track of and hold mapped
*          segments, and another to keep track of the addresses of unmapped
*          segments.
*/

#include "seg.h"



//gavin sucks balls

struct Seg_T{
    Seq_T mapped;
    Seq_T unmapped;
};


Seg_T new_segt()
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

void seg_free(Seg_T *segments)
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



Seg_T read_program(FILE *fp){

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


uint32_t get_instr(Seg_T segments, uint32_t segment, uint32_t index)
{
    Seq_T seg = (Seq_T)Seq_get(segments->mapped, segment);
    return (uint32_t)(uintptr_t)Seq_get(seg , index);
}



uint32_t  map_segment(Seg_T segments, uint32_t size)
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


void unmap_segment(Seg_T segments, uint32_t index)
{
    Seq_T seg = Seq_get(segments->mapped, index);

    Seq_free(&seg);
    Seq_put(segments->mapped, index, NULL);
    Seq_addhi(segments->unmapped, (void *)(uintptr_t)index);
}


Seq_T Seg_get_index(Seg_T segments, uint32_t index)
{
        return (Seq_T)Seq_get(segments->mapped, index);
}
