#pragma once
#include <windows.h>

template<typename T>
T ReadLocalMemory(uintptr_t address) {
    __try {
        return *(T*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return T();
    }
}

template<typename T>
void WriteLocalMemory(uintptr_t address, T value) {
    DWORD oldProtection;
    VirtualProtect((LPVOID)address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtection);
    *(T*)address = value;
    VirtualProtect((LPVOID)address, sizeof(T), oldProtection, &oldProtection);
}