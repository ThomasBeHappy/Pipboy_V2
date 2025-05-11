#include <SPI.h>
#include <Arduino.h>
#include <lvgl.h>
#include "ui.h"
#include <TFT_eSPI.h>
#include <EEPROM.h>
#include "settings.h"
#include "ui_events.h"
#include "DFRobotDFPlayerMini.h"
#include "Games/flappy.h"
#include "Games/games.h"

#define FPSerial Serial1

// Display configuration
#define TFT_WIDTH 320   // Physical display width
#define TFT_HEIGHT 240  // Physical display height
#define BUFFER_SIZE (TFT_WIDTH * 75)  // Buffer for 30 lines (or more, if RAM allows)

// Settings instance
Settings settings;

// EEPROM settings address
#define EEPROM_SIZE 512
#define SETTINGS_ADDR 0

// Display driver instance
TFT_eSPI tft = TFT_eSPI();

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[BUFFER_SIZE];

// Time variables
uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;

// DFPlayer
DFRobotDFPlayerMini myDFPlayer;

// Calculate checksum for settings
uint8_t calculateChecksum(Settings &s) {
    uint8_t sum = 0;
    uint8_t *ptr = (uint8_t*)&s;
    for(size_t i = 0; i < sizeof(Settings)-1; i++) {
        sum += ptr[i];
    }
    return sum;
}

#ifdef __cplusplus
extern "C" {
#endif

void saveSettings() {
    settings.checksum = calculateChecksum(settings);
    EEPROM.put(SETTINGS_ADDR, settings);
    EEPROM.commit();
}

void loadSettings() {
    EEPROM.get(SETTINGS_ADDR, settings);
    if (settings.checksum != calculateChecksum(settings)) {
        // If checksum doesn't match, load defaults
        settings = {
            .brightness = 100,
            .volume = 10,
            .soundEnabled = true,
            .wifiEnabled = false,
            .bluetoothEnabled = false,
            .hours = 12,
            .minutes = 0,
            .checksum = 0
        };
        saveSettings();
    }
}

#ifdef __cplusplus
}
#endif

#pragma region Time Rollers
void updateTimeRollers(uint8_t _hours, uint8_t _minutes) {
    const char* hours_options = lv_roller_get_options(ui_Roller1);
    const char* minutes_options = lv_roller_get_options(ui_Roller2);
    uint16_t hours_option = 0;
    uint16_t minutes_option = 0;

    for (int i = 0; i < 24; i++) {
        if (atoi(&hours_options[i * 3]) == _hours) {
            hours_option = i;
        }
    }

    for (int i = 0; i < 60; i++) {
        if (atoi(&minutes_options[i * 3]) == _minutes) {
            minutes_option = i;
        }
    }

    lv_roller_set_selected(ui_Roller1, hours_option, LV_ANIM_ON);
    lv_roller_set_selected(ui_Roller2, minutes_option, LV_ANIM_ON);
}
#pragma endregion

#pragma region Display Driver   
// Display driver callback functions
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

static uint16_t calData[5] = {686, 2786, 494, 3022, 4};

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY, 600); // 600 = threshold, adjust if needed
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}
#pragma endregion

#pragma region DFPlayer
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
#pragma endregion

#pragma region Menu Sounds
void PlayMenuSound(uint8_t _sound) {
    myDFPlayer.play(_sound);
}
#pragma endregion

#pragma region Games
void StartFlappyGameMain() {
    StartGame(GAME_FLAPPY);
}

void StartSnakeGameMain() {
    StartGame(GAME_SNAKE);
}

void StartPongGameMain() {
    StartGame(GAME_PONG);
}

void Start2048GameMain() {
    StartGame(GAME_2048);
}
#pragma endregion

