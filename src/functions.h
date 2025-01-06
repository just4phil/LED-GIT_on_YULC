//=====================================================================
//=========== HELPER FUNCTIONS ========================================
//=====================================================================


#pragma pack(push, 1)   // Das #pragma pack(push, 1) stellt sicher, dass die Struktur ohne zusätzliche Ausrichtung (also ohne Padding) gespeichert wird.
struct SongAndPart {
    uint8_t songID;
    uint8_t part;
};
int getRandomColorValue();
int getRandomColor();
int getRandomColorIncludingBlack();
void switchToPart(byte part);
void switchToSong(byte song);
void switchToSongAndPart(byte song, byte part);
