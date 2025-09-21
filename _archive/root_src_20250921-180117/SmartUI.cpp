
#include "SmartUI.h"

bool SmartUI::begin(Adafruit_SSD1306* d, PulseEngine* e, StorageManager* s,
                    SafetyManager* safe, AIAdvisor* ai, SenseEngine* se, Logger* lg){
  display=d; engine=e; storage=s; safety=safe; advisor=ai; sense=se; logger=lg;
  storage->load(cfg); // ambil kalibrasi dari NVS
  st.v_scale=cfg.v_scale; st.v_offset=cfg.v_offset; st.i_scale=cfg.i_scale; st.i_offset=cfg.i_offset;
  st.tgt = advisor->estimateTarget(st.t_mm, nullptr); st.E_target = st.tgt.E_target; st.rec = advisor->recommend(st.tgt, nullptr, st.v_guess, st.i_guess);
  return true;
}

void SmartUI::tick(){ if (millis()-lastUi < (1000/UI_FPS)) return; lastUi=millis(); display->clearDisplay();
  switch(view){ case SmartView::HOME: drawHome(); break; case SmartView::DETAIL: drawDetail(); break; case SmartView::RUNNING: drawRunning(); break; case SmartView::RATING: drawRating(); break; case SmartView::SETTINGS: drawSettings(); break; case SmartView::CALIB: drawCalib(); break; }
  display->display(); }

void SmartUI::onRotate(int d){
  if (view==SmartView::HOME || view==SmartView::DETAIL){ st.t_mm += d*0.01f; st.t_mm = constrain(st.t_mm, 0.10f, 0.25f); refreshRecommendation(); }
  else if (view==SmartView::RATING){ st.ratingSel += d; if (st.ratingSel<-1) st.ratingSel=-1; if (st.ratingSel>+1) st.ratingSel=+1; }
  else if (view==SmartView::SETTINGS){ st.i_guess += d*10.0f; st.i_guess = constrain(st.i_guess, 50.0f, 800.0f); refreshRecommendation(); }
  else if (view==SmartView::CALIB){
    // Edit value
    CalibField f = st.calSel; float step = (f==CalibField::V_SCALE || f==CalibField::I_SCALE)? 0.01f : 0.01f;
    if (f==CalibField::V_SCALE) st.v_scale = max(0.10f, st.v_scale + d*step);
    if (f==CalibField::V_OFFSET) st.v_offset += d*step;
    if (f==CalibField::I_SCALE) st.i_scale = max(0.10f, st.i_scale + d*step);
    if (f==CalibField::I_OFFSET) st.i_offset += d*step;
  }
}

void SmartUI::onClick(){
  if (view==SmartView::HOME){ view=SmartView::DETAIL; }
  else if (view==SmartView::DETAIL){ if (safety && !safety->canStart()) return; if (sense) sense->armForPulse(); engine->start(st.rec); view=SmartView::RUNNING; }
  else if (view==SmartView::RUNNING){ /* no-op */ }
  else if (view==SmartView::RATING){ if (logger) logger->logRecord(st.t_mm, st.tgt, st.rec, st.last, st.ratingSel); if (advisor) advisor->updateRLS(st.t_mm, st.last, st.ratingSel); view=SmartView::HOME; }
  else if (view==SmartView::SETTINGS){ view=SmartView::CALIB; }
  else if (view==SmartView::CALIB){ // cycle field
    st.calSel = (CalibField)(((int)st.calSel + 1) % 4);
  }
}

void SmartUI::onTrigger(){ if (view==SmartView::HOME) view=SmartView::DETAIL; onClick(); }

void SmartUI::onLongPress(){
  if (view==SmartView::CALIB){ // Save & Exit
    cfg.v_scale=st.v_scale; cfg.v_offset=st.v_offset; cfg.i_scale=st.i_scale; cfg.i_offset=st.i_offset; storage->save(cfg); view=SmartView::SETTINGS; return; }
  engine->cancel(); view=SmartView::HOME;
}

void SmartUI::onStage(PulseStage stg){ uint8_t id = stageToId(stg); if (id==1 || id==3 || id==5) { if (sense) sense->onStageStart(id); }
  if (stg==PulseStage::GAP1) { if (sense) sense->onStageEnd(1); }
  if (stg==PulseStage::GAP2) { if (sense) sense->onStageEnd(3); }
  if (stg==PulseStage::DONE){ if (sense){ sense->onStageEnd(5); WeldMetrics wm; sense->finalize(wm); // apply calibration
      wm.Vrms_pre   = cfg.v_scale*wm.Vrms_pre   + cfg.v_offset; wm.Irms_pre   = cfg.i_scale*wm.Irms_pre   + cfg.i_offset;
      wm.Vrms_main  = cfg.v_scale*wm.Vrms_main  + cfg.v_offset; wm.Irms_main  = cfg.i_scale*wm.Irms_main  + cfg.i_offset;
      wm.Vrms_temper= cfg.v_scale*wm.Vrms_temper+ cfg.v_offset; wm.Irms_temper= cfg.i_scale*wm.Irms_temper+ cfg.i_offset;
      // recompute energies using calibrated values (approx):
      // Note: E_stage sebelumnya dihitung dgn nilai sebelum kalibrasi; untuk akurasi penuh pindahkan kalibrasi ke SenseEngine
      st.last=wm; st.E_actual=wm.E_total(); }
    st.ratingSel=0; view=SmartView::RATING; }
  if (stg==PulseStage::ABORT){ view=SmartView::HOME; }
}

