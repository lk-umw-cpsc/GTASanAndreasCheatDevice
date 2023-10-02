#include "Memory.h"
#include <Windows.h>

void overwriteInstructions(void* target, void* newInstructions, const unsigned int numBytes, void* oldInstructionsOut) {
	DWORD oldProtectionFlags;
	VirtualProtect(target, numBytes, PAGE_EXECUTE_READWRITE, &oldProtectionFlags);
	memcpy(oldInstructionsOut, target, numBytes);
	memcpy(target, newInstructions, numBytes);
	VirtualProtect(target, numBytes, oldProtectionFlags, &oldProtectionFlags);
}

void restoreInstructions(void* target, void* oldInstructions, const unsigned int numBytes) {
	DWORD oldProtectionFlags;
	VirtualProtect(target, numBytes, PAGE_EXECUTE_READWRITE, &oldProtectionFlags);
	memcpy(target, oldInstructions, numBytes);
	VirtualProtect(target, numBytes, oldProtectionFlags, &oldProtectionFlags);
}