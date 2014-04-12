#include "..\include\CGameStatePlay1.h"
 
CGameStatePlay1::CGameStatePlay1(){
}

CGameStatePlay1::~CGameStatePlay1()
{
	m_TE.clear();
}

void CGameStatePlay1::Initialize(){
  m_bFirstTime = true;
  m_bRenderReady = false;
  m_explodeCount = 50;
}

void CGameStatePlay1::Activate(CGameData &data){
  //m_bFirstTime = true;
  //m_bActivated = true;
  m_bStateActive = true;
  data.m_bGameOver = false;
  data.m_bGameWon = false;
  data.m_track.clear();
  m_train.ResetWaypoints();
  CLog *pLog = CLog::Instance();
  pLog->Log("Play Activate");
}
//***************************************************************
//                         U P D A T E
//***************************************************************
CGameStateObject* CGameStatePlay1::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  m_event = EVENT_GO_NO_WHERE;
  CLog *pLog = CLog::Instance();

  if(::GetActiveWindow() != data.m_hWnd)
    return NULL;

  //exit game if dead
  static double killTime = 0;
  if(data.m_bGameOver == true){
    killTime += timeDifference;
    if(killTime > 2)
      m_event = EVENT_GO_MAIN;
  }

  CAudioManager *pAudio = CAudioManager::Instance();
 
  //winner
  if(cfg.PlayMusic == true && data.m_bGameWon == true){
  //if(cfg.PlayMusic == true && m_bCelebrate == true){
    if(pAudio->IsPlaying(5) == false)
      pAudio->PlaySoundClip(5, 1);
  }

  if(m_bPause == true)
    pAudio->PauseAll();
  else
    pAudio->UnPauseAll();

  //play background music
  if(cfg.PlayMusic == true && data.m_bGameOver == false && data.m_bGameWon == false){
    if(pAudio->IsPlaying(3) == false)
      pAudio->PlaySoundClip(3, 1);
  }
  else{
    pAudio->StopSoundClip(3);
  }

  //player background sounds
  if(cfg.PlaySounds == true && data.m_bGameOver == false && data.m_bGameWon == false){
    if(cfg.PlaySounds == true && pAudio->IsPlaying(0) == false)//ambient
      pAudio->PlaySoundClip(0, 1);
    if(cfg.PlaySounds == true && pAudio->IsPlaying(7) == false)//water
      pAudio->PlaySoundClip(7, 1);
    if(cfg.PlaySounds == true && pAudio->IsPlaying(2) == false)//foreman
      pAudio->PlaySoundClip(2, 1);
    if(cfg.PlaySounds == true && pAudio->IsPlaying(6) == false)//voice
      pAudio->PlaySoundClip(6, 1);
  }
  else{    
    pAudio->StopSoundClip(0);
    pAudio->StopSoundClip(7);
    pAudio->StopSoundClip(2);
    pAudio->StopSoundClip(6);
  }



  //manage random explosion track
  static double explodeTime = 0;
  explodeTime += timeDifference;
  if(explodeTime > 1.0){
    m_explodeCount--;
    if(m_explodeCount < 1){
      if(cfg.PlaySounds == true && data.m_bGameOver == false && data.m_bGameWon == false){
        if(cfg.PlaySounds == true && pAudio->IsPlaying(1) == false)//explosion
          pAudio->PlaySoundClip(1, 0);
      }
      else
        pAudio->StopSoundClip(1);

      m_explodeCount = 25 + rand() % 100;
    }
    explodeTime = 0;
  }

  if(m_bStateActive == true){
    data.m_gameClock.Pause(false);
    data.m_gameClock.Update();
    if(data.m_gameClock.GetSeconds() < 1)
      data.m_bGameOver = true;
  }
  else
    data.m_gameClock.Pause(true);

  static bool bStop = false;
  CTrack tempTrack; 

  //prevents a partial display from being rendered
  static double renderTime = 0;
  renderTime += timeDifference;
  if(renderTime > 0.5)
    m_bRenderReady = true;
  
  //perform this code only once
  //****************************
  //static bool bFirstTime = true;

  if(m_bFirstTime == true){// || m_bActivated == true){
    //bFirstTime = false;
    m_bFirstTime = false;
    //m_bActivated = false;
    m_bCelebrate = false;
    m_maskX = 0;
    m_maskY = 0;
    m_maskAlpha = 0;//133;
    m_lightAlpha = 52;
    m_zoom = 1.0;
    m_waterAlpha = 125;
    m_waterAlphaCountUp = true;
    m_lightOn = false;
    data.m_numberPeople = 1000;
    data.m_numberTeams = 10;
    
    //position of icons
    m_iconSelectX = 263;
    m_iconSelectY = 620;
    m_mapX = 215;
    m_mapY = 255;
    m_exitX = 584;//215;
    m_exitY = 625;//687;
    m_calendarX = 15;//120;
    m_calendarY = 615;
    m_selectMapX = 215;
    m_selectMapY = 625;
    m_watchX = 890;//916;//812;
    m_watchY = 568;
    m_zoomX = 704;
    m_zoomY = 618;
    m_undoX = 756;
    m_undoY = 677;    
    m_saveX = 836;
    m_saveY = 677;    
    m_lanternX = 322;
    m_lanternY = 25;
    m_checkX = 676;
    m_checkY = 677;
    m_bCheck = false;
    m_bExit = false;
    m_bCalendar = false;
    m_bMap = false;
    m_bSelectMap = false;
    m_bWatch = false;
    m_bSave = false;
    m_bTopBar = false;
    m_bBottomBar = false;
    m_bZoom = false;
    m_bWorld = false;
    m_bPause = false;
    m_bSelectMap = false;
    m_bRenderReady = false;
    m_offset = 62.0f;
    m_bEdit = false;
    m_bStateActive = true; //false if play is deactivated
    m_velocity = 140;
    m_train.SetVelocity(m_velocity);// * data.m_gameClock.GetTimeScale());
    m_train.SetVelocity(m_velocity);
    m_bMusic = true;
    m_bSound = true;
    m_musicX = 850;
    m_musicY = 50;
    m_soundX = 925;
    m_soundY = 50;
    data.m_bGameOver = false;

    //experiment
    m_initX = 0;
    m_initY = 0;
    m_currX = 0;
    m_currY = 0;
    m_bReset = true;
    m_train2Delay = 0;
    //data.m_track.clear();
    m_show.clear();


    //data.LoadGameLevel("assets\\data\\saved1.dat");
    if(data.m_playNewGame == true)    
      data.LoadGameLevel("assets\\data\\level1.dat"); //new game level
    else
      data.LoadGameLevel("assets\\data\\saved1.dat"); //saved game data

    data.AddGraphicDataToLevelData(con);            //update graphic data
  }
  
  m_mouse.SetHandle(data.m_hWnd);//needed for mouse

  //if(::GetActiveWindow() == data.m_hWnd && keyDown(VK_ESCAPE))//quit this mode
  //  m_event = EVENT_GO_MAIN;

  //determine mouse position over things
  //******************************************
  m_bCalendar = false;
  m_bWatch = false;
  m_bZoom = false;
  m_bUndo = false;  
  m_bMap = false;
  m_bSave = false;
  m_bTopBar = false;
  m_bBottomBar = false;
  m_bMapIcon = false;
  m_bWorld = false;
  m_bExit = false;
  m_bCheck = false;
  m_bMusicEnabled = false;
  m_bSoundEnabled = false;


  if(m_bSelectMap == false){
    if(m_mouse.MouseX() > m_calendarX && m_mouse.MouseX() < m_calendarX + 190 &&
      m_mouse.MouseY() > m_calendarY && m_mouse.MouseY() < m_calendarY + 136)
      m_bCalendar = true;
    else if(m_mouse.MouseX() > m_watchX && m_mouse.MouseX() < m_watchX + 113 &&
      m_mouse.MouseY() > m_watchY && m_mouse.MouseY() < m_watchY + 156)
      m_bWatch = true;
    else if(m_mouse.MouseX() > m_zoomX && m_mouse.MouseX() < m_zoomX + 202 &&
      m_mouse.MouseY() > m_zoomY && m_mouse.MouseY() < m_zoomY + 51)
      m_bZoom = true;
    else if(m_mouse.MouseX() > m_undoX && m_mouse.MouseX() < m_undoX + 63 &&
        m_mouse.MouseY() > m_undoY && m_mouse.MouseY() < m_undoY + 71)
        m_bUndo = true;
    else if(m_mouse.MouseX() > m_checkX && m_mouse.MouseX() < m_checkX + 63 &&
        m_mouse.MouseY() > m_checkY && m_mouse.MouseY() < m_checkY + 71)
        m_bCheck = true;
    else if(m_mouse.MouseX() > m_saveX && m_mouse.MouseX() < m_saveX + 63 &&
        m_mouse.MouseY() > m_saveY && m_mouse.MouseY() < m_saveY + 71)
        m_bSave = true;
    else if(m_mouse.MouseX() > m_selectMapX && m_mouse.MouseX() < m_selectMapX + 100 &&
        m_mouse.MouseY() > m_selectMapY && m_mouse.MouseY() < m_selectMapY + 50)
        m_bMapIcon = true;
    else if(m_mouse.MouseX() > m_exitX && m_mouse.MouseX() < m_exitX + 100 &&
        m_mouse.MouseY() > m_exitY && m_mouse.MouseY() < m_exitY + 50)
        m_bExit = true;
    else if(m_mouse.MouseX() > m_soundX && m_mouse.MouseX() < m_soundX + 46 &&
        m_mouse.MouseY() > m_soundY && m_mouse.MouseY() < m_soundY + 51)
        m_bSoundEnabled = true;
    else if(m_mouse.MouseX() > m_musicX && m_mouse.MouseX() < m_musicX + 46 &&
        m_mouse.MouseY() > m_musicY && m_mouse.MouseY() < m_musicY + 51)
        m_bMusicEnabled = true;
    else
      m_bWorld = true; //means cursor is over a non-interface item
  }
  else{
    if(m_mouse.MouseX() > m_selectMapX && m_mouse.MouseX() < m_selectMapX + 100 &&
        m_mouse.MouseY() > m_selectMapY && m_mouse.MouseY() < m_selectMapY + 50)
        m_bMapIcon = true;
  }
  

  //let's control the game world horizontally for development
  static double keyTime = 0;
  keyTime += timeDifference;
  double scrollSpeed2 = cfg.ScrollSpeed/data.m_scale;
  //double scrollSpeed2 = 750/data.m_scale;

  double scrollSpeed = 500/ data.m_scale;
  PROFILE tempProfile;
  double keyTimeLimit = 0.1;
  
  //click on calendar to pause game
  //**********************************
  static double pauseTime = 0;
  pauseTime += timeDifference;
  if(data.m_bLeftMouseDown == true && m_bCalendar == true &&  pauseTime > 0.2 && m_bReset == true){
    if(m_bPause == true)
      m_bPause = false;
    else
      m_bPause = true;
    pauseTime = 0;
  }
  data.m_gameClock.Pause(m_bPause);
  if(m_bPause == true)
   return NULL;

  //only process keys if this window has focus
  //*****************************************************************
  //coarse speed control arrows
  if(keyDown(VK_LEFT) == true)
    data.m_worldX -= scrollSpeed * timeDifference;
  if(keyDown(VK_RIGHT) == true)
    data.m_worldX += scrollSpeed * timeDifference;
  if(keyDown(VK_UP) == true)
    data.m_worldY -= scrollSpeed * timeDifference;
  if(keyDown(VK_DOWN) == true)
    data.m_worldY += scrollSpeed * timeDifference;

  //mouse control for scrolling
  int border = 10;
  if(m_mouse.MouseX() < border)
    data.m_worldX -= scrollSpeed2 * timeDifference;
  if(m_mouse.MouseX() > cfg.ScreenWidth - border)
    data.m_worldX += scrollSpeed2 * timeDifference;
  if(m_mouse.MouseY() < border)
    data.m_worldY -= scrollSpeed2 * timeDifference;
  if(m_mouse.MouseY() > cfg.ScreenHeight - border)
    data.m_worldY += scrollSpeed2 * timeDifference;

  //update position based upon world boundaries
  if(data.m_worldX < data.m_worldLeft)
    data.m_worldX = data.m_worldLeft;
  if(data.m_worldX > data.m_worldRight - cfg.ScreenWidth / data.m_scale)
    data.m_worldX = data.m_worldRight - cfg.ScreenWidth / data.m_scale;
  if(data.m_worldY < data.m_worldTop)
    data.m_worldY = data.m_worldTop;
  if(data.m_worldY > data.m_worldBottom - cfg.ScreenHeight / data.m_scale)
    data.m_worldY = data.m_worldBottom - cfg.ScreenHeight / data.m_scale;

  //manage audio
  static double audioTime = 0;
  audioTime += timeDifference;
  if(data.m_bLeftMouseDown == true && audioTime > 0.2){
    if(m_mouse.MouseX() > m_musicX && m_mouse.MouseX() < m_musicX + 46 &&
        m_mouse.MouseY() > m_musicY && m_mouse.MouseY() < m_musicY + 51)
        m_bMusic = !m_bMusic;
    else if(m_mouse.MouseX() > m_soundX && m_mouse.MouseX() < m_soundX + 46 &&
        m_mouse.MouseY() > m_soundY && m_mouse.MouseY() < m_soundY + 51)
        m_bSound = !m_bSound;
    
    cfg.PlaySounds = m_bSound;
    cfg.PlayMusic = m_bMusic;
    audioTime = 0;
  }

  //editor - adds items
  static double keyEdit = 0;
  if(m_bEdit == true){
    keyEdit += timeDifference;
    long editX = ((long)((float)m_mouse.MouseX() / data.m_scale + data.m_worldX) / m_offset) * m_offset;
    long editY = ((long)((float)m_mouse.MouseY() / data.m_scale + data.m_worldY) / m_offset) * m_offset;
    WATER water;
    HILL hill;
    TREE tree;
    BUILDING building;

    if(keyDown('1') == true && keyEdit > 0.2){
      water.spriteID = 80;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('2') == true && keyEdit > 0.2){
      water.spriteID = 81;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('3') == true && keyEdit > 0.2){
      water.spriteID = 82;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('4') == true && keyEdit > 0.2){
      water.spriteID = 83;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('5') == true && keyEdit > 0.2){
      water.spriteID = 84;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('6') == true && keyEdit > 0.2){
      water.spriteID = 85;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('7') == true && keyEdit > 0.2){
      water.spriteID = 86;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('8') == true && keyEdit > 0.2){
      water.spriteID = 87;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('9') == true && keyEdit > 0.2){
      water.spriteID = 88;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    if(keyDown('0') == true && keyEdit > 0.2){
      water.spriteID = 89;
      water.x = editX;
      water.y = editY;
      data.m_water.push_back(water);
      keyEdit = 0;
    }
    //30, 31, 32, 33
    if(keyDown('I') == true && keyEdit > 0.2){
      hill.spriteID = 30 + rand() % 4;
      hill.x = editX;
      hill.y = editY;
      hill.angle = 0;
      hill.scale = 1;
      data.m_hill.push_back(hill);
      keyEdit = 0;
    }
    if(keyDown('O') == true && keyEdit > 0.2){
      hill.spriteID = 30 + rand() % 4;
      hill.x = editX;
      hill.y = editY;
      hill.angle = 1.57;
      hill.scale = 1;
      data.m_hill.push_back(hill);
      keyEdit = 0;
    }
    if(keyDown('K') == true && keyEdit > 0.2){
      hill.spriteID = 30 + rand() % 4;
      hill.x = editX;
      hill.y = editY;
      hill.angle = 3.14;
      hill.scale = 1;
      data.m_hill.push_back(hill);
      keyEdit = 0;
    }
    if(keyDown('L') == true && keyEdit > 0.2){
      hill.spriteID = 30 + rand() % 4;
      hill.x = editX;
      hill.y = editY;
      hill.angle = 4.71;
      hill.scale = 1;      
      data.m_hill.push_back(hill);
      keyEdit = 0;
    }

    if(keyDown('T') == true && keyEdit > 0.2){
      tree.spriteID = 96;
      tree.x = editX;
      tree.y = editY;
      tree.angle = (float)(rand() % 628) * 0.01f;
      tree.scale = 0.75f + (float)(rand() % 200) * 0.01f;      
      data.m_tree.push_back(tree);
      keyEdit = 0;
    }

    //buildings
    if(keyDown('F') == true && keyEdit > 0.2){
      building.spriteID = 112;
      building.x = editX;
      building.y = editY;
      building.angle = 0;
      building.scale = 1.0f; //0.75f + (float)(rand() % 200) * 0.01f;      
      data.m_building.push_back(building);
      keyEdit = 0;
    }
    if(keyDown('G') == true && keyEdit > 0.2){
      building.spriteID = 113;
      building.x = editX;
      building.y = editY;
      building.angle = 0;
      building.scale = 1.0f; //0.75f + (float)(rand() % 200) * 0.01f;      
      data.m_building.push_back(building);
      keyEdit = 0;
    }
    if(keyDown('H') == true && keyEdit > 0.2){
      building.spriteID = 114;
      building.x = editX;
      building.y = editY;
      building.angle = 0;
      building.scale = 1.0f; //0.75f + (float)(rand() % 200) * 0.01f;      
      data.m_building.push_back(building);
      keyEdit = 0;
    }
    if(keyDown('A') == true && keyEdit > 0.2){
      building.spriteID = 113;
      building.x = editX;
      building.y = editY;
      building.angle = 1.57;
      building.scale = 1.0f; //0.75f + (float)(rand() % 200) * 0.01f;      
      data.m_building.push_back(building);
      keyEdit = 0;
    }
    if(keyDown('S') == true && keyEdit > 0.2){
      building.spriteID = 114;
      building.x = editX;
      building.y = editY;
      building.angle = 1.57;
      building.scale = 1.0f; //0.75f + (float)(rand() % 200) * 0.01f;      
      data.m_building.push_back(building);
      keyEdit = 0;
    }
    //quick movement keys
    if(keyDown(VK_HOME) == true){
      data.m_worldX = data.m_startX;
      data.m_worldY = data.m_startY - 300;
    }
    if(keyDown(VK_END) == true){
      data.m_worldX = data.m_endX - 100;
      data.m_worldY = data.m_endY - 300;
    }

    if(keyDown('U') == true  && keyTime > 0.2){
      data.m_track.pop_back();
      keyTime = 0;
    }

    //clears track sections except for starting and beginning lengths
    if(keyDown('C') == true){
      m_bReset = true;
      std::vector<CTrack> tempTrack;
      for(int i = 0; i < 9; ++i)
          tempTrack.push_back(data.m_track[i]);
      data.m_track.clear();
      for(int i = 0; i < 9; ++i)
          data.m_track.push_back(tempTrack[i]);
      m_train.ResetWaypoints();
      m_train.Reset();
      for(int i = 0; i < 10; ++i)
        data.m_lastTrack[i].complete = true;

  }

  }

  //controls darkness and light
  int sec = data.m_gameClock.GetLightSeconds();
  /*
  if(sec < 30)
    m_maskAlpha = 255;
  else if(sec < 53)
    m_maskAlpha = 255 - (int)(255.0f * ((float)(sec - 30)/23.0f));
  else if(sec < 135)
    m_maskAlpha = 0;
  else if(sec < 165)
    m_maskAlpha = (int)(255.0f * ((float)(sec - 135)/30.0f));
  else
    m_maskAlpha = 255;
*/

  if(m_maskAlpha < 0)
    m_maskAlpha = 0;
  else if(m_maskAlpha > 255)
    m_maskAlpha = 255;

  //lantern calculated based upon darkness
  m_lightAlpha = (int)((float)m_maskAlpha *.392f);
  
  //count number of track sections in series beginning at starting point
  static double wpTime = 0;
  if(data.m_bGameWon == false)
    wpTime += timeDifference;  
  WAYPOINT wp;
  if( (m_bCheck == true && data.m_bLeftMouseDown == true && keyTime > 1.0) || wpTime > 60){
    m_bReset = true;
    m_train.SetVelocity(m_velocity);
    m_train.ResetWaypoints();            
    CountSeries(data);
    m_train.Reset();
    keyTime = 0;
    wpTime = 0;
  }

  //removes railroad
  bool bRemoved = false;
  if(data.m_bRightMouseDown == true && keyTime > keyTimeLimit/4){
    tempTrack.x = ((long)((float)m_mouse.MouseX() / data.m_scale + data.m_worldX) / m_offset) * m_offset;
    tempTrack.y = ((long)((float)m_mouse.MouseY() / data.m_scale + data.m_worldY) / m_offset) * m_offset;

    for(int i = 0; i < data.m_track.size();++i){

      //is there track directly under this "root" tile?
      if(bRemoved  == false && data.m_track[i].x == tempTrack.x && data.m_track[i].y == tempTrack.y &&
        (data.m_track[i].profileID == 21 || data.m_track[i].profileID == 22 || data.m_track[i].profileID == 94)){
        data.m_track.erase(data.m_track.begin() + i, data.m_track.begin() + i + 1);
        keyTime = 0;
        bRemoved = true;
        break;
      }
      //maybe the click is inside a 4x4 grid containing a root.
      else if(bRemoved == false && 
        tempTrack.x >= data.m_track[i].x &&
        tempTrack.x <  data.m_track[i].x + m_offset * 4 &&
        tempTrack.y >= data.m_track[i].y &&
        tempTrack.y <  data.m_track[i].y + m_offset * 4){
        if(data.m_track[i].profileID > 16 && data.m_track[i].profileID < 21){
          data.m_track.erase(data.m_track.begin() + i, data.m_track.begin() + i + 1);
          keyTime = 0;
          bRemoved = true;
          break;
        }        
      }

      if(bRemoved == true){
        keyTime = 0;
        break;
      }
    }//for
  }

  //needed for checking teams
  //**************************
  double lastTime = 0;
  double timeAmount = 0.08;
  int lastUniqueID = -1;
  int nextTrack = -1;
  int nextTrack1 = -1;
  int nextTrack2 = -1;

  //update status of jobs by checking unique ID numbers
  //*******************************************************
  for(int i = 0; i < 10; ++i){
    if(data.m_lastTrack[i].complete == false){//this is an assigned section of construction
      for(int j = 0; j < data.m_track.size(); ++j){
        if(data.m_track[j].uniqueID == data.m_lastTrack[i].uniqueID){
          data.m_lastTrack[i].complete = data.m_track[j].complete;
          break;
        }
      }//for j
    }
  }//for i

  //determine number of free teams and next available 3 numbers
  //*****************************************************
  data.m_freeTeams = 0;
  for(int i = 0; i < 10; ++i){
    if(data.m_lastTrack[i].complete == true){
      if(data.m_freeTeams == 0)
        nextTrack = i;
      else if(data.m_freeTeams == 1)
        nextTrack1 = i;
      else if(data.m_freeTeams == 2)
        nextTrack2 = i;

      data.m_freeTeams++;
      //data.m_lastTrack[i].complete = false;
      //break;
    }
  }
 
  //click on map to select new position
  //******************************************
  if(m_bSelectMap == true && data.m_bLeftMouseDown == true && keyTime > .2){
      m_bSelectMap = false;
      data.m_scale = m_lastScale;
      data.m_worldX = (float)(m_mouse.MouseX() - 304)/720.0f * data.m_worldRight - 512/ data.m_scale;
      data.m_worldY = (float)(m_mouse.MouseY() - 47)/720.0f * data.m_worldBottom -384/ data.m_scale;
      
      keyTime = 0;
  }

  //undo icon
  //**********
  if(data.m_bLeftMouseDown == true &&  m_bUndo == true && keyTime > keyTimeLimit && m_bReset == true){
        data.m_track.pop_back();
        keyTime = 0;
    }

  //save level to file
  //*******************
  else if(m_bSave == true && data.m_bLeftMouseDown == true && keyTime > keyTimeLimit/2 && m_bReset == true){
    data.SaveLevelFile(1);
    keyTime = 0;
  }

  //exit 
  //******
  else if(data.m_bLeftMouseDown == true &&  m_bExit == true && keyTime > keyTimeLimit && m_bReset == true){
    m_event = EVENT_GO_MAIN;  
  }

  //zoom
  //**********
  else if(data.m_bLeftMouseDown == true && m_bZoom == true && keyTime > keyTimeLimit/2  && m_bReset == true){
    long centerX = data.m_worldX + 512 / data.m_scale;
    long centerY = data.m_worldY + 384 / data.m_scale;
    if(m_mouse.MouseX() < m_zoomX + 120){ //zoom in +++
      data.m_scale = 0.125;
      data.m_worldX = centerX - 512 / data.m_scale;
      data.m_worldY = centerY - 384 / data.m_scale;
    }
    else if(m_mouse.MouseX() < m_zoomX + 144){ //zoom in +++
      data.m_scale = 0.25;
      data.m_worldX = centerX - 512 / data.m_scale;
      data.m_worldY = centerY - 384 / data.m_scale;
    }
    else if(m_mouse.MouseX() < m_zoomX + 167){ //zoom in +++
      data.m_scale = 0.5;
      data.m_worldX = centerX - 512 / data.m_scale;
      data.m_worldY = centerY - 384 / data.m_scale;
    }
    else{
      data.m_scale = 1.0;
      data.m_worldX = centerX - 512 / data.m_scale;
      data.m_worldY = centerY - 384 / data.m_scale;
    }
    keyTime = 0;
  }

  //map select icon
  //*********************
  else if(data.m_bLeftMouseDown == true &&  m_bMapIcon == true && keyTime > .3 && m_bReset == true){
    if(m_bSelectMap == false){
      m_bSelectMap = true;
      m_lastScale = data.m_scale;
      data.m_lastWorldLeft = data.m_worldX + 512 / data.m_scale;
      data.m_lastWorldTop = data.m_worldY + 384 / data.m_scale;
      data.m_scale = 0.03;//0.04;
    }
    else{
      m_bSelectMap = false;
      data.m_scale = m_lastScale;
      data.m_worldX = data.m_lastWorldLeft - 512 / data.m_scale;
      data.m_worldY = data.m_lastWorldTop - 384 / data.m_scale;      
    }
    keyTime = 0;
    }

  //click on watch to change time scale
  //************************************
  else if(data.m_bLeftMouseDown == true &&  m_bWatch == true && keyTime > 0.2 && m_bReset == true){
    double timeScale = data.m_gameClock.GetTimeScale();
    timeScale *= 2;
    if(timeScale > 16)
      timeScale = 1;
    data.m_gameClock.SetTimeScale(timeScale);     
    keyTime = 0;
  }
        
  //left click to pick initial train track route
  //***********************************************
  else if(m_bWorld == true && data.m_bLeftMouseDown == true && m_bReset == true && keyTime > keyTimeLimit/2){

    m_initX = ((long)((float)m_mouse.MouseX() / data.m_scale + data.m_worldX) / m_offset) * m_offset;
    m_initY = ((long)((float)m_mouse.MouseY() / data.m_scale + data.m_worldY) / m_offset) * m_offset;
    m_bReset = false;
  }    
  //draw train track after left button is released
  //add temporary train tracks to main track
  //************************************************
  else if(m_bSelectMap == false && data.m_bLeftMouseDown == false && m_bReset == false && keyTime > keyTimeLimit/4){
    double lastTime = 0;
    double timeAmount = 0.08;
    int lastUniqueID = -1;
    int nextTrack = -1;
    int nextTrack1 = -1;
    int nextTrack2 = -1;

    //update status of jobs by checking unique ID numbers
    for(int i = 0; i < 10; ++i){
      if(data.m_lastTrack[i].complete == false){//this is an assigned section of construction
        for(int j = 0; j < data.m_track.size(); ++j){
          if(data.m_track[j].uniqueID == data.m_lastTrack[i].uniqueID){
            data.m_lastTrack[i].complete = data.m_track[j].complete;
            break;
          }
        }//for j
      }
    }//for i

    //determine number of free teams and next available 3 numbers
    data.m_freeTeams = 0;
    for(int i = 0; i < 10; ++i){
      if(data.m_lastTrack[i].complete == true){
        if(data.m_freeTeams == 0)
          nextTrack = i;
        else if(data.m_freeTeams == 1)
          nextTrack1 = i;
        else if(data.m_freeTeams == 2)
          nextTrack2 = i;

        data.m_freeTeams++;
      }
    }

    //if no free teams bail out on this
    if(nextTrack > -1){
      double rockTime = cfg.RockTime;
      double waterTime = cfg.WaterTime;
      for(int i = 0; i < m_show.size(); ++i){
        float terrain = 1; //level ground
        bool bTerrainFound = false;
        int width = 62;
        int height = 62;
        if(m_show[i].id >= 17 && m_show[i].id <= 20){ //curved section
          width *= 4;
          height *= 4;
        }
        //is the track over the water?
        for(int j = 0; j < data.m_water.size(); ++j){
          if(
            (m_show[i].x >= data.m_water[j].x && m_show[i].x <= data.m_water[j].x + 248 &&
             m_show[i].y >= data.m_water[j].y && m_show[i].y <= data.m_water[j].y + 248) ||
            (m_show[i].x + width >= data.m_water[j].x && m_show[i].x + width <= data.m_water[j].x + 248 &&
             m_show[i].y >= data.m_water[j].y && m_show[i].y <= data.m_water[j].y + 248) ||
            (m_show[i].x >= data.m_water[j].x && m_show[i].x <= data.m_water[j].x + 248 &&
             m_show[i].y + height >= data.m_water[j].y && m_show[i].y + height <= data.m_water[j].y + 248) ||
            (m_show[i].x + width>= data.m_water[j].x && m_show[i].x + width <= data.m_water[j].x + 248 &&
             m_show[i].y + height >= data.m_water[j].y && m_show[i].y + height <= data.m_water[j].y + 248)){
              terrain = waterTime;
              bTerrainFound = true;
              break;
          }
        }

        //is the track over rock
        if(bTerrainFound == false){
          for(int j = 0; j < data.m_hill.size(); ++j){
            if(
            (m_show[i].x >= data.m_hill[j].x && m_show[i].x <= data.m_hill[j].x + 496 &&
             m_show[i].y >= data.m_hill[j].y && m_show[i].y <= data.m_hill[j].y + 496) ||
            (m_show[i].x + width >= data.m_hill[j].x && m_show[i].x + width <= data.m_hill[j].x + 496 &&
             m_show[i].y >= data.m_hill[j].y && m_show[i].y <= data.m_hill[j].y + 496) ||
            (m_show[i].x >= data.m_hill[j].x && m_show[i].x <= data.m_hill[j].x + 496 &&
             m_show[i].y + height >= data.m_hill[j].y && m_show[i].y + height <= data.m_hill[j].y + 496) ||
            (m_show[i].x + width>= data.m_hill[j].x && m_show[i].x + width <= data.m_hill[j].x + 496 &&
             m_show[i].y + height >= data.m_hill[j].y && m_show[i].y + height <= data.m_hill[j].y + 496)){
                terrain = rockTime;
                bTerrainFound = true;
                break;
            }
          }
        }

        //horizontal track
        if(m_show[i].id == 21){
          tempTrack.profileID = 21;
          tempTrack.ballastID = 50;
          tempTrack.tieID = 51;
          tempTrack.railID = 52;
          tempTrack.spriteID = tempTrack.profileID;
          tempTrack.x = m_show[i].x;
          tempTrack.y = m_show[i].y;        
          tempTrack.m_countDown = lastTime;
          lastTime += (timeAmount * terrain);
        }      
        //vertical track
        else if(m_show[i].id == 22){
          tempTrack.profileID = 22;  
          tempTrack.ballastID = 53;
          tempTrack.tieID = 54;
          tempTrack.railID = 55;
          tempTrack.spriteID = tempTrack.profileID;
          tempTrack.x = m_show[i].x;
          tempTrack.y = m_show[i].y;
          tempTrack.m_countDown = lastTime;
          lastTime += (timeAmount * terrain);
        }
        //top-left corner
        else if(m_show[i].id == 17){
          tempTrack.profileID = 17;  
          tempTrack.ballastID = 62;
          tempTrack.tieID = 63;
          tempTrack.railID = 64;
          tempTrack.spriteID = tempTrack.profileID;
          tempTrack.x = m_show[i].x;
          tempTrack.y = m_show[i].y;
          tempTrack.m_countDown = lastTime;
          lastTime += (timeAmount * terrain) * 4;
        }
        //top-right corner
        else if(m_show[i].id == 18){
          tempTrack.profileID = 18;  
          tempTrack.ballastID = 56;
          tempTrack.tieID = 57;
          tempTrack.railID = 58;
          tempTrack.spriteID = tempTrack.profileID;
          tempTrack.x = m_show[i].x;
          tempTrack.y = m_show[i].y;
          tempTrack.m_countDown = lastTime;
          lastTime += (timeAmount * terrain) * 4;
        }
        //bottom-left corner
        else if(m_show[i].id == 19){
          tempTrack.profileID = 19;  
          tempTrack.ballastID = 65;
          tempTrack.tieID = 66;
          tempTrack.railID = 67;
          tempTrack.spriteID = tempTrack.profileID;
          tempTrack.x = m_show[i].x;
          tempTrack.y = m_show[i].y;
          tempTrack.m_countDown = lastTime;
          lastTime += (timeAmount * terrain) * 4;
        }
        //bottom-right corner
        else if(m_show[i].id == 20){
          tempTrack.profileID = 20;  
          tempTrack.ballastID = 59;
          tempTrack.tieID = 60;
          tempTrack.railID = 61;
          tempTrack.spriteID = tempTrack.profileID;
          tempTrack.x = m_show[i].x;
          tempTrack.y = m_show[i].y;
          tempTrack.m_countDown = lastTime;
          lastTime += (timeAmount * terrain) * 4;
        }

        //make sure a track is not under the track
        bool bFound = false;
        bool bIntersection = false;
        int intersection = -1;

        for(int i = 0; i < data.m_track.size();++i){
          if(data.m_track[i].x == tempTrack.x && data.m_track[i].y == tempTrack.y){
            bFound = true;
            if(data.m_track[i].profileID == 21 && tempTrack.profileID == 22 ||
              data.m_track[i].profileID == 22 && tempTrack.profileID == 21){
              bIntersection = true;
              intersection = i;
              }
            break;
          }
        }

        //assign unique number to each track section
        data.m_lastUniqueID++;
        tempTrack.uniqueID = data.m_lastUniqueID;

        //add section to m_track vector
        if(bFound == false && tempTrack.profileID > -1){
          keyTime = 0;
          data.m_track.push_back(tempTrack);
        }
        //may be an intersection
        else{
          if(bIntersection == true){
            tempTrack.profileID = 94;
            tempTrack.ballastID = 94;
            tempTrack.tieID = 95;
            tempTrack.railID = 93;
            tempTrack.spriteID = tempTrack.profileID;
            data.m_track[intersection] = tempTrack; 
          }
        }
        //update team information
        data.m_lastTrack[nextTrack].uniqueID = tempTrack.uniqueID;
        data.m_lastTrack[nextTrack].complete = false;

        lastUniqueID = data.m_lastUniqueID;
      }//for
    }//if
    m_show.clear();
    m_bReset = true;
  }//end section

  //determines track profiles
  if(m_bReset == false){
    PROFILE profile;
    long dx;
    long dy;
    
    m_show.clear();
    m_currX = ((long)((float)m_mouse.MouseX() / data.m_scale + data.m_worldX) / m_offset) * m_offset;
    m_currY = ((long)((float)m_mouse.MouseY() / data.m_scale + data.m_worldY) / m_offset) * m_offset;
    long mx = (long)((float)m_mouse.MouseX() / data.m_scale + data.m_worldX);
    long my = (long)((float)m_mouse.MouseY() / data.m_scale + data.m_worldY);

    //allows two possible tracks to be laid
    bool bUpper;
    if(my > m_currY + m_offset/2)
      bUpper = false;
    else
      bUpper = true;

    //determines correct quadrant
    dx = (m_currX - m_initX)/m_offset;
    dy = (m_currY - m_initY)/m_offset;

    //RIGHT SIDE
    if(dx > 0){
      if(dy > 0){
        //bottom right quadrant...add a curve
        if(dx > 2 && dy > 2){
          //upper == true
          if(bUpper == true){
            for(int i = 0; i < abs(dx) - 3; ++i){
                profile.id = 21;
                profile.x = m_initX + (i * m_offset);
                profile.y = m_initY;
                m_show.push_back(profile);
            }
            profile.id = 18;
            profile.x = m_initX + ((dx - 3) * m_offset);
            profile.y = m_initY;
            m_show.push_back(profile);

            for(int i = 4; i < abs(dy) + 1; ++i){
                profile.id = 22;
                profile.x = m_currX;// - m_offset;
                profile.y = m_initY + (i * m_offset); 
                m_show.push_back(profile);
            }
          }
          //upper == false
          else{
            for(int i = 0; i < abs(dy) - 3; ++i){
                profile.id = 22;
                profile.x = m_initX;
                profile.y = m_initY + (i * m_offset); 
                m_show.push_back(profile);
            }
            profile.id = 19;
            profile.x = m_initX;
            profile.y = m_currY - (3 * m_offset);
            m_show.push_back(profile);
            for(int i = 4; i < abs(dx) + 1; ++i){
                profile.id = 21;
                profile.x = m_initX + (i * m_offset);
                profile.y = m_currY;
                m_show.push_back(profile);
            }
          }
        }//if(dx > 2 && dy > 2
        //not a curved piece
        else{
          if(abs(dx) >= abs(dy)){
            for(int i = 0; i < abs(dx); ++i){
                profile.id = 21;
                profile.x = m_initX + (i * m_offset);
                profile.y = m_initY; 
                m_show.push_back(profile);
            }
          }
          else{
            for(int i = 0; i < abs(dy); ++i){
                profile.id = 22;
                profile.x = m_initX;
                profile.y = m_initY + (i * m_offset); 
                m_show.push_back(profile);
            }  
          }
        }
      }//if(dy > 0)
      else if(dy < 0){
        //uppper right quadrant...add a curve
        if(dx > 2 && dy < - 2){
          //upper == true
          if(bUpper == true){
            for(int i = 0; i < abs(dy) - 3; ++i){
                profile.id = 22;
                profile.x = m_initX;// - m_offset;
                profile.y = m_initY - (i * m_offset); 
                m_show.push_back(profile);
            }
            profile.id = 17;
            profile.x = m_initX;// + m_offset;// + ((dx - 4) * m_offset);
            profile.y = m_currY;// + m_offset ;
            m_show.push_back(profile);
            for(int i = 4; i < abs(dx) + 1; ++i){
                profile.id = 21;
                profile.x = m_initX + (i * m_offset);
                profile.y = m_currY;
                m_show.push_back(profile);
            }
          }
          //upper == false
          else{
            for(int i = 0; i < abs(dx) - 3; ++i){
                profile.id = 21;
                profile.x = m_initX + (i * m_offset);
                profile.y = m_initY;
                m_show.push_back(profile);
            }
            profile.id = 20;
            profile.x = m_currX - (3 * m_offset);
            profile.y = m_initY - (3 * m_offset);
            m_show.push_back(profile);
            for(int i = 4; i < abs(dy) + 1; ++i){
                profile.id = 22;
                profile.x = m_currX;// - m_offset;
                profile.y = m_initY - (i * m_offset); 
                m_show.push_back(profile);
            }
          }
        }
        //not a curved piece
        else{
          if(abs(dx) >= abs(dy)){
            for(int i = 0; i < abs(dx); ++i){
                profile.id = 21;
                profile.x = m_initX + (i * m_offset);
                profile.y = m_initY; 
                m_show.push_back(profile);
            }
          }
          else{
            for(int i = 0; i < abs(dy); ++i){
                profile.id = 22;
                profile.x = m_initX;
                profile.y = m_initY - (i * m_offset); 
                m_show.push_back(profile);
            }  
          }
        }
      }
      else{
        for(int i = 0; i < abs(dx); ++i){
            profile.id = 21;
            profile.x = m_initX + (i * m_offset);
            profile.y = m_initY; 
            m_show.push_back(profile);
        }
      }
    }
    //LEFT side
    else if(dx < 0){
      if(dy > 0){
        //bottom left quadrant...add a curve
        if(dx < -2 && dy > 2){
          //upper == false
          if(bUpper == false){
            for(int i = 0; i < abs(dy)- 3; ++i){
                profile.id = 22;
                profile.x = m_initX;
                profile.y = m_initY + (i * m_offset); 
                m_show.push_back(profile);
            }
            profile.id = 20;
            profile.x = m_initX - (3 * m_offset);
            profile.y = m_currY - (3 * m_offset);
            m_show.push_back(profile);
            for(int i = 4; i < abs(dx) + 1; ++i){
                profile.id = 21;
                profile.x = m_initX - (i * m_offset);
                profile.y = m_currY;
                m_show.push_back(profile);
            }            
          }
          //upper == true
          else{
            for(int i = 0; i < abs(dx) - 3; ++i){
                profile.id = 21;
                profile.x = m_initX - (i * m_offset);
                profile.y = m_initY;
                m_show.push_back(profile);
            }
            profile.id = 17;
            profile.x = m_currX;
            profile.y = m_initY; //currY - (3 * m_offset);
            m_show.push_back(profile);
            for(int i = 4; i < abs(dy) + 1; ++i){
                profile.id = 22;
                profile.x = m_currX;
                profile.y = m_initY + (i * m_offset); 
                m_show.push_back(profile);
            }
          }
        }
        //not a curved piece
        else{
          if(abs(dx) >= abs(dy)){
            for(int i = 0; i < abs(dx); ++i){
                profile.id = 21;
                profile.x = m_initX - (i * m_offset);
                profile.y = m_initY; 
                m_show.push_back(profile);
            }
          }
          else{
            for(int i = 0; i < abs(dy); ++i){
                profile.id = 22;
                profile.x = m_initX;
                profile.y = m_initY + (i * m_offset); 
                m_show.push_back(profile);
            }  
          }
        }
      }
      else if(dy < 0){
        //upper left quadrant...add a curve
        if(dx < -2 && dy < - 2){
          //upper == true
          if(bUpper == true){
            for(int i = 0; i < abs(dy) - 3; ++i){
                profile.id = 22;
                profile.x = m_initX;
                profile.y = m_initY - (i * m_offset); 
                m_show.push_back(profile);
            }
            profile.id = 18;
            profile.x = m_initX - (3 * m_offset);
            profile.y = m_currY;
            m_show.push_back(profile);
            for(int i = abs(dx) - 4; i > -1; --i){
                profile.id = 21;
                profile.x = m_currX + (i * m_offset);
                profile.y = m_currY;
                m_show.push_back(profile);
            }
          }
          //upper == false
          else{
            for(int i = 0; i < abs(dx) - 3; ++i){
                profile.id = 21;
                profile.x = m_initX - (i * m_offset);
                profile.y = m_initY;
                m_show.push_back(profile);
            }
            profile.id = 19;
            profile.x = m_currX;
            profile.y = m_initY - (3 * m_offset);
            m_show.push_back(profile);
            for(int i = 4; i < abs(dy) + 1; ++i){
                profile.id = 22;
                profile.x = m_currX;
                profile.y = m_initY - (i * m_offset); 
                m_show.push_back(profile);
            }
          }
        }
        //not a curved piece
        else{
          if(abs(dx) >= abs(dy)){
            for(int i = 0; i < abs(dx); ++i){
                profile.id = 21;
                profile.x = m_initX - (i * m_offset);
                profile.y = m_initY; 
                m_show.push_back(profile);
            }
          }
          else{
            for(int i = 0; i < abs(dy); ++i){
                profile.id = 22;
                profile.x = m_initX;
                profile.y = m_initY - (i * m_offset); 
                m_show.push_back(profile);
            }  
          }
        }
      }
      else{
        for(int i = 0; i < abs(dx); ++i){
            profile.id = 21;
            profile.x = m_initX - (i * m_offset);
            profile.y = m_initY; 
            m_show.push_back(profile);
        }  
      }
    }
    //VERTICAL TRACKS
    else if(dx == 0){
      if(dy > 0){
        for(int i = 0; i < abs(dy); ++i){
            profile.id = 22;
            profile.x = m_initX;
            profile.y = m_initY + (i * m_offset); 
            m_show.push_back(profile);
        }
      }
      else if(dy < 0){
        for(int i = 0; i < abs(dy); ++i){
            profile.id = 22;
            profile.x = m_initX;
            profile.y = m_initY - (i * m_offset); 
            m_show.push_back(profile);
        }  
      }
    }
  }

  //perform updates...not allowed if game is paused
  //***************************************************************
  m_train.SetVelocity(m_velocity);// * data.m_gameClock.GetTimeScale());
  if(data.m_bGameWon == true){
    m_train.Update(timeDifference  * data.m_gameClock.GetTimeScale());
    data.m_worldX = m_train.GetPosX() - 512/data.m_scale;
    data.m_worldY = m_train.GetPosY() - 350/data.m_scale;

    if(m_train.GetWaypoint() > m_train.GetNumberWaypoints() - 2)//bail out of game after victory
       m_event = EVENT_GO_MAIN;  
  }

 //m_train2.SetVelocity(m_velocity);// * data.m_gameClock.GetTimeScale());
 //m_train2.Update(timeDifference  * data.m_gameClock.GetTimeScale());

  //track animation
  double scale = data.m_gameClock.GetTimeScale() / 12;  //was 8
  int numberComplete = 0;
  
  for(int i = 0; i < data.m_track.size(); ++i){
    data.m_track[i].Update(timeDifference, scale);
    if(data.m_track[i].complete == true)
      numberComplete++;
  }
//  CAudioManager *pAudio = CAudioManager::Instance();
  if(numberComplete > data.m_track.size() - 1 && data.m_bRailroadComplete == true){
    //pAudio->PlaySoundClip(5, 1);
    data.m_bGameWon = true;
    m_bCelebrate = true;
    data.m_gameClock.SetTimeScale(2);
    data.m_scale = 0.25;
    //m_train.Reset();
    //m_train.SetVelocity(60);
    //data.m_gameClock.Pause(true);
  }
    
  //update cloud motion
  static double cloudTime;
  double cloudSpeed = 200;
  cloudTime += timeDifference;

  if(cloudTime > 0.02){
    for(int i = 0; i < data.m_cloud.size(); ++i){
      //move right
      data.m_cloud[i].x += data.m_cloud[i].velocity * timeDifference * data.m_gameClock.GetTimeScale();
      if(data.m_cloud[i].x > data.m_worldRight)
        data.m_cloud[i].x = data.m_worldLeft - 1024;
    }
    cloudTime = 0;
  }

  for(unsigned int i=0; i< m_TE.size(); i++){
    if (m_TE[i].event == m_event){
      return m_TE[i].p_gso;
    }
  }
  return 0;
}

