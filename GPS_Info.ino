#include <HardwareSerial.h>

// --- Modem pins ---
#define MODEM_TX_PIN      43
#define MODEM_RX_PIN      44
#define MODEM_PWRKEY_PIN  42
#define MODEM_RESET_PIN   41
#define MODEM_POWER_EN    40
#define MODEM_STATUS_PIN  18

// UART0 (U0TXD / U0RXD)
HardwareSerial SerialAT(0);

//  clear UART buffer
void clearSerialAT() {
  while (SerialAT.available()) SerialAT.read();
}


// Send AT command 
bool sendATCommand(const String &cmd, String &response, uint32_t timeout = 5000) {
  clearSerialAT();
  SerialAT.print(cmd);
  SerialAT.print("\r\n");

  uint32_t start = millis();
  response = "";

  while (millis() - start < timeout) {
    while (SerialAT.available()) {
      char c = SerialAT.read();
      response += c;
      if (response.indexOf("\r\nOK\r\n") != -1 ||
          response.indexOf("\r\nERROR\r\n") != -1 ||
          response.indexOf("NO CARRIER") != -1) {
        return true;
      }
    }
  }
  return response.length() > 0;
}


// Power-on 
void modemPowerOn() {
  Serial.println("\n=== MODEM POWER ON ===");

  pinMode(MODEM_POWER_EN, OUTPUT);
  pinMode(MODEM_PWRKEY_PIN, OUTPUT);
  pinMode(MODEM_RESET_PIN, OUTPUT);
  pinMode(MODEM_STATUS_PIN, INPUT);

  digitalWrite(MODEM_POWER_EN, HIGH);
  delay(100);

  digitalWrite(MODEM_RESET_PIN, LOW);
  delay(100);

  digitalWrite(MODEM_PWRKEY_PIN, HIGH);
  delay(1200);
  digitalWrite(MODEM_PWRKEY_PIN, LOW);

  Serial.print("Waiting for STATUS... ");
  uint32_t t0 = millis();
  while (millis() - t0 < 10000) {
    if (digitalRead(MODEM_STATUS_PIN)) {
      Serial.println("OK");
      return;
    }
    delay(100);
  }
  Serial.println("TIMEOUT");
}

// --------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("\n===== ESP32-S3 SIM767XX AUTO CONFIG =====");

  modemPowerOn();

  Serial.println("Waiting 12 seconds for modem boot...");
  delay(12000);

  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

  String resp;

  Serial.println("\n=== BASIC SETUP ===");

  sendATCommand("AT", resp);
  Serial.println(resp);

  sendATCommand("ATE0", resp);
  Serial.println(resp);

  sendATCommand("AT+CSCLK=0", resp);
  Serial.println(resp);

  sendATCommand("AT+IPR=115200", resp);
  Serial.println(resp);

  sendATCommand("AT+IPREX=115200", resp);
  Serial.println(resp);

  sendATCommand("AT+ICF=3,0", resp);
  Serial.println(resp);

  Serial.println("\n=== PRODUCT IDENTIFICATION ===");

  sendATCommand("ATI", resp, 6000);
  Serial.println(resp);

  sendATCommand("AT+CGMI", resp);
  Serial.println(resp);

  sendATCommand("AT+CGMM", resp);
  Serial.println(resp);

  sendATCommand("AT+CGMR", resp);
  Serial.println(resp);

  sendATCommand("AT+CGSN", resp);
  Serial.println(resp);

  Serial.println("\n=== CONFIGURATION & STATUS ===");

  sendATCommand("AT&V", resp, 1200);
  Serial.println(resp);

  sendATCommand("AT+CPIN?", resp);
  Serial.println(resp);

  sendATCommand("AT+CSQ", resp);
  Serial.println(resp);

  sendATCommand("AT+CREG?", resp);
  Serial.println(resp);

  sendATCommand("AT+CEREG?", resp);
  Serial.println(resp);

  sendATCommand("AT+CFUN?", resp);
  Serial.println(resp);

  Serial.println("\n=== GNSS / GPS CONFIGURATION ===");

  sendATCommand("AT+CGNSSPWR=1", resp);
  Serial.println(resp);

  sendATCommand("AT+CGNSSTST=1", resp);
  Serial.println(resp);

  sendATCommand("AT+CGNSSIPR=115200", resp);
  Serial.println(resp);

  sendATCommand("AT+CGNSSMODE=15", resp);
  Serial.println(resp);

  sendATCommand("AT+CGNSSNMEA=1,1,1,1,1,1,0,0,0,0", resp);
  Serial.println(resp);

  sendATCommand("AT+CGNSSNMEARATE=1", resp);
  Serial.println(resp);

  sendATCommand("AT+CGNSSPORTSWITCH=0,1", resp);
  Serial.println(resp);

  sendATCommand("AT+CGNSSRTC=0", resp);
  Serial.println(resp);

  sendATCommand("AT+CGNSSWAKEUP", resp);
  Serial.println(resp);

  
  Serial.println("\n=== WAITING FOR GPS FIX (UP TO 2 MINUTES) ===");

  bool gpsFix = false;

  for (int i = 0; i < 24; i++) {
    sendATCommand("AT+CGPSINFO", resp, 5000);
    Serial.println(resp);

    if (resp.indexOf(",,,,,,,,") == -1 && resp.indexOf(",") != -1) {
      gpsFix = true;
      Serial.println("GPS FIX ACQUIRED!");
      break;
    }

    Serial.println("Searching for satellites...");
    delay(5000);
  }

  if (!gpsFix) {
    Serial.println("GPS FIX NOT ACQUIRED YET. ");
  }

  Serial.println("\n=== MODEM + GNSS READY ===");
}

// --------------------------------------------------
void loop() {
  if (SerialAT.available()) Serial.write(SerialAT.read());
  if (Serial.available()) SerialAT.write(Serial.read());
}
