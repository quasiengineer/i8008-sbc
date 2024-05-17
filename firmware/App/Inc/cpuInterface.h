#pragma once

#include <stdint.h>

typedef enum { ProcessorRunning, ProcessorShutdown, ProcessorStarting } ProcessorState;

extern volatile ProcessorState processorState;

// 10Kb
#define IO_BUFFER_LENGTH (10 * 1024)

extern uint8_t processorIOBuffer[IO_BUFFER_LENGTH];
extern uint16_t processorIOBufferLen;

extern void startProcessor(uint8_t * initialMemory, uint16_t initialMemorySize);
extern void handleProcessorSignals();
extern void handleStartupSequence();
extern uint64_t getProcessorElapsedStatesCount();