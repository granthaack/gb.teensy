#include "Menu.h"

File menuBegin(FT81x &ft81x) {
    // Init the buttons
    pinMode(JOYPAD_START, INPUT_PULLUP);
    pinMode(JOYPAD_UP, INPUT_PULLUP);
    pinMode(JOYPAD_DOWN, INPUT_PULLUP);
    pinMode(JOYPAD_A, INPUT_PULLUP);
    // See if the card is present and can be initialized
    if (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("SD Card failed, or not present");
        SD.open("");
    }
    // Open the root directory
    File root = SD.open("/");
    uint8_t romCount = 0;

    // Get a valid file count
    while (true) {
        File entry = root.openNextFile();
        if(checkValidRomFile(entry)){
            // If it's a valid file, add it to the list
            Serial.printf("Found ROM file %s\n", entry.name());
            romCount++;
        }
        if (!entry) {
            // no more files
            break;
        }
        entry.close();
    }
    Serial.printf("Total ROMS: %i\n", romCount);
    root.close();

    // If there are no ROMs, return NULL
    if(romCount == 0){
        Serial.println("No ROMs found");
        return SD.open("");
    };
    // Otherwise, the user has to choose a ROM
    uint8_t sel = 0;
    char romName[17] = {0};
    uint8_t i = 0;
    while(true){
        root = SD.open("/");
        i = 0;
        // Init the display frame
        ft81x.beginDisplayList();
        ft81x.clear(FT81x_COLOR_RGB(0, 0, 0));
        // Iterate through all the valid ROM files
        // and print them on the screen
        // Scroll up
        if(!digitalReadFast(JOYPAD_UP)){
            if(sel <= 0){
                sel = romCount;
            }
            else{
                sel--;
            }
            delay(300);
        }
        // Scroll down
        else if(!digitalReadFast(JOYPAD_DOWN)){
            if(sel >= romCount){
                sel = 0;
            }
            else{
                sel++;
            }
            delay(300);
        }
        while(i < romCount){
            File entry = root.openNextFile();
            Serial.println(entry.name());
            if(checkValidRomFile(entry)){
                getRomFileName(entry, romName);
                // If it's a valid file, print it
                // Highlight the currently selected ROM in blue
                if(i == sel){
                    ft81x.drawText(10, 15 * i, 16, FT81x_COLOR_RGB(0, 0, 255), 0, romName);
                    // Select a ROM
                    if(!digitalReadFast(JOYPAD_START)){
                        Serial.printf("Loading %s...\n", romName);
                        return entry;
                    }
                }
                else{
                    ft81x.drawText(10, 15 * i, 16, FT81x_COLOR_RGB(255, 255, 255), 0, romName);
                }
                i++;
            }
            if (!entry) {
                // no more files
                break;
            }
            // Poll all the buttons
            entry.close();
        }
        ft81x.swapScreen();
    }

}