void setup() {
    Serial.begin(115200);
    Serial.println("\n\nStarting setup...");
    
    pinMode(32, INPUT);

    FPSerial.begin(9600, SERIAL_8N1, /*rx =*/5, /*tx =*/17);

    Serial.println(F("DFRobot DFPlayer Mini Demo"));
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

    // Try initialization multiple times
    uint8_t initAttempts = 0;
    bool dfPlayerInitialized = false;
    
    while (!dfPlayerInitialized && initAttempts < 3) {
        Serial.printf("DFPlayer initialization attempt %d...\n", initAttempts + 1);
        dfPlayerInitialized = myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true);
        if (!dfPlayerInitialized) {
            Serial.println(F("Initialization failed, retrying..."));
            delay(1000);
        }
        initAttempts++;
    }

    if (!dfPlayerInitialized) {
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        Serial.println(F("3.Check if the SD card is formatted as FAT32"));
        Serial.println(F("4.Check if there are MP3 files in the root directory"));
        while(true);
    }

    Serial.println(F("DFPlayer Mini online."));
    
    // Set volume and verify it worked
    myDFPlayer.volume(20);
    delay(100);

    // Initialize EEPROM
    EEPROM.begin(EEPROM_SIZE);
    loadSettings();
    
    // initialize time settings to ram
    hours = settings.hours;
    minutes = settings.minutes;

    // Initialize display
    Serial.println("Initializing display...");
    tft.init();
    tft.setRotation(1);
    tft.setTouch(calData);
    tft.fillScreen(TFT_BLACK);
    Serial.printf("Display initialized. Width: %d, Height: %d\n", tft.width(), tft.height());
    
    
    // Initialize LVGL
    Serial.println("Initializing LVGL...");
    lv_init();
    
    // Initialize display buffer with double buffering
    Serial.printf("Initializing display buffer. Size: %d bytes\n", BUFFER_SIZE * sizeof(lv_color_t));
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, BUFFER_SIZE);
    
    // Initialize display driver
    Serial.println("Setting up display driver...");
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_WIDTH;   // Set to physical width
    disp_drv.ver_res = TFT_HEIGHT;  // Set to physical height
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    Serial.printf("Display driver configured. Resolution: %dx%d\n", TFT_WIDTH, TFT_HEIGHT);
    
    // Register touch input device with LVGL
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
    
    // Initialize UI
    Serial.println("Initializing UI...");
    ui_init();
    lv_timer_handler();

    // Update booting screen to simulate progress
    Serial.println("Updating boot progress...");
    for (int i = 0; i < 10; i++) {
      lv_bar_set_value(ui_Bar1, i * 10, LV_ANIM_OFF);
      lv_timer_handler();
      lv_tick_inc(500);
      delay(500);
    }

    // Load main screen
    Serial.println("Loading main screen...");
    lv_scr_load(ui_Main);
    updateTimeRollers(settings.hours, settings.minutes);
    Serial.println("Setup complete!");
}

String getTimeString() {
    // Get current time based on settings
    if (hours < 12) {
        return String(hours) + ":" + String(minutes) + " AM";
    } else {
        return String(hours - 12) + ":" + String(minutes) + " PM";
    }
}

void updateTimeSettings(uint8_t _hours, uint8_t _minutes) {
    // Update time eeprom settings

    hours = _hours;
    minutes = _minutes;

    settings.hours = _hours;
    settings.minutes = _minutes;
    saveSettings();
}

void loop() {
    if (!isPlaying) {
        lv_timer_handler(); // Handle LVGL tasks
        lv_tick_inc(1);
        delay(1);
        // Get current time once for all timing operations
        unsigned long currentTime = millis();

        // Update Time
        static String lastTimeString = "";
        String currentTimeString;

        // Format time string with leading zeros and correct AM/PM
        uint8_t displayHour = hours % 12;
        if (displayHour == 0) displayHour = 12;
        String ampm = (hours < 12) ? "AM" : "PM";
        currentTimeString = String(displayHour) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + " " + ampm;

        // Only update label if the time string has changed
        if (currentTimeString != lastTimeString) {
            lv_label_set_text(ui_Time, currentTimeString.c_str());
            lastTimeString = currentTimeString;
        }

        // Increase time based on time passed since last update
        static unsigned long lastUpdate = 0;
        if (currentTime - lastUpdate >= 1000) {
            lastUpdate = currentTime;
            seconds++;
        } else {
            seconds += (currentTime - lastUpdate) / 1000;
        }

        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            updateTimeRollers(hours, minutes);
        } 

        if (minutes >= 60) {
            minutes = 0;
            hours++;
            updateTimeRollers(hours, minutes);
        } 

        if (hours >= 24) {
            hours = 0;
            updateTimeRollers(hours, minutes);
        }

        // Every 5 minutes update time settings
        static unsigned long lastTimeUpdate = 0;
        if (currentTime - lastTimeUpdate >= 5 * 60 * 1000) {
            lastTimeUpdate = currentTime;
            updateTimeSettings(hours, minutes);
        }
    }

    if (myDFPlayer.available()) {
        printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }

    if (isPlaying) {
        HandleGameInput();
        UpdateGame();
        DrawGame();
        delay(16); // 60fps
    }
}