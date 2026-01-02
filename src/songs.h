/**
 * @brief Startup animation sequence
 * 
 * Runs on system boot to display initialization pattern and
 * verify LED functionality. Shows a sequence of colors and
 * patterns before entering normal operation mode.
 * 
 * @note Only runs once during initialization
 * @see setup()
 */
void STARTUP();

/**
 * @brief Pause animation state
 * 
 * Displays a pause indicator pattern when song playback is
 * paused. Shows a visual cue that the system is in pause mode.
 * 
 * @note Active when PAUSED flag is set
 * @see switchToPart()
 */
void SONGPAUSE();

/**
 * @brief Default loop pattern for unknown songs
 * 
 * Fallback animation pattern used when no specific song is
 * selected or when an invalid song ID is provided. Shows a
 * generic pattern to indicate default state.
 * 
 * @note Used for song ID 0
 * @see switchToSong()
 */
//#0
void defaultLoop();

/**
 * @brief Physical - Trailer version pattern
 * 
 * LED pattern for "Physical" song trailer sequence.
 * Displays specific timing and patterns for this song's
 * intro/preview section.
 * 
 * Song: Physical by Dua Lipa
 * Version: Trailer/Intro section
 * Status: Completed 12.08.2023
 * 
 * @see Physical()
 */
// #1
void PhysicalTrailer();

/**
 * @brief Physical full song pattern
 * 
 * Complete LED pattern sequence for "Physical" by Dua Lipa.
 * Includes all song sections with proper timing and visual
 * cues synchronized to the music.
 * 
 * Song: Physical by Dua Lipa
 * Status: Completed 13.08.2023
 * 
 * @see PhysicalTrailer()
 */
// #2
void Physical();

/**
 * @brief Take On Me song pattern
 * 
 * LED pattern for "Take On Me" by A-ha. Features the iconic
 * synthesizer melody and song structure visualization.
 * 
 * Song: Take On Me by A-ha
 * 
 * @see switchToSong()
 */
// #3 - TakeOnMe
void TakeOnMe();

/**
 * @brief Don't Stop The Music song pattern
 * 
 * LED pattern for "Don't Stop The Music" by Rihanna.
 * Displays rhythmic patterns and song sections.
 * 
 * Song: Don't Stop The Music by Rihanna
 * 
 * @see switchToSong()
 */
//#4 DontStopTheMusic 
void DontStopTheMusic();

/**
 * @brief Use Somebody song pattern
 * 
 * LED pattern for "Use Somebody" by Kings of Leon.
 * Features guitar-driven visualization and song structure.
 * 
 * Song: Use Somebody by Kings of Leon
 * Status: Completed 25.08.2023
 * 
 * @see switchToSong()
 */
//#5 -> FERTIG: 25.08.2023
void UseSomebody();

/**
 * @brief No Roots song pattern
 * 
 * LED pattern for "No Roots" by Alice Merton. Includes
 * driving bass visualization and song structure.
 * 
 * Song: No Roots by Alice Merton
 * Status: Completed 25.08.2023
 * Note: Root notes may need timing adjustment (arrive 1/8 note early)
 * 
 * @see switchToSong()
 */
//#6 -> FERTIG: 25.08.2023
void NoRoots();

/**
 * @brief Firework song pattern
 * 
 * LED pattern for "Firework" by Katy Perry. Features
 * explosive visual effects matching the song's energy.
 * 
 * Song: Firework by Katy Perry
 * Status: Completed 25.08.2023
 * 
 * @see switchToSong()
 */
//#7 -> FERTIG: 25.08.2023
void Firework();

/**
 * @brief Dancing On My Own song pattern
 * 
 * LED pattern for "Dancing On My Own" by Robyn.
 * Emotional visualization matching the song's mood.
 * 
 * Song: Dancing On My Own by Robyn
 * 
 * @see switchToSong()
 */
// #8
void DancingOnMyOwn();

/**
 * @brief I Love It song pattern
 * 
 * LED pattern for "I Love It" by Icona Pop.
 * High-energy visualization for this upbeat track.
 * 
 * Song: I Love It by Icona Pop
 * 
 * @see switchToSong()
 * @see ILoveItTRAILER()
 */
//#9 ILoveIt 
void ILoveIt();

/**
 * @brief Bloody Mary song pattern
 * 
 * LED pattern for "Bloody Mary" by Lady Gaga.
 * Dark, atmospheric visualization matching the song's tone.
 * 
 * Song: Bloody Mary by Lady Gaga
 * 
 * @see switchToSong()
 */
// #10 BloodyMary();
void BloodyMary();

