#include "..\include\CConfigData.h"

CConfigData::CConfigData(){
  Initialize();
}


CConfigData::CConfigData(std::string configFilename){
  Initialize();
  LoadConfigFile(configFilename);
}

CConfigData::~CConfigData(){
}

bool CConfigData::LoadConfigFile(std::string configFilename){
  CLog *pLog = CLog::Instance();

  //function variables
  std::string line;
  int equalCount = 0;
  std::ifstream file(configFilename.c_str());
  
  //open file for reading
  if(file.is_open()){

    //read one line at a time
    while(!file.eof()){
      getline (file, line);
      line = RemoveSpaces(line);
      line = RemoveComments(line);
      equalCount = CountEqualSigns(line);
      if(line.size() != 0 && equalCount == 1){
        m_lines.push_back(line);
        equalCount = 0;
        pLog->Log("Config", line);
      }
    }

    file.close();
  }

  //file doesn't exist
  else
    return false;

  //break string into key and value
  std::string key;
  std::string value;
  int equalPos = 0;
  bool bFound;
  
  pLog->Log("***************************************");
  pLog->Log("Configuration File Data (key = value)");
  pLog->Log("***************************************");

  for(int i = 0; i < m_lines.size(); ++i){
    
    equalPos = m_lines[i].find("=");
    if(equalPos > 0){
      key = m_lines[i].substr(0, equalPos);
      value = m_lines[i].substr(equalPos + 1);
      
      pLog->LogPair(key, value);

      //load all config variables with values
      bFound = false;

      if(key == "ScreenLeft"){
        ScreenLeft = atoi(value.c_str());
        bFound = true;
      }
      else if(key == "ScreenTop"){
        ScreenTop = atoi(value.c_str());
        bFound = true;
      }
      else if(key == "ScreenWidth"){
        ScreenWidth = atoi(value.c_str());
        bFound = true;
      }
      else if(key == "ScreenHeight"){
        ScreenHeight = atoi(value.c_str());
        bFound = true;
      }
      else if(key == "FullScreen"){
        FullScreen = (bool)(atoi(value.c_str()));
        bFound = true;
      }
      else if(key == "DisplayDebugInfo"){
        DisplayDebugInfo = (bool)(atoi(value.c_str()));
        bFound = true;
      }
      else if(key == "LogDebugInfo"){
        LogDebugInfo = (bool)(atoi(value.c_str()));
        bFound = true;
      }
      else if(key == "PlaySounds"){
        PlaySounds = (bool)(atoi(value.c_str()));
        bFound = true;
      }
      else if(key == "PlayMusic"){
        PlayMusic = (bool)(atoi(value.c_str()));
        bFound = true;
      }
      else if(key == "FrameworkAssetFile"){
        FrameworkAssetFile = value;
        bFound = true;
      }
      else if(key == "GamePlayAssetFile"){
        GamePlayAssetFile = value;
        bFound = true;
      }
      else if(key == "GameObjectFile"){
        GameObjectFile = value;
        bFound = true;
      }
      else if(key == "CreditsFile"){
        CreditsFile = value;
        bFound = true;
      }
      else if(key == "ProgramName"){
        ProgramName = value;
        bFound = true;
      }
      else if(key == "ProgramVersion"){
        ProgramVersion = value;
        bFound = true;
      }     
      else if(key == "ScrollSpeed"){
        ScrollSpeed = atof(value.c_str());
        bFound = true;
      }
      else if(key == "RockTime"){
        RockTime = atof(value.c_str());
        bFound = true;
      }
      else if(key == "WaterTime"){
        WaterTime = atof(value.c_str());
        bFound = true;
      }
      if(bFound == false)
        pLog->Log("No match for config file key", key);

    }
  }
}

//sets default values for all configuration values
void CConfigData::Initialize(){

  //[Screen]
  ScreenLeft = 0;
  ScreenTop = 0;
  ScreenWidth = 1024;
  ScreenHeight = 768;
  FullScreen = false;

  //[Debug]
  DisplayDebugInfo = true;
  LogDebugInfo = true;

  //[Audio]
  PlaySounds = false;
  PlayMusic = false;

  //[Files]
  FrameworkAssetFile = "assets//data//editor.dat";
  GamePlayAssetFile  = "assets//data//assets.dat";
  GameObjectFile     = "assets//data//objects.dat";
  CreditsFile        = "assets//data//credits.dat";

  //[Data]
  ProgramName        = "gc11_chuck";
  ProgramVersion     = "v006";


}

//removes all spaces from a string
std::string CConfigData::RemoveSpaces(std::string in){
  std::string temp;

  for(int i = 0; i < in.size(); i++){
    if(in.substr(i, 1) != " "){
      temp = temp + in.substr(i,1);      
    }
  }

  return temp;
}


//removes all comments from a string //
std::string CConfigData::RemoveComments(std::string in){
  std::string temp;

  for(int i = 0; i < in.size(); i ++){
    if(in.substr(i,1) != ";")
      temp = temp + in.substr(i, 1);
    else
      return temp;
  }
  return temp;
}

//counts number of equal signs in string
int CConfigData::CountEqualSigns(std::string in){
  int count = 0;

  for(int i = 0; i < in.size(); i++){
    if(in.substr(i, 1) == "=")
      count++;
  }
  
  return count;
}