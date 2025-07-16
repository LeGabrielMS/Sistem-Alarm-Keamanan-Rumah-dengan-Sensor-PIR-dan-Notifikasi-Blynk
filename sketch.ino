#define BLYNK_TEMPLATE_ID "TMPL6o3Xlg7q1"
#define BLYNK_TEMPLATE_NAME "Proyek Smart Home Menggunakan Wokwi dan Blynk"
#define BLYNK_AUTH_TOKEN "qZhD_CdVfjILZMbaTiSa4d28d_OD6wgl"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat default I2C

const char* ssid = "Wokwi-GUEST";
const char* pass = "";

#define PIR_PIN 14
#define BUZZER_PIN 19
#define LED_PIN 4
#define VPIN_RESET V9

bool alarmActive = false;
unsigned long lastResetTime = 0;
const unsigned long cooldown = 3000; // 3 detik cooldown

void setup() {
  Serial.begin(115200);

  lcd.init();          // Wajib: inisialisasi LCD
  lcd.backlight();     // Nyalakan backlight LCD

  lcd.setCursor(0, 0);
  lcd.print("Smart Home Ready");
  delay(2000);
  lcd.clear();

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  unsigned long now = millis();

  if (!alarmActive && digitalRead(PIR_PIN) == HIGH && now - lastResetTime > cooldown) {
    alarmActive = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motion Detected");
    lcd.setCursor(0, 1);
    lcd.print("ALARM ACTIVE");

    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Gerakan terdeteksi! Alarm aktif.");
    Blynk.logEvent("alarm_triggered", "Gerakan terdeteksi!");
    Blynk.virtualWrite(VPIN_RESET, 1);
  }

  // Tampilkan status jika tidak ada gerakan
  if (!alarmActive && now - lastResetTime > cooldown) {
    lcd.setCursor(0, 0);
    lcd.print("System Ready    ");
    lcd.setCursor(0, 1);
    lcd.print("Monitoring...   ");
  }
}

BLYNK_WRITE(VPIN_RESET) {
  int value = param.asInt();
  if (value == 0 && alarmActive) {
    alarmActive = false;
    lastResetTime = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm Reset");
    lcd.setCursor(0, 1);
    lcd.print("System Ready");

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    Serial.println("Alarm dimatikan oleh pengguna.");
  }
}
