#include "..\include\CGameStateIntro.h"

CGameStateIntro::CGameStateIntro(void){
}

CGameStateIntro::~CGameStateIntro(void)
{
	m_TE.clear();
}

void CGameStateIntro::Initialize(){
  m_timer.Initialize();
 
}

void CGameStateIntro::Activate(CGameData &data){

}

CGameStateObject* CGameStateIntro::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  m_event = EVENT_GO_NO_WHERE;
  m_mouse.SetHandle(data.m_hWnd);//needed for mouse

  //initialize data first time this state is called
  static bool bFirstTime = true;
  if(bFirstTime == true){
    bFirstTime = false;
    m_fadeAlpha = 255;    
    m_graphic = 0;
  }

  /*
  CAudioManager *pAudio = CAudioManager::Instance();
  if(cfg.PlayMusic == true && pAudio->IsPlaying(0) == false)
    pAudio->PlaySoundClip(0, 1);
  */

  //left click to continue
  if(::GetActiveWindow() == data.m_hWnd && data.m_bLeftMouseDown == true && m_fadeAlpha > 240)
    m_event = EVENT_GO_MAIN;

  //keystrokes
  if(m_graphic > 0){//don't exit splash..just title screen
    if(::GetActiveWindow() == data.m_hWnd && keyDown(VK_SPACE))
      m_event = EVENT_GO_MAIN;
  }

  //manage fade of splash screen
  if(m_timer.getTimer(0.01) == true){
    m_fadeAlpha += 10;
    if(m_fadeAlpha >= 255){
      m_fadeAlpha = 255;
      m_graphic = 1; //title screen
    }
  }

  for(unsigned int i=0; i< m_TE.size(); i++){
    if (m_TE[i].event == m_event){
      return m_TE[i].p_gso;
    }
  }
  return 0;
}

void CGameStateIntro::Render(CGraphics &con, CGameData &data, CConfigData &cfg){
  ::ShowCursor(false);
  CLog *pLog = CLog::Instance();
  static bLostDevice = false;

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
  if(m_graphic == 0)
    con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 111, 255), 1.0f, 0);
  else
    con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
  con.m_d3ddev->BeginScene();    // begins the 3D scene

  GRAPHIC_IMAGE gi;
  int posX, posY;
  
  con.BeginRender();
  if(m_graphic == 0){//logo
    gi = con.GetSprite(1000);
    gi.alpha = m_fadeAlpha;
  }
  else{ //title screen
    gi = con.GetSprite(1001);
    con.RenderGraphicModulate(gi, 0, 0, 255, 255, 255);
    //gi.scale = 0.8;
  }

  //con.RenderGraphicModulate(gi, 0, 0, 255, 255, 255); 
  if(m_graphic > 0){//title
    gi = con.GetSprite(1003);
    con.RenderGraphicModulate(gi, 142, 20, 255, 255, 255);
  }

  //red mouse cursor
  if(m_graphic > 0){
    gi = con.GetSprite(1002);
    con.RenderGraphicModulate(gi, m_mouse.MouseX() - gi.width/2,m_mouse.MouseY() - gi.height/2, 255, 255, 255);
  }

  con.EndRender();
  //con.Print("Introduction Splash Screen", F_V20B, cfg.ScreenWidth/2 - 120, 100, 255, 255, 255, 255);

  //player prompt
  if(m_graphic > 0){
    con.Print("Game Challenge 11 - Chuck Bolin", F_V20, cfg.ScreenWidth/2 - 135, cfg.ScreenHeight - 65, 0, 155, 0, 255);
    con.Print("Left Click to Continue!", F_V20, cfg.ScreenWidth/2 - 100, cfg.ScreenHeight - 40, 255, 255, 0, 255);
  }
  else if(m_graphic == 0){
    std::string text = "Loading files ";
    for(int i = 0; i < (int)(m_fadeAlpha/ 10); ++i){
      text = text + ".";    
    }
    con.Print(text, F_V20, cfg.ScreenWidth/2 - 230, cfg.ScreenHeight/2 - 40, 255, 255, 255, 255);
  }

  if(cfg.DisplayDebugInfo == true){
    con.Print("Version", cfg.ProgramVersion, F_V16, 40, 730,  255, 255, 255, 255);
    con.Print("FPS", data.m_FPS , F_V16, 930, 730,  255, 255, 255, 255);
    //con.Print("Alpha", m_fadeAlpha, F_V16, 40, 700, 255, 255, 255, 255);
  }  

  con.m_d3ddev->EndScene();    // ends the 3D scene
  hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
 
  return;
}

void CGameStateIntro::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
}

void CGameStateIntro::Deactivate(CGameData &data){
}
void CGameStateIntro::Resume(){
}
void CGameStateIntro::Pause(){
}
void CGameStateIntro::Save(){
}

void CGameStateIntro::AddTransitionEvent(int event, CGameStateObject* p_Next){
  TRANSITION_EVENT te;
  te.event=event;
  te.p_gso = p_Next;

  m_TE.push_back(te);
}