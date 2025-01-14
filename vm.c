#include <stdio.h>

#include "common.h"
#include "vm.h"
#include "debug.h"
#include "memory.h"

VM vm;

void GROW_VM() {
    if (vm.stackCapacity == 0) {
        vm.stackCapacity = 256;
        vm.stack = GROW_ARRAY(Value, vm.stack, 0, 256);
        vm.stackTop = vm.stack;
    }
    else {
        int oldCapacity = vm.stackCapacity;
        vm.stackCapacity = GROW_CAPACITY(oldCapacity);
        int offset = vm.stackTop - vm.stack;
        vm.stack = GROW_ARRAY(Value, vm.stack, oldCapacity, vm.stackCapacity);
        vm.stackTop = vm.stack + offset;
    }
}


void push(Value value) {
    if(vm.stackCapacity <= vm.stackCount+ 1) {
        GROW_VM();
    }
    *vm.stackTop = value;
    vm.stackTop++;
    vm.stackCount++;
}

Value pop() {
    vm.stackTop--;
    vm.stackCount--;
    return *vm.stackTop;
}

static void resetStack() {
    vm.stackTop = vm.stack;
}

static InterpretResult run() {
    #define READ_BYTE() (*vm.ip++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    #define BINARY_OP(op) \
        do { \
            double b = pop(); \
            double a = pop(); \
            push(a op b); \
        } while (false)
    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
            printf("       ");
            for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }
            printf("\n");
            disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif
        uint8_t instruction;
        switch(instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE: BINARY_OP(/); break;
            case OP_NEGATE: push(-pop()); break;
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

void initVM() {
    vm.stack = NULL;
    vm.stackCapacity = 0;
    vm.stackCount = 0;
    resetStack();
}

void freeVM() {
    FREE_ARRAY(Value, vm.stack, vm.stackCount);
}

InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

