#pragma once
#include <Windows.h>

void overwriteInstructions(void* target, void* newInstructions, const unsigned int numBytes, void* oldInstructionsOut);
void restoreInstructions(void* target, void* oldInstructions, const unsigned int numBytes);