#include <string.h>

#include "main.h"
#include "cpuInterface.h"
#include "pins.h"
#include "i8008.h"

typedef enum { StartupWaitingForT1i, StartupWaitingForT3, StartupWaitingForNextT1 } StartupStage;

volatile ProcessorState processorState = ProcessorShutdown;

uint8_t processorIOBuffer[IO_BUFFER_LENGTH];
uint16_t processorIOBufferLen = 0;

#define MEMORY_SIZE_IN_BYTES  16384
#define RST_0_BYTECODE        0x05

static uint8_t memory[MEMORY_SIZE_IN_BYTES];

static uint64_t statesCount = 0;

static uint8_t currentCycle = 0;
static uint16_t currentAddr = 0;

static StartupStage startupStage = StartupWaitingForT1i;

uint64_t getProcessorElapsedStatesCount() {
  return statesCount;
}

void startProcessor(uint8_t * initialMemory, uint16_t initialMemorySize) {
  memset(memory, 0, sizeof(memory));
  memcpy(memory, initialMemory, initialMemorySize);

  processorIOBufferLen = 0;
  processorState = ProcessorStarting;
  startupStage = StartupWaitingForT1i;

  setPinHigh(STM_OUT_INT);
  setPinHigh(STM_OUT_RDY);
}

static inline uint8_t waitForState() {
  // i8008 steps into new state after phi22 fall, but i8008 state lines
  // reflects that after some delay, so we need to wait a proper moment
  while (isPinLow(STM_OUT_PH2));

  // check phase of state (based on SYNC): if it is 1st or 2nd tick
  // we need 1st ph2 rise
  if (isPinLow(STM_IN_SYNC)) {
    while (isPinLow(STM_IN_SYNC));
    while (isPinLow(STM_OUT_PH2));
  }

  while (isPinLow(STM_OUT_PH1));

  return i8008_getState();
}

// After reset we need to have special startup sequence: wait for T1I, inject bytecode that would be executed on
// CPU startup and then wait for next T1
void handleStartupSequence() {
  uint8_t i8008_state = waitForState();

  if (i8008_state == I8008_STATE_STOPPED && startupStage != StartupWaitingForT1i) {
    processorState = ProcessorShutdown;
    return;
  }

  switch (startupStage) {
    case StartupWaitingForT1i:
      if (i8008_state == I8008_STATE_T1I) {
        setPinLow(STM_OUT_INT);
        startupStage = StartupWaitingForT3;
      }
      break;

    case StartupWaitingForT3:
      if (i8008_state == I8008_STATE_T3) {
        // jump to 0x0000
        i8008_writeData(RST_0_BYTECODE);
        startupStage = StartupWaitingForNextT1;
      }
      break;

    case StartupWaitingForNextT1: {
      while (isPinHigh(STM_OUT_PH1));
      currentAddr = i8008_readDataBus();
      statesCount = 1;
      processorState = ProcessorRunning;
      break;
    }
  }

  // after second ph1 fall we can free data bus at any moment, does not matter if it has been acquired by us or not
  i8008_freeDataBus();
}

void handleProcessorSignals() {
  uint8_t i8008State = waitForState();
  if (i8008State == I8008_STATE_STOPPED) {
    processorState = ProcessorShutdown;
    return;
  }

  statesCount++;
  while (isPinHigh(STM_OUT_PH1)); // wait for i8008 to put data onto bus
  switch (i8008State) {
    case I8008_STATE_T1:
      currentAddr = i8008_readDataBus();
      break;

    case I8008_STATE_T2: {
      uint8_t highAddr = i8008_readDataBus();
      currentAddr = ((highAddr & 0x3FU) << 8U) | currentAddr;
      currentCycle = highAddr >> 6U;
      if (currentCycle == I8008_CYCLE_PCI || currentCycle == I8008_CYCLE_PCR) {
        uint8_t currentData = memory[currentAddr];
        while (isPinLow(STM_OUT_PH1)); // send data to the bus at the very begging of T3
        i8008_writeData(currentData);
      }
      break;
    }

    case I8008_STATE_T3:
      if (currentCycle == I8008_CYCLE_PCW) {
        memory[currentAddr] = i8008_readDataBus();
      } else if (currentCycle == I8008_CYCLE_PCC) {
        processorIOBuffer[processorIOBufferLen] = currentAddr & 0xFF;
        processorIOBufferLen++;
      } else {
        i8008_freeDataBus();
      }
      break;

    default:
      break;
  }
}
