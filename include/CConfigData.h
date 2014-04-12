/*****************************************************
  CConfigData - Written by Chuck Bolin
  Game configuration data is loaded from a file,
  stored in public variables.  An initialization
  routines sets the config variables to default values.

  Sample Data:
  ;Program configuration file

  [Screen]
  ScreenLeft        = 0
  ScreenTop         = 0
  ScreenWidth       = 1024
  ScreenHeight      = 768
  FullScreen        = 0

  [Debug]
  DisplayDebugInfo  = 1  ;Displays data to screen
  LogDebugInfo      = 1  ;Logs data to file

  [Audio]
  PlaySounds = 0 
  PlayMusic = 0
*****************************************************/
#ifndef CCONFIG_DATA
#define CCONFIG_DATA

#include <string>
#include <vector>
#include <fstream>
#include "CLog.h"

class CConfigData{
public:
  CConfigData();

  CConfigData(std::string configFilename);
  ~CConfigData();
  bool LoadConfigFile(std::string configFilename);

  //configuration variables MUST match config file names
  //if variable added here, add an initialization code
  //in initialization function and load file.
  // 1) Add variable declaration here (public).
  // 2) Add to initialization code.
  // 3) Add to loadconfigfile(...).
  // 4) Add to the config.cfg file.
  //
  //[Screen]
  int ScreenLeft;
  int ScreenTop;
  int ScreenWidth;
  int ScreenHeight;
  bool FullScreen;

  //[Debug]
  bool DisplayDebugInfo;
  bool LogDebugInfo;

  //[Audio]
  bool PlaySounds;
  bool PlayMusic;  

  //[Files]
  std::string FrameworkAssetFile;
  std::string GamePlayAssetFile;
  std::string GameObjectFile;
  std::string CreditsFile;

  //[Data]
  std::string ProgramName;
  std::string ProgramVersion;
  double ScrollSpeed;
  double RockTime;
  double WaterTime;

private:
  void Initialize();
  std::string RemoveSpaces(std::string in);
  std::string RemoveComments(std::string in);
  int CountEqualSigns(std::string in);

  std::vector<std::string> m_lines; //stores parsed lines  
};


#endif