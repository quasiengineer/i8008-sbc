#include <stdarg.h>

#include "usbd_cdc_if.h"
#include "hostInterface.h"

/*
 * ================================
 *              OUTPUT
 * ================================
 */

extern USBD_HandleTypeDef hUsbDeviceHS;

typedef enum { PacketOutputMessage = 0, PacketEndOfOutput = 1 } PacketType;

#define MAX_OUTPUT_MSG_LEN 1024

/*
 * Send text message to host
 */
void sendOutputMessageToHost(const char * format, ...) {
  va_list args;
  va_start(args, format);

  char buf[MAX_OUTPUT_MSG_LEN];

  int dataSize = vsnprintf(&buf[3], sizeof(buf) - 3, format, args);

  // packet type
  buf[0] = (char)PacketOutputMessage;

  // data size
  buf[1] = (char)(dataSize & 0xFF);
  buf[2] = (char)((dataSize >> 8) & 0xFF);

  while (CDC_Transmit_HS((uint8_t *)buf, (uint16_t)(dataSize + 3)) != USBD_OK);

  // wait when transmission is done
  while (((USBD_CDC_HandleTypeDef *)hUsbDeviceHS.pClassData)->TxState != 0);

  va_end(args);
}

/*
 * Send flag that we sent all output that we wanted
 */
void sendEndOfOutputMessageToHost() {
  uint8_t buf[3] = { (uint8_t)PacketEndOfOutput, 0, 0};

  while (CDC_Transmit_HS(buf, sizeof(buf)) != USBD_OK);

  // wait when transmission is done
  while (((USBD_CDC_HandleTypeDef *)hUsbDeviceHS.pClassData)->TxState != 0);
}

/*
 * ================================
 *              INPUT
 * ================================
 */

volatile ProgramDump currentProgram = { .size = 0, .data = 0 };

typedef enum { ReceivingDataLengthLow, ReceivingDataLengthHigh, ReceivingData } ReceivingStage;

static ReceivingStage stage = ReceivingDataLengthLow;
static uint8_t * recvData = NULL;
static uint16_t recvDataLen = 0;
static uint16_t recvDataPos = 0;

/*
 * Process incoming data from host machine
 */
void handleDataFromHost(uint8_t * data, uint16_t dataLen) {
  for (uint8_t * current = data, * end = data + dataLen; current < end; current++) {
    switch (stage) {
      case ReceivingDataLengthLow:
        recvDataLen = *current;
        stage = ReceivingDataLengthHigh;
        break;

      case ReceivingDataLengthHigh:
        recvDataLen = recvDataLen | (*current << 8);
        recvDataPos = 0;
        recvData = malloc((size_t)recvDataLen);
        stage = ReceivingData;
        break;

      case ReceivingData:
        recvData[recvDataPos] = *current;
        recvDataPos++;
        if (recvDataPos == recvDataLen) {
          if (currentProgram.data) {
            free(currentProgram.data);
          }

          currentProgram.data = recvData;
          currentProgram.size = recvDataLen;
          stage = ReceivingDataLengthLow;
        }
        break;
    }
  }
}
