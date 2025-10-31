//*******************************************************
//*   I2C_Deployment_Module_test_with_TrinketM0.ino     *
//*   Test Script code for the I2C Deployment Module    *
//*       attached to an Adafruit Trinket M0            *
//* Provides framework for adding sensors & ITDT Logic  *
//*              Mike Loman 8/12/25                     *
//*******************************************************
//  https://oshpark.com/shared_projects/tfre3jx1
//  test code assumes:
//   1.  Single THT I2C Deployment Module on bus
//   2.  Test config:  Xmas lights connected to Ch1 & Ch2, 3S LiPo connected 
//   3.  Ch1 is Drogue and Ch2 is Main
//   4.  Adafruit Trinket M0 is MCU driving the I2C Deployment Module and 
//   5.  On-board Dotstar is primary HMI

//   ****************   set Global environment   *****************
  //  include libraries
#include <Adafruit_DotStar.h>
  #include <Adafruit_MCP23XXX.h>
  #include <Adafruit_MCP23X08.h>

//     ##############      I2C Service Module Silk                  #################  
  #define TEST1      2
  #define TEST2      4
  #define DROGUE     1   //  CH1,  blue pigtail
  #define MAIN       6   //  CH2,  yellow pigtrail  
  #define SNDROGUE   0
  #define SNMAIN     7
  #define PWRON      5
  #define BATTMON    3
  
//     ##############     M0 Trinket Pinouts      #################  
#define AudioPin   A0
#define FireBatt   A2
#define VBATT      A3
#define DotData     7  // Digital #7 - You can't see this pin but it is connected to the internal RGB DotStar data in pin
#define DotClock    8  // Digital #8 - You can't see this pin but it is connected to the internal RGB DotStar clock in pin
#define OnBoardLED 13  // Digital #13 - You can't see this pin but it is connected to the little red status LED

    
  //   instantiate libraries
  Adafruit_DotStar led(1, DotData, DotClock, DOTSTAR_BRG);
  Adafruit_MCP23X08 DD;
    
  //  declare variable types
  bool ArmReq, TestReq, DrogueReq, MainReq, ARMED, FAIL1, FAIL2, DrogueTimerSet, MainTimerSet, TestTimerSet;
  long TestTimer, DrogueTimer, MainTimer;
  enum conditions {
    awaiting_1stTest, 
    awaiting_Arm, 
    awaiting_2ndTest, 
    awaiting_Apogee, 
    awaiting_MainAlt, 
    awaiting_Landing
  }FLTCON;
  
