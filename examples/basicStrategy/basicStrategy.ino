/*
  Sumo robot basic strategy

  Description TODO

  created 2022-03-28
  by Lukas Neverauskis
*/

#include "subot.hpp"

void setup() { Serial.begin(9600); }

void loop(void) {
  Serial.print("basic strategy  ");
  Serial.print( addTwoInts(1, 2));
  Serial.println();
  delay(1000);
}
