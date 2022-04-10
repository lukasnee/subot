#include <IRremote.h>
#include <Servo.h>

#include "suBot/timer.hpp"
using namespace SuBot;

#define Number1 0xFFA25D
#define Number2 0xFF629D
#define Number3 0xFFE21D
#define Number4 0xFF22DD
#define Number5 0xFF02FD
#define Number6 0xFFC23D
#define Number7 0xFFE01F
#define Number8 0xFFA857
#define Number9 0xFF906F
#define Number0 0xFF9867
#define Ok 0xFF38C7

#define MY_PROTOCOL NEC;
//--------------------------
const byte RECV_PIN = 20; //
IRrecv irrecv(RECV_PIN);
decode_results results;
int etr = 0;
//-------------------------
byte myLINE = B00;
byte myLINEBACK = B00;

byte myFRONT = B00;    //
byte mySIDES = B00000; // 0000-----
//--------------------------
Servo myservo1; //---------
Servo myservo2; //-

const byte PWM1 = 4; ///
const byte PWM2 = 3; //
const byte DIR1 = 2; //
const byte DIR2 = 1; //
//----------------greitejimas---------
int StartSpeed = 80; // 150
int StopSpeed = 255; // 255
int i = StartSpeed;
unsigned long TimeNow;
unsigned long PreviousMillis = 0;
unsigned long Delay = 4; // 10 //

//-------------------------------
const byte LeftLine1 = 23;  //
const byte RightLine1 = 22; //

const byte Left2 = 18; //
const byte Left1 = 16; //

const byte Down = 12; //

const byte Middle1 = 14; //
const byte Middle2 = 15; //

const byte Right1 = 17; //
const byte Right2 = 19; //

const byte START_MODULE = 21; //

int k = 0;
int b = 0;

// Varikliu greiciai
const byte SPEED_NORMAL =
    80; // su minkstom padangom
        // 150//80-------------------------------------------

const byte SPEED_TURN =
    100; // 255//200---------------------------------------------------------------------
const byte SPEED1 = 255;
const byte SPEED_GO_BACK_LINE = 255;       // 200
const byte SPEED_GO_BACK_LINE_TURN = 255;  // 200
const byte SPEED_GO_BACK_LINE_TURN2 = 255; // 200
// atsitraukimo ir pasisukimo atsitraukiant laikai priekiniu linijos
const unsigned short TIME_FOR_GO_BACK = 150; // 180 atsitraukimas
const unsigned short TIME_FOR_GO_BACK_1 =
    130; // 130 neuzstringa puslankyje-------- atsisukimas

const byte SPEED_CHARGE = 50; // 255//----------------taimerio valdomas
                              // greitis-------------------------

byte START_STRATEGY_STATE = 0;
byte MAIN_STRATEGY_STATE = 0;
// Milis
unsigned long time;
bool Front = false;
byte check;