/**
 * @brief Titanium song pattern
 * 
 * LED pattern for "Titanium" by David Guetta ft. Sia.
 * Features strong, metallic visual effects.
 * 
 * Song: Titanium by David Guetta ft. Sia
 * Status: Completed 25.08.2023
 * 
 * @see switchToSong()
 */
//#11 -> FERTIG: 25.08.2023
void Titanium();

/**
 * @brief Such A Shame song pattern
 * 
 * LED pattern for "Such A Shame" by Talk Talk.
 * Matches the song's distinctive synth sound.
 * 
 * Song: Such A Shame by Talk Talk
 * 
 * @see switchToSong()
 */
// #12 SuchAshame();
void SuchAshame();

/**
 * @brief In The Dark song pattern
 * 
 * LED pattern for "In The Dark" by Dev.
 * Club-style visualization for this dance track.
 * 
 * Song: In The Dark by Dev
 * 
 * @see switchToSong()
 */
// #13 InTheDark();
void InTheDark();

/**
 * @brief Shivers song pattern
 * 
 * LED pattern for "Shivers" by Ed Sheeran.
 * Energetic visualization matching the song's tempo.
 * 
 * Song: Shivers by Ed Sheeran
 * 
 * @see switchToSong()
 */
// #14 Shivers();
void Shivers();

/**
 * @brief abcdefu song pattern
 * 
 * LED pattern for "abcdefu" by Gayle.
 * Bold, expressive visualization for this anthem.
 * 
 * Song: abcdefu by Gayle
 * Status: Completed 25.08.2023
 * 
 * @see switchToSong()
 */
// #15 Abcdefu -> FERTIG: 25.08.2023
void Abcdefu();

/**
 * @brief Enjoy The Silence song pattern
 * 
 * LED pattern for "Enjoy The Silence" by Depeche Mode.
 * Atmospheric visualization for this classic synth-pop track.
 * 
 * Song: Enjoy The Silence by Depeche Mode
 * Status: Completed 25.08.2023
 * 
 * @see switchToSong()
 * @see enjoyTheSilenceINTRO()
 */
//#16 -> FERTIG: 25.08.2023
void enjoyTheSilence();

/**
 * @brief apt. song pattern
 * 
 * LED pattern for "apt." by Rosé & Bruno Mars.
 * Currently empty (placeholder).
 * 
 * Song: apt. by Rosé & Bruno Mars
 * Status: Not implemented
 * 
 * @see switchToSong()
 */
//#17 leer
void apt();

/**
 * @brief Prisoner song pattern
 * 
 * LED pattern for "Prisoner" by Miley Cyrus ft. Dua Lipa.
 * Features collaborative duet visualization.
 * 
 * Song: Prisoner by Miley Cyrus ft. Dua Lipa
 * Status: Completed 5.3.2022
 * 
 * @see switchToSong()
 */
//#18 -> ok: 5.3.22
void prisoner();

/**
 * @brief Hot N Cold song pattern
 * 
 * LED pattern for "Hot N Cold" by Katy Perry.
 * Playful visualization matching the song's theme.
 * 
 * Song: Hot N Cold by Katy Perry
 * 
 * @see switchToSong()
 */
// #19 Hot n Cold();
void Hotncold();

/**
 * @brief Kids song pattern
 * 
 * LED pattern for "Kids" by MGMT.
 * Psych-pop visualization for this indie classic.
 * 
 * Song: Kids by MGMT
 * 
 * @see switchToSong()
 */
// #20 Kids();
void Kids();

/**
 * @brief Tell It To My Heart song pattern
 * 
 * LED pattern for "Tell It To My Heart" by Taylor Swift.
 * Covers Taylor Swift's version of this song.
 * 
 * Song: Tell It To My Heart by Taylor Swift
 * 
 * @see switchToSong()
 */
// #21 Tellittomyheart();
void Tellittomyheart();

/**
 * @brief Enjoy The Silence intro pattern
 * 
 * Special intro sequence for "Enjoy The Silence" before
 * the main pattern begins.
 * 
 * Song: Enjoy The Silence by Depeche Mode
 * Section: Intro only
 * 
 * @see enjoyTheSilence()
 */
// #24
void enjoyTheSilenceINTRO();

/**
 * @brief Friday I'm In Love song pattern
 * 
 * LED pattern for "Friday I'm In Love" by The Cure.
 * Upbeat, romantic visualization for this classic.
 * 
 * Song: Friday I'm In Love by The Cure
 * 
 * @see switchToSong()
 */
// #25 FridayImInLove();
void FridayImInLove();

/**
 * @brief I Love It trailer pattern
 * 
 * Special trailer/intro pattern for "I Love It" before
 * the main song pattern begins.
 * 
 * Song: I Love It by Icona Pop
 * Section: Trailer/Intro
 * 
 * @see ILoveIt()
 */
// #80
void ILoveItTRAILER();
