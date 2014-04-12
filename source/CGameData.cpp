#include "..\include\CGameData.h"

//game data constructor
//set all initial game data variables here
CGameData::CGameData(){

  //default information...may be replaced by level data file
  m_screenColorRed = 0;
  m_screenColorGreen = 111;
  m_screenColorBlue = 255;  
  m_worldX = 0;
  m_worldY = 0;
  m_worldLeft = 0;
  m_worldRight = 72000;
  m_worldTop = 0;
  m_worldBottom = 48000;
  m_scale = 1.0;
  m_landscapeTexture = -1;
  m_bLeftMouseDown = false;
  m_bRightMouseDown = false;

  m_startX = 0;
  m_startY = 100;
  m_endX = 800;
  m_endY = 100;

  //manages teams and lanterns
  m_lastUniqueID = -1;
  LASTTRACK temp;
  temp.complete = true;
  temp.uniqueID = -1;
  for(int i = 0; i < 10; ++i)
    m_lastTrack[i] = temp;
  m_freeTeams = 10;
  m_numberPeople = 1000;
  m_numberTeams = 10;

  //manages total numbers of tracks
  m_seriesCount = 0;
  m_bTrackComplete = false;
  m_bGameOver = false;
  m_bGameWon = false;
  m_bRailroadComplete = false;
}

//need to add sprite data to level data
bool CGameData::AddGraphicDataToLevelData(CGraphics & con){
  CLog *pLog = CLog::Instance();
  pLog->Log("Completed the addition of graphic data to level data");
  return true;
}

bool CGameData::AddObject(CGraphics &con, std::string objectName, int x, int y){
  return true;
}