//***************************************************************
//                         R E N D E R 
//***************************************************************
void CGameStatePlay1::Render(CGraphics &con,  CGameData &data, CConfigData &cfg){
  //::ShowCursor(true);
  CLog *pLog = CLog::Instance();
  static bLostDevice = false; 

  if(m_bRenderReady == false)
    return;

  //test to see if graphic device is lost, if so, deal with it
  //**********************************************************
  HRESULT hr;
  if(FAILED(hr = con.m_d3ddev->TestCooperativeLevel())){
    
    //device is lost...cannot be reset...exit Render() function
    if(hr == D3DERR_DEVICELOST){
      if(bLostDevice == false){
        bLostDevice = true;
        con.DeleteLost();    //delete one time per failure
      }
      return;
    }

    //device was lost...but now can be RESET
    else if( hr == D3DERR_DEVICENOTRESET ){
      hr = con.m_d3ddev->Reset(&con.GetDPP());      
      if(SUCCEEDED(hr)){
        con.LoadAssetFile(cfg.GamePlayAssetFile);
        con.LoadAssetFile(cfg.FrameworkAssetFile);     
        con.ReloadLost();
        bLostDevice = false;
      }
      return;
    }
  }
  
  // clear the window to a deep blue
  con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(data.m_screenColorRed, data.m_screenColorGreen, data.m_screenColorBlue), 1.0f, 0);//0, 111, 255
  con.m_d3ddev->BeginScene();    // begins the 3D scene
  con.BeginRender();
  GRAPHIC_IMAGE gi;

  if(m_bSelectMap == true){
    gi = con.GetSprite(115);
    con.RenderGraphicModulate(gi, 0, 0, 255, 255, 255);
    m_maskAlpha = 255;
  }  
  else
    m_maskAlpha = 0;

  //render the default ground
  if(data.m_landscapeTexture > -1){
    gi = con.GetSprite(data.m_landscapeTexture);//1);
    long x = data.m_worldX % (long)(gi.width);
    long y = data.m_worldY % (long)(gi.height);
    gi.scale = data.m_scale;
    gi.alpha = 255;//m_maskAlpha;
    x = x * -1;
    y = y * -1;
    int maxI = 0;
    int maxJ = 0;
    if(gi.scale == 2){
      maxI = 3;
      maxJ = 3;
    }
    else if(gi.scale == 1){
      maxI = 5;
      maxJ = 4;
    }
    else if(gi.scale == 0.5){
      maxI = 9;
      maxJ = 8;
    }
    else if(gi.scale == 0.25){
      maxI = 17;
      maxJ = 13;
    }
    else if(gi.scale == 0.125){
      maxI = 33;
      maxJ = 26;
    }
    else{
      //maxI = (int)(5.0f/gi.scale);
      //maxJ = (int)(3.0f/gi.scale);
    }

    for(int j = -1; j < maxJ; ++ j){
      for(int i = -1; i < maxI; ++i)    
        con.RenderGraphicModulate(gi, gi.scale * (x + gi.width * i),(y + gi.height * j) * gi.scale, 255, 255, 255);
    }
  }

  //draw a hill
  //******************************************
  for(int i = 0; i < data.m_hill.size(); ++i){
    if(data.m_hill[i].x > data.m_worldX - (8 * m_offset/data.m_scale) && data.m_hill[i].x < data.m_worldX + 1024/data.m_scale &&
      data.m_hill[i].y > data.m_worldY - (8 * m_offset/data.m_scale) && data.m_hill[i].y < data.m_worldY + 768/data.m_scale){
      gi = con.GetSprite(data.m_hill[i].spriteID);
      gi.angle = data.m_hill[i].angle;
      gi.screenScale = data.m_scale;
      gi.scale = data.m_hill[i].scale;
      if(m_bSelectMap == false)
        con.RenderGraphicModulate(gi, (data.m_hill[i].x - data.m_worldX) * data.m_scale,
                                      (data.m_hill[i].y - data.m_worldY) * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);//55, 55, 55);
      else
        con.RenderGraphicModulate(gi, (data.m_hill[i].x - data.m_worldX) * data.m_scale,
                                      (data.m_hill[i].y - data.m_worldY) * data.m_scale,  153, 102, 51);//55, 55, 55);

    }
  }

  //render water
  for(int i = 0; i < data.m_water.size();++i){
    if(data.m_water[i].x > data.m_worldX - (8 * m_offset/data.m_scale) && data.m_water[i].x < data.m_worldX + 1024/data.m_scale &&
      data.m_water[i].y > data.m_worldY - (8 * m_offset/data.m_scale) && data.m_water[i].y < data.m_worldY + 768/data.m_scale){
    gi = con.GetSprite(data.m_water[i].spriteID);
    gi.scale = data.m_scale;
    if(m_bSelectMap == false)
      con.RenderGraphicModulate(gi, (data.m_water[i].x - data.m_worldX)  * data.m_scale, (data.m_water[i].y - data.m_worldY)  * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);
    else
      con.RenderGraphicModulate(gi, (data.m_water[i].x - data.m_worldX)  * data.m_scale, (data.m_water[i].y - data.m_worldY)  * data.m_scale, 204, 255, 255);
    }
  }

  //render building
  for(int i = 0; i < data.m_building.size();++i){
    if(data.m_building[i].x > data.m_worldX - (8 * m_offset/data.m_scale) && data.m_building[i].x < data.m_worldX + 1024/data.m_scale &&
      data.m_building[i].y > data.m_worldY - (8 * m_offset/data.m_scale) && data.m_building[i].y < data.m_worldY + 768/data.m_scale){
    //shadow
    gi = con.GetSprite(data.m_building[i].spriteID + 4);
    gi.scale = data.m_scale;
    gi.alpha = 127;
    con.RenderGraphicModulate(gi, (data.m_building[i].x - data.m_worldX)  * data.m_scale + (8 * data.m_scale), (data.m_building[i].y - data.m_worldY)  * data.m_scale - (8 * data.m_scale) , 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);
    //building
    gi = con.GetSprite(data.m_building[i].spriteID);
    gi.scale = data.m_scale;
    gi.alpha = 255;
    con.RenderGraphicModulate(gi, (data.m_building[i].x - data.m_worldX)  * data.m_scale, (data.m_building[i].y - data.m_worldY)  * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);

    }
  }

  //render starting and ending points
  gi = con.GetSprite(68);
  gi.scale = data.m_scale;
  //gi.alpha = m_maskAlpha;
  con.RenderGraphicModulate(gi, (data.m_startX - data.m_worldX)  * data.m_scale, (data.m_startY - data.m_worldY)  * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);
  gi = con.GetSprite(69);
  gi.scale = data.m_scale;
  con.RenderGraphicModulate(gi, (data.m_endX - data.m_worldX)  * data.m_scale, (data.m_endY - data.m_worldY)  * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);

  //render all tracks
  //*******************************************
  for(int i = 0; i < data.m_track.size(); ++i){
    if(data.m_track[i].x > data.m_worldX - (2 * m_offset/data.m_scale) && data.m_track[i].x < data.m_worldX + 1024/data.m_scale &&
      data.m_track[i].y > data.m_worldY - (2 * m_offset/data.m_scale) && data.m_track[i].y < data.m_worldY + 768/data.m_scale){
        gi = con.GetSprite(data.m_track[i].spriteID);
        gi.scale = data.m_scale;
        con.RenderGraphicModulate(gi,
                          (data.m_track[i].x - data.m_worldX) * data.m_scale,
                          (data.m_track[i].y - data.m_worldY) * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);
      }
  }

  //render the train 
  //**************************
  gi = con.GetSprite(101);
  gi.angle = m_train.GetAngle() + 1.57;
  gi.screenScale = data.m_scale;
  gi.alpha = 127;
  //shadow
  con.RenderGraphicModulate(gi,
                      (m_train.GetPosX() + 8 - data.m_worldX - gi.width/2) * data.m_scale,
                      (m_train.GetPosY() - 8 - data.m_worldY - gi.height/2) * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);
  //render train actual
  gi = con.GetSprite(100);
  gi.angle = m_train.GetAngle() + 1.57;
  gi.screenScale = data.m_scale;
  if(m_bSelectMap == false){
    gi.scale = 1;
    con.RenderGraphicModulate(gi,
                      (m_train.GetPosX() - data.m_worldX - gi.width/2) * data.m_scale,
                      (m_train.GetPosY() - data.m_worldY - gi.height/2) * data.m_scale, 255,255, 255);
  }
  /*
  else{
    gi.scale = 3;
    con.RenderGraphicModulate(gi,
                      (m_train.GetPosX() - data.m_worldX - 1.5 * gi.width) * data.m_scale,
                      (m_train.GetPosY() - data.m_worldY - 1.5 * gi.height) * data.m_scale, 255,255, 255);
  }*/

  /*
  //2nd car
  gi = con.GetSprite(101);
  gi.angle = m_train2.GetAngle() + 1.57;
  gi.screenScale = data.m_scale;
  gi.alpha = 128;//m_maskAlpha > 90 ? 255 - m_maskAlpha : 165;
  con.RenderGraphicModulate(gi,
                      (m_train2.GetPosX() + 5 - data.m_worldX - gi.width/2) * data.m_scale,
                      (m_train2.GetPosY() - 5 - data.m_worldY - gi.height/2) * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);
  gi = con.GetSprite(100);
  gi.angle = m_train2.GetAngle() + 1.57;
  gi.screenScale = data.m_scale;
  gi.alpha = 255;
  con.RenderGraphicModulate(gi,
                      (m_train2.GetPosX() - data.m_worldX - gi.width/2) * data.m_scale,
                      (m_train2.GetPosY() - data.m_worldY - gi.height/2) * data.m_scale, 255,255, 255);
  */

  //render trees
  for(int i = 0; i < data.m_tree.size(); ++i){
    if((data.m_tree[i].x - data.m_worldX) * data.m_scale > 0 && (data.m_tree[i].x - data.m_worldX) * data.m_scale < cfg.ScreenWidth &&
      (data.m_tree[i].y - data.m_worldY) * data.m_scale > 0 && (data.m_tree[i].y - data.m_worldY) * data.m_scale < cfg.ScreenHeight){
       gi = con.GetSprite(data.m_tree[i].spriteID);
       gi.angle = data.m_tree[i].angle;
       gi.screenScale = data.m_scale;
       gi.scale = data.m_tree[i].scale;
       con.RenderGraphicModulate(gi, (data.m_tree[i].x - data.m_worldX) * data.m_scale,
                                     (data.m_tree[i].y - data.m_worldY) * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);
    }
  }

