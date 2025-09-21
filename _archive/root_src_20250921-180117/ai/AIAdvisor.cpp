
#include "ai/AIAdvisor.h"
#include <math.h>

static void eye6(float* P, float val){ for(int r=0;r<6;r++){ for(int c=0;c<6;c++){ P[r*6+c] = (r==c? val:0); } } }

void AIAdvisor::initDefaults(){ M.version=1; M.theta[0]=6.0f; M.theta[1]=80.0f; M.theta[2]=120.0f; M.theta[3]=0.0f; M.theta[4]=0.0f; M.theta[5]=0.0f; eye6(M.P, 10.0f); }
void AIAdvisor::begin(){ initDefaults(); }

AITarget AIAdvisor::estimateTarget(float t_mm, const WeldMetrics* last){
  float Ir= last? last->Irms_main:0.0f; float Rp=last? last->R_pre:0.0f; float Rm=last? last->R_main:0.0f;
  float x[6] = {1.0f, t_mm, t_mm*t_mm, Ir, Rp, Rm}; float Et=0.0f; for(int i=0;i<6;i++) Et += M.theta[i]*x[i];
  if (Et<6.0f) Et=6.0f; if (Et>40.0f) Et=40.0f;
  AITarget tgt; tgt.E_target=Et;
  if (t_mm<0.15f){ tgt.useDouble=false; tgt.useTriple=false; tgt.preFrac=0.0f; tgt.temperFrac=0.0f; }
  else if (t_mm<0.20f){ tgt.useDouble=true;  tgt.useTriple=false; tgt.preFrac=0.18f; tgt.temperFrac=0.0f; }
  else { tgt.useDouble=true; tgt.useTriple=true; tgt.preFrac=0.22f; tgt.temperFrac=0.12f; }
  _lastTarget=Et; return tgt;
}

PulseProfile AIAdvisor::recommend(const AITarget& tgt, const WeldMetrics* last,
                                  float v_rms_guess, float i_rms_guess){
  float Vr = last? (last->Vrms_main>0? last->Vrms_main : v_rms_guess) : v_rms_guess;
  float Ir = last? (last->Irms_main>0? last->Irms_main : i_rms_guess) : i_rms_guess;
  float P = fmaxf(1.0f, Vr*Ir);
  float E_pre = tgt.E_target*tgt.preFrac; float E_temper = tgt.useTriple? tgt.E_target*tgt.temperFrac:0.0f; float E_main = fmaxf(0.0f, tgt.E_target-E_pre-E_temper);
  auto e2ms = [&](float E){ return (uint16_t)roundf(1000.0f*E/P); };
  PulseProfile p{}; p.pre_ms=e2ms(E_pre); p.main_ms=e2ms(E_main); p.temper_ms=e2ms(E_temper);
  p.gap1_ms=(tgt.useDouble||tgt.useTriple)?50:0; p.gap2_ms=(tgt.useTriple?60:0);
  auto clamp=[&](uint16_t &x, uint16_t lo,uint16_t hi){ x=(x<lo?lo:(x>hi?hi:x)); };
  if (tgt.preFrac>0) clamp(p.pre_ms,3,60); else p.pre_ms=0; clamp(p.main_ms,3,120); if (tgt.useTriple) clamp(p.temper_ms,3,80); else p.temper_ms=0; return p;
}

void AIAdvisor::rlsUpdate(const float* x, float y){ const float lambda=0.99f; float* P=M.P; float* th=M.theta; float Px[6]; for(int r=0;r<6;r++){ float s=0; for(int c=0;c<6;c++) s+=P[r*6+c]*x[c]; Px[r]=s; } float xTPx=0; for(int i=0;i<6;i++) xTPx+=x[i]*Px[i]; float denom=lambda+xTPx; float K[6]; for(int i=0;i<6;i++) K[i]=Px[i]/denom; float xTtheta=0; for(int i=0;i<6;i++) xTtheta+=x[i]*th[i]; float err=y-xTtheta; for(int i=0;i<6;i++) th[i]+=K[i]*err; float KxT[36]; for(int r=0;r<6;r++){ for(int c=0;c<6;c++){ KxT[r*6+c]=K[r]*x[c]; } } float KxTP[36]; for(int r=0;r<6;r++){ for(int c=0;c<6;c++){ float s=0; for(int k=0;k<6;k++) s+=KxT[r*6+k]*P[k*6+c]; KxTP[r*6+c]=s; } } for(int r=0;r<6;r++){ for(int c=0;c<6;c++){ P[r*6+c]=(P[r*6+c]-KxTP[r*6+c])/lambda; } }
}

void AIAdvisor::updateRLS(float t_mm, const WeldMetrics& wm, int8_t rating){ float scale=1.0f+0.1f*(float)rating; float y=scale*wm.E_total(); float x[6]={1.0f, t_mm, t_mm*t_mm, wm.Irms_main, wm.R_pre, wm.R_main}; rlsUpdate(x,y); }