//  *****************************  END Globals - Begin Setup   ****************  
void setup() {
  
  //  *************  start Serial - uncomment while (!Serial); to ensure connection  ****************  
  Serial.begin(115200);
//  while (!Serial);
  Serial.println("I2C Deployment Module Test!");
  
  //  *********  start Qt Py Built-in NeoPixel under Adafruit NeoPixel driver  ****************  
  led.begin();
  led.clear();  // Set all pixel colors to 'off'
  led.show();   // Send the updated pixel colors to the hardware.    
  
  //  *********  start I2C Deployment Module under Adafruit MCP23008 drivers  ****************  
  if (!DD.begin_I2C(0x23)) {   //  Module I2C address is 0x21 or 0x23. Default is 0x21.
    Serial.println("Error.  No MCP23008 detected on 0x21...");
  }
  // configure Test, Arm and Ch pins for output
  DD.pinMode(TEST1, OUTPUT);
  DD.pinMode(TEST2, OUTPUT);
  DD.pinMode(PWRON, OUTPUT);
  DD.pinMode(DROGUE, OUTPUT);
  DD.pinMode(MAIN, OUTPUT);
  
  // configure Sn and FireBatt pins for input
  DD.pinMode(BATTMON, INPUT); 
  DD.pinMode(SNDROGUE, INPUT); 
  DD.pinMode(SNMAIN, INPUT); 
  
  delay(100);  //   give it a few...
  //  set all outputs LOW
  DD.digitalWrite(TEST1, LOW);
  DD.digitalWrite(TEST2, LOW);
  DD.digitalWrite(PWRON, LOW);
  DD.digitalWrite(DROGUE, LOW);
  DD.digitalWrite(MAIN, LOW);

//  set/initialize flight options
  FLTCON = awaiting_1stTest;
  
//  *******************  END Setup - Begin loop  ********************************   
}
void loop() {
//  *****************  read sensors  *******************************    

//  ************  calculate derived values  ************************

//  *****************   set bools   ********************************
  //  For the test script, just use timer triggers vs. sensor inputs
  //  Kinda makes it into a PET2....
  switch (FLTCON){
    case awaiting_1stTest:
      if(millis()>5000){
        TestReq = true;       //  should light Dotstar blue
        Serial.println("Continuity test requested");        
        FLTCON = awaiting_Arm;
      }
    break;
    case awaiting_Arm:
      if(millis()>10000){
        ArmReq = true;
        Serial.println("Arming requested");               
        FLTCON = awaiting_2ndTest;
      }
    break;
    case awaiting_2ndTest:
      if(millis()>15000){
        TestReq = true;       //  should light Dotstar green
        Serial.println("2nd continuity test requested");               
        FLTCON = awaiting_Apogee;
      }
    break;
    case awaiting_Apogee:
      if(millis()>20000){
        DrogueReq = true;       //  should light Ch1 Xmas light
        Serial.println("Drogue power requested");               
        FLTCON = awaiting_MainAlt;
      }
    break;
    case awaiting_MainAlt:
      if(millis()>25000){
        MainReq = true;       //  should light Ch2 Xmas light
        Serial.println("Main power requested");               
        FLTCON = awaiting_Landing;
      }
    break;
    case awaiting_Landing:


    break;
  }

//  *****************   Take Actions   ********************************
  if(ArmReq){
    DD.digitalWrite(PWRON, HIGH);
    ArmReq = false;
    ARMED = DD.digitalRead(BATTMON);
              Serial.print("ARMED   ");
          Serial.println(ARMED);
  }
  if(TestReq){
    DD.digitalWrite(TEST1, HIGH);
    DD.digitalWrite(TEST2, HIGH);
    TestTimer = millis()+500l;
    TestTimerSet=true;
    TestReq=false;
  }
  if(DrogueReq){
    DD.digitalWrite(DROGUE, HIGH);
    DrogueTimer = millis()+2000l;  
    DrogueTimerSet=true;
    DrogueReq=false;
  }
  if(MainReq){
    DD.digitalWrite(MAIN, HIGH);
    MainTimer = millis()+2000l; 
    MainTimerSet=true;
    MainReq=false;
  }

//  *****************   check timers   ********************************
  if(TestTimerSet && TestTimer<=millis()){
    //  Read and Report continuity test results
    led.clear();
    led.setPixelColor(0, led.Color(150, 150, 150));    //  Display white for default/unknown    
    FAIL1=DD.digitalRead(SNDROGUE);
          Serial.print("FAIL1   ");
          Serial.println(FAIL1);
    FAIL2=DD.digitalRead(SNMAIN);
          Serial.print("FAIL2   ");
          Serial.println(FAIL2);    
    if(!FAIL1 && !FAIL2 && ARMED){
      Serial.println("Ready to Launch!");
      led.clear();
      led.setPixelColor(0, led.Color(150, 0, 0));    //  Display green
    }
    if(FAIL1){
      Serial.println("Drogue failed continuity check...");
      led.clear();
      led.setPixelColor(0, led.Color(0, 150, 0));    //  Display red
    }
    if(FAIL2){
      Serial.println("Main failed continuity check...");
      led.clear();
      led.setPixelColor(0, led.Color(150, 150, 0));  //  Display yellow
    }
    if(FAIL1 && FAIL2){
      Serial.println("BOTH failed continuity check...");
      led.clear();
      led.setPixelColor(0, led.Color(0, 50, 150));  //  Display purple
    }
    if(!ARMED){
      Serial.println("Not Armed - Battery may be weak or disconnected...");
      led.clear();
      led.setPixelColor(0, led.Color(0, 0, 150));    //  Display blue
    }
    led.show();
    DD.digitalWrite(TEST1, LOW);
    DD.digitalWrite(TEST2, LOW);
    TestTimerSet=false;    
  }  
  if(DrogueTimerSet && DrogueTimer<=millis()){
    DD.digitalWrite(DROGUE, LOW);
    Serial.println("Drogue power off");  
    DrogueTimerSet=false;
  }
  if(MainTimerSet && MainTimer<=millis()){
    DD.digitalWrite(MAIN, LOW);
    Serial.println("Main power off");  
    MainTimerSet=false;
  }
  
//  *****************   END loop   ********************************
}