void CGameData::LoadGameLevel(std::string filename){
  CLog *pLog = CLog::Instance();
  //validate asset data and add to appropriate vectors
//  GAME_OBJECT temp;
  std::string value;
  std::string asset;
  std::string parameter;

  //new
  pLog->Log("Loading game level", filename);
  CCSVReader csv;
  csv.LoadFile(filename);
  pLog->Log("Level table size", csv.GetTableSize());

  CLOUD tempCloud;
  HILL tempHill;
  WATER tempWater;
  CTrack tempTrack;
  TREE tempTree;
  BUILDING tempBuilding;
  m_track.clear();
  m_hill.clear();
  m_water.clear();
  m_tree.clear();
  m_building.clear();

  for(int i = 0; i < csv.GetTableSize(); ++i){

    asset = csv.GetTerm(i, 0);        
    if(asset == "back_color" && csv.GetNumberParameters(i)== 4 ){
      parameter = csv.GetTerm(i, 1);
      m_screenColorRed = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_screenColorGreen = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      m_screenColorBlue = atoi(parameter.c_str());      
    }
    else if(asset == "world" && csv.GetNumberParameters(i)  == 5 ){
      parameter = csv.GetTerm(i, 1);
      m_worldLeft = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_worldTop = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      m_worldRight = atoi(parameter.c_str());      
      parameter = csv.GetTerm(i, 4);
      m_worldBottom = atoi(parameter.c_str());      
    }
    else if(asset == "position" && csv.GetNumberParameters(i)  == 3 ){
      parameter = csv.GetTerm(i, 1);
      m_worldX = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_worldY = atoi(parameter.c_str());

    }
    else if(asset == "start" && csv.GetNumberParameters(i)  == 3 ){
      parameter = csv.GetTerm(i, 1);
      m_startX = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_startY = atoi(parameter.c_str());

      //add 6 horizontal tracks to m_track
      for(int i = 0; i < 6; ++i){
        tempTrack.profileID = 21;
        tempTrack.x = (i * 62) + m_startX;
        tempTrack.y = m_startY - 5;
        tempTrack.ballastID = 50;
        tempTrack.tieID = 51;
        tempTrack.railID = 52;
        tempTrack.spriteID = tempTrack.railID;
        tempTrack.complete = true;
        m_lastUniqueID++;
        tempTrack.uniqueID = m_lastUniqueID;
        m_track.push_back(tempTrack);
      }
    }
    else if(asset == "end" && csv.GetNumberParameters(i)  == 3 ){
      parameter = csv.GetTerm(i, 1);
      m_endX = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      m_endY = atoi(parameter.c_str());

      //add 6 horizontal tracks to m_track
      for(int i = 0; i < 8; ++i){
        tempTrack.profileID = 21;

        tempTrack.x = (i * 62) + m_endX;
        tempTrack.y = m_endY - 5;
        tempTrack.ballastID = 50;
        tempTrack.tieID = 51;
        tempTrack.railID = 52;
        tempTrack.spriteID = tempTrack.railID;//profileID;
        tempTrack.complete = true;
        //tempTrack.m_countDown = 0; //
        m_lastUniqueID++;
        tempTrack.uniqueID = m_lastUniqueID;
        m_track.push_back(tempTrack);
      }
    }
    else if(asset == "landscape" && csv.GetNumberParameters(i)  == 2 ){
      parameter = csv.GetTerm(i, 1);
      m_landscapeTexture = atoi(parameter.c_str());
    }
    else if(asset == "scale" && csv.GetNumberParameters(i)  == 2 ){
      parameter = csv.GetTerm(i, 1);
      m_scale = atof(parameter.c_str());
    }
    else if(asset == "time" && csv.GetNumberParameters(i)  == 2 ){
      parameter = csv.GetTerm(i, 1);
      m_gameClock.SetSeconds(atoi(parameter.c_str()));
    }

    //clouds
    else if(asset == "cloud" && csv.GetNumberParameters(i) == 6){
      parameter = csv.GetTerm(i, 1);
      tempCloud.spriteID = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      tempCloud.x = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      tempCloud.y = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 4);
      tempCloud.angle = atof(parameter.c_str());
      parameter = csv.GetTerm(i, 5);
      tempCloud.scale = atof(parameter.c_str());

      tempCloud.velocity = 200 + rand() % 200;
      m_cloud.push_back(tempCloud);
    }
    //hills
    else if(asset == "hill" && csv.GetNumberParameters(i) == 6){
      parameter = csv.GetTerm(i, 1);
      tempHill.spriteID = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      tempHill.x = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      tempHill.y = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 4);
      tempHill.angle = atof(parameter.c_str());
      parameter = csv.GetTerm(i, 5);
      tempHill.scale = atof(parameter.c_str());
      m_hill.push_back(tempHill);
    }
    //trees
    else if(asset == "tree" && csv.GetNumberParameters(i) == 6){
      parameter = csv.GetTerm(i, 1);
      tempTree.spriteID = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      tempTree.x = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      tempTree.y = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 4);
      tempTree.angle = atof(parameter.c_str());
      parameter = csv.GetTerm(i, 5);
      tempTree.scale = atof(parameter.c_str());
      m_tree.push_back(tempTree);
    }
    //building
    else if(asset == "building" && csv.GetNumberParameters(i) == 6){
      parameter = csv.GetTerm(i, 1);
      tempBuilding.spriteID = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      tempBuilding.x = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      tempBuilding.y = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 4);
      tempBuilding.angle = atof(parameter.c_str());
      parameter = csv.GetTerm(i, 5);
      tempBuilding.scale = atof(parameter.c_str());
      m_building.push_back(tempBuilding);
    }

    //water
    else if(asset == "water" && csv.GetNumberParameters(i) == 4){
      parameter = csv.GetTerm(i, 1);
      tempWater.spriteID = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      tempWater.x = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      tempWater.y = atoi(parameter.c_str());
      m_water.push_back(tempWater);
    }

    else if(asset == "track" && csv.GetNumberParameters(i) == 4){
      parameter = csv.GetTerm(i, 1);
      tempTrack.profileID = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 2);
      tempTrack.x = atoi(parameter.c_str());
      parameter = csv.GetTerm(i, 3);
      tempTrack.y = atoi(parameter.c_str());
      tempTrack.complete = true;
      tempTrack.m_countDown = 0;

      if(tempTrack.profileID == 21){//hor track
        tempTrack.ballastID = 50;
        tempTrack.tieID = 51;
        tempTrack.railID = 52;
        tempTrack.spriteID = tempTrack.railID;
        m_track.push_back(tempTrack);
      }
      else if(tempTrack.profileID == 22){//vert track
        tempTrack.ballastID = 53;
        tempTrack.tieID = 54;
        tempTrack.railID = 55;
        tempTrack.spriteID = tempTrack.railID;
        m_track.push_back(tempTrack);
      }
      else if(tempTrack.profileID == 18){//quad I
        tempTrack.ballastID = 56;
        tempTrack.tieID = 57;
        tempTrack.railID = 58;
        tempTrack.spriteID = tempTrack.railID;
        m_track.push_back(tempTrack);
      }
      else if(tempTrack.profileID == 20){//quad IV
        tempTrack.ballastID = 59;
        tempTrack.tieID = 60;
        tempTrack.railID = 61;
        tempTrack.spriteID = tempTrack.railID;
        m_track.push_back(tempTrack);
      }
      else if(tempTrack.profileID == 17){//quad II
        tempTrack.ballastID = 62;
        tempTrack.tieID = 63;
        tempTrack.railID = 64;
        tempTrack.spriteID = tempTrack.railID;
        m_track.push_back(tempTrack);
      }
      else if(tempTrack.profileID == 19){//quad III
        tempTrack.ballastID = 65;
        tempTrack.tieID = 66;
        tempTrack.railID = 67;
        tempTrack.spriteID = tempTrack.railID;
        m_track.push_back(tempTrack);
      }
    }
  }
}

