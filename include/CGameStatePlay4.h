#ifndef GAME_STATE_PLAY4_H
#define GAME_STATE_PLAY4_H

#include <iomanip>
#include <sstream>
#include <string>
#include "CGameStateObject.h"
#include "CGraphics.h"
#include "CAudioManager.h"
#include "CTimer.h"
#include "keystatus.h"
#include "CGameData.h"
#include "CLog.h"

class CGameStatePlay4 :  public CGameStateObject{
public:
  CGameStatePlay4(void);
  ~CGameStatePlay4(void);
  void Initialize();
  void Activate();
  CGameStateObject* Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con);
  void Render(CGraphics &con,  CGameData &data, CConfigData &cfg);
  void ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
  void Deactivate();
  void Resume();
  void Pause();
  void Save();
  void AddTransitionEvent(int event, CGameStateObject* p_Next);

private:

  std::vector<TRANSITION_EVENT> m_TE; //stores all transition events
  CTimer m_timer;
  int m_event;
  CMouse m_mouse;

};

#endif
