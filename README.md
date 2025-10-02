# C++ Dynamic Memory Debugger

Lightweight memory debugger that tracks allocations, detects leaks, double deletes, and invalid frees using operator overloading.

## Features

- Tracks all dynamic allocations with file and line information
- Detects memory leaks at program termination
- Catches double deletes
- Identifies invalid free attempts
- Header-only implementation
- Zero dependencies beyond standard library

## How It Works

Uses operator overloading to intercept `new` and `delete` calls:
1. Custom `operator new` stores pointer metadata in a hash map
2. Custom `operator delete` validates and removes tracked pointers
3. Preprocessor macro injects file/line information automatically
4. `reportLeaks()` displays unfreed memory at program end

## Usage

1. Include the header:
```cpp
#include "main.h"
```

2. Replace `new` with `MEMDBG_NEW`:
```cpp
int* ptr = MEMDBG_NEW int;
```

3. Use `delete` normally:
```cpp
delete ptr;
```

4. Check for leaks before exit:
```cpp
MemoryDebugger::reportLeaks();
```

## Example

```cpp
#include "main.h"

int main() {
    int* a = MEMDBG_NEW int;        // Tracked allocation
    int* b = MEMDBG_NEW int[10];    // Array allocation
    
    delete a;                        // Valid delete
    delete[] b;                      // Valid array delete
    
    int* leak = MEMDBG_NEW int[5];  // Memory leak (not freed)
    
    delete a;                        // Double delete (error)
    
    MemoryDebugger::reportLeaks();  // Reports: 1 leak detected
    return 0;
}
```

## Output

```
[Alloc] 4 bytes at 0x1234 (test.cpp:4)
[Alloc] 40 bytes at 0x5678 (test.cpp:5)
[Free] 0x1234
[Free] 0x5678
[Alloc] 20 bytes at 0x9abc (test.cpp:7)
[Error] Invalid free or double free at 0x1234
[Leak] Detected 1 leaks:
  Leak at 0x9abc, size: 20, allocated at test.cpp:7
```

## Compilation

```bash
g++ -std=c++17 test.cpp -o test
./test
```

## Technical Details

### Operator Overloading
```cpp
void* operator new(size_t size, const char* file, int line)
void operator delete(void* ptr) noexcept
```

### Macro Magic
```cpp
#define MEMDBG_NEW new(__FILE__, __LINE__)
```
Automatically injects file and line info into allocations.

### Internal Storage
```cpp
static std::unordered_map<void*, Allocation> allocations;
```
Hash map tracks pointer address to metadata (size, file, line).

### Avoiding Recursion
Uses `malloc()`/`free()` internally instead of `new`/`delete` to prevent infinite recursion.

## Limitations

- Does not support `new[]` and `delete[]` tracking separately
- Not thread-safe
- Basic implementation for educational purposes
- Not a replacement for production tools (Valgrind, AddressSanitizer)