/*
  //lantern light source
  //if(m_lightOn == true){
    gi = con.GetSprite(11);
    gi.alpha = m_lightAlpha; //m_maskAlpha - 255;                //90;
    gi.scale = 1.03;
    con.RenderGraphicModulate(gi, m_maskX + 325, m_maskY + 60, 255, 255, 0);
  //}
  //black mask with hole...covers screen
  gi = con.GetSprite(10);
  gi.alpha = m_maskAlpha;
  con.RenderGraphicModulate(gi, m_maskX, m_maskY, 255, 255, 255);
*/

  //render clouds
  //**********************************
  if(m_bSelectMap == false){
    for(int i = 0; i < data.m_cloud.size(); ++i){
      if(data.m_cloud[i].x > data.m_worldX - (3 * 450.0f/data.m_scale) && data.m_cloud[i].x < data.m_worldX + (1024 + 3 * 450)/data.m_scale &&
        data.m_cloud[i].y > data.m_worldY - (3 * 350.0f/data.m_scale) && data.m_cloud[i].y < data.m_worldY + (768 + 3 * 350)/data.m_scale){
          gi = con.GetSprite(data.m_cloud[i].spriteID);
          gi.angle = data.m_cloud[i].angle;
          gi.screenScale = data.m_scale;
          //shadow
          con.RenderGraphicModulate(gi, (data.m_cloud[i].x - data.m_worldX + 50)  * data.m_scale, (data.m_cloud[i].y - data.m_worldY - 50)  * data.m_scale, 0, 0, 0);// 255, 255, 255);//255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);//55, 55, 55);
          
          //actual cloud
          gi.alpha = 220;
          con.RenderGraphicModulate(gi, (data.m_cloud[i].x - data.m_worldX) *data.m_scale, (data.m_cloud[i].y - data.m_worldY)  * data.m_scale, 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);//55, 55, 55);
        }
    }
  }
  
  //draw waypoint
  //**************
  gi = con.GetSprite(102);
  gi.screenScale = data.m_scale;

  for(int i = 0; i < m_train.GetNumberWaypoints(); ++i){
    con.RenderGraphicModulate(gi, (m_train.GetWaypointX(i) - data.m_worldX) *data.m_scale - (gi.width * data.m_scale/2), (m_train.GetWaypointY(i) - data.m_worldY)  * data.m_scale - (gi.height * data.m_scale/2), 255 - m_maskAlpha,255 - m_maskAlpha, 255 - m_maskAlpha);//55, 55, 55);
  }

  //render calendar
  //****************************************
  if(m_bSelectMap == false){
    gi = con.GetSprite(43);
    m_bCalendar == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_calendarX, m_calendarY, 255, 255, 255);

    //add calendar marks
    gi = con.GetSprite(49);
    m_bCalendar == true ? gi.alpha = 255 : gi.alpha = 50;
    for(int i = 1; i < data.m_gameClock.GetDay(); ++i){
      if(i < 8)
        con.RenderGraphicModulate(gi, m_calendarX + 3 + (i * 20), m_calendarY + 53, 255, 255, 255);
      else
        con.RenderGraphicModulate(gi, m_calendarX + 3 + ((i - 7) * 20), m_calendarY + 66, 255, 255, 255);
    }
  }

  //render watch
  //****************************************************
  if(m_bSelectMap == false){
    gi = con.GetSprite(44);
    m_bWatch == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_watchX, m_watchY, 255, 255, 255);

    //render time speed on watch faceplate
    double timeScale = data.m_gameClock.GetTimeScale();
    if(timeScale == 1)
      gi = con.GetSprite(103);
    else if(timeScale == 2)
      gi = con.GetSprite(104);
    else if(timeScale == 4)
      gi = con.GetSprite(105);
    else if(timeScale == 8)
      gi = con.GetSprite(106);
    else if(timeScale == 16)
      gi = con.GetSprite(107);
    m_bWatch == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_watchX + 43, m_watchY + 103, 255, 255, 255);

    //draw watch hand
    gi = con.GetSprite(45);
    m_bWatch == true ? gi.alpha = 255 : gi.alpha = 50;
    gi.angle = 6.28 * (float)(data.m_gameClock.GetHour())/12.0f;
    con.RenderGraphicModulate(gi, m_watchX + 26, m_watchY + 68, 255, 255, 255); 
  }

  //render lanterns
  //***********************
  if(m_bSelectMap == false){
    for(int i = 0; i < 10; ++i){
      if(data.m_lastTrack[i].complete == true)
        gi = con.GetSprite(48); //lantern on
      else
        gi = con.GetSprite(47);  //lantern off
      con.RenderGraphicModulate(gi, m_lanternX + i * 38, m_lanternY, 255, 255, 255);
    }
  }

  //render map select icon
  //********************************************8
  if(m_bSelectMap == false){
    gi = con.GetSprite(37); //map is off
    m_bMapIcon == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_selectMapX, m_selectMapY, 255, 255, 255);
  }
  else{
    con.Print("Click on map to return!", F_V20, 20, 370,  255, 255, 0, 255);

  }

  //render exit icon
  //*****************
  if(m_bSelectMap == false){
    gi = con.GetSprite(111);
    m_bExit == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_exitX, m_exitY, 255, 255, 255);
  }

  //render check mark
  //*******************
  if(m_bSelectMap == false){
    gi = con.GetSprite(119);
    m_bCheck  == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_checkX, m_checkY, 255, 255, 255);
  }

  //render sound and muisc icons
  if(m_bSound == true)
     gi = con.GetSprite(162);
  else
    gi = con.GetSprite(164);
  m_bSoundEnabled == true ? gi.alpha = 255 : gi.alpha = 50;
  con.RenderGraphicModulate(gi, m_soundX, m_soundY, 255, 255, 255);
  if(m_bMusic == true)
     gi = con.GetSprite(163);
  else
    gi = con.GetSprite(165);
  m_bMusicEnabled == true ? gi.alpha = 255 : gi.alpha = 50;
  con.RenderGraphicModulate(gi, m_musicX, m_musicY, 255, 255, 255);

  //render zoom icon
  //*********************
  if(m_bSelectMap == false){
    gi = con.GetSprite(70);
    if(data.m_scale == 0.125){
      gi = con.GetSprite(70);    
    }
    else if(data.m_scale == 0.25){
      gi = con.GetSprite(71);
    }
    else if(data.m_scale == 0.5){
      gi = con.GetSprite(72);
    }
    else if(data.m_scale == 1.0){
      gi = con.GetSprite(73);
    }
    m_bZoom == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_zoomX, m_zoomY, 255, 255, 255);
  }

  //render glove if mouse is on edge
  //********************************
  if(m_mouse.MouseX() < 62 && data.m_worldX > 0){
    gi = con.GetSprite(124);
    gi.scale = .5;
    con.RenderGraphicModulate(gi, 5, 346, 255, 255, 255);
  }
  if(m_mouse.MouseX() > 962 && data.m_worldX < data.m_worldRight - cfg.ScreenWidth / data.m_scale){
    gi = con.GetSprite(125);
    gi.scale = .5;
    con.RenderGraphicModulate(gi, 980, 346, 255, 255, 255);
  }
  if(m_mouse.MouseY() < 62 && data.m_worldY > 0){
    gi = con.GetSprite(126);
    gi.scale = .5;
    con.RenderGraphicModulate(gi, 476, 5, 255, 255, 255);
  }
  if(m_mouse.MouseY() > 706 && data.m_worldY < data.m_worldBottom - cfg.ScreenHeight / data.m_scale){
    gi = con.GetSprite(127);
    gi.scale = .5;
    con.RenderGraphicModulate(gi, 476, 726, 255, 255, 255);
  }

  //render undo icon and save icon
  //*********************************
  if(m_bSelectMap == false){
    gi = con.GetSprite(8);
    m_bUndo == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_undoX, m_undoY, 255, 255, 255);
    gi = con.GetSprite(7);
    m_bSave == true ? gi.alpha = 255 : gi.alpha = 50;
    con.RenderGraphicModulate(gi, m_saveX, m_saveY, 255, 255, 255);
  }

  //map - renders red dot on map indicating position

  if(m_bSelectMap == true){

    //player position - red dot
    float posX = 720 *  (float)data.m_lastWorldLeft / (float)(data.m_worldRight);// - 512/m_lastScale;
    float posY = 720 * (float)data.m_lastWorldTop / (float)(data.m_worldBottom);// - 384/m_lastScale;
    gi = con.GetSprite(9);
    con.RenderGraphicModulate(gi, 304 + posX, 47 + posY, 255, 255, 255);
  }

  if(m_bPause == true){
    gi = con.GetSprite(13);
    con.RenderGraphicModulate(gi, 363, 328, 255, 255, 255);      
  }

  //render mouse cursor
  //*************************
  if(m_bWorld == false || m_bSelectMap == true){
    gi = con.GetSprite(108);  
    con.RenderGraphicModulate(gi, m_mouse.MouseX() - gi.width/2,m_mouse.MouseY() - gi.height/2, 255, 255, 255);
  }
  //render mouse square
  else{
    //render profile
    gi = con.GetSprite(110);
    long snapX = m_mouse.MouseX()/ data.m_scale;
    long snapY = m_mouse.MouseY()/ data.m_scale;
    snapX += data.m_worldX;
    snapY += data.m_worldY;
    snapX = (long)(snapX/m_offset) * m_offset;
    snapY = (long)(snapY/m_offset) * m_offset;
    snapX = snapX - data.m_worldX;
    snapY = snapY - data.m_worldY;
    snapX *= data.m_scale;
    snapY *= data.m_scale;
    gi.scale = data.m_scale;   
    con.RenderGraphicModulate(gi, snapX, snapY, 255, 255, 255);//render profile
  }

  //render white shadow train track layout
  //****************************************
  if(m_show.size() > 0){
    for(int i = 0; i < m_show.size(); ++i){
      gi = con.GetSprite(m_show[i].id);
      gi.scale = data.m_scale;   
      con.RenderGraphicModulate(gi, (m_show[i].x - data.m_worldX) * data.m_scale, (m_show[i].y - data.m_worldY) * data.m_scale, 255,255, 255);
    }
  }  

  if(data.m_bGameOver == true){
    gi = con.GetSprite(168);
    con.RenderGraphicModulate(gi, 93, 308, 255, 255, 255);
  }
  if(data.m_bGameWon == true){
    gi = con.GetSprite(167);
    con.RenderGraphicModulate(gi, 98, 420, 255, 255, 255);
  }

  con.EndRender();

  //player prompt
  long cursorX = (data.m_worldX + m_mouse.MouseX()/data.m_scale)/m_offset; 
  long cursorY = (data.m_worldY + m_mouse.MouseY()/data.m_scale)/m_offset;

  if(cfg.DisplayDebugInfo == true && m_bSelectMap == false){
    //con.Print("Position X", cursorX, F_V16, 20, 30,  0, 0, 0, 255);
    //con.Print("Position Y", cursorY, F_V16, 20, 50,  0, 0, 0, 255);
    con.Print("Mouse X", m_mouse.MouseX(), F_V16, 20, 30,  255, 0, 0, 255);
    con.Print("Mouse Y", m_mouse.MouseY(), F_V16, 20, 50,  255, 0, 0, 255);

    con.Print("Series",     data.m_seriesCount, F_V16, 20, 70,  0, 0, 0, 255);
    con.Print("Profiles",   data.m_track.size(), F_V16, 20, 90,  255, 255, 255, 255);
    con.Print("Scale",      data.m_scale, F_V16, 20, 110,  255, 255, 255, 255);
    con.Print("Version", cfg.ProgramVersion, F_V16, 40, 15,  255, 255, 255, 255);
    con.Print("FPS", data.m_FPS , F_V16, 930, 15,  255, 255, 255, 255);

  }  
    con.Print("Version", cfg.ProgramVersion, F_V16, 40, 15,  255, 255, 255, 255);
    con.Print("FPS", data.m_FPS , F_V16, 930, 15,  255, 255, 255, 255);

  con.m_d3ddev->EndScene();    // ends the 3D scene
  hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen

}

