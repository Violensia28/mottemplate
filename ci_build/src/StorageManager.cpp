
#include "StorageManager.h"

#define STORAGE_NS "motsmart"
#define KEY_CONF   "cfg"

void StorageManager::begin(){ prefs.begin(STORAGE_NS, false); }

void StorageManager::loadDefaults(AppConfig& c){
  c.version = MODEL_VERSION; c.presetIndex = 0;
  c.doublePulseEnabled = true; c.triplePulseEnabled = true;
  c.cooldown_ms = COOLDOWN_MS; c.lockout_ms = LOCKOUT_MS;
  for(int i=0;i<10;i++){
    c.presets[i].pre_ms    = (uint16_t)(5 + i/3);
    c.presets[i].gap1_ms   = (uint16_t)(40 + i*5);
    c.presets[i].main_ms   = (uint16_t)(18 + i*3);
    c.presets[i].gap2_ms   = (uint16_t)(60 + i*4);
    c.presets[i].temper_ms = (i>=7)? (uint16_t)(6 + (i-7)*2) : 0;
  }
  c.v_scale=1.0f; c.v_offset=0.0f; c.i_scale=1.0f; c.i_offset=0.0f;
}

void StorageManager::load(AppConfig& cfg){
  size_t need = sizeof(AppConfig);
  if (!prefs.isKey(KEY_CONF)) { loadDefaults(cfg); save(cfg); return; }
  size_t got = prefs.getBytesLength(KEY_CONF);
  if (got != need) { loadDefaults(cfg); save(cfg); return; }
  prefs.getBytes(KEY_CONF, &cfg, need);
  if (cfg.version != MODEL_VERSION) { loadDefaults(cfg); save(cfg); }
}

void StorageManager::save(const AppConfig& cfg){ prefs.putBytes(KEY_CONF, &cfg, sizeof(AppConfig)); }
