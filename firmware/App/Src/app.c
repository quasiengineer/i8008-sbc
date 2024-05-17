#include <stdlib.h>
#include <stdio.h>

#include "tim.h"
#include "app.h"
#include "cpuInterface.h"
#include "hostInterface.h"

typedef enum { WaitingForProgram, RunningProgram } State;

static State state = WaitingForProgram;

static ProgramDump runningProgram;

#define MAX_IO_BYTES_TO_SEND_IN_SINGLE_TRANSMIT 100

/*
 * Send summary for finished program
 */
static void sendFinalState() {
  uint64_t statesCount = getProcessorElapsedStatesCount();
  sendOutputMessageToHost("Program has been finished\r\n");
  sendOutputMessageToHost("Instruction cycles processed = %08X%08X\r\n", (uint32_t)(statesCount >> 32U), (uint32_t)(statesCount & 0xFFFFFFFF));
  sendOutputMessageToHost("Writes to I/O ports:\r\n");
  for (uint16_t ptr = 0; ptr < processorIOBufferLen; ) {
    char bytesStr[MAX_IO_BYTES_TO_SEND_IN_SINGLE_TRANSMIT * 2 + 1];
    for (uint16_t currentLen = 0, chunkEnd = ptr + MAX_IO_BYTES_TO_SEND_IN_SINGLE_TRANSMIT; ptr < chunkEnd && ptr < processorIOBufferLen; ptr++) {
      currentLen += snprintf(bytesStr + currentLen, sizeof(bytesStr) - currentLen, "%X ", processorIOBuffer[ptr]);
    }

    sendOutputMessageToHost("  %s\r\n", bytesStr);
  }

  sendEndOfOutputMessageToHost();
}

/*
 * Processor now in stopped state, send information to host and shutdown processor
 */
static void finishProgramExecution() {
  if (runningProgram.data) {
    free(runningProgram.data);
    runningProgram.data = 0;
  }

  __enable_irq();
  sendFinalState();

  state = WaitingForProgram;
}

/*
 * Check if there is new program to be run on target CPU
 */
static void checkForNewProgramToRun() {
  // make sure that "currentProgram" would not be modified inside interrupt handler
  __disable_irq();

  if (!currentProgram.data) {
    __enable_irq();
    return;
  }

  runningProgram = currentProgram;
  currentProgram.data = 0;

  // need to send message to host, but we safely acquired pointer to dump, so now we don't care about interrupts
  __enable_irq();
  sendOutputMessageToHost("Program dump has been received, size = %04X, starting CPU...\r\n", runningProgram.size);
  __disable_irq();

  state = RunningProgram;
  startProcessor(runningProgram.data, runningProgram.size);
}

/*
 * Infinite loop
 */
void infiniteLoopIteration() {
  switch (state) {
    case RunningProgram:
      switch (processorState) {
        case ProcessorShutdown:
          finishProgramExecution();
          break;

        case ProcessorStarting:
          handleStartupSequence();
          break;

        case ProcessorRunning:
          handleProcessorSignals();
          break;
      }
      break;

    case WaitingForProgram:
      checkForNewProgramToRun();
      break;
  }
}

/*
 * Clock period tCY = 1250ns (targetFreq = 800kHz) and pulse width tPW = 350ns (active HIGH)
 * Offset between rising edge of phi1 and rising edge of phi2 should be tPW + tD3 = 350ns + 200ns = 550ns
 * To achieve that master-slave timers are used with same period (frequency): TIM_Period = (timerFreq / targetFreq) - 1 = 280Mhz / 0.8Mhz - 1 = 349
 * Pulse width should be (TIM_Period + 1) * (targetPeriod / timerPeriod) - 1 = 350 * (350ns / 1250ns) - 1 = 97
 * Phase offset should be (TIM_Period + 1) * (targetOffset / timerPeriod) - 1 = 350 * (550ns / 1250ns) - 1 = 153
 */
void initApp() {
  // generate clock signals
  HAL_TIM_Base_Start(&htim8);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);

  HAL_GPIO_WritePin(LED_OUT_GPIO_Port, LED_OUT_Pin, GPIO_PIN_SET);
}