void CGameStatePlay1::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
}

void CGameStatePlay1::Deactivate(CGameData &data){
  CAudioManager *pAudio = CAudioManager::Instance();
  pAudio->KillAll();
  m_bStateActive = false;  
  CLog *pLog = CLog::Instance();

  data.m_track.clear();
  data.m_hill.clear();
  data.m_water.clear();
  data.m_tree.clear();
  data.m_building.clear();
  m_bFirstTime = true;
  pLog->Log("Play Deactivate");
  
}

void CGameStatePlay1::Resume(){
}
void CGameStatePlay1::Pause(){
}
void CGameStatePlay1::Save(){
}

void CGameStatePlay1::AddTransitionEvent(int event, CGameStateObject* p_Next){
  TRANSITION_EVENT te;
  te.event=event;
  te.p_gso = p_Next;

  m_TE.push_back(te);
}

long CGameStatePlay1::MouseToWorldX(long mx, long wx, float scale){
  long x;
  x = (long)((float)(mx)/scale + wx);
  return x;
}

long CGameStatePlay1::MouseToWorldY(long my, long wy, float scale){
  long y;
  y = (long)((float)(my)/scale + wy);
  return y;
}

//((long)((float)m_mouse.MouseX()/ data.m_scale + data.m_worldX) / offset) * offset;
long CGameStatePlay1::MouseToWorldTileX(long mx, long wx, float scale, long offset){
  long x;
  x = ((long)((float)mx/scale + wx )/ offset) * offset;
  return x;
}