uint8_t SmartUI::stageToId(PulseStage stg){ switch(stg){ case PulseStage::PRE: return 1; case PulseStage::MAIN: return 3; case PulseStage::TEMPER: return 5; default: return 0; } }

void SmartUI::drawHeader(const __FlashStringHelper* title){ display->setTextSize(1); display->setTextColor(SSD1306_WHITE); display->setCursor(0,0); display->print(title); drawIconsStatus(); display->drawLine(0,10,127,10,SSD1306_WHITE); }

void SmartUI::drawFooter(const String& hint){ display->drawLine(0,53,127,53,SSD1306_WHITE); display->setCursor(2,56); display->print(hint); }

void SmartUI::drawEnergyBar(int16_t x,int16_t y,int16_t w,int16_t h,float frac){ display->drawRect(x,y,w,h,SSD1306_WHITE); int fill=(int)(w*constrain(frac,0.f,1.2f)); display->fillRect(x+1,y+1,max(0,fill-2),h-2,SSD1306_WHITE); }

void SmartUI::drawIconsStatus(){
  int x=80; display->setCursor(x,0); // V icon
  bool vok = sense? sense->vPinValid():false; display->print("V"); display->print(vok?"✔":"✘"); x+=18;
  display->setCursor(x,0); bool iok = sense? sense->iPinValid():false; display->print("I"); display->print(iok?"✔":"✘"); x+=18;
  display->setCursor(x,0); bool sdok = logger? logger->sdOK():false; display->print("SD"); display->print(sdok?"✔":"✘");
}

void SmartUI::drawHome(){ drawHeader(F("Smart Home")); display->setCursor(0,14); display->printf("t: %.2f mm\n", st.t_mm); display->printf("Mode: %s\n", st.tgt.useTriple? "Triple":(st.tgt.useDouble? "Double":"Single")); display->printf("E*: %.1f J\n", st.E_target); display->printf("pre/main/temp: %u/%u/%u ms\n", st.rec.pre_ms, st.rec.main_ms, st.rec.temper_ms); drawFooter(F("Click: Detail  | Rotate: t")); }

void SmartUI::drawDetail(){ drawHeader(F("Rekomendasi")); display->setCursor(0,14); display->printf("E_target: %.1f J\n", st.E_target); display->printf("Pola: %s\n", st.tgt.useTriple? "Triple":(st.tgt.useDouble? "Double":"Single")); display->printf("PRE : %u ms\n", st.rec.pre_ms); display->printf("MAIN: %u ms\n", st.rec.main_ms); display->printf("TEMP: %u ms\n", st.rec.temper_ms); drawEnergyBar(0,45,127,7,1.0f); drawFooter(F("Click/Trigger: Mulai | Long-press: Settings")); }

void SmartUI::drawRunning(){ drawHeader(F("Welding...")); display->setCursor(0,14); display->println("Stage: RUN"); display->printf("t=%.2f E*=%.1fJ\n", st.t_mm, st.E_target); static uint8_t k=0; k=(k+10)%128; drawEnergyBar(0,32,127,10,k/127.0f); drawFooter(F("Long-press: Cancel")); }

void SmartUI::drawRating(){ drawHeader(F("Hasil & Rating")); display->setCursor(0,14); display->printf("E_actual: %.1f J\n", st.E_actual); display->printf("E_target: %.1f J\n", st.E_target); float frac=(st.E_target>0? st.E_actual/st.E_target:0); drawEnergyBar(0,32,127,10,frac); display->setCursor(0,45); display->print("Rating: "); if (st.ratingSel<0) display->print("Bad"); else if (st.ratingSel==0) display->print("OK"); else display->print("Great"); drawFooter(F("Rotate: pilih | Click: simpan")); }

void SmartUI::drawSettings(){ drawHeader(F("Settings")); display->setCursor(0,14); display->printf("I_guess: %.0f A\n", st.i_guess); display->println("Calibration → Click"); drawFooter(F("Rotate: I_guess | Click: Calib | Long-press: Back")); }

void SmartUI::drawCalib(){ drawHeader(F("Calibration")); display->setCursor(0,12); display->printf("V_SCALE : %.3f %s\n", st.v_scale, (st.calSel==CalibField::V_SCALE? "<":" ")); display->printf("V_OFFSET: %.3f %s\n", st.v_offset, (st.calSel==CalibField::V_OFFSET?"<":" ")); display->printf("I_SCALE : %.3f %s\n", st.i_scale, (st.calSel==CalibField::I_SCALE? "<":" ")); display->printf("I_OFFSET: %.3f %s\n", st.i_offset, (st.calSel==CalibField::I_OFFSET?"<":" ")); drawFooter(F("Rotate: edit | Click: next | Long-press: Save")); }

void SmartUI::refreshRecommendation(){ st.tgt = advisor->estimateTarget(st.t_mm, nullptr); st.E_target = st.tgt.E_target; st.rec = advisor->recommend(st.tgt, nullptr, st.v_guess, st.i_guess); }
