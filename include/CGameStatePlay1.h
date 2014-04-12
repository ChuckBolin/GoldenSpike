#ifndef GAME_STATE_PLAY1_H
#define GAME_STATE_PLAY1_H

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
#include "CTrain.h"

//const double C_ROCK_TIME = 24;
//const double C_WATER_TIME = 10;
struct PROFILE{
  long x;
  long y;
  int id;
};

class CGameStatePlay1 :  public CGameStateObject{
public:
  CGameStatePlay1(void);
  ~CGameStatePlay1(void);
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

  long MouseToWorldX(long mx, long wx, float scale);
  long MouseToWorldY(long my, long wy, float scale);
  long MouseToWorldTileX(long mx, long wx, float scale, long offset);
  long MouseToWorldTileY(long my, long wy, float scale, long offset);

private:

  std::vector<TRANSITION_EVENT> m_TE; //stores all transition events
  CTimer m_timer;
  int m_event;
  CMouse m_mouse;
  float m_vx;
  float m_vy;
  bool m_bPause;
  bool m_bActivated;
  float m_maskX;
  float m_maskY;
  float m_zoom;
  int m_maskAlpha;
  int m_lightAlpha;
  int m_waterAlpha;
  bool m_waterAlphaCountUp;
  bool m_lightOn;
  float m_scale;
  float m_lastScale;
  bool m_bStateActive;
  bool m_bFirstTime;
  bool m_bGameOver;
  bool m_bCelebrate;

  //track profiles
  //int m_trackProfile[6];
  int m_currentProfile;

  //lanterns
  int m_lanternX;
  int m_lanternY;

  //map
  //bool m_bMap;
  int m_mapX;
  int m_mapY;
  
  int m_selectMapX;
  int m_selectMapY;

  bool m_bRenderReady;

  //true if the mouse is over this location
  bool m_bCalendar;
  bool m_bSelectMap;
  bool m_bMapIcon;
  bool m_bMap;
  bool m_bWatch;
  bool m_bSave;
  bool m_bTopBar;
  bool m_bBottomBar;
  bool m_bZoom;
  bool m_bUndo;
  bool m_bWorld;
  bool m_bExit;
  bool m_bEdit;
  //exit
  int m_exitX;
  int m_exitY;

  //calendar
  int m_calendarX;
  int m_calendarY;

  //watch
  int m_watchX;
  int m_watchY;

  //icons
  int m_iconSelectX;
  int m_iconSelectY;

  //zoom
  int m_zoomX;
  int m_zoomY;

  //save
  int m_saveX;
  int m_saveY;

  //undo
  int m_undoX;
  int m_undoY;
  
  //check connection
  int m_checkX;
  int m_checkY;
  bool m_bCheck;

  //sound music
  bool m_bMusic;
  bool m_bSound;
  int m_musicX;
  int m_musicY;
  int m_soundX;
  int m_soundY;
  bool m_bMusicEnabled;
  bool m_bSoundEnabled;

  //train test data
  CTrain m_train;
  CTrain m_train2;
  double m_train2Delay;

  //stores profiles
  std::vector<PROFILE> m_profile;
  long m_offset;

  //experiment
  long m_initX;
  long m_initY;
  long m_currX;
  long m_currY;
  std::vector<PROFILE> m_show;
  bool m_bReset;

  void CountSeries(CGameData &data);
  long m_startX;
  long m_startY;
  long m_endX;
  long m_endY;
  int m_explodeCount;
  float m_velocity;
};

#endif
