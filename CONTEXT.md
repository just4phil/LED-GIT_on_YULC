# Domain glossary

Terms the code, the docs and architecture reviews use. German terms appear as they do in the code comments.

## Devices and roles

- **Proxy**: the guitar (`ANDRESGIT`). Receives MIDI from the WIDI CORE and is the BLE server. Its song/part
  state is the reference for all others.
- **Client**: every other ESP32 device (bass, lamps, matrix). BLE client of the proxy.

## LED sync (`src/ledSync.h/.cpp`)

- **LED sync / Abgleich**: bringing song + part of proxy and clients into line. Only the song start, manual
  part jumps and the handshakes below go over BLE. Automatic part changes run on each device's own timer.
- **Nachricht / BLEmessage**: the 3-byte wire message `msgType, songID, part`. The format and the numbers 0–7 are
  fixed, so old firmware must still sync.
- **Einstieg (entry, `SYNC_ENTRY`)**: "part n starts now". After a device has jumped to the right song/part, it
  only knows *which* part, not *how far* into it. It starts the part in time on the other side's next part
  change. Waiting for it = *wartet auf den Einstieg*.
- **Abgleich erzwingen (force, `SYNC_FORCE`)**: the proxy pushes its song/part to all clients (knob short
  click, emergency stop). The clients go dark and wait for the entry.
- **Stand lesen (read)**: a client reads the proxy's stored state, `SYNC_STATE` (knob short click on the client).
  The proxy notices the read and sends the entry on its next part change.
- **Stand abfragen (ask, `SYNC_ASK` → `SYNC_REPLY`)**: the proxy takes over a client's song/part (knob double
  click on the proxy). The client also sends the entry on its own next part change.
- **Adapter**: the two NimBLE files (`midiProxyBLEserver_nimBLE.cpp`, `BLE_client_nimBLE.cpp`). They move bytes and
  execute a `LedSyncAction`; they decide nothing.

## Songs

- **Song / Part**: a song is a sequence of parts (`prog`). Each part has a length and a following part.
- **Generated song**: assembled by `tools/songgen.py` from `songs/<Song>/quelle/struktur.xlsx` + `show.yaml`.
- **Hand-written song**: an old song in `songs.cpp` with hand-tuned part lengths. It stays untouched unless it is
  replaced by a generated show.
