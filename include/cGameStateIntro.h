#ifndef GAME_STATE_INTRO_H
#define GAME_STATE_INTRO_H

#include "CGameStateObject.h"
#include "CGraphics.h"
#include "CAudioManager.h"
#include "CTimer.h"
#include <sstream>
#include <string>
#include "keystatus.h"
#include "CGameData.h"
#include "CLog.h"
#include "CConfigData.h"

class CGameStateIntro :  public CGameStateObject{
public:
  CGameStateIntro(void);
  ~CGameStateIntro(void);
  void Initialize();
  void Activate(CGameData &data);
  CGameStateObject* Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con);
  void Render(CGraphics &con,  CGameData &data, CConfigData &cfg);
  void ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
  void Deactivate(CGameData &data);
  void Resume();
  void Pause();
  void Save();
  void AddTransitionEvent(int event, CGameStateObject* p_Next);

private:
  std::vector<TRANSITION_EVENT> m_TE; //stores all transition events
  CTimer m_timer;  //used for fading effect
  int m_fadeAlpha; //0..255 
  int m_graphic;   //0 = world map, 1 = title screen
  int m_event;
  CMouse m_mouse;

};

#endif