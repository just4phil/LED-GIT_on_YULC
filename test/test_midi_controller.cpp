#include <unity.h>
#include "../src/definitions.h"
#include "../src/functions.h"
#include "../src/midi_in.h"

// Mock variables to verify state changes
static byte currentSong = 0;
static byte currentPart = 0;

#ifdef IS_MIDI_PROXY
    volatile bool newMidiValuesToBroadcast = false;
    volatile byte midiInCC = 0;
    volatile byte midiInValue = 0;
#endif

// Mock functions
void switchToSong(byte value) {
    currentSong = value;
}

void switchToPart(byte value) {
    currentPart = value;
}

// Implementation für den Test
void MidiDatenAuswerten(byte channel, byte number, byte value) {
    if (number == 22 && value > 0) {
        switchToSong(value);
    }
    else if (number == 23 && value >= 0) {
        switchToPart(value);
    }
    #ifdef IS_MIDI_PROXY
        newMidiValuesToBroadcast = true;    
        midiInCC = number;
        midiInValue = value;
    #endif
}

void setUp(void) {
    // Reset state before each test
    currentSong = 0;
    currentPart = 0;
    newMidiValuesToBroadcast = false;
    midiInCC = 0;
    midiInValue = 0;
}

void tearDown(void) {
    // Clean up after each test if needed
}

// Test normal song switching
void test_song_switch_normal(void) {
    MidiDatenAuswerten(1, 22, 5);  // Channel 1, CC 22, Value 5
    TEST_ASSERT_EQUAL(5, currentSong);
    #ifdef IS_MIDI_PROXY
        TEST_ASSERT_TRUE(newMidiValuesToBroadcast);
        TEST_ASSERT_EQUAL(22, midiInCC);
        TEST_ASSERT_EQUAL(5, midiInValue);
    #endif
}

void test_song_switch_zero(void) {
    MidiDatenAuswerten(1, 22, 0);  // Should not switch song
    TEST_ASSERT_EQUAL(0, currentSong);
}

void test_part_switch_normal(void) {
    MidiDatenAuswerten(1, 23, 3);  // Channel 1, CC 23, Value 3
    TEST_ASSERT_EQUAL(3, currentPart);
    #ifdef IS_MIDI_PROXY
        TEST_ASSERT_TRUE(newMidiValuesToBroadcast);
        TEST_ASSERT_EQUAL(23, midiInCC);
        TEST_ASSERT_EQUAL(3, midiInValue);
    #endif
}

void test_part_switch_zero(void) {
    MidiDatenAuswerten(1, 23, 0);  // Should switch to part 0
    TEST_ASSERT_EQUAL(0, currentPart);
}

void test_irrelevant_cc(void) {
    byte originalSong = currentSong;
    byte originalPart = currentPart;
    
    MidiDatenAuswerten(1, 24, 100);  // CC 24 should be ignored
    TEST_ASSERT_EQUAL(originalSong, currentSong);
    TEST_ASSERT_EQUAL(originalPart, currentPart);
    
    #ifdef IS_MIDI_PROXY
        TEST_ASSERT_TRUE(newMidiValuesToBroadcast);
        TEST_ASSERT_EQUAL(24, midiInCC);
        TEST_ASSERT_EQUAL(100, midiInValue);
    #endif
}

void test_rapid_cc_changes(void) {
    for(int i = 0; i < 10; i++) {
        MidiDatenAuswerten(1, 22, i + 1);
        TEST_ASSERT_EQUAL(i + 1, currentSong);
        MidiDatenAuswerten(1, 23, i + 1);
        TEST_ASSERT_EQUAL(i + 1, currentPart);
    }
}

void test_boundary_values(void) {
    MidiDatenAuswerten(1, 22, 127);
    TEST_ASSERT_EQUAL(127, currentSong);
    
    MidiDatenAuswerten(1, 23, 127);
    TEST_ASSERT_EQUAL(127, currentPart);
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_song_switch_normal);
    RUN_TEST(test_song_switch_zero);
    RUN_TEST(test_part_switch_normal);
    RUN_TEST(test_part_switch_zero);
    RUN_TEST(test_irrelevant_cc);
    RUN_TEST(test_rapid_cc_changes);
    RUN_TEST(test_boundary_values);
    return UNITY_END();
}

// Required by PlatformIO Unit Testing
void setup() {
    delay(2000);  // Required on some boards
    runUnityTests();
}

void loop() {}

// For native platform test execution
int main(int argc, char **argv) {
    return runUnityTests();
}