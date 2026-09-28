Der Generator ist fertig und getestet. Als Testsong habe ich „Dancing On My Own“ als Song #40 angelegt: gleiche Struktur wie das Original, aber mit Szenen neu gestaltet. Auf die Bühne hat das noch niemand gebracht, und die Audio-Analyse lief bisher nur mit einem künstlichen Testsong.

Was geprüft ist:
- Timing: Aus 122 BPM und 245 ms MIDI-Offset kommt der Generator auf dieselben Part-Grenzen wie deine Excel-Werte. Nach 4:25 min weicht er nur 1 ms ab (265 329 statt 265 330 ms). Durch den Offset wird der erste Part automatisch kürzer, sonst verschiebt sich nichts.
- Builds: Die Gitarre, Rinas Bass, Lampe1 und die Scrollmatrix bauen fehlerfrei. src/definitions.h habe ich dafür kurz umgeschaltet und danach byte-genau wiederhergestellt.
- Teensy (GITBOARD): Der Build scheitert an esp_random in FXprograms.cpp:2932. Der Fehler war vorher schon da und kommt nicht von meinen Änderungen, deshalb ist der generierte Song auf dem GITBOARD noch nicht gebaut.
- Audio-Analyse: Ich habe einen künstlichen 122-BPM-Song erzeugt, bei dem Lautstärke und Instrumente pro Abschnitt festgelegt waren. Die Analyse hat Takt 1 auf 14 ms genau gefunden (auch hinter 3 Takten Stille) und das Tempo mit 122,05 statt 122 BPM gemessen. Die Power-Werte passten zum Aufbau: Stille 0, Bass 1, Verse 3, Pre-Chorus 4 mit steigendem Build, Chorus 5. Die Formwechsel lagen auf den echten Klangwechseln.

So testest du es:
1. Flashen und Song 40 starten: MIDI-CC 22 = 40 auf Kanal 10. In der CLAUDE.md steht noch CC#0, im Code ist es aber CC 22.
2. Mit echtem Audio: Leg eine Datei nach songs/audio/ und aktiviere in songs/dancing_on_my_own.yaml die Zeile audio:. Dann:
tools/.venv/Scripts/python tools/songanalyze.py songs/dancing_on_my_own.yaml
   Wie gut Stimmung und Power bei einem echten, abgemischten Song erkannt werden, ist noch offen. Das wäre der interessanteste Test.
3. Neuer Song: Nenn mir BPM, Offset und Struktur, gern auch als lose Beschreibung wie „8 Takte Intro langsam, 4 Takte Verse…“. Ich schreibe daraus die YAML, gestalte die Szenen, generiere den Code und baue.

Neue Dateien:
- tools/songgen.py: Generator
- tools/songanalyze.py: Audio-Analyse, eigene Python-Umgebung unter tools/.venv
- songs/dancing_on_my_own.yaml: der Testsong
- .claude/skills/new-song/SKILL.md: Ablauf und Gestaltungsregeln, dazu ein kurzer Hinweis in der CLAUDE.md

Abweichungen vom Plan:
- Die Messwerte landen in einer eigenen Datei songs/<name>.analysis.yaml statt in deiner YAML. So bleiben deine Kommentare dort erhalten.
- Generierte Songs bekommen noch keine Bundmarker-LEDs.

In main.cpp sind nur ein Include und der Marker-Block dazugekommen. Committet ist noch nichts.