//---------------------------
void setup() {
  pinMode(13, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(LeftLine1, INPUT);
  pinMode(RightLine1, INPUT);
  //-----------------------------------
  pinMode(Left2, INPUT_PULLUP);
  pinMode(Left1, INPUT_PULLUP);

  pinMode(Down, INPUT_PULLUP);

  pinMode(Middle1, INPUT_PULLUP);
  pinMode(Middle2, INPUT_PULLUP);
  pinMode(Right1, INPUT_PULLUP);
  pinMode(Right2, INPUT_PULLUP);

  irrecv.enableIRIn();
  IR_Remote();

  pinMode(START_MODULE, INPUT);
  while (true) {
    if (digitalRead(START_MODULE)) {
      break;
    }
    Jutikliu_duom();
    if (myFRONT != 0b00000 or mySIDES != 0b00) { // 00 visi sensoriai
      // if (myFRONT != 0b00 ) {//00 tik priekio  sensoriai
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
    }
  }
  Start_Strategy();
}
//-------------------------
void loop() {
  Jutikliu_duom(); //
  Line();
  switch (MAIN_STRATEGY_STATE) {
  case 1: //
    switch (myFRONT) {
    case 0b00000:      // 0000
      Jutikliu_duom(); //
      Line();

      switch (mySIDES) //
      {
      case 0b00000: // 0000
        Jutikliu_duom();
        Line();
        break;
      //---------------soniniai---------------------
      case 0b00001:                 // 01000
        SpinRight(SPEED_TURN, 100); // 95
        break;

      case 0b10000:                // 00010
        SpinLeft(SPEED_TURN, 100); // 95
        break;

      //---------------kaire 10cm-------------
      case 0b01000: // 01000
      {
        static Timer spinTimer(Timer::Mode::normal, 500);
        if (!spinTimer.isTriggered()) {
          TimerDrive(100, 0);
        } else {
          static Timer chargeTimer(Timer::Mode::normal, 2000);
          if (!chargeTimer.isTriggered()) {
            TimerDrive(255, 255);
          } else {
            spinTimer.reset();
            chargeTimer.reset();
          }
        }
      }

      break;
        //-------------desine 10cm---------------
      case 0b00010: // 00010
      {
        static Timer spinTimer(Timer::Mode::normal, 500);
        if (!spinTimer.isTriggered()) {
          TimerDrive(0, 100);
        } else {
          static Timer chargeTimer(Timer::Mode::normal, 2000);
          if (!chargeTimer.isTriggered()) {
            TimerDrive(255, 255);
          } else {
            spinTimer.reset();
            chargeTimer.reset();
          }
        }
      }

      break;
        //----------------------- abu 10cm------------------------------
      case 0b01010:
        motor(255, 255); // 200
        delay(320);

        break;
      //------------------------------apačia---------------------------------------------
      case 0b00100:
        motor(255, 255); // 200
        delay(320);

        break;

        //----------------------------------------------------------------------------
      }
      break;

    default:
      time = millis();
      // while (millis() - time <= 1 or digitalRead(Middle1) == 0 or
      // digitalRead(Middle2) == 0 or digitalRead(Down) == 0 or
      // digitalRead(Left2) == 0 or digitalRead(Right2) == 0  )//
      while (millis() - time <= 1 or digitalRead(Middle1) == 0 or
             digitalRead(Middle2) == 0) //
      {
        //--------------------------------------------------------------
        // motor(230,230);//
        if (i < StopSpeed)

        {
          unsigned long CurrentMillis = millis(); //

          if (CurrentMillis - PreviousMillis >= Delay)

          {
            PreviousMillis = CurrentMillis; //

            i++;
          }
        }
        motor(i, i);
        //--------------------------------------------------------------
      }
      break;
    }
    break;

  case 2:
    switch (myFRONT) {

    case 0b00000: // 0000

      Jutikliu_duom();
      Line();

      switch (mySIDES) {
      case 0b00000: // 0000
        Jutikliu_duom();
        Line();
        break;
        //-----mato tik ---90 laipsniu sensoriai - ir priekis------------------
      case 0b00001: //

        SpinLeft(SPEED_TURN, 95); //
        break;

      case 0b10000: //

        SpinRight(SPEED_TURN, 95); //
        break;

      case 0b01010:
        Chargeforward(SPEED1, 500); //
        break;
        //------------------------
      }
      break;
    default:
      time = millis();

      while (millis() - time <= 1 or digitalRead(Middle1) == 0 or
             digitalRead(Middle2) == 0) //

      {
        motor(230, 230); //
        // delay(20);
        //  motor(220,220);//
        // delay(30);
        // motor(255,255);
      }
      break;
    }
    break;
  case 3: //----------------------------mato tik priekiniai 2 sensoriai
    switch (myFRONT) {

    case 0b0000:

      Jutikliu_duom();
      Line();

      switch (mySIDES) {
      case 0b0000: //
        Jutikliu_duom();
        Line();
        break;
      case 0b0110:
        Chargeforward(SPEED1, 500); // ka1 mato 45 ir 2 priekiniai
        break;

        // case 0b0001://

        // SpinLeft(SPEED_TURN, 100);//
        // break;

        // case 0b1000://

        // SpinRight(SPEED_TURN, 100);//
        // break;
      }
      break;
    default:
      time = millis();
      //-----------------------------mato tik priekis

      while (millis() - time <= 1 or digitalRead(Middle1) == 0 or
             digitalRead(Middle2) == 0) //

      {
        motor(200, 200); //
        delay(20);
        motor(220, 220); //
        delay(30);
        motor(255, 255);
      }
      break;
    }
    break;

    //--------------------------------------
  case 4:
    Jutikliu_duom();
    Line();
    break;
  case 9:
    motor(150, 150);
    delay(2000);
    motor(-150, -150);
    delay(2000);
    break;
  }
}
//-------------------------------------------
void motor(int a, int b) {
  a = constrain(a, -255, 255);
  b = constrain(b, -255, 255);
  if (a || b) {
    a -= 5; // kalibracija
  }
  if (a <= -1) //
  {
    digitalWrite(DIR1, 1);
    analogWrite(PWM1, (255 - abs(a)));
  } else if (a >= 1) {
    digitalWrite(DIR1, 0);
    analogWrite(PWM1, a);
  } else if (a == 0) {
    digitalWrite(DIR1, 0);
    analogWrite(PWM1, 0);
  }

  if (b >= 1) //
  {

    digitalWrite(DIR2, 0);
    analogWrite(PWM2, b); //
  } else if (b <= -1)     //
  {
    digitalWrite(DIR2, 1);
    analogWrite(PWM2, (255 - abs(b)));
  } else if (b == 0) {
    digitalWrite(DIR2, 0);
    analogWrite(PWM2, 0);
  }
}

bool arMato(byte pinas) { return digitalRead(pinas) == 0; }

bool arMatoLinija(byte pinas) { return digitalRead(pinas) == 1; }

bool arVidurysKaNorsMato() { return (arMato(Middle1) || arMato(Middle2)); }
//-------------------------- motoraiSuLaiku `arNutrauktiFn` variantai

bool tikLinija() { return false; }

bool arBetKurisSensoriusKaNorsMato() {
  return (arMato(Right1) || arMato(Middle1) || arMato(Middle2) ||
          arMato(Down) || arMato(Left1));
}
//---------------------------------------------------
void motoraiSuLaiku(int desinioMotoroGreitis, int kairioMotoroGreitis,
                    unsigned long kiekMsLaukti, bool (*arNutrauktiFn)(void)) {
  int pradinisLaikas = millis();

  motor(kairioMotoroGreitis, desinioMotoroGreitis);

  while (millis() - pradinisLaikas < kiekMsLaukti) {

    Jutikliu_duom(); // #NEW! Gali būt, kad sutvarkys.
    if (linijaBeDefaultCase() || (arNutrauktiFn && arNutrauktiFn())) {
      motor(SPEED_NORMAL, SPEED_NORMAL);
      // kiekMsLaukti = -1;
      return;
    }
  }

  return;
}
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------------
void Jutikliu_duom() {
  for (int i = 0; i <= 7; i++) { //
    bitClear(myLINE, i);
    bitClear(myFRONT, i);
    bitClear(mySIDES, i);
  }

  // Sonai---------------------------------------------------
  if (digitalRead(Right2) == 0)
    bitSet(mySIDES, 3); // 3
  if (digitalRead(Right1) == 0)
    bitSet(mySIDES, 0); // 0
  if (digitalRead(Down) == 0)
    bitSet(mySIDES, 2); ////2
  if (digitalRead(Left1) == 0)
    bitSet(mySIDES, 4); // 4
  if (digitalRead(Left2) == 0)
    bitSet(mySIDES, 1); // 1
  // Priekis-------------------------------------------------
  if (digitalRead(Middle1) == 0)
    bitSet(myFRONT, 0); //
  if (digitalRead(Middle2) == 0)
    bitSet(myFRONT, 1); //
  /// if (digitalRead(Down) == 0)     bitSet(mySIDES, 2);//
  //--------------------------------------------------------
  // Linija
  if (digitalRead(LeftLine1) == 1)
    bitSet(myLINE, 0); //
  if (digitalRead(RightLine1) == 1)
    bitSet(myLINE, 1); //
}

//---------------------------------------------------------------------------------
void Line() {

  switch (myLINE) {
  //-------------------------------
  case 0b01: // Right
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK) {
      motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);

      if (myLINEBACK != 0b00) {
        break;
      }
    }
    //--------------------------
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK_1) {
      motor(SPEED_GO_BACK_LINE_TURN2, -SPEED_GO_BACK_LINE_TURN); //

      if (myLINEBACK != 0b00) {
        break;
      }
    }
    break;
  //----------------------------
  case 0b10: //
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK) {
      motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);

      if (myLINEBACK != 0b00) {
        break;
      }
    }
    //---------------------------
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK_1) {

      motor(-SPEED_GO_BACK_LINE_TURN2, SPEED_GO_BACK_LINE_TURN);
      if (myLINEBACK != 0b00) {
        break;
      }
    }
    break;
    //---------------------------------
  case 0b11: // Both
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK) {
      motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);

      if (myLINEBACK != 0b00) {
        break;
      }
    }
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK) {
      motor(-SPEED_GO_BACK_LINE_TURN, SPEED_GO_BACK_LINE_TURN2);

      if (myLINEBACK != 0b00) {
        break;
      }
    }
    break;
  case 0:
    // Speed = StartSpeed;
    motor(SPEED_NORMAL, SPEED_NORMAL);
    i = StartSpeed;
    break;
  }
}
//--------------------------------------------------
bool linijaBeDefaultCase() {
  switch (myLINE) {
  case 0b01: // Right
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK) {
      motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);

      if (myLINEBACK != 0b00) {
        break;
      }
    }
    //--------------------------
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK_1) {

      motor(SPEED_GO_BACK_LINE_TURN2, -SPEED_GO_BACK_LINE_TURN);
      if (myLINEBACK != 0b00) {
        break;
      }
    }
    return 1;
    break;   //
             //-----------------------------
  case 0b10: // Left

    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK) {
      motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);

      if (myLINEBACK != 0b00) {
        break;
      }
    }
    //---------------------------
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK_1) {

      motor(-SPEED_GO_BACK_LINE_TURN2, SPEED_GO_BACK_LINE_TURN);
      if (myLINEBACK != 0b00) {
        break;
      }
    }

    return 1;
    break;

  case 0b11: // Both
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK) {
      motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);

      if (myLINEBACK != 0b00) {
        break;
      }
    }
    time = millis();
    while (millis() - time <= TIME_FOR_GO_BACK) {
      motor(-SPEED_GO_BACK_LINE_TURN, SPEED_GO_BACK_LINE_TURN2);

      if (myLINEBACK != 0b00) {
        break;
      }
    }

    return 1;
    break;
  default:
    return 0;
  }

  return 0;
}
//-----------------------------------------------------
void SpinRight(int a, unsigned int t) {
  time = millis();
  while (millis() - time <= t && (digitalRead(Middle1) == 1)) {
    motor(a, -a); //(a, -a)
  }
}
void SpinLeft(int a, unsigned int t) {
  time = millis();
  while (millis() - time <= t && (digitalRead(Middle1) == 1)) {
    motor(-a, a); //(-a, a)
  }
}
//-----------------------10 cm sensoriu
// valdymas----------------------------------------
void DriveCharge(int a, unsigned int t) { // r

  time = millis();
  while (millis() - time <= t) {
    motor(a, a); // buvo motor(-a, -a);
  }
}

