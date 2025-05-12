#include <SPI.h>
#include <Arduino.h>
#include <lvgl.h>
#include "UI/ui.h"
#include <TFT_eSPI.h>
#include <EEPROM.h>
#include "settings.h"
#include "UI/ui_events.h"
#include "DFRobotDFPlayerMini.h"
#include "Games/games.h"
#include "music.h"
#include "RFID/rfid.h"
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

uint8_t currentSong = 0;

// Add these variables at the top with other globals
static unsigned long lastTouchTime = 0;
const unsigned long TOUCH_DEBOUNCE = 200; // 200ms debounce time

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

#pragma region RFID
void WriteRFIDMessageMain() {
    // Get data from text field
    const char *data = lv_textarea_get_text(ui_TextArea1);
    writeRFIDMessage(data);
}

void ReadRFIDMessageMain() {
    const char *data = readRFID();
    lv_textarea_set_text(ui_TextArea1, data);
}
#pragma endregion

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
    unsigned long currentTime = millis();
    
    bool touched = tft.getTouch(&touchX, &touchY, 600);
    
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        // Only register touch if enough time has passed since last touch
        if (currentTime - lastTouchTime >= TOUCH_DEBOUNCE) {
            data->state = LV_INDEV_STATE_PR;
            data->point.x = touchX;
            data->point.y = touchY;
            lastTouchTime = currentTime;
        } else {
            data->state = LV_INDEV_STATE_REL;
        }
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

#pragma region Audio

void PreviousSongMain() {
    currentSong = myDFPlayer.readCurrentFileNumber();
    myDFPlayer.previous();
    Serial.print("DFPlayer song number: ");
    Serial.println(currentSong - 1);
    
    // Adjust index (DFPlayer uses 1-based indexing)
    uint8_t adjustedIndex = (currentSong > 0) ? currentSong - 1 : 0;
    Serial.print("Adjusted index: ");
    Serial.println(adjustedIndex - 1);
    Serial.print("Song name: ");
    Serial.println(songs[adjustedIndex - 1]);
    
    lv_label_set_text(ui_Label3, (const char*)songs[adjustedIndex - 1]);
}   

void SkipSongMain() {
    currentSong = myDFPlayer.readCurrentFileNumber();
    myDFPlayer.next();
    Serial.print("DFPlayer song number: ");
    Serial.println(currentSong + 1);
    
    // Adjust index (DFPlayer uses 1-based indexing)
    uint8_t adjustedIndex = (currentSong > 0) ? currentSong - 1 : 0;
    Serial.print("Adjusted index: ");
    Serial.println(adjustedIndex + 1);
    Serial.print("Song name: ");
    Serial.println(songs[adjustedIndex + 1]);
    
    lv_label_set_text(ui_Label3, (const char*)songs[adjustedIndex + 1]);
}

bool AreWePlaying = false;

void PlayPauseMain() {
    currentSong = myDFPlayer.readCurrentFileNumber();
    Serial.print("DFPlayer song number: ");
    Serial.println(currentSong);
    
    // Adjust index (DFPlayer uses 1-based indexing)
    uint8_t adjustedIndex = (currentSong > 0) ? currentSong - 1 : 0;
    Serial.print("Adjusted index: ");
    Serial.println(adjustedIndex);
    Serial.print("Song name: ");
    Serial.println(songs[adjustedIndex]);
    
    lv_label_set_text(ui_Label3, (const char*)songs[adjustedIndex]);

    if (currentSong == 0) {
        myDFPlayer.playMp3Folder(1);
        currentSong = 1;
        AreWePlaying = true;
        lv_label_set_text(ui_Label3, (const char*)songs[0]); // First song in array
    } else if (AreWePlaying) {  // If we're playing, pause
        AreWePlaying = false;
        myDFPlayer.pause();
    } else {  // If we're paused, resume
        myDFPlayer.start();
        AreWePlaying = true;
    }
}

#pragma endregion

#pragma region Menu Sounds
void PlayMenuSound(uint8_t _sound) {
    myDFPlayer.advertise(_sound);
}

void ChangeVolumeMain(uint8_t _volume) {
    myDFPlayer.volume(_volume);
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
    
    // Initialize display first
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
    
    // Initialize UI and show boot screen
    Serial.println("Initializing UI...");
    ui_init();
    lv_timer_handler();
    lv_bar_set_value(ui_Bar1, 10, LV_ANIM_OFF); // Display initialized
    lv_timer_handler();
    lv_tick_inc(100);
    delay(100);

    // Initialize DFPlayer
    pinMode(32, INPUT);
    FPSerial.begin(9600, SERIAL_8N1, /*rx =*/5, /*tx =*/17);
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
    lv_bar_set_value(ui_Bar1, 20, LV_ANIM_OFF);
    lv_timer_handler();
    lv_tick_inc(1);    

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
    myDFPlayer.volume(20);
    lv_bar_set_value(ui_Bar1, 40, LV_ANIM_OFF); // Audio initialized
    lv_timer_handler();
    lv_tick_inc(100);
    delay(100);

    // Initialize EEPROM and load settings
    Serial.println("Loading settings...");
    EEPROM.begin(EEPROM_SIZE);
    loadSettings();
    hours = settings.hours;
    minutes = settings.minutes;
    lv_bar_set_value(ui_Bar1, 60, LV_ANIM_OFF); // Settings loaded
    lv_timer_handler();
    lv_tick_inc(100);
    delay(100);

    // Initialize time system
    Serial.println("Initializing time system...");
    updateTimeRollers(settings.hours, settings.minutes);
    lv_bar_set_value(ui_Bar1, 80, LV_ANIM_OFF); // Time system initialized
    lv_timer_handler();
    lv_tick_inc(100);
    delay(100);

    // Initialize RFID
    Serial.println("Initializing RFID...");
    setupRFID();
    lv_bar_set_value(ui_Bar1, 90, LV_ANIM_OFF); // RFID initialized
    lv_timer_handler();
    lv_tick_inc(100);
    delay(100);

    // Final setup steps
    Serial.println("Finalizing setup...");
    lv_bar_set_value(ui_Bar1, 100, LV_ANIM_OFF); // Setup complete
    lv_timer_handler();
    lv_tick_inc(500);
    delay(500); // Show 100% for a moment

    // Populate radio Dropdown1 with songs
    String songList = "";
    for (int i = 0; i < sizeof(songs)/sizeof(songs[0]); i++) {
        songList += songs[i];
        if (i < sizeof(songs)/sizeof(songs[0]) - 1) {
            songList += "\n";
        }
    }
    lv_dropdown_set_options(ui_Dropdown1, songList.c_str());
    // Load main screen
    Serial.println("Loading main screen...");
    lv_scr_load(ui_Main);
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

    if (myDFPlayer.available() && AreWePlaying) {
        uint8_t type = myDFPlayer.readType();
        int value = myDFPlayer.read();
        
        if (type == DFPlayerPlayFinished && value > 0 && value <= 9) {  // Only auto-play for songs 1-9
            currentSong = myDFPlayer.readCurrentFileNumber();
            myDFPlayer.next();
            uint8_t adjustedIndex = (currentSong > 0) ? currentSong - 1 : 0;
            lv_label_set_text(ui_Label3, (const char*)songs[adjustedIndex + 1]);
        }
        
        printDetail(type, value);
    }

    if (isPlaying) {
        HandleGameInput();
        UpdateGame();
        DrawGame();
        delay(16); // 60fps
    }
}