#pragma once

// bit idx:     2  1  0
// bit value:  S0 S1 S2
#define I8008_STATE_T1      0b010
#define I8008_STATE_T1I     0b011
#define I8008_STATE_T2      0b001
#define I8008_STATE_WAIT    0b000
#define I8008_STATE_T3      0b100
#define I8008_STATE_STOPPED 0b110
#define I8008_STATE_T4      0b111
#define I8008_STATE_T5      0b101

// bit idx:     1  0
// bit value:  D7 D6
#define I8008_CYCLE_PCI 0b00
#define I8008_CYCLE_PCR 0b10
#define I8008_CYCLE_PCC 0b01
#define I8008_CYCLE_PCW 0b11
