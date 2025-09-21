
# calibration.md – Kalibrasi Sensor (LV‑only)

Gunakan menu **Settings → Calibration** di OLED untuk memasukkan:
- `V_SCALE`, `V_OFFSET` untuk ZMPT,
- `I_SCALE`, `I_OFFSET` untuk ACS.

Langkah cepat:
1) Siapkan sumber **AC terisolasi berlevel rendah** untuk uji tegangan & beban primer untuk uji arus.
2) Bandingkan pembacaan firmware (Ekran/Serial) dengan multimeter/clamp → sesuaikan **scale/offset**.
3) **Long‑press** di layar Calibration untuk **Save & Exit** (disimpan ke NVS, persistent).

Catatan keselamatan tetap sama: **tanpa HV**.
