/*
*   ServoMotor örneği,
*   D9 pinine bağlanan servo motorun mili 60 derece dönmektedir.
*/
#include <Deneyap_Servo.h>  // Deneyap Servo kütüphanesi eklenmesi

Servo myservo;  // Servo için class tanımlanması

void setup() {
  myservo.attach(D9);
}
void loop() {

  myservo.write(100);  // Servo motorun milinin 60 derece dönmesi
  delay(1000);
  myservo.write(180);
  delay(3000);
}