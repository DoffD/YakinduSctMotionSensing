#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define DBG_ALL

#ifdef DBG_ALL
#define DBG_PORT Serial
#define DBG_BAUD 9600

#define DBG_PR_MN_SM
#define DBG_OU_BRIGHT
#endif

const uint8_t PIN_IN_BTN_QTY = 3;

const uint8_t PIN_IN_BTN_USR_OFF = A0;

const uint8_t PIN_IN_BTN_USR_ON = A1;
const bool ACTVST_BTN_USR_ON = LOW;

const uint8_t PIN_IN_BTN_MOTION = A2;
const bool ACTVST_BTN_MOTION = LOW;

const uint8_t PIN_IN_BTN_MODE = A3;

const uint8_t PIN_OUT_LED_RED = 11;
#endif