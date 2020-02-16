#include "Arduino.h"
#include "definitions.h"
#include "debBtns.h"

uint8_t gStPins[PIN_IN_BTN_QTY]{
  PIN_IN_BTN_USR_OFF,
  PIN_IN_BTN_USR_ON,
  PIN_IN_BTN_MOTION
};
// An array to hold debounced pin states.
bool gStDebStates[PIN_IN_BTN_QTY]{};


void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_IN_BTN_USR_OFF, INPUT_PULLUP);
  pinMode(PIN_IN_BTN_USR_ON, INPUT_PULLUP);
  // const bool isEnabled = (ACTV_ST == LOW) ? !digitalRead(PIN_IN_BTN_USR_ON):digitalRead(PIN_IN_BTN_USR_ON);
  pinMode(PIN_IN_BTN_MOTION, INPUT_PULLUP);
  //const bool isEnabled = (ACTV_ST == LOW) ? !digitalRead(PIN_IN_BTN_MOTION):digitalRead(PIN_IN_BTN_MOTION);
  pinMode(PIN_IN_BTN_MODE, INPUT_PULLUP);

  btn_deb_Init(gStPins, gStDebStates);

  pinMode(PIN_OUT_LED_RED, OUTPUT);
  digitalWrite(PIN_OUT_LED_RED, LOW);

  #ifdef DBG_ALL
  DBG_PORT.begin(DBG_BAUD);
  DBG_PORT.println(F("MotionSensing Implementation."));
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:
  // *******************************************
  // ********** Global vars dec start **********
  // Static: use gSt
  static bool gStIsInitDone{};
  static uint8_t gStBrightness{};

  // Non-static: use gNs
  bool gNsIsUserBtnOff{};
  bool gNsIsUserBtnOn{};
  bool gNsIsMotion{};
  // ********** Global vars dec end   **********
  // *******************************************

  // *******************************************
  // ************** Inputs start ***************
  // use in_
  // Debounce multiple pins start =====
  btn_deb_Btns(gStPins, gStDebStates);
  // Debounce multiple pins end   =====
  // ************** Inputs end   ***************
  // *******************************************

  // *******************************************
  // ********* Post-input maint start **********
  // use pi_
  gNsIsUserBtnOff = !gStDebStates[0];

  static uint8_t pi_isUserBtnOnPrev{0xFF};
  if(pi_isUserBtnOnPrev != (uint8_t)gStDebStates[1]){
    gNsIsUserBtnOn = !gStDebStates[1];
    pi_isUserBtnOnPrev = (uint8_t)gStDebStates[1];
  }

  gNsIsMotion = !gStDebStates[2];
  // ********* Post-input maint end   **********
  // *******************************************

  // *******************************************
  // ************** Process start **************
  // use pr_
  static uint32_t pr_mn_sm_tRef{};
  enum class pr_mn_sm_St:uint8_t
  {
    init,
    bootDelay,
    waitForMain,
    motionSensing,
    manual
  };
  static pr_mn_sm_St pr_mn_sm_st{};
  switch(pr_mn_sm_st)
  {
    case pr_mn_sm_St::init:
    {
      pr_mn_sm_st = pr_mn_sm_St::bootDelay;
      #ifdef DBG_PR_MN_SM
      DBG_PORT.println(F("DBG_PR_MN_SM: init > bootDelay"));
      #endif
    }
      break;
    case pr_mn_sm_St::bootDelay:
    {
      if(millis() - pr_mn_sm_tRef > 1000){ //const uint32_t SUFFIX_TSPAN_DELAY_BOOT = 3000;
        pr_mn_sm_st = pr_mn_sm_St::waitForMain;
        #ifdef DBG_PR_MN_SM
        DBG_PORT.println(F("DBG_PR_MN_SM: bootDelay > waitForMain"));
        #endif
      }
    }
      break;
    case pr_mn_sm_St::waitForMain:
    {
      gStIsInitDone = true;
      if(gStIsInitDone) {
        pr_mn_sm_st = pr_mn_sm_St::motionSensing;
        #ifdef DBG_PR_MN_SM
        DBG_PORT.println(F("DBG_PR_MN_SM: waitForMain > motionSensing"));
        #endif
      }
    }
      break;
    case pr_mn_sm_St::motionSensing:
    {
      static uint32_t tRef{};
      static uint8_t st{};
      switch (st)
      {
      case 0: // Off entry.
        gStBrightness = 0;
        st = 1;
        break;
      case 1: // Off.
        if(gNsIsMotion){
          st = 2;
        }
        break;
      case 2: // On entry.
        gStBrightness = 1;
        tRef = millis();
        st = 3;
        break;
      case 3: // On.
        if(gNsIsMotion){
          st = 2;
        }
        else if(millis() - tRef > 5000){
          st = 0;
        }
        break;
      default:
        st = 0;
        break;
      }

      if(gNsIsUserBtnOn) {
        st = 0;
        pr_mn_sm_st = pr_mn_sm_St::manual;
        #ifdef DBG_PR_MN_SM
        DBG_PORT.println(F("DBG_PR_MN_SM: motionSensing > manual"));
        #endif
      }
    }
      break;
    case pr_mn_sm_St::manual:
    {
      static uint8_t st{};
      switch (st)
      {
        case 0: // Entry.
          gStBrightness += 1;
          st = 1;
          break;
        case 1:
          if(gNsIsUserBtnOn){
            if(gStBrightness < 10){
              st = 0;
            }
          }
          else if(gNsIsUserBtnOff){
            st = 0;
            pr_mn_sm_st = pr_mn_sm_St::motionSensing;
            #ifdef DBG_PR_MN_SM
            DBG_PORT.println(F("DBG_PR_MN_SM: manual > motionSensing"));
            #endif
          }
          break;
        default:
          st = 0;
          break;
      }
    }
      break;
    default:
      /*Do nothing.*/
      break;
  }
  // ************** Process end   **************
  // *******************************************

  // *******************************************
  // ************** Outputs start **************
  // use ou_
  static uint8_t ou_brightPrev{0xFF};
  if(ou_brightPrev != gStBrightness){
    analogWrite(PIN_OUT_LED_RED, gStBrightness ? gStBrightness*20 : 0);
    #ifdef DBG_OU_BRIGHT
    DBG_PORT.print(F("DBG_OU_BRIGHT: gStBrightness: ")); DBG_PORT.println(gStBrightness);
    #endif
    ou_brightPrev = gStBrightness;
  }
  // ************** Outputs end   **************
  // *******************************************

  // *******************************************
  // ********* Post-output maint start *********
  // use po_
  // ********* Post-output maint end   *********
  // *******************************************

}
