#pragma once

#include <stdint.h>

typedef struct {
    uint16_t size;
    uint8_t * data;
} ProgramDump;

extern volatile ProgramDump currentProgram;

void sendOutputMessageToHost(const char *format, ...);
void sendEndOfOutputMessageToHost();
void handleDataFromHost(uint8_t * data, uint16_t dataLen);