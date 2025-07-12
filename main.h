#ifndef MEMORY_DEBUGGER_H
#define MEMORY_DEBUGGER_H

#include <iostream>
#include <unordered_map>
#include <cstdlib>


struct Allocation
{
    size_t size;
    const char* file;
    int line;
};

class MemoryDebugger
{
private:
    static std::unordered_map<void*, Allocation>& getAllocations()
    {
        static std::unordered_map<void*, Allocation> allocations;
        return allocations;
    }

public:
    static void* allocate(size_t size, const char* file, int line)
    {
        void* ptr = malloc(size);
        getAllocations()[ptr] = { size, file, line };
        std::cout << "[Alloc] " << size << " bytes at " << ptr << " (" << file << ":" << line << ")\n";
        return ptr;
    }

    static void deallocate(void* ptr)
    {
        auto& allocations = getAllocations();
        auto it = allocations.find(ptr);
        if (it != allocations.end())
        {
            std::cout << "[Free] " << ptr << "\n";
            allocations.erase(it);
            free(ptr);
        }
        else
        {
            std::cerr << "[Error] Invalid free or double free at " << ptr << "\n";
        }
    }

    static void reportLeaks()
    {
        auto& allocations = getAllocations();
        if (allocations.empty())
        {
            std::cout << "[Info] No memory leaks detected.\n";
        } 
        else
        {
            std::cerr << "[Leak] Detected " << allocations.size() << " leaks:\n";
            for (auto& [ptr, alloc] : allocations)
            {
                std::cerr << "  Leak at " << ptr << ", size: " << alloc.size
                          << ", allocated at " << alloc.file << ":" << alloc.line << "\n";
            }
        }
    }
};

#define MEMDBG_NEW new(__FILE__, __LINE__)
inline void* operator new(size_t size, const char* file, int line)
{
    return MemoryDebugger::allocate(size, file, line);
}
inline void operator delete(void* ptr) noexcept
{
    MemoryDebugger::deallocate(ptr);
}
inline void operator delete(void* ptr, const char*, int) noexcept
{
    MemoryDebugger::deallocate(ptr);
}

#endif