long CGameStatePlay1::MouseToWorldTileY(long my, long wy, float scale, long offset){
  long y;
  y = ((long)((float)my/scale + wy) / offset) * offset;
  return y;
}

void CGameStatePlay1::CountSeries(CGameData &data){
  data.m_seriesCount = 0;
  data.m_bTrackComplete = false;
  bool bFound = false;
  bool bConstructionComplete = true;

  long ix, iy, nx, ny, lx, ly;  //tiles
  int ip, np, lp;               //sprite profiles
  int dir = 0;                  //+ direct...right or down
  
  //starting point
  lx = data.m_startX / m_offset;
  ly = data.m_startY / m_offset;
  lp = 21;  

  //for(int k = 0; k <= 1; ++k){ //data.m_track.size(); ++k){ 
  for(int i = 0; i < data.m_track.size(); ++i){
    bFound = false;

    for(int j = 0; j < data.m_track.size(); ++j){
      if(i == j)
        break;

      nx = data.m_track[i].x /  m_offset;
      ny = data.m_track[i].y /  m_offset;
      np = data.m_track[i].profileID;

      if(lp == 17 && dir == 0 && (np == 21 || np == 94) && nx == lx + 1 && ny == ly){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 17 && dir == 3 && (np == 22 || np == 94) && nx == lx && ny == ly + 1){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 3;
        bFound = true;
      }
      else if(lp == 17 && dir == 0 && np == 20 && nx == lx + 1 && (ny == ly - 3 || ny == ly - 4)){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny; lp = np; dir = 1;
        bFound = true;
      }
      else if(lp == 17 && dir == 0 && np == 18 && nx == lx + 1 && ny == ly){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny + 3; lp = np; dir = 3;
        bFound = true;
      }
      else if(lp == 17 && dir == 3 && np == 19 && nx == lx && ny == ly + 1){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny + 3; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 17 && dir == 3 && np == 20 && nx == lx - 3 && ny == ly + 1){
        data.m_seriesCount++; 
        lx = nx; ly = ny + 3; lp = np; dir = 2;
        bFound = true;
      }

      else if(lp == 18 && dir == 3 && (np == 22 || np == 94) && nx == lx && ny == ly + 1){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 3;
        bFound = true;
      }
      else if(lp == 18 && dir == 2 && (np == 21 || np == 94) && nx == lx - 1 && ny == ly){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 2;
        bFound = true;
      }
      else if(lp == 18 && dir == 3 && np == 20 && nx == lx - 3 && ny == ly + 1){
        data.m_seriesCount++; 
        lx = nx; ly = ny + 3; lp = np; dir = 2;
        bFound = true;
      }
      else if(lp == 18 && dir == 3 && np == 19 && nx == lx && ny == ly + 1){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny + 3; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 18 && dir == 2 && np == 17 && nx == lx - 4 && ny == ly){
        data.m_seriesCount++; 
        lx = nx; ly = ny + 3; lp = np; dir = 3;
        bFound = true;
      }
      else if(lp == 18 && dir == 2 && np == 19 && nx == lx - 4 && (ny == ly - 3 || ny == ly - 4)){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 1;
        bFound = true;
      }

      else if(lp == 19 && dir == 0 && (np == 21 || np == 94) && nx == lx + 1 && ny == ly){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 19 && dir == 1 && (np == 22 || np == 94) && nx == lx && ny == ly - 1){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 1;
        bFound = true;
      }
      else if(lp == 19 && dir == 0 && np == 20 && nx == lx + 1 && (ny == ly - 3 || ny == ly - 4)){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny; lp = np; dir = 1;
        bFound = true;
      }
      else if(lp == 19 && dir == 0 && np == 18 && nx == lx + 1 && ny == ly){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny + 3; lp = np; dir = 3;
        bFound = true;
      }
      else if(lp == 19 && dir == 1 && np == 17 && nx == lx && (ny == ly - 4 || ny == ly - 3)){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 19 && dir == 1 && np == 18 && nx == lx - 3 && (ny == ly - 3 || ny == ly - 4)){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 2;
        bFound = true;
      }

      else if(lp == 20 && dir == 1 && (np == 22 || np == 94) && nx == lx && ny == ly - 1){
        data.m_seriesCount++; 
        lx = nx; ly = ny - 1; lp = np; dir = 1;
        bFound = true;
      }
      else if(lp == 20 && dir == 2 && (np == 21 || np == 94) && nx == lx - 1 && (ny == ly || ny == ly - 1)){/////
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 2;
        bFound = true;
      }
      else if(lp == 20 && dir == 1 && np == 18 && nx == lx - 3 && (ny == ly - 3 || ny == ly - 4)){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 2;
        bFound = true;
      }
      else if(lp == 20 && dir == 1 && np == 17 && nx == lx && (ny == ly - 3 || ny == ly - 4)){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 20 && dir == 2 && np == 19 && nx == lx - 4 && (ny == ly - 3 || ny == ly - 4)){
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 1;
        bFound = true;
      }
      else if(lp == 20 && dir == 2 && np == 17 && nx == lx - 4 && ny == ly){
        data.m_seriesCount++; 
        lx = nx; ly = ny + 3; lp = np; dir = 3;
        bFound = true;
      }


      //horizontal sections
      else if(lp == 21 && dir == 0 && np == 21 && nx == lx + 1 && ny == ly){///
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 21 && dir == 2 && np == 21 && nx == lx - 1 && ny == ly){///
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 2;
        bFound = true;
      }
      else if(lp == 21 && dir == 0 && np == 94 && nx == lx + 1 && ny == ly){///
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 21 && dir == 2 && np == 94 && nx == lx - 1 && ny == ly){///
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 2;
        bFound = true;
      }
      else if(lp == 21 && dir == 0 && np == 18 && nx == lx + 1 && ny == ly){///
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny + 3; lp = np;  dir = 3;
        bFound = true;
      }
      else if((lp == 21 || lp == 94) && dir == 0 && np == 20 && nx == lx + 1 && ny == ly - 3){///
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny; lp = np;  dir = 1;
        bFound = true;
      }
      else if(lp == 21 && dir == 2 && np == 17 && nx == lx - 4 && ny == ly){///
        data.m_seriesCount++; 
        lx = nx; ly = ny + 3; lp = np;  dir = 3;
        bFound = true;
      }
      else if(lp == 21 && dir == 2 && np == 19 && nx == lx - 4 && ny == ly - 3){///
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np;  dir = 1;
        bFound = true;
      }

      else if(lp == 22 && dir == 3 && np == 22 && nx == lx && ny == ly + 1){///
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 3;
        bFound = true;
      }
      else if(lp == 22 && dir == 1 && np == 22 && nx == lx && ny == ly - 1){///
        data.m_seriesCount++; 
        lx = nx; ly = ny - 1; lp = np; dir = 1;
        bFound = true;
      }
      else if(lp == 22 && dir == 3 && np == 94 && nx == lx && ny == ly + 1){///
        data.m_seriesCount++; 
        lx = nx; ly = ny + 1; lp = np; dir = 3;
        bFound = true;
      }
      else if(lp == 22 && dir == 1 && np == 94 && nx == lx && ny == ly - 1){///
        data.m_seriesCount++; 
        lx = nx; ly = ny - 1; lp = np; dir = 1;
        bFound = true;
      }
      else if(lp == 22 && dir == 3 && np == 19 && nx == lx && (ny == ly + 1 || ny == ly)){
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny + 3; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 22 && dir == 3 && np == 20 && nx == lx - 3 && ny == ly + 1){
        data.m_seriesCount++; 
        lx = nx; ly = ny + 3; lp = np; dir = 2;
        bFound = true;
      }      
      else if(lp == 22 && dir == 1 && np == 17 && nx == lx && (ny == ly - 3 || ny == ly - 4) ){//works for odd row numbers -4 for even row numbers
        data.m_seriesCount++; 
        lx = nx + 3; ly = ny ; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 22 && dir == 1 && np == 18 && nx == lx - 3 && (ny == ly - 3 || ny == ly - 4)){///
        data.m_seriesCount++; 
        lx = nx; ly = ny ; lp = np; dir = 2;
        bFound = true;
      }

      else if(lp == 94 && dir == 0 && np == 21 && nx == lx + 1 && ny == ly){///
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 0;
        bFound = true;
      }
      else if(lp == 94 && dir == 2 && np == 21 && nx == lx - 1 && ny == ly){///
        data.m_seriesCount++; 
        lx = nx; ly = ny; lp = np; dir = 2;
        bFound = true;
      }
      else if(lp == 94 && dir == 3 && np == 22 && nx == lx && ny == ly + 1){///
        data.m_seriesCount++; 
        lx = nx; ly = ny + 1; lp = np; dir = 3;
        bFound = true;
      }
      else if(lp == 94 && dir == 1 && np == 22 && nx == lx && ny == ly - 1){///
        data.m_seriesCount++; 
        lx = nx; ly = ny - 1; lp = np; dir = 1;
        bFound = true;
      }

      //add waypoints
      //****************
      if(bFound == true){
        if((lp == 21 || lp == 94) && dir == 0){//works
          if(j < 9)
           m_train.AddWaypoint(nx * m_offset + m_offset/2, ny * m_offset + m_offset/2, 0);
        }
        else if((lp == 21 || lp == 94) && dir == 2){//works
          if(j < 9)
            m_train.AddWaypoint(nx * m_offset - m_offset/2, ny * m_offset + m_offset/2, 3.14);
        }
        else if(lp == 22 && dir == 1){
         // m_train.AddWaypoint(nx * m_offset + m_offset/2, ny * m_offset + m_offset/2, -1.57);
         // m_train.AddWaypoint(nx * m_offset + m_offset/2, ny * m_offset - m_offset/2, -1.57);
        }
        else if(lp == 22 && dir == 3){//works
         // m_train.AddWaypoint((nx) * m_offset + m_offset/2, ny * m_offset + m_offset/2, 1.57);
         // m_train.AddWaypoint((nx) * m_offset + m_offset/2, ny * m_offset - m_offset/2, 1.57);
        }
        else if(lp == 94 && dir == 3){
         // m_train.AddWaypoint((nx) * m_offset + m_offset/2, ny * m_offset + m_offset/2, 1.57);
        }

        //top left curve moving right
        //****************************
        else if(lp == 17 && dir == 0){
          m_train.AddWaypoint(nx * m_offset + 31, ny * m_offset + 248, -1.571);
          m_train.AddWaypoint(nx * m_offset + 38, ny * m_offset + 192, -1.309);
          m_train.AddWaypoint(nx * m_offset + 60, ny * m_offset + 139, -1.047);
          m_train.AddWaypoint(nx * m_offset + 95, ny * m_offset + 95, -.785);
          m_train.AddWaypoint(nx * m_offset + 139, ny * m_offset + 60, -.524);
          m_train.AddWaypoint(nx * m_offset + 192, ny * m_offset + 38, -.262);
          m_train.AddWaypoint(nx * m_offset + 248, ny * m_offset + 31, 0);
        }    
        else if(lp == 17 && dir == 3){
          m_train.AddWaypoint(nx * m_offset + 248, ny * m_offset + 31, 3.142);
          m_train.AddWaypoint(nx * m_offset + 192, ny * m_offset + 38, 2.880);
          m_train.AddWaypoint(nx * m_offset + 139, ny * m_offset + 60, 2.618);
          m_train.AddWaypoint(nx * m_offset + 95, ny * m_offset + 95, 2.356);
          m_train.AddWaypoint(nx * m_offset + 60, ny * m_offset + 139, 2.095);
          m_train.AddWaypoint(nx * m_offset + 38, ny * m_offset + 192, 1.833);
          m_train.AddWaypoint(nx * m_offset + 31, ny * m_offset + 248, 1.571);
        }    

        //top right curve
        //******************
        else if(lp == 18 && dir == 3){//works
          m_train.AddWaypoint(nx * m_offset + 0, ny * m_offset + 31, 0);
          m_train.AddWaypoint(nx * m_offset + 56, ny * m_offset + 38, .262);
          m_train.AddWaypoint(nx * m_offset + 108, ny * m_offset + 60, .524);
          m_train.AddWaypoint(nx * m_offset + 153, ny * m_offset + 95, .785);
          m_train.AddWaypoint(nx * m_offset + 187, ny * m_offset + 139, 1.047);
          m_train.AddWaypoint(nx * m_offset + 209, ny * m_offset + 192, 1.309);
          m_train.AddWaypoint(nx * m_offset + 217, ny * m_offset + 248, 1.571);
        }       
        else if(lp == 18 && dir == 2){//works
          m_train.AddWaypoint(nx * m_offset + 217, ny * m_offset + 248, -1.571);
          m_train.AddWaypoint(nx * m_offset + 209, ny * m_offset + 192, -1.833);
          m_train.AddWaypoint(nx * m_offset + 187, ny * m_offset + 139, -2.095);
          m_train.AddWaypoint(nx * m_offset + 153, ny * m_offset + 95, -2.356);
          m_train.AddWaypoint(nx * m_offset + 108, ny * m_offset + 60, -2.618);
          m_train.AddWaypoint(nx * m_offset + 56, ny * m_offset + 38, -2.880);
          m_train.AddWaypoint(nx * m_offset + 0, ny * m_offset + 31, -3.142);
        }    

        //bottom left curve
        //********************
        else if(lp == 19 && dir == 0){
          m_train.AddWaypoint(nx * m_offset + 31, ny * m_offset + 0, 1.57);
          m_train.AddWaypoint(nx * m_offset + 38, ny * m_offset + 56, 1.309);
          m_train.AddWaypoint(nx * m_offset + 60, ny * m_offset + 108, 1.047);
          m_train.AddWaypoint(nx * m_offset + 95, ny * m_offset + 153, .785);
          m_train.AddWaypoint(nx * m_offset + 140, ny * m_offset + 188, .524);
          m_train.AddWaypoint(nx * m_offset + 192, ny * m_offset + 209, .262);
          m_train.AddWaypoint(nx * m_offset + 248, ny * m_offset + 217, 0);
        }
        else if(lp == 19 && dir == 1){
          m_train.AddWaypoint(nx * m_offset + 248, ny * m_offset + 217, -3.0);//-3.142);
          m_train.AddWaypoint(nx * m_offset + 192, ny * m_offset + 209, -2.8);//-2.880);
          m_train.AddWaypoint(nx * m_offset + 140, ny * m_offset + 188, -2.6);//-2.618);
          m_train.AddWaypoint(nx * m_offset + 95, ny * m_offset + 153, -2.3);//-2.356);
          m_train.AddWaypoint(nx * m_offset + 60, ny * m_offset + 108, -2);//-2.095);
          m_train.AddWaypoint(nx * m_offset + 38, ny * m_offset + 56, -1.8);//-1.833);
          m_train.AddWaypoint(nx * m_offset + 31, ny * m_offset + 0, -1.571);
        }

        //bottom right curve       
        //*******************************
        else if(lp == 20 && dir == 1){
          m_train.AddWaypoint(nx * m_offset + 0, ny * m_offset + 217, 0);
          m_train.AddWaypoint(nx * m_offset + 56, ny * m_offset + 209, -.262);
          m_train.AddWaypoint(nx * m_offset + 108, ny * m_offset + 187, -.524);
          m_train.AddWaypoint(nx * m_offset + 153, ny * m_offset + 153, -.785);
          m_train.AddWaypoint(nx * m_offset + 187, ny * m_offset + 108, -1.047);
          m_train.AddWaypoint(nx * m_offset + 209, ny * m_offset + 56, -1.309);
          m_train.AddWaypoint(nx * m_offset + 217, ny * m_offset + 0, -1.571);
        }
        else if(lp == 20 && dir == 2){
          m_train.AddWaypoint(nx * m_offset + 217, ny * m_offset + 0, 1.571);
          m_train.AddWaypoint(nx * m_offset + 209, ny * m_offset + 56, 1.833);
          m_train.AddWaypoint(nx * m_offset + 187, ny * m_offset + 108, 2.095);
          m_train.AddWaypoint(nx * m_offset + 153, ny * m_offset + 153, 2.356);
          m_train.AddWaypoint(nx * m_offset + 108, ny * m_offset + 187, 2.618);
          m_train.AddWaypoint(nx * m_offset + 56, ny * m_offset + 209, 2.880);
          m_train.AddWaypoint(nx * m_offset + 0, ny * m_offset + 217, 3.142);
        }

        //if(bFound == true && data.m_track[i].complete == false)
        //  bConstructionComplete = false;


        //test for completion
        if (data.m_endX == (lx + 1) * m_offset && data.m_endY == (ly * m_offset) + 5){ // && bConstructionComplete == true){
          data.m_bRailroadComplete = true;
         // ::MessageBox(0, "Done", "Done", 0);
        }
        //break;
      }
      if(bFound == true)
        break;
    }//for(int j
  }//for(int i
  //}
}

















