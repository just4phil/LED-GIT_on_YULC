//=====================================================================
//=========== HELPER FUNCTIONS ========================================
//=====================================================================


/* msgType -> 
    0 = set song & Part
    1 = change Song -> only songID
    2 = change part -> only partID
    3 = force sync to clients -> songID & partID
    4 = switch part after LEDsync
*/
#pragma pack(push, 1)   // Das #pragma pack(push, 1) stellt sicher, dass die Struktur ohne zusätzliche Ausrichtung (also ohne Padding) gespeichert wird.
struct BLEmessage {
    uint8_t msgType; // msgType -> 0 = setNextPart / 1 = change Song / 2 = change part / 3 = force sync to clients
    uint8_t songID;
    uint8_t part;
};

int getRandomColorValue();
int getRandomColor();
int getRandomColorIncludingBlack();
void switchToPart(byte part);
void switchToSong(byte song);
void switchToSongAndPart(byte song, byte part);
