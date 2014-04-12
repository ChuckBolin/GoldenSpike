/*
  CGameStateMain.cpp - Controls the user interface and display for the main menu. 

*/
#include "..\include\CGameStateMain.h"
#include "..\include\keystatus.h"

CGameStateMain::CGameStateMain(void)
{
}

CGameStateMain::~CGameStateMain(void)
{
	m_TE.clear();
}

void CGameStateMain::Initialize(){
  m_selection = 0; //engage

}

void CGameStateMain::Activate(CGameData &data){

}

CGameStateObject* CGameStateMain::Update(double timeDifference,  CGameData &data, CConfigData &cfg, CGraphics &con){
  m_event = EVENT_GO_NO_WHERE;

  //first time code
  static bool bFirstTime = true;
  if(bFirstTime == true){
    m_spikeAngle = 0;
    m_targetAngle = 0;
    m_pauseSpike = true;

    bFirstTime = false;
  }

  //CAudioManager *pAudio = CAudioManager::Instance();
  m_mouse.SetHandle(data.m_hWnd);
  static double timeDiff = 0;
  timeDiff += timeDifference;

  static double activateTime = 0;
  if(data.m_bLeftMouseDown == false)
    activateTime += timeDifference;
  if(activateTime < 0.5)
    return NULL;  

  //animate spike
  static int spikeDir = 0; //rotate right
  if(m_selection == 0)
    m_targetAngle = 1.48;
  else if(m_selection == 1)
    m_targetAngle = .98;
  else if(m_selection == 2)
    m_targetAngle = .28;

  if(m_spikeAngle > m_targetAngle - 0.02 && m_spikeAngle < m_targetAngle + 0.02)
    m_spikeAngle = m_targetAngle;
  else if(m_targetAngle > m_spikeAngle)
    m_spikeAngle += 1.25 * timeDifference;
  else if(m_targetAngle < m_spikeAngle)
    m_spikeAngle -= 1.25 * timeDifference;

  if(::GetActiveWindow() == data.m_hWnd){// && data.m_bLeftMouseDown == true){
    if(m_mouse.MouseX() < 443){
      if(m_mouse.MouseY() < 292)
        m_selection = 0;
      else if(m_mouse.MouseY() < 511)
        m_selection = 1;
      else
        m_selection = 2;
    }
  }

  if(::GetActiveWindow() == data.m_hWnd && (keyDown(VK_RETURN) || data.m_bLeftMouseDown == true ) &&
     m_mouse.MouseX() < 443){
    if(m_selection == 0)
      m_event = EVENT_GO_PLAY1;
    else if(m_selection == 1)
      m_event = EVENT_GO_HELP;
    else if(m_selection == 2)
      m_event = EVENT_GO_CREDITS;
  }

  /*
  if(timeDiff > 0.2){

    if(::GetActiveWindow() == data.m_hWnd && keyDown(VK_UP)){
      m_selection--;
      if(m_selection < 0)
        m_selection = 2;
    }
    if(::GetActiveWindow() == data.m_hWnd && keyDown(VK_DOWN)){
      m_selection++;
      if(m_selection > 2)
        m_selection = 0;
    }
    timeDiff = 0;

  }
*/

  for(int i=0; i< m_TE.size(); i++){
    if (m_TE[i].event == m_event){
        return m_TE[i].p_gso;
    }
  }

  return 0;
}

void CGameStateMain::Render(CGraphics &con,  CGameData &data, CConfigData &cfg){
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
  con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
  con.m_d3ddev->BeginScene();    // begins the 3D scene
  con.BeginRender();

  GRAPHIC_IMAGE gi;
  con.BeginRender();

  //background image
  gi = con.GetSprite(1004);
  con.RenderGraphicModulate(gi, 0, 0, 255, 255, 255); 

  //play game
  gi = con.GetSprite(1005);
  gi.angle = 0.3;
  con.RenderGraphicModulate(gi, -30, -10, 255, 255, 255);
  
  //how to play?
  gi = con.GetSprite(1006);
  gi.angle = -0.3;
  con.RenderGraphicModulate(gi, 30, 230, 255, 255, 255);

  //exit game
  gi = con.GetSprite(1007);
  gi.angle = 0.05;
  con.RenderGraphicModulate(gi, 105, 425, 255, 255, 255);

  //outline
  gi = con.GetSprite(1008);
  switch(m_selection){
    case 0: //play game
      gi.angle = 0.3;
      con.RenderGraphicModulate(gi, -30, -10, 255, 255, 255);
      break;
    case 1: //how to play?
      gi.angle = -0.3;
      con.RenderGraphicModulate(gi, 30, 230, 255, 255, 255);
      break;
    case 2: //exit
      gi.angle = 0.05;
      con.RenderGraphicModulate(gi, 105, 425, 255, 255, 255);
      break;
  }

  //title of game
  //gi = con.GetSprite(1003);
  //gi.angle = - 0.6;
  //con.RenderGraphicModulate(gi, 300, 50, 255, 255, 255);

  //golden spike
  gi = con.GetSprite(1009);
  gi.angle = m_spikeAngle;
  con.RenderGraphicModulate(gi, 650, 100, 255, 255, 255);

  //mouse cursor
  gi = con.GetSprite(1002);
  con.RenderGraphicModulate(gi, m_mouse.MouseX() - gi.width/2,m_mouse.MouseY() - gi.height/2, 255, 255, 255);
  con.EndRender();
 
  if(cfg.DisplayDebugInfo == true){
    //con.Print("Version", cfg.ProgramVersion, F_V16, 40, 730,  255, 255, 255, 255);
    //con.Print("FPS", data.m_FPS , F_V16, 930, 730,  255, 255, 255, 255);
    //con.Print("Mouse X", m_mouse.MouseX(), F_V16, 900, 30,  255, 255, 255, 255);
    //con.Print("Mouse Y", m_mouse.MouseY(), F_V16, 900, 50,  255, 255, 255, 255);
    //con.Print("Spike", m_spikeAngle, F_V16, 900, 70,  255, 255, 255, 255);
  }  
  
  con.EndRender();
  con.m_d3ddev->EndScene();    // ends the 3D scene
  hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen


}

void CGameStateMain::ProcessEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){

}

void CGameStateMain::Deactivate(CGameData &data){

}
void CGameStateMain::Resume(){
}
void CGameStateMain::Pause(){
}
void CGameStateMain::Save(){
}

void CGameStateMain::AddTransitionEvent(int event, CGameStateObject* p_Next){
  TRANSITION_EVENT te;
  te.event=event;
  te.p_gso = p_Next;

  m_TE.push_back(te);
}