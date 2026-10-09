//=====================================================================
// songs.h - die handgeschriebenen Songs (eine Funktion je Song)
//=====================================================================
// Jede Funktion hier ist die Lichtshow zu EINEM Song. loop() in main.cpp ruft über switch(songID) bei jedem
// Durchlauf die Funktion des laufenden Songs auf. Wie eine Song-Funktion aufgebaut ist, steht am Anfang von
// songs.cpp.
//
// Die Nummer hinter "#" ist die Song-ID: mit ihr wird der Song per MIDI (Kanal 10, CC 22) gestartet.
// Die Zuordnung ID -> Funktion steht in main.cpp, die Bund-Marker je Song in markerLEDs.cpp.
//
// Neue Songs werden nicht mehr hier von Hand geschrieben, sondern aus der Excel-Tabelle des Songs
// (songs/<Song>/quelle/struktur.xlsx) und der show.yaml erzeugt: siehe songs_generated.h und tools/songgen.py.

//==================================================================
//=========== Grundzustände (keine Songs) ==========================
//==================================================================

/**
 * @brief #99 Startbild: dunkel, bis das Intro gestartet wird
 *
 * Gedacht: 60 Sekunden dunkel, dann Wechsel in den defaultLoop (#100).
 */
void STARTUP();

/**
 * @brief #0 Pause zwischen den Songs
 *
 * Ruhiges Glitzern in Dauerschleife (auf der LED-Fläche zuerst der Lauftext "Nerds on Fire").
 * Das ist der Zustand nach dem Einschalten, nach jedem Song-Ende und nach dem Not-Aus.
 */
void SONGPAUSE();

/**
 * @brief Pausenbild für Song-IDs, zu denen es (noch) keinen Song gibt
 *
 * Sieht aus wie SONGPAUSE, springt am Ende aber NICHT auf Song 0 zurück. So bleibt die gewählte
 * Song-ID erhalten und man kann für einen neuen Song schon die Bund-Marker einrichten und sehen,
 * bevor seine Show existiert. Wird in main.cpp im "default"-Zweig aufgerufen.
 */
//#0
void SONGPAUSE_ohne_switchToSong0();

/**
 * @brief #100 Bunter Dauerlauf durch viele Effekte (läuft nach STARTUP)
 */
//#0
void defaultLoop();

//==================================================================
//=========== Songs ================================================
//==================================================================

/**
 * @brief #1 Vorspann zu "Physical"
 *
 * Bleibt während des Einspielers dunkel (auf der LED-Fläche Lauftexte) und springt am Ende direkt
 * in Song #2 Physical, hinter dessen Intro.
 */
// #1
void PhysicalTrailer();

/** @brief #2 Physical (Dua Lipa) - fertig 13.08.2023 */
// #2
void Physical();

/** @brief #3 Take On Me (a-ha) */
// #3 - TakeOnMe
void TakeOnMe();

/** @brief #4 Don't Stop The Music (Rihanna) */
//#4 DontStopTheMusic
void DontStopTheMusic();

/** @brief #5 Use Somebody (Kings of Leon) - fertig 25.08.2023 */
//#5 -> FERTIG: 25.08.2023
void UseSomebody();

/** @brief #6 No Roots (Alice Merton) - fertig 25.08.2023 */
//#6 -> FERTIG: 25.08.2023
void NoRoots();

/** @brief #7 Firework (Katy Perry) - fertig 25.08.2023 */
//#7 -> FERTIG: 25.08.2023
void Firework();

/**
 * @brief #8 Dancing On My Own (Robyn) - alte, handgeschriebene Fassung
 *
 * Wird nicht mehr aufgerufen: main.cpp ruft für Song 8 die generierte Fassung gen_DancingOnMyOwn() auf.
 */
// #8
void DancingOnMyOwn();

/** @brief #9 I Love It (Icona Pop). Der Vorspann dazu ist #80 (alt: ILoveItTRAILER, jetzt generiert: gen_ILoveItIntro). */
//#9 ILoveIt
void ILoveIt();

/** @brief #10 Bloody Mary (Lady Gaga) */
// #10 BloodyMary();
void BloodyMary();