void TimerDrive(int l, int r) {
  motor(l, r);
  delay(1); // labai svarbu 1 ms
}
//----------------------------------------

void Chargeforward(int a, unsigned int t) {
  time = millis();

  while (millis() - time <= t &&
         (digitalRead(Middle1) == 1 or digitalRead(Middle2) == 1)) {
    motor(255, 255);
  }
}

void Start_Strategy() {
  //----------------------------------------------------BUTINA PO 2 KOVU VALYTI
  // RATUKUS !!!!!!!!!!!!!!!!!-------------------------------------------------
  //-------------- per visa ringa pulti be užlaikymo, per puse su
  // užlaikymu--------
  switch (START_STRATEGY_STATE) {
  case 1: //------------kai puola per visa ringa--------------------
    motor(SPEED_NORMAL, SPEED_NORMAL);
    break;
  case 2: //------kai puola nuo arti-------
    motoraiSuLaiku(0, 0, 500, &arBetKurisSensoriusKaNorsMato); //
    break;
  case 3: // zigzagas is desines------ ---------------------------------
    motoraiSuLaiku(190, -190, 30,
                   &arBetKurisSensoriusKaNorsMato); // pasisuka i desine 90
                                                    // laipsniu//170-170 70
    motoraiSuLaiku(190, 190, 210,
                   &arBetKurisSensoriusKaNorsMato); // tiesiai//170 170 210
    motoraiSuLaiku(
        -190, 190, 210,
        &arBetKurisSensoriusKaNorsMato); // priesingai//-190 170 300
                                         // motoraiSuLaiku(0, 0, 4800,
                                         // &arVidurysKaNorsMato);//  nuimti jei
                                         // veluoja//
    break;
  case 4: //   zigzagas is kaires----
    motoraiSuLaiku(
        -190, 190, 30,
        &arBetKurisSensoriusKaNorsMato); // pasisuka i desine 90 laipsniu
    motoraiSuLaiku(190, 190, 210, &arBetKurisSensoriusKaNorsMato); // tiesiai
    motoraiSuLaiku(
        190, -190, 210,
        &arBetKurisSensoriusKaNorsMato); // priesingai
                                         //  motoraiSuLaiku(0, 0, 4800,
                                         //  &arVidurysKaNorsMato);// nuimti jei
                                         //  veluoja
    break;
  case 5: //  i kaire puslankis
    motoraiSuLaiku(170, 200, 135, &arBetKurisSensoriusKaNorsMato); //
    motoraiSuLaiku(130, 200, 135, &arBetKurisSensoriusKaNorsMato); //
    motoraiSuLaiku(100, 160, 125, &arBetKurisSensoriusKaNorsMato); //
    motoraiSuLaiku(100, 100, 135, &arBetKurisSensoriusKaNorsMato); // 135
    break;
  case 6: //   i desine puslankis
    motoraiSuLaiku(200, 170, 135,
                   &arBetKurisSensoriusKaNorsMato); // 180, 150, 155,
    motoraiSuLaiku(200, 130, 135,
                   &arBetKurisSensoriusKaNorsMato); // 170, 110, 145
    motoraiSuLaiku(160, 100, 125,
                   &arBetKurisSensoriusKaNorsMato); // 130, 70, 135,
    motoraiSuLaiku(100, 100, 135,
                   &arBetKurisSensoriusKaNorsMato); // 100, 100, 155,
    break;
  case 7: //   Marcio puslankis is kaires apvaziuojant visa ringa
    motoraiSuLaiku(-170, 170, 50,
                   &arBetKurisSensoriusKaNorsMato); //-170, 170, 70, r
    motoraiSuLaiku(200, 130, 460,
                   &arBetKurisSensoriusKaNorsMato); // 200, 110, 450,r
    motoraiSuLaiku(190, 70, 150,
                   &arBetKurisSensoriusKaNorsMato); // 180, 90, 160,r
    motoraiSuLaiku(190, 60, 130,
                   &arBetKurisSensoriusKaNorsMato); // 180, 40, 165,r
    motoraiSuLaiku(200, -200, 170,
                   &arBetKurisSensoriusKaNorsMato); // 200, -200, 180,r
    motoraiSuLaiku(200, 200, 90, &arBetKurisSensoriusKaNorsMato);

    break;
    //-----------------------------------
  case 8: //   Marcio puslankis is desines apvaziuojant visa ringa
    motoraiSuLaiku(
        170, -170, 50,
        &arBetKurisSensoriusKaNorsMato); // pasisuka i desine 90 laipsniu
    motoraiSuLaiku(130, 200, 460, &arBetKurisSensoriusKaNorsMato);  //
    motoraiSuLaiku(70, 190, 150, &arBetKurisSensoriusKaNorsMato);   //
    motoraiSuLaiku(60, 190, 130, &arBetKurisSensoriusKaNorsMato);   //
    motoraiSuLaiku(-200, 200, 170, &arBetKurisSensoriusKaNorsMato); //
    motoraiSuLaiku(200, 200, 90, &arBetKurisSensoriusKaNorsMato);

    break;

    //---------------------------------
  case 9: //--  trukciojimas-
    while (true) {
      motoraiSuLaiku(100, 100, 20, &tikLinija);
      motoraiSuLaiku(0, 0, 4800, &arBetKurisSensoriusKaNorsMato);
      if (arBetKurisSensoriusKaNorsMato()) {
        break;
      }
    }
    break;
  }
}
