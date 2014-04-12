#include "..\include\CGameStateControl.h"
 
CGameStateControl::CGameStateControl(){
}

CGameStateControl::~CGameStateControl()
{
	m_TE.clear();
}

void CGameStateControl::Initialize(){

}

void CGameStateControl::Activate(CGameData &data){

}
//***************************************************************
//                         U P D A T E
//***************************************************************
CGameStateObject* CGameStateControl::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  m_event = EVENT_GO_NO_WHERE;
  CLog *pLog = CLog::Instance();

  static bool bStop = false;
  
  //perform this code only once
  //****************************
  static bool bFirstTime = true;
  if(bFirstTime == true){
    bFirstTime = false;

    //data.LoadGameLevel("assets\\data\\level1.dat"); //missing sprite data
    //data.AddGraphicDataToLevelData(con);            //update graphic data
  }

  
  m_mouse.SetHandle(data.m_hWnd);//needed for mouse

  if(::GetActiveWindow() == data.m_hWnd && keyDown(VK_ESCAPE))//quit this mode
    m_event = EVENT_GO_MAIN;
  else if(::GetActiveWindow() == data.m_hWnd && keyDown('1'))
    m_event = EVENT_GO_PLAY1;
  else if(::GetActiveWindow() == data.m_hWnd && keyDown('2'))
    m_event = EVENT_GO_PLAY2;
  else if(::GetActiveWindow() == data.m_hWnd && keyDown('3'))
    m_event = EVENT_GO_PLAY3;
  else if(::GetActiveWindow() == data.m_hWnd && keyDown('4'))
    m_event = EVENT_GO_PLAY4;
  
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
void CGameStateControl::Render(CGraphics &con,  CGameData &data, CConfigData &cfg){
  //::ShowCursor(true);
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
  con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);
  con.m_d3ddev->BeginScene();    // begins the 3D scene
 
  GRAPHIC_IMAGE gi;
  gi = con.GetSprite(0);
  con.RenderGraphicModulate(gi, 20, 20, 255, 255, 255); 

 
  //player prompt
  con.Print("Control Screen", F_V20B, cfg.ScreenWidth/2 - 60, 100, 255, 255, 255, 255);
  con.Print("Press ESCAPE to Return to Main Menu!", F_V20B, cfg.ScreenWidth/2 - 175, cfg.ScreenHeight - 100, 255, 255, 255, 255);
  con.Print("Press 1)Play ONE,  2) Play TWO,  3) Play THREE,  4) Play FOUR to goto Play State!", F_V20B, cfg.ScreenWidth/2 - 400, cfg.ScreenHeight - 80, 255, 255, 255, 255);

  if(cfg.DisplayDebugInfo == true){
    con.Print("Version", cfg.ProgramVersion, F_V16, 40, 730,  255, 255, 255, 255);
    con.Print("FPS", data.m_FPS , F_V16, 930, 730,  255, 255, 255, 255);
  }  

  con.m_d3ddev->EndScene();    // ends the 3D scene
  hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen


}

void CGameStateControl::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
}

void CGameStateControl::Deactivate(CGameData &data){
  CAudioManager *pAudio = CAudioManager::Instance();
  pAudio->KillAll();

  
}
void CGameStateControl::Resume(){
}
void CGameStateControl::Pause(){
}
void CGameStateControl::Save(){
}

void CGameStateControl::AddTransitionEvent(int event, CGameStateObject* p_Next){
  TRANSITION_EVENT te;
  te.event=event;
  te.p_gso = p_Next;

  m_TE.push_back(te);
}
/*
float CGameStateControl::GetDistance(CObject *obj1, CObject *obj2){
  return sqrt((obj1->go.pos.x - obj2->go.pos.x) * (obj1->go.pos.x - obj2->go.pos.x) + 
              (obj1->go.pos.y - obj2->go.pos.y) * (obj1->go.pos.y - obj2->go.pos.y));
}
*/