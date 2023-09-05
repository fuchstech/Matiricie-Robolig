#include <esp_now.h>
#include <WiFi.h>
#include <Deneyap_Servo.h>
// mesaj yapısının göndericiyle uyumlu olması gerekir
typedef struct struct_message {
  char a[32];
  int leftMotor;
  int rightMotor;
  bool direct;
} struct_message;


struct_message myData;
Servo myservo;
int servoStart = 75;
int servoEnd = 180;
// veri geldiğinde çalışacak fonksiyon
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Sol Motor: ");
  Serial.println(myData.leftMotor);
  Serial.print("Sağ Motor: ");
  Serial.println(myData.rightMotor);
  Serial.print("Yön ");
  Serial.println(myData.direct);
}

void setup() {
  myservo.attach(D9);

#if defined(ARDUINO_DYDK) || defined(ARDUINO_DYM)  // DENEYAP KART veya DENEYAP MİNİ kartı kullanılıyorsa
  pinMode(LEDR, OUTPUT);                           // Kirmizi led cikis olarak ayarlandi
  pinMode(LEDG, OUTPUT);                           // Yesil led cikis olarak ayarlandi
  pinMode(LEDB, OUTPUT);                           // Mavi led cikis olarak ayarlandi
#endif
  pinMode(D12, OUTPUT);
  pinMode(D13, OUTPUT);
  pinMode(D14, OUTPUT);
  pinMode(D15, OUTPUT);
  pinMode(PWM0, OUTPUT);
  pinMode(PWM1, OUTPUT);

  Serial.begin(115200); //serial başlatma

  WiFi.mode(WIFI_STA); //Deneyap Kartı WIFI_STA olarak ayarlama

  // ESP-NOW Protokolünü Başlatma
  if (esp_now_init() != ESP_OK) {
    Serial.println("Esp-Now Baslatılırken Hata");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if (myData.direct) { // Yazılımın ilk aşamalarında direct verisi motor yönü tayini için iletiliyordu ancak sonraki aşamalarda atış modu ve sürüş modu olarak güncellendi
    servoStart = constrain(servoStart,0,180); // servo açı ayarlamalarında açı verisinin 0,180 arasında kısıtlanması
    servoEnd = constrain(servoEnd,0,180);
    if (myData.leftMotor < -150) { //sol joystick yukarı --> servo başlangıç açısı arttırımı
      servoStart = servoStart + 10;
    } else if (myData.leftMotor > 90) {
      servoStart = servoStart - 10; //sol joystick aşağı --> servo başlangıç açısı azaltımı
    }
    if (myData.rightMotor < -150) {
      servoEnd = servoEnd - 10;//sağ joystick yukarı --> servo bitiş açısı arttırımı
    } else if (myData.rightMotor > 90) {
      servoEnd = servoEnd + 10;//sol joystick aşağı --> servo bitiş açısı azaltımı
    }

    myservo.write(servoStart); // Otonom atış loop
    delay(1000);
    myservo.write(servoEnd);
    delay(2000);
    Serial.print(servoStart);
    Serial.print("\t");
    Serial.print(servoEnd);
  } else {
    tank_move(myData.leftMotor, myData.rightMotor, 1); //Sürüş modu 
    delay(50); //Bağlantı Delay
  }
}
void tank_move(int left, int right, bool direct) { // tank sürüş fonksiyonu

  if (left < 0) {
    digitalWrite(D12, !direct); //Eğer sol joystick yukarı ise değer eksi olacağı için (sender analogRead-155) sol motorlar ileri
    digitalWrite(D13, direct);
    analogWrite(PWM0, abs(left) * 1.64); //155 değerini 255e oranlamak için çarpı 1.7

  } else {
    digitalWrite(D12, direct);
    digitalWrite(D13, !direct);
    analogWrite(PWM0, left); //Eğer joystick aşağı ise sol motorlar geri
  }
  if (right < 0) {
    digitalWrite(D14, direct);//Eğer sağ joystick yukarı ise değer eksi olacağı için (sender analogRead-155) sağ motorlar ileri
    digitalWrite(D15, !direct);
    analogWrite(PWM1, abs(right) * 1.64);

  } else {
    digitalWrite(D14, !direct);//Eğer sağ joystick aşağı ise sağ motorlar geri
    digitalWrite(D15, direct);
    analogWrite(PWM1, right);
  }
  /* if (direct) {
    analogWrite(PWM0, left);
    analogWrite(PWM1, right);
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, LOW);
    delay(50);

  } else {
    analogWrite(PWM0, right);
    analogWrite(PWM1, left);
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, LOW);
    delay(50);
  }*/
}
