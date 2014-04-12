/**************************************************************************************
Filename: CGameData.h Date: December, 2008
Purpose: Manages all game data.
**************************************************************************************/
#ifndef CGAMEDATA_H
#define CGAMEDATA_H

#include <string>
#include <windows.h>
#include "CLog.h"
#include "CFileReader.h"
#include "CCSVReader.h"
#include "CGraphics.h"
#include "CObject.h"
#include "CTrack.h"
#include "CGameClock.h"

class CGraphics;
class CConfigData;

//game object structures
struct CLOUD{
  int spriteID;
  long x;
  long y;
  float angle;
  float scale;
  double velocity;
};
struct HILL{
  int spriteID;
  long x;
  long y;
  float angle;
  float scale;
};
struct WATER{
  int spriteID;
  long x;
  long y;
};
struct TREE{
  int spriteID;
  long x;
  long y;
  float angle;
  float scale;
};
struct BUILDING{
  int spriteID;
  long x;
  long y;
  float angle;
  float scale;
};

struct LASTTRACK{
  int uniqueID;
  bool complete;
};

class CGameData{
public:
  CGameData();
  bool LoadConfigFile(std::string filename);
  bool SaveLevelFile(int fileNum);
  int m_FPS;
  HWND m_hWnd;

  //environmental boundaries
  int m_worldX;
  int m_worldY;
  
  bool m_bLeftMouseDown;
  bool m_bRightMouseDown;

  void LoadGameLevel(std::string filename);
  int LevelSize();

  bool AddGraphicDataToLevelData(CGraphics &con);
  bool AddObject(CGraphics &con, std::string objectName, int x, int y);
  std::string GetObjectName(int id);

  void LogObjects();
  void ClearLevel();
  void SaveLevel (int level);
  void OpenLevel (int level, CGraphics &con);
  bool m_playNewGame;

  //variables loaded from level1.dat file
  int m_screenColorRed;
  int m_screenColorGreen;
  int m_screenColorBlue;
  long m_worldLeft;
  long m_worldRight;
  long m_lastWorldLeft;
  long m_lastWorldTop;
  long m_worldTop;
  long m_worldBottom;
  float m_scale;
  int m_landscapeTexture;
  std::vector<CLOUD> m_cloud;
  std::vector<HILL> m_hill;
  std::vector<TREE> m_tree;
  std::vector<WATER> m_water;
  std::vector<CTrack> m_track;
  std::vector<BUILDING> m_building;
  bool GenerateTrainData();

  //game over
  bool m_bGameOver;
  bool m_bGameWon;
  bool m_bRailroadComplete;

  //starting and ending junctions
  long m_startX;
  long m_startY;
  long m_endX;
  long m_endY;

  int m_lastUniqueID;
  LASTTRACK m_lastTrack[10];
  int m_freeTeams;
  int m_numberPeople;
  int m_numberTeams;

  //track detection
  int m_seriesCount; //counts number of sections in track layout
  bool m_bTrackComplete; //true if track is complete

  CGameClock m_gameClock;

private:

};

#endif