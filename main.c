#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
    // For testing

    initVM();

    Chunk chunk;
    initChunk(&chunk);
    
    
    writeConstant(&chunk, 1.2, 123);
    writeConstant(&chunk, 3.4, 123);


    writeChunk(&chunk, OP_ADD, 123);

    writeConstant(&chunk, 5.6, 123);


    writeChunk(&chunk, OP_DIVIDE, 123);

    writeChunk(&chunk, OP_NEGATE, 123);

    writeChunk(&chunk, OP_RETURN, 123);



    disassembleChunk(&chunk, " test chunk ");
    interpret(&chunk);
    freeVM();
    freeChunk(&chunk);

}