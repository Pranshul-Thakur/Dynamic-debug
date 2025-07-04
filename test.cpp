#include "main.h"

int main() {
    int* a = new int;
    int* b = new int[10];
    delete a;
    delete[] b;

    int* leak = new int[5];

    delete a;

    MemoryDebugger::reportLeaks();
    return 0;
}
