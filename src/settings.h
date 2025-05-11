#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif


// Settings structure
struct Settings {
    uint8_t brightness;
    uint8_t volume;
    bool soundEnabled;
    bool wifiEnabled;
    bool bluetoothEnabled;
    uint8_t hours;
    uint8_t minutes;
    uint8_t checksum;
};

extern struct Settings settings;
void loadSettings();
void saveSettings();
void updateTimeSettings(uint8_t _hours, uint8_t _minutes);

// Sound functions
void PlayMenuSound(uint8_t _sound);
void ChangeVolumeMain(uint8_t _volume);
// Music functions
void PreviousSongMain();
void SkipSongMain();
void PlayPauseMain();

// Game functions
void StartFlappyGameMain();
void StartSnakeGameMain();
void StartPongGameMain();
void Start2048GameMain();


#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H 