/** @brief #11 Titanium (David Guetta feat. Sia) - fertig 25.08.2023 */
//#11 -> FERTIG: 25.08.2023
void Titanium();

/** @brief #12 Such A Shame (Talk Talk) - fertig 17.09.2023 */
// #12 SuchAshame();
void SuchAshame();

/** @brief #13 In The Dark - fertig 16.09.2023 */
// #13 InTheDark();
void InTheDark();

/** @brief #14 Shivers (Ed Sheeran) */
// #14 Shivers();
void Shivers();

/** @brief #15 abcdefu (GAYLE) - fertig 25.08.2023 */
// #15 Abcdefu -> FERTIG: 25.08.2023
void Abcdefu();

/** @brief #16 Enjoy The Silence (Depeche Mode) - fertig 25.08.2023. Der Vorspann dazu ist #24. */
//#16 -> FERTIG: 25.08.2023
void enjoyTheSilence();

/** @brief #17 APT. (ROSÉ & Bruno Mars) */
//#17 leer
void apt();

/** @brief #18 Prisoner (Miley Cyrus feat. Dua Lipa) */
//#18 -> ok: 5.3.22
void prisoner();

/** @brief #19 Hot N Cold (Katy Perry) */
// #19 Hot n Cold();
void Hotncold();

/** @brief #20 Kids (MGMT) */
// #20 Kids();
void Kids();

/** @brief #21 Tell It To My Heart */
// #21 Tellittomyheart();
void Tellittomyheart();

/** @brief #24 Vorspann zu "Enjoy The Silence" (#16) */
// #24
void enjoyTheSilenceINTRO();

/** @brief #25 Friday I'm In Love (The Cure) */
// #25 FridayImInLove();
void FridayImInLove();

/** @brief #26 Be Mine */
// #26 BeMine();
void BeMine();

/** @brief #27 I Wanna Dance With Somebody (Whitney Houston) */
// #27 IWannaDanceWithSomebody();
void IWannaDanceWithSomebody();

/**
 * @brief #28 Billie Jean (Michael Jackson) - alte, handgeschriebene Fassung
 *
 * Die Deklaration ist auskommentiert: Song 28 läuft über die generierte Fassung gen_BillieJean().
 */
// #28 BillyJean();
//void BillyJean();	// ersetzt durch gen_BillieJean() (songs_generated.h), alter Code auskommentiert in songs.cpp


/** @brief #29 Maniac (Michael Sembello) - alte Fassung; gespielt wird #30 */
// #29 Maniac();
void Maniac();

/** @brief #30 Maniac in der transponierten Fassung "T-1" - das ist die Fassung, die gespielt wird */
void Maniac_Tminus1();

/**
 * @brief #80 Vorspann zu "I Love It"
 *
 * Bleibt während des Einspielers dunkel (auf der LED-Fläche Lauftexte) und springt am Ende direkt
 * in Song #9 I Love It, hinter dessen Intro.
 * Seit 09.10.2026 läuft stattdessen die generierte Fassung gen_ILoveItIntro() (songs/ILoveItIntro_v1,
 * Aufruf in main.cpp); diese alte Funktion bleibt als Referenz stehen und wird nicht mehr aufgerufen.
 */
// #80
void ILoveItTRAILER();

/** @brief #81 Vorspann zu "Dancing On My Own": springt am Ende in den ersten Refrain von Song #8 */
// #81
void INTROdancing();

//==================================================================
//=========== Demos (zum Ansehen und Abnehmen der Effekte) =========
//==================================================================
// Start per Song-ID oder direkt nach dem Einschalten über START_WITH_..._DEMO in definitions.h.

/**
 * @brief Demo aller neuen Kontur-Effekte aus guitarShapeFX (Song-ID 90, läuft in Dauerschleife)
 */
void neueEffekteDemo();

/**
 * @brief Demo der Szenen + Farbschemata auf allen Geräten (Song-ID 91, läuft in Dauerschleife)
 */
void szenenDemo();

/**
 * @brief Demo der Ausgabestufe: alle Übergänge, Modifikatoren und Ebenen-Modi (Song-ID 92, läuft in Dauerschleife)
 */
void pipelineDemo();
