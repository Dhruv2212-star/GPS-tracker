#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <SPI.h>
#include <SD.h>

// GPS TRACKER - Seeed XIAO RP2040
// Pin assignments match our PCB design.

// LEDs:
// D4 LED -> XIAO A1 / D1 / GPIO27
// D5 LED -> XIAO A2 / D2 / GPIO28
// D6 LED -> XIAO A3 / D3 / GPIO29
// D4 and D5 LEDs are ACTIVE-LOW.
// D6 is the hardware power LED and needs no GPIO control.
//
// Button:
// SW1 -> XIAO A4 / D4 / GPIO6 -> GND
//
// GPS:
// XIAO D6/TX -> GPS RX
// XIAO D7/RX <- GPS TX
//
// microSD SPI:
// XIAO D0  -> CS
// XIAO D8  -> SCK
// XIAO D9  <- MISO
// XIAO D10 -> MOSI

const uint8_t LED_GPS     = D1;
const uint8_t LED_LOGGING = D2;
const uint8_t BUTTON_PIN  = D4;

const uint8_t GPS_TX_PIN  = D6;
const uint8_t GPS_RX_PIN  = D7;

const uint8_t SD_CS_PIN   = D0;
const uint8_t SD_SCK_PIN  = D8;
const uint8_t SD_MISO_PIN = D9;
const uint8_t SD_MOSI_PIN = D10;

const uint32_t GPS_BAUD = 9600;
const uint32_t LOG_INTERVAL_MS = 1000;

TinyGPSPlus gps;
File logFile;

bool sdOK = false;
bool logging = false;
uint32_t lastLogTime = 0;
bool lastButtonState = HIGH;
uint32_t lastButtonChange = 0;
char logFilename[13];

void gpsLED(bool on) {
  digitalWrite(LED_GPS, on ? LOW : HIGH);
}

void loggingLED(bool on) {
  digitalWrite(LED_LOGGING, on ? LOW : HIGH);
}

bool createLogFile() {
  for (int i = 0; i < 100; i++) {
    snprintf(logFilename, sizeof(logFilename), "TRACK%02d.CSV", i);

    if (!SD.exists(logFilename)) {
      logFile = SD.open(logFilename, FILE_WRITE);

      if (!logFile) {
        return false;
      }

      logFile.println(
        "Date,Time,Latitude,Longitude,Altitude_m,Speed_kmh,Satellites"
      );
      logFile.flush();

      Serial.print("Logging to: ");
      Serial.println(logFilename);
      return true;
    }
  }

  return false;
}

void startLogging() {
  if (!sdOK || logging) {
    return;
  }

  if (!createLogFile()) {
    Serial.println("ERROR: Could not create log file.");
    return;
  }

  logging = true;
  lastLogTime = millis();
  loggingLED(true);
  Serial.println("Logging STARTED");
}

void stopLogging() {
  if (!logging) {
    return;
  }

  if (logFile) {
    logFile.flush();
    logFile.close();
  }

  logging = false;
  loggingLED(false);
  Serial.println("Logging STOPPED");
}

void writeGPSRecord() {
  if (!logging || !logFile) {
    return;
  }

  if (!gps.location.isValid()) {
    Serial.println("No valid GPS fix - record skipped.");
    return;
  }

  // Date
  if (gps.date.isValid()) {
    if (gps.date.month() < 10) logFile.print('0');
    logFile.print(gps.date.month());
    logFile.print('/');
    if (gps.date.day() < 10) logFile.print('0');
    logFile.print(gps.date.day());
    logFile.print('/');
    logFile.print(gps.date.year());
  } else {
    logFile.print("NO_DATE");
  }

  logFile.print(',');

  // UTC time
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) logFile.print('0');
    logFile.print(gps.time.hour());
    logFile.print(':');
    if (gps.time.minute() < 10) logFile.print('0');
    logFile.print(gps.time.minute());
    logFile.print(':');
    if (gps.time.second() < 10) logFile.print('0');
    logFile.print(gps.time.second());
  } else {
    logFile.print("NO_TIME");
  }

  logFile.print(',');
  logFile.print(gps.location.lat(), 6);
  logFile.print(',');
  logFile.print(gps.location.lng(), 6);
  logFile.print(',');

  if (gps.altitude.isValid()) {
    logFile.print(gps.altitude.meters(), 2);
  } else {
    logFile.print("NA");
  }

  logFile.print(',');

  if (gps.speed.isValid()) {
    logFile.print(gps.speed.kmph(), 2);
  } else {
    logFile.print("NA");
  }

  logFile.print(',');

  if (gps.satellites.isValid()) {
    logFile.println(gps.satellites.value());
  } else {
    logFile.println("NA");
  }

  logFile.flush();
  Serial.println("GPS record written.");
}

void handleButton() {
  bool currentState = digitalRead(BUTTON_PIN);

  if (currentState != lastButtonState) {
    lastButtonChange = millis();
    lastButtonState = currentState;
  }

  if ((millis() - lastButtonChange) > 40) {
    static bool handledPress = false;

    if (currentState == LOW && !handledPress) {
      handledPress = true;

      if (logging) {
        stopLogging();
      } else {
        startLogging();
      }
    }

    if (currentState == HIGH) {
      handledPress = false;
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED_GPS, OUTPUT);
  pinMode(LED_LOGGING, OUTPUT);
  gpsLED(false);
  loggingLED(false);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // GPS UART
  Serial1.setTX(GPS_TX_PIN);
  Serial1.setRX(GPS_RX_PIN);
  Serial1.begin(GPS_BAUD);

  // microSD SPI
  SPI.setRX(SD_MISO_PIN);
  SPI.setTX(SD_MOSI_PIN);
  SPI.setSCK(SD_SCK_PIN);
  SPI.setCS(SD_CS_PIN);
  SPI.begin();

  Serial.println("Initializing microSD...");
  sdOK = SD.begin(SD_CS_PIN);

  if (sdOK) {
    Serial.println("microSD OK");
  } else {
    Serial.println("ERROR: microSD initialization failed.");
  }

  Serial.println("GPS Tracker ready.");
  Serial.println("Press SW1 to START/STOP logging.");
}

void loop() {
  // Feed GPS NMEA data to TinyGPSPlus.
  while (Serial1.available()) {
    gps.encode(Serial1.read());
  }

  gpsLED(gps.location.isValid());

  handleButton();

  if (logging && (millis() - lastLogTime >= LOG_INTERVAL_MS)) {
    lastLogTime += LOG_INTERVAL_MS;
    writeGPSRecord();
  }

  static uint32_t lastDebug = 0;

  if (millis() - lastDebug >= 5000) {
    lastDebug = millis();

    Serial.print("Fix: ");
    Serial.print(gps.location.isValid() ? "YES" : "NO");

    Serial.print(" | Satellites: ");
    if (gps.satellites.isValid()) {
      Serial.print(gps.satellites.value());
    } else {
      Serial.print("NA");
    }

    Serial.print(" | Logging: ");
    Serial.println(logging ? "YES" : "NO");
  }
}
