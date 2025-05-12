#include "rfid.h"
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>

#define RC522_CS   26
#define RC522_RST  25

// HSPI pins
#define HSPI_SCK   14
#define HSPI_MISO  12
#define HSPI_MOSI  13

SPIClass hspi(HSPI);
MFRC522DriverPinSimple ss_pin(RC522_CS);
MFRC522DriverSPI driver{ss_pin, hspi};
MFRC522 mfrc522{driver};
MFRC522::MIFARE_Key key;

// List of writable blocks (avoids sector trailers like 7, 11, 15, etc.)
const byte writableBlocks[] = {
  4, 5, 6,
  8, 9, 10,
  12, 13, 14,
  16, 17, 18,
  20, 21, 22,
  24, 25, 26,
  28, 29, 30,
  32, 33, 34,
  36, 37, 38,
  40, 41, 42,
  44, 45, 46,
  48, 49, 50,
  52, 53, 54,
  56, 57, 58,
  60, 61, 62
}; // total: 45 blocks x 16 bytes = 720 bytes max


void setupRFID() {
  hspi.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, RC522_CS);
  mfrc522.PCD_Init();
  Serial.println("RC522 initialized on custom SPI bus.");
}

const char* readRFID() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("RFID card detected");

    Serial.print("Card UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    
    Serial.println();

    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD

  }
}

void writeRFIDMessage(const char *message) {
  size_t messageLen = strlen(message);
  size_t blocksNeeded = (messageLen + 15) / 16;

  if (blocksNeeded > sizeof(writableBlocks)) {
    Serial.println("Error: Message too long for tag.");
    return;
  }

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    Serial.println("No tag present.");
    return;
  }

  // Set default key (factory default)
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  for (size_t b = 0; b < blocksNeeded; b++) {
    byte buffer[16];

    // Fill this block’s 16 bytes
    for (byte j = 0; j < 16; j++) {
      size_t idx = b * 16 + j;
      buffer[j] = (idx < messageLen) ? message[idx] : ' ';
    }

    byte block = writableBlocks[b];

    // Authenticate
    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
      MFRC522Constants::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522Constants::STATUS_OK) {
      Serial.print("Auth failed at block "); Serial.println(block);
      return;
    }

    // Write
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522Constants::STATUS_OK) {
      Serial.print("Write failed at block "); Serial.println(block);
      return;
    }

    Serial.print("Wrote to block "); Serial.println(block);
  }

  Serial.println("✅ Message written successfully.");
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}


