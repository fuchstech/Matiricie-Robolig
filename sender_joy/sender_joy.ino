#include <esp_now.h>
#include <WiFi.h>


#define initButton D13  //Deneyap Mininin üzerindeki düğme


// Kendi Mac adresiniz ile değiştirin mac adresini öğrenmek için mac_address_learn kodunu çalıştırabilirsiniz
uint8_t broadcastAddress[] = { 0x48, 0xE7, 0x29, 0x3C, 0xFC, 0x80 };  //mac adres çıktısını değişken haline getirmek için mac_converter.py
int rjoyx, rjoyy, ljoyx, ljoyy;                                       // joystick eksenlerinin verileri için değişkenler
//alıcı veri yapısı ile uyumlu olmak zorundadır
typedef struct struct_message {
  char a[32];
  int leftMotor;
  int rightMotor;
  bool direct;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;
bool dstat = true;
// Veri gönderildiğinde çalışacak callback fonksiyonu
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nSon Paket Gönderim Durumu:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Gönderim Başarılı" : "Gönderim Yapılamadı");
}

void setup() {
#if defined(ARDUINO_DYDK) || defined(ARDUINO_DYM)  // DENEYAP KART veya DENEYAP MİNİ kartı kullanılıyorsa
  pinMode(LEDR, OUTPUT);                           // Kirmizi led cikis olarak ayarlandi
  pinMode(LEDG, OUTPUT);                           // Yesil led cikis olarak ayarlandi
  pinMode(LEDB, OUTPUT);                           // Mavi led cikis olarak ayarlandi
#endif
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  digitalWrite(LEDR, HIGH);  //Program Başlangıç Beyaz ışık
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
  delay(300);
  Serial.begin(115200);  //Seri Port 115200 baud Serial Monitor Başlatma


  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Esp-Now Baslatılırken Hata");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // peer'a bağlan
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // peer'ı ekle
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer Eklenirken Hata");
    return;
  }
}

void loop() {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));  //Veri Gönderimi
  rjoyy = map(analogRead(A1), 0, 8191, 0, 255) - 155;                                     //Verinin oranlanması ve joystick ekseninin sıfırlanması
  rjoyx = map(analogRead(A0), 0, 8191, 0, 255) - 155;
  ljoyy = map(analogRead(A3), 0, 8191, 0, 255) - 155;
  ljoyx = map(analogRead(A2), 0, 8191, 0, 255) - 155;

  if (result == ESP_OK) {
    Serial.println("Başarıyla Gönderildi");
  } else {
    Serial.println("Veri Gönderiminde Hata");
  }
  if (!digitalRead(initButton)) {  //Sürüş Modu ve Servo Modu değişimi
    dstat = !dstat;
    return;
  } else {
  }
  if (dstat) {
    digitalWrite(LEDR, LOW);  //Yeşil atış modu
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, LOW);

  } else {
    digitalWrite(LEDR, HIGH);  //kırmızı sürüş modu
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, LOW);
  }
  myData.leftMotor = ljoyy;   //solMotor verisi
  myData.rightMotor = rjoyy;  //sağMotor verisi
  myData.direct = dstat;      //mod düğmesi verisi
  Serial.print(myData.leftMotor);
  Serial.print("\t");
  Serial.print(myData.rightMotor);
  Serial.print("\t");


  delay(100);
}
