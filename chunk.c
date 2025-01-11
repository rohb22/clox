#include <stdlib.h>
#include <stdio.h>
#include "chunk.h"
#include "memory.h"


void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lineCapacity = 0;
    chunk->lineCount = 0;
    chunk->lines = NULL;
    // Array of constants
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    // Check if adding a byte would fit
    // If not grow the array by 2 times the old capacity
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity); // Would return twice the oldCapacity if not 0 else would be 8
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    // Insert the byte at the end of the array
    chunk->code[chunk->count] = byte;
    chunk->count++;

    // Check if the instruction is in the same line
    if (chunk->lineCount > 0 && chunk->lines[chunk->lineCount -1].line == line) {
        return; // same line no need to save
    }

    // Check if line array would need to be expanded
    if(chunk->lineCapacity < chunk->lineCount + 1) {
        int oldCapacity = chunk->lineCapacity;
        chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
        chunk->lines = GROW_ARRAY(LineStart, chunk->lines, oldCapacity, chunk->lineCapacity);
    }
    // Using RLE store line
    LineStart* linestart = &chunk->lines[chunk->lineCount++];
    linestart->offset = chunk->count - 1;
    linestart->line = line;

}

// Free the memory chunk
void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(LineStart, chunk->lines, chunk->lineCapacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

// Add a single constant returns the index of the constant
int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count -1;
}


// Using binary search find the line of a instruction
int getLine(Chunk* chunk, int instruction) {
    int start = 0;
    int end = chunk->lineCount -1 ;

    // loop until find
    for (;;) {
        int mid = (start + end) / 2;
        LineStart* line = &chunk->lines[mid];
        
        if(instruction < line->offset) {
            end = mid - 1;
        } else if (mid == chunk->lineCount -1 || instruction < chunk->lines[mid + 1].offset){
            return line->line;
        } else {
            start = mid + 1;
        }
    }
}

void writeConstant(Chunk* chunk, Value value, int line) {
    int index = addConstant(chunk, value);
    // check if the constants is one byte
    if(index < 256){
        // write constant as simple op_constant
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, (uint8_t)index, line);
    // if not store in 3 bytes using little endian
    } else {
        // Chunk instruction
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        // write the least significant byte ex. 0x123456 this would take the 0x56
        writeChunk(chunk, (uint8_t)(index & 0xff), line);
        // write the middle byte ex. 0x123456 this would take the 0x34
        writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);
        // write the significant byte ex. 0x123456 this would take the 0x12
        writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line);
    }
}