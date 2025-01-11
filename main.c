#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
    // For testing
    Chunk chunk;
    initChunk(&chunk);


    writeConstant(&chunk, 1.2, 123);

    writeChunk(&chunk, OP_RETURN, 123);

    writeConstant(&chunk, 1324115151.1412414, 123);

    for (int i = 0; i < 300; i++) {
    writeConstant(&chunk, i, 123); 
}

    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);

}