bool CGameData::SaveLevelFile(int fileNum){
  if(fileNum < 1 || fileNum > 4)
    return false;

  std::string filename;
  if(fileNum == 1)
    filename = "assets\\data\\saved1.dat";
  else if(fileNum == 2)
    filename = "assets\\data\\saved2.dat";
  else if(fileNum == 3)
    filename = "assets\\data\\saved3.dat";
  else if(fileNum == 4)
    filename = "assets\\data\\saved4.dat";

  std::string line;
  std::ostringstream oss;
  
  std::ofstream file(filename.c_str());
  if(file.is_open()){
    //add header
    file << "//Golden Spike Saved Game: " << filename << std::endl;
    file << "//****************************************************" << std::endl;
    
    //game variables
    file << "//miscellaneous game variables" << std::endl;
    file << "******************************" << std::endl;
    oss.str("");
    oss << "back_color" << ", " << m_screenColorRed << ", " << m_screenColorGreen << ", " << m_screenColorBlue << std::endl;
    file << oss.str();
    
    oss.str("");    
    oss << "landscape" << ", " <<  m_landscapeTexture << std::endl;
    file << oss.str();
    
    oss.str("");    
    oss << "world" << ", " << m_worldLeft  << ", " <<  m_worldTop  << ", " << m_worldRight  << ", " <<  m_worldBottom << std::endl;
    file << oss.str();

    oss.str("");
    oss << "position" << ", " <<  m_worldX << ", " << m_worldY <<std::endl;
    file << oss.str();

    oss.str("");
    oss << "start" << ", " <<  m_startX << ", " << m_startY << std::endl;
    file << oss.str();

    oss.str("");
    oss << "end" << ", " <<  m_endX << ", " << m_endY <<std::endl;
    file << oss.str();

    oss.str("");    
    oss << "scale" << ", " <<  m_scale << std::endl;
    file << oss.str();

    oss.str("");    
    oss << "time" << ", " <<  m_gameClock.GetSeconds() << std::endl;
    file << oss.str();

    oss.str("");        
    file << std::endl;

    //save level data
    file << "//track data" << std::endl;
    for(int i = 0; i < m_track.size(); ++i)
      file << "track, " << m_track[i].profileID << ", " << m_track[i].x << ", " << m_track[i].y << std::endl;
    oss.str("");        
    file << std::endl;

    //save hill data
    file << "//hill data" << std::endl;
    for(int i = 0; i < m_hill.size(); ++i)
      file << "hill, " << m_hill[i].spriteID << ", " << m_hill[i].x << ", " << m_hill[i].y << ", " << m_hill[i].angle << ", " << m_hill[i].scale << std::endl;
    oss.str("");        
    file << std::endl;

    //save tree data
    file << "//tree data" << std::endl;
    for(int i = 0; i < m_tree.size(); ++i)
      file << "tree, " << m_tree[i].spriteID << ", " << m_tree[i].x << ", " << m_tree[i].y << ", " << m_tree[i].angle << ", " << m_tree[i].scale << std::endl;
    oss.str("");        
    file << std::endl;

    //save building data
    file << "//building data" << std::endl;
    for(int i = 0; i < m_building.size(); ++i)
      file << "building, " << m_building[i].spriteID << ", " << m_building[i].x << ", " << m_building[i].y << ", " << m_building[i].angle << ", " << m_building[i].scale << std::endl;
    oss.str("");        
    file << std::endl;

    //save river data
    file << "//water data" << std::endl;
    for(int i = 0; i < m_water.size(); ++i)
      file << "water, " << m_water[i].spriteID << ", " << m_water[i].x << ", " << m_water[i].y << std::endl;
    oss.str("");        
    file << std::endl;

    file.close();
  }  

    return true;

}

void CGameData::ClearLevel(){
//  m_level.clear();
}


void CGameData::OpenLevel (int level, CGraphics &con){
  CLog *pLog = CLog::Instance();

  if(level == 1)
    LoadGameLevel("assets\\data\\level1.dat");
  else if(level == 2)
    LoadGameLevel("assets\\data\\level2.dat");
  else if(level == 3)
    LoadGameLevel("assets\\data\\level3.dat");
  else if(level == 4)
    LoadGameLevel("assets\\data\\level4.dat");

  pLog->Log("Level loaded", level);

  AddGraphicDataToLevelData(con);  
}

//reads m_track data and calculates
// m_train data
bool CGameData::GenerateTrainData(){


  return true;
}