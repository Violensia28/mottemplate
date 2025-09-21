
# MOTSMART CI Patch (SAFE_MODE OFF via CI + Runtime Bypass for Test)

This ZIP contains:

- `.github/workflows/ci_build.yml` — GitHub Actions workflow to build env `full`,
  inject macros via `EXTRA_FLAGS` (SAFE_MODE=0, BYPASS_SAFETY=1, BUILD_TAG),
  upload artifacts, optional merged image, optional GitHub Release.
- `ci_build/platformio.ini` (template) — shows how to add `${sysenv.EXTRA_FLAGS}`
  under `[env:full] -> build_flags`.
- `ci_build/src/SAFE_OFF_SNIPPET.txt` — code snippet to paste into your `setup()`.
- `tools/apply_patch_locally.sh` — helper script to copy files into your repo, commit, push, and tag.

## How to apply (Termux)

1) Extract this ZIP inside your local repo root (folder containing `ci_build/`).

2) Ensure your `ci_build/platformio.ini` has this line at the bottom of `build_flags`:

    ${sysenv.EXTRA_FLAGS}

3) Open `ci_build/src/main.cpp`, paste the content of `ci_build/src/SAFE_OFF_SNIPPET.txt`
   **right after** `Serial.begin(115200);` in `setup()` and adjust includes if needed.

4) Commit & push to `main`, then create a new tag to trigger Actions:

```bash
cd /path/to/your/local/mottemplate
git add .github/workflows/ci_build.yml ci_build/platformio.ini ci_build/src/main.cpp
git commit -m "safeoff: inject EXTRA_FLAGS and add runtime bypass snippet"
git push origin main

git tag v0.4.2-safeoff
git push origin v0.4.2-safeoff
```

5) Download artifacts from GitHub → Actions → latest run → Artifacts → `motsmart-full`.

6) Flash via Android flasher:
   - 0x1000 → `bootloader*.bin`
   - 0x8000 → `partitions*.bin`
   - 0x10000 → `firmware.bin`
   If only `firmware.bin` exists, flash at `0x10000` (assuming bootloader/partitions unchanged).

7) Serial monitor (115200) should show:

```
[CFG] SAFE_MODE (compile-time) = 0
[CFG] BUILD_TAG = v0.4.2-safeoff
[SAFETY] BYPASS_SAFETY=1 -> FORCE SAFE_SIM OFF (TEST MODE)
[BOOT] motsmartfirmware v0.3-SMART r1 (SAFE_SIM OFF)
```

**Safety note:** BYPASS_SAFETY=1 is only for testing with PC817 + buzzer.
Disable it (set to 0) before real welding.
