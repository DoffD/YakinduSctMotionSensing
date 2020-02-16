#ifndef DEBBTNS_H
#define DEBBTNS_H
#include "definitions.h"

// Debounces the state of a pin.
// Returns true if there is a new debounced state.
// stRaw: raw input from button
// stPrev: pointer to a boolean where the prev debounced state is stored (should be static).
// stDeb: pointer to a boolean where the new debounced state will be stored (should be static).
// st: pointer to a uint8_t var where the state machine state is kept (should be static).
// tRef: pointer to timer var (should be static).
bool btn_deb_Btn(bool stRaw, bool* stPrev, bool* stDeb, uint8_t* st, uint32_t* tRef){
  bool isNewSt{};
  switch (*st)
  {
    case 0:
      if(*stPrev != stRaw){
        *tRef = millis();
        *st = 1;
      }
      break;
    case 1:
      if(*stPrev == stRaw){
        *st = 0;
      }
      else if(millis() - *tRef > 100){ // const uint32_t TSPAN_BTNDEBOUNCE = 100;
        *stPrev = stRaw;
        *stDeb = stRaw;
        isNewSt = true;
        *st = 0;
      }
      break;
    default:
      break;
  }
  return isNewSt;
}

// Debounces the states of multiple pins.
// pins: pointer to an array of pins.
// statesDeb: pointer to an array of uint8_t where the debounced states will be stored.
void btn_deb_Btns(uint8_t* pins, bool* statesDeb){
  static bool statesPrev[PIN_IN_BTN_QTY]{};
  static uint8_t statesSm[PIN_IN_BTN_QTY]{};
  static uint32_t tRefs[PIN_IN_BTN_QTY]{};
  for (uint8_t i=0; i<PIN_IN_BTN_QTY; i++) {
    btn_deb_Btn(digitalRead(*(pins+i)), &statesPrev[i], statesDeb+i, &statesSm[i], &tRefs[i]);
  }
}

// Initializes the states of multiple buttons at boot.
// pins: pointer to an array of pins.
// statesDeb: pointer to an array of uint8_t where the debounced states will be stored.
void btn_deb_Init(uint8_t* pins, bool* statesDeb){
  for (uint8_t i=0; i<PIN_IN_BTN_QTY; i++) {
    *(statesDeb+i) = digitalRead(*(pins+i));
  }
}
#endif