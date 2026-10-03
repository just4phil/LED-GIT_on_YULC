#!/usr/bin/env python3
"""
songanalyze.py - misst Stimmung und Power der Song-Abschnitte aus dem Audio

    tools/.venv/Scripts/python tools/songanalyze.py <Song>     # Ordnername unter songs/ (Anfang genügt)

Braucht im YAML:  audio: <pfad relativ zum Projekt>   optional: audio_beat1_ms: <ms>
Schreibt:
    songs/<Song>/audio-analyse/analysis.yaml   Messwerte pro Abschnitt + Akzente + Warnungen
    songs/<Song>/audio-analyse/analysis.png    Lautstärke, Spektrogramm, Merkmale, Formgrenzen
song.yaml wird nur gelesen.

Alle Merkmale sind relativ zum Song normiert (lautester Part = power 5), nicht absolut.
"""
import argparse
import sys
from pathlib import Path

import numpy as np
import yaml

import librosa
import librosa.display
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

sys.path.insert(0, str(Path(__file__).resolve().parent))
from songgen import ROOT, SONG_FILE, SongError, find_audio, find_song_dir, section_times, section_bpm, section_beats  # noqa: E402

SR = 22050
HOP = 512
SILENT_DB = -30		# Part gilt als still, wenn sein Mittel so weit unter dem lautesten Moment liegt
DB_PER_POWER = 1.5	# Masters sind stark limitiert: pro 1,5 dB unter dem lautesten Part eine Power-Stufe weniger

# Krumhansl-Profile für Dur/Moll-Tendenz
MAJOR = np.array([6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88])
MINOR = np.array([6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17])
NOTES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"]


def norm01(values):
	v = np.asarray(values, dtype=float)
	lo, hi = np.nanmin(v), np.nanmax(v)
	return np.zeros_like(v) if hi - lo < 1e-9 else (v - lo) / (hi - lo)


def plain(o):
	"""numpy-Typen -> Python-Typen für yaml.safe_dump"""
	if isinstance(o, dict):
		return {k: plain(v) for k, v in o.items()}
	if isinstance(o, (list, tuple)):
		return [plain(v) for v in o]
	if isinstance(o, np.generic):
		return o.item()
	return o


def key_of(chroma_mean):
	best = (-2.0, "", "")
	for i in range(12):
		for prof, mode in ((MAJOR, "Dur"), (MINOR, "Moll")):
			r = np.corrcoef(np.roll(prof, i), chroma_mean)[0, 1]
			if r > best[0]:
				best = (r, NOTES[i], mode)
	return best


def estimate_beat1(song, times, rms_db, dur_s):
	"""Takt 1 im Audio: der Versatz, bei dem die YAML-Grenzen am besten auf Lautstärke-/Klangwechsel fallen.
	Funktioniert auch, wenn der Song mit Stille beginnt (dann ist der erste Ton NICHT Takt 1)."""
	rel = np.array(section_times(song)[1:-1]) / 1000.0	# innere Grenzen ab Takt 1
	if len(rel) == 0:
		return 0.0, 0.0
	bar_s = 60.0 / song["bpm"] * song.get("beats_per_bar", 4)
	fps = 1.0 / (times[1] - times[0])
	w = max(2, int(bar_s * fps))					# 1 Takt vor/nach der Grenze vergleichen
	csum = np.concatenate([[0.0], np.cumsum(rms_db)])
	n = len(rms_db)

	def mean(a, b):
		a, b = np.clip(a, 0, n), np.clip(b, 0, n)
		return (csum[b] - csum[a]) / np.maximum(1, b - a)

	cands = np.arange(0, int(min(30.0, dur_s / 3) * fps))
	scores = np.zeros(len(cands))
	for t in rel:
		f = cands + int(round(t * fps))
		scores += np.abs(mean(f, f + w) - mean(f - w, f))
	best = int(np.argmax(scores))
	conf = float((scores[best] - np.median(scores)) / (np.std(scores) + 1e-9))
	return float(cands[best] / fps), conf


def checkerboard_novelty(feat, width=16):
	"""Foote-Novelty auf einer Self-Similarity-Matrix (Formgrenzen)."""
	f = librosa.util.normalize(feat, axis=0)
	ssm = f.T @ f
	k = np.ones((2 * width, 2 * width))
	k[:width, width:] = -1
	k[width:, :width] = -1
	g = np.outer(np.hanning(2 * width), np.hanning(2 * width))
	k *= g
	n = ssm.shape[0]
	pad = np.pad(ssm, width, mode="constant")
	nov = np.array([np.sum(pad[i:i + 2 * width, i:i + 2 * width] * k) for i in range(n)])
	nov[nov < 0] = 0
	return norm01(nov)


def analyze(song, audio_path, out_yaml, out_png):
	y, sr = librosa.load(str(audio_path), sr=SR, mono=True)
	dur_s = len(y) / sr
	y_harm, y_perc = librosa.effects.hpss(y)

	S = np.abs(librosa.stft(y, hop_length=HOP))
	freqs = librosa.fft_frequencies(sr=sr)
	rms = librosa.feature.rms(S=S)[0]
	rms_db = librosa.amplitude_to_db(rms, ref=np.max)
	times = librosa.frames_to_time(np.arange(len(rms)), sr=sr, hop_length=HOP)
	onset_env = librosa.onset.onset_strength(y=y, sr=sr, hop_length=HOP)
	onset_frames = librosa.onset.onset_detect(onset_envelope=onset_env, sr=sr, hop_length=HOP)
	onset_times = librosa.frames_to_time(onset_frames, sr=sr, hop_length=HOP)
	centroid = librosa.feature.spectral_centroid(S=S, sr=sr)[0]
	low = S[freqs < 150].sum(axis=0) / (S.sum(axis=0) + 1e-9)
	perc_rms = librosa.feature.rms(y=y_perc, hop_length=HOP)[0]
	harm_rms = librosa.feature.rms(y=y_harm, hop_length=HOP)[0]
	chroma = librosa.feature.chroma_cqt(y=y_harm, sr=sr, hop_length=HOP)
	mfcc = librosa.feature.mfcc(y=y, sr=sr, hop_length=HOP, n_mfcc=13)

	warnings = []

	#--- Tempo und Takt 1 ---
	tempo, beat_frames = librosa.beat.beat_track(onset_envelope=onset_env, sr=sr, hop_length=HOP, start_bpm=song["bpm"])
	beat_times = librosa.frames_to_time(beat_frames, sr=sr, hop_length=HOP)
	measured_bpm = None
	if len(beat_times) > 16:
		idx = np.arange(len(beat_times))
		slope = np.polyfit(idx, beat_times, 1)[0]	# Sekunden pro Beat, robust über den ganzen Song
		measured_bpm = 60.0 / slope
		# halbes/doppeltes Tempo erkannt -> auf YAML-Tempo falten
		for f in (0.5, 2.0):
			if abs(measured_bpm * f - song["bpm"]) < abs(measured_bpm - song["bpm"]):
				measured_bpm *= f

	if "audio_beat1_ms" in song:
		beat1 = song["audio_beat1_ms"] / 1000.0
		beat1_src = "YAML (audio_beat1_ms)"
	else:
		beat1, conf = estimate_beat1(song, times, rms_db, dur_s)
		# auf den nächsten erkannten Beat einrasten (genauer als das Frame-Raster)
		if len(beat_times):
			nb = beat_times[np.argmin(np.abs(beat_times - beat1))]
			if abs(nb - beat1) < 0.08:
				beat1 = float(nb)
		beat1_src = f"aus Struktur geschätzt (Sicherheit {conf:.1f}, >4 gut) - ggf. audio_beat1_ms setzen"
		if conf < 4:
			warnings.append(f"Takt 1 im Audio unsicher geschätzt ({beat1 * 1000:.0f} ms). Bitte prüfen und audio_beat1_ms setzen.")

	sec_t = [beat1 + t / 1000.0 for t in section_times(song)]
	if sec_t[-1] > dur_s + 0.5:
		warnings.append(f"Struktur ist {sec_t[-1] - dur_s:.1f} s länger als das Audio - Taktzahlen/BPM prüfen.")
	if measured_bpm:
		song_min = (sec_t[-1] - beat1) / 60.0
		drift_ms = abs(1 - song["bpm"] / measured_bpm) * (sec_t[-1] - beat1) * 1000
		if drift_ms > 150:
			warnings.append(f"Gemessenes Tempo {measured_bpm:.2f} BPM vs. YAML {song['bpm']} -> Drift ca. {drift_ms:.0f} ms "
							f"über {song_min:.1f} min. bpm im YAML auf {measured_bpm:.2f} setzen?")

	#--- Formgrenzen (Novelty) vs. YAML-Grenzen ---
	# festes Zeitraster (1/4 s), damit die Kurve nicht vom Beat-Tracking abhängt
	blk = max(1, int(round(0.25 * sr / HOP)))
	feat = np.vstack([librosa.util.normalize(mfcc, axis=1), chroma, rms_db[np.newaxis, :] / 10.0])
	nb = feat.shape[1] // blk
	feat_b = feat[:, :nb * blk].reshape(feat.shape[0], nb, blk).mean(axis=2)
	nov_t = (np.arange(nb) + 0.5) * blk * HOP / sr
	bar_blocks = max(2, int(round(60.0 / song["bpm"] * song.get("beats_per_bar", 4) / 0.25)))
	nov = checkerboard_novelty(feat_b, width=bar_blocks * 2)			# vergleicht je 2 Takte davor/danach
	peaks = librosa.util.peak_pick(nov, pre_max=bar_blocks, post_max=bar_blocks, pre_avg=bar_blocks * 2,
								   post_avg=bar_blocks * 2, delta=0.1, wait=bar_blocks * 2)
	peak_t = nov_t[peaks] if len(peaks) else np.array([])

	#--- Merkmale pro Abschnitt ---
	raw = []
	for i, sec in enumerate(song["sections"]):
		a, b = sec_t[i], sec_t[i + 1]
		m = (times >= a) & (times < b)
		if m.sum() < 4:
			raw.append(None)
			continue
		seg_db = rms_db[m]
		tt = times[m] - a
		slope_db_per_s = np.polyfit(tt, seg_db, 1)[0] if len(tt) > 8 else 0.0
		n_on = ((onset_times >= a) & (onset_times < b)).sum()
		raw.append({
			"loud": float(np.mean(seg_db)),
			"dur": b - a,
			"slope": float(slope_db_per_s * (b - a)),			# dB Anstieg über den ganzen Part
			"onsets": n_on / (b - a),
			"perc": float(perc_rms[m].mean() / (perc_rms[m].mean() + harm_rms[m].mean() + 1e-9)),
			"bright": float(np.mean(centroid[m])),
			"low": float(np.mean(low[m])),
			"chroma": chroma[:, m].mean(axis=1),
		})

	# Normierung nur über die klingenden Parts, stille Parts verzerren sonst die Skala
	for r in raw:
		if r:
			r["silent"] = r["loud"] < SILENT_DB
	valid = [r for r in raw if r and not r["silent"]]
	if not valid:
		raise SystemExit("FEHLER: kein klingender Abschnitt im Audio gefunden (Takt 1 / Audiodatei prüfen)")
	max_on = max(1e-9, max(v["onsets"] for v in valid))
	# Bezug: lautester Abschnitt ab 4 Takten (ein einzelner lauter Takt soll nicht alles nach unten drücken)
	bar_ref = 4 * 60.0 / song["bpm"] * song.get("beats_per_bar", 4)
	loudest = max([r["loud"] for r in valid if r["dur"] >= bar_ref * 0.99] or [r["loud"] for r in valid])
	drive_n = dict(zip(map(id, valid), norm01([0.5 * r["onsets"] / max_on + 0.5 * r["perc"] for r in valid])))
	bright_n = dict(zip(map(id, valid), norm01([r["bright"] for r in valid])))
	low_n = dict(zip(map(id, valid), norm01([r["low"] for r in valid])))
	song_key = key_of(np.mean([r["chroma"] for r in valid], axis=0))

	sections_out = []
	for i, sec in enumerate(song["sections"]):
		entry = {"name": sec["name"], "start_ms": round((sec_t[i] - beat1) * 1000), "dur_ms": round((sec_t[i + 1] - sec_t[i]) * 1000)}
		r = raw[i]
		if r is None:
			entry["hinweis"] = "außerhalb des Audios"
			sections_out.append(entry)
			continue
		if r["silent"]:
			entry.update({"power": 0, "loudness_db": round(r["loud"], 1), "build": 0.0, "drive": 0.0,
						  "brightness": 0.0, "lowend": 0.0, "mood": "still"})
			sections_out.append(entry)
			continue
		key = key_of(r["chroma"])
		vi = id(r)
		entry.update({
			"power": int(np.clip(5 - round((loudest - r["loud"]) / DB_PER_POWER), 1, 5)),	# 1..5, 0 = still
			"loudness_db": round(r["loud"], 1),
			"build": round(float(np.clip(r["slope"] / 12.0, -1, 1)), 2),	# +1 = steigt stark an, -1 = fällt ab
			"drive": round(float(drive_n[vi]), 2),
			"brightness": round(float(bright_n[vi]), 2),
			"lowend": round(float(low_n[vi]), 2),
			"mood": f"{key[1]}-{key[2]}" + (" (unsicher)" if key[0] < 0.6 else ""),
		})
		# passt die Grenze zum Audio? (Novelty-Peak innerhalb eines Takts)
		if i > 0:
			bar_s = 60.0 / section_bpm(sec, song) * song.get("beats_per_bar", 4)
			near = [p for p in peak_t if abs(p - sec_t[i]) < bar_s]
			entry["grenze_hoerbar"] = bool(near)
		sections_out.append(entry)

	# erkannte Formgrenzen, die in keiner YAML-Grenze liegen
	bar_s = 60.0 / song["bpm"] * song.get("beats_per_bar", 4)
	for p in peak_t:
		if p > beat1 and p < sec_t[-1] and min(abs(p - t) for t in sec_t) > bar_s * 1.5 and nov[np.argmin(abs(nov_t - p))] > 0.6:
			idx = int(np.searchsorted(sec_t, p)) - 1
			bar_in = (p - sec_t[idx]) / bar_s
			warnings.append(f"Deutliche Formgrenze bei {(p - beat1) * 1000:.0f} ms (ca. Takt {bar_in + 1:.1f} in '{song['sections'][idx]['name']}') "
							f"ohne YAML-Grenze - fehlt ein Abschnitt oder stimmt eine Taktzahl nicht?")

	#--- Akzente ---
	accents = []
	for i, sec in enumerate(song["sections"]):
		a, b = sec_t[i], sec_t[i + 1]
		beat_s = 60.0 / section_bpm(sec, song)
		bpb = sec.get("beats_per_bar", song.get("beats_per_bar", 4))
		m = (times >= a) & (times < b)
		if m.sum() < 4 or (raw[i] and raw[i]["silent"]):
			continue
		med = np.median(rms_db[m])

		def pos(t):
			beats = (t - a) / beat_s
			return {"bar": int(beats // bpb) + 1, "beat": round(beats % bpb + 1, 1)}

		# Stille >= 1/2 Takt
		quiet = rms_db < max(med - 18, -45)
		run_start = None
		for j in np.where(m)[0]:
			if quiet[j] and run_start is None:
				run_start = j
			elif not quiet[j] and run_start is not None:
				if times[j] - times[run_start] >= beat_s * bpb / 2 and times[j] < b - beat_s / 2:
					accents.append({"section": sec["name"], **pos(times[run_start]), "type": "stop",
									"len_beats": round((times[j] - times[run_start]) / beat_s, 1)})
					accents.append({"section": sec["name"], **pos(times[j]), "type": "hit_after_stop"})
				run_start = None

		# letzter Takt: Crescendo / Wirbel vor lauterem Folgepart
		last_bar = (times >= b - beat_s * bpb) & (times < b)
		if i + 1 < len(song["sections"]) and last_bar.sum() > 4:
			nxt = (times >= b) & (times < b + beat_s * bpb * 2)
			rise = rms_db[last_bar][-8:].mean() - rms_db[last_bar][:8].mean()
			on_last = ((onset_times >= b - beat_s * bpb) & (onset_times < b)).sum() / (beat_s * bpb)
			on_sec = ((onset_times >= a) & (onset_times < b)).sum() / (b - a)
			if nxt.sum() and rms_db[nxt].mean() > med + 3 and (rise > 3 or on_last > 1.5 * on_sec):
				accents.append({"section": sec["name"], "bar": int(round((b - a) / (beat_s * bpb))), "beat": 1.0,
								"type": "fill_into_next", "rise_db": round(float(rise), 1)})

	result = {
		"audio": str(audio_path.relative_to(ROOT)) if audio_path.is_relative_to(ROOT) else str(audio_path),
		"audio_dauer_s": round(dur_s, 1),
		"takt1_im_audio_ms": round(beat1 * 1000),
		"takt1_quelle": beat1_src,
		"bpm_yaml": song["bpm"],
		"bpm_gemessen": round(measured_bpm, 2) if measured_bpm else None,
		"tonart_gesamt": f"{song_key[1]}-{song_key[2]}",
		"legende": {
			"power": "0-5 Lautheit: 5 = lautester Part, je 1,5 dB leiser eine Stufe weniger, 0 = still",
			"build": "-1..+1 Lautstärkeverlauf im Part (+ = steigert sich)",
			"drive": "0-1 rhythmische Dichte / perkussiver Anteil",
			"brightness": "0-1 Klangfarbe dunkel/warm -> hell/scharf",
			"lowend": "0-1 Bass-Anteil (Drop, Bass setzt ein)",
			"mood": "Dur/Moll-Tendenz des Parts",
			"grenze_hoerbar": "an der Part-Grenze ändert sich hörbar der Klang",
		},
		"sections": sections_out,
		"accents": accents,
		"warnungen": warnings,
	}
	out_yaml.write_text(yaml.safe_dump(plain(result), allow_unicode=True, sort_keys=False, width=140), encoding="utf-8")
	plot(song, y, sr, times, rms_db, S, sec_t, sections_out, accents, nov_t, nov, peak_t, beat1, out_png)
	return result


def plot(song, y, sr, times, rms_db, S, sec_t, sections_out, accents, nov_t, nov, peak_t, beat1, out_png):
	fig, ax = plt.subplots(4, 1, figsize=(18, 11), sharex=True,
						   gridspec_kw={"height_ratios": [2, 2, 1.6, 1]})
	t0 = beat1
	rel = lambda t: np.asarray(t) - t0	# x-Achse: Sekunden ab Takt 1

	# 1) Lautstärke
	ax[0].plot(rel(times), rms_db, lw=0.6, color="#555")
	ax[0].plot(rel(times), np.convolve(rms_db, np.ones(40) / 40, mode="same"), lw=1.6, color="#d62728")
	ax[0].set_ylabel("RMS dB")
	ax[0].set_ylim(-50, 2)

	# 2) Spektrogramm
	mel = librosa.feature.melspectrogram(S=S ** 2, sr=sr)
	librosa.display.specshow(librosa.power_to_db(mel, ref=np.max), sr=sr, hop_length=HOP, x_axis=None, y_axis="mel",
							 ax=ax[1], cmap="magma", x_coords=rel(times[:mel.shape[1]]))
	ax[1].set_ylabel("Mel")

	# 3) Merkmale pro Abschnitt als Balken
	colors = {"power": "#d62728", "drive": "#ff7f0e", "brightness": "#bcbd22", "lowend": "#1f77b4"}
	for i, s in enumerate(sections_out):
		if "power" not in s:
			continue
		a, b = rel(sec_t[i]), rel(sec_t[i + 1])
		w = (b - a) / 5
		for k, (name, col) in enumerate(colors.items()):
			v = s[name] / 5 if name == "power" else s[name]
			ax[2].bar(a + w * (k + 0.5), v, width=w * 0.9, color=col, label=name if i == 0 else None)
	ax[2].set_ylim(0, 1.05)
	ax[2].legend(loc="upper left", ncol=4, fontsize=8)
	ax[2].set_ylabel("Merkmale")

	# 4) Novelty
	ax[3].plot(rel(nov_t), nov, color="#2ca02c")
	for p in peak_t:
		ax[3].axvline(rel(p), color="#2ca02c", ls=":", lw=1)
	ax[3].set_ylabel("Formwechsel")
	ax[3].set_xlabel("Sekunden ab Takt 1")

	# Abschnitte über alle Plots
	for i, s in enumerate(sections_out):
		a = rel(sec_t[i])
		for axx in ax:
			axx.axvline(a, color="#1f77b4", lw=1)
		label = s["name"] + (f"\nP{s['power']}" if "power" in s else "")
		ax[0].text(a + 0.3, 0.5, label, fontsize=7, va="bottom", rotation=90, color="#1f77b4")
	for acc in accents:
		i = [s["name"] for s in song["sections"]].index(acc["section"])
		bpb = song.get("beats_per_bar", 4)
		t = sec_t[i] + ((acc["bar"] - 1) * bpb + acc["beat"] - 1) * 60.0 / section_bpm(song["sections"][i], song)
		ax[0].plot(rel(t), -3, marker="v", color="black")
		ax[0].text(rel(t), -1, acc["type"], fontsize=6, ha="center")
	ax[0].axvline(rel(sec_t[-1]), color="#1f77b4", lw=1)

	fig.suptitle(f"#{song['id']} {song['name']} - {song['bpm']} BPM", fontsize=12)
	fig.tight_layout()
	fig.savefig(out_png, dpi=90)
	plt.close(fig)


def main():
	ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
	ap.add_argument("song", help="Ordnername unter songs/ (Anfang genügt)")
	args = ap.parse_args()

	try:
		song_dir = find_song_dir(args.song)
	except SongError as e:
		print(f"FEHLER: {e}", file=sys.stderr)
		return 1
	path = song_dir / SONG_FILE
	if not path.exists():
		print(f"FEHLER: {path.relative_to(ROOT)} fehlt", file=sys.stderr)
		return 1
	song = yaml.safe_load(path.read_text(encoding="utf-8"))
	audio = find_audio(song, song_dir)
	if audio is None:
		print(f"FEHLER: Audiodatei nicht gefunden (audio: {song.get('audio')}) - MP3 nach songs/{song_dir.name}/quelle/ legen", file=sys.stderr)
		return 1
	for sec in song["sections"]:
		section_beats(sec, song)	# Validierung

	out_dir = song_dir / "audio-analyse"
	out_dir.mkdir(exist_ok=True)
	out_yaml = out_dir / "analysis.yaml"
	out_png = out_dir / "analysis.png"
	res = analyze(song, audio, out_yaml, out_png)

	print(f"Tempo: YAML {res['bpm_yaml']}  gemessen {res['bpm_gemessen']}   Takt 1 im Audio: {res['takt1_im_audio_ms']} ms ({res['takt1_quelle']})")
	print(f"Tonart gesamt: {res['tonart_gesamt']}\n")
	print(f"  {'abschnitt':<22} {'power':>5} {'build':>6} {'drive':>6} {'bright':>6} {'low':>5}  mood")
	for s in res["sections"]:
		if "power" in s:
			print(f"  {s['name']:<22} {s['power']:>5} {s['build']:>6} {s['drive']:>6} {s['brightness']:>6} {s['lowend']:>5}  {s['mood']}")
		else:
			print(f"  {s['name']:<22} ({s.get('hinweis')})")
	if res["accents"]:
		print("\nAkzente:")
		for a in res["accents"]:
			print(f"  {a['section']:<22} Takt {a['bar']} Beat {a['beat']}: {a['type']}")
	if res["warnungen"]:
		print("\nWarnungen:")
		for w in res["warnungen"]:
			print("  - " + w)
	print(f"\n-> {out_yaml}\n-> {out_png}")
	return 0


if __name__ == "__main__":
	sys.exit(main())
