/**************************************************************************************
Filename: main.cpp Date: December 17, 2008 (original)
Purpose: Entry point and initial setup for standard 2D game
Updated: January 18, 2009
**************************************************************************************/

// include the basic windows header files and the Direct3D header file
#define _WIN32_WINNT 0x0500 //needed for TrackMouseEvent
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <string>
#include "..\include\CLog.h"
#include "..\include\CGraphics.h"
#include "..\include\CTimer.h"
#include "..\include\CGameData.h"
#include "..\include\Keystatus.h"
#include "..\include\CAudioManager.h"
#include "..\include\CGameStateObject.h"
#include "..\include\CGameStateControl.h"
#include "..\include\CGameStateIntro.h"
#include "..\include\CGameStateMain.h"
#include "..\include\CGameStateHelp.h"
#include "..\include\CGameStateCredits.h"
#include "..\include\CGameStateQuit.h"
#include "..\include\CGameStateScore.h"
#include "..\include\CGameStatePlay1.h"
#include "..\include\CGameStatePlay2.h"
#include "..\include\CGameStatePlay3.h"
#include "..\include\CGameStatePlay4.h"
#include "..\include\resource.h"
#include "..\include\CConfigData.h"
#include "..\resource.h"

// global declarations
LPDIRECT3D9 d3d; // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev; // the pointer to the device class

// function prototypes
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Shutdown();

//create game states
CGameStateObject* g_pStateControl = new CGameStateControl;
CGameStateObject* g_pStateMain = new CGameStateMain;
CGameStateObject* g_pStateIntro = new CGameStateIntro;
CGameStateObject* g_pStateCredits = new CGameStateCredits;
CGameStateObject* g_pStateHelp = new CGameStateHelp;
//CGameStateObject* g_pStateScore = new CGameStateScore;
CGameStateObject* g_pStatePlay1 = new CGameStatePlay1;
//CGameStateObject* g_pStatePlay2 = new CGameStatePlay2;
//CGameStateObject* g_pStatePlay3 = new CGameStatePlay3;
//CGameStateObject* g_pStatePlay4 = new CGameStatePlay4;
CGameStateObject* g_pCurrent = 0;
CGameStateObject* g_pNext = 0;
CGameStateObject* g_pLast = 0;
CGameStateObject* g_pStateQuit = new CGameStateQuit;

CMouse mouse;
CTimer gTimerKey;
CTimer gAudioTimer;

/***********************************************************************************
WinMain - Entry point for program.
***********************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    
    CLog *pLog = CLog::Instance();
    pLog->Log(" ");
    pLog->Log("***************************************");
    pLog->Log("Program Start"); 
    pLog->LogDate();   
    pLog->Log("***************************************");
    
    //request play choose fullscreen or windowed mode
    bool bFullscreen = false;
    int msgReturn = ::MessageBox(NULL, "Fullscreen? (Y/N)", "Select Display Option", MB_YESNO);
    if(msgReturn == IDYES)
      bFullscreen = true;

    //variable declarations
    CGameData gameData;
    CConfigData cfg;
    pLog->Log("Program Name", cfg.ProgramName);
    pLog->Log("Version", cfg.ProgramVersion);
    CTimer gTimerFPS;
    int gLoopCount = 0;
    int gSecondCount = 0;
    bool g_bRunning = true;
    bool gExitProgram = false; //this is set true with ESC, for rendering to stop properly

    //determine if we play new or saved game
    gameData.m_playNewGame = true;
    HRESULT hr = ::MessageBox(0, "Play new game?", "New or saved game!", MB_YESNO);
    if(hr == IDYES)
      gameData.m_playNewGame = true;
    else
      gameData.m_playNewGame = false;


    //setup game data    
    cfg.LoadConfigFile("assets//data//config.cfg");

    //create window
    HWND hWnd;
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "WindowClass";
    wc.hIconSm = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_ICON1)); 
    RegisterClassEx(&wc);
        
    //screen data - need at least 800x600 
    int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
    int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
    if(cfg.ScreenWidth < 800 || cfg.ScreenWidth > cxScreen)
      cfg.ScreenWidth = cxScreen;   
    if(cfg.ScreenHeight < 600 || cfg.ScreenHeight > cyScreen)
      cfg.ScreenHeight = cyScreen;    
    cfg.ScreenLeft = cxScreen/2 - cfg.ScreenWidth/2;
    cfg.ScreenTop = cyScreen/2 - cfg.ScreenHeight/2;

    //create window
    hWnd = CreateWindowEx(NULL,
                          "WindowClass",
                          cfg.ProgramVersion.c_str(),  
                          bFullscreen == true ? WS_EX_TOPMOST | WS_POPUP : WS_BORDER | WS_CAPTION | WS_SYSMENU,  
                          cfg.ScreenLeft, cfg.ScreenTop, 
                          bFullscreen == true ? CW_USEDEFAULT : cfg.ScreenWidth, 
                          bFullscreen == true ? CW_USEDEFAULT : cfg.ScreenHeight,
                          NULL,NULL,hInstance,NULL);
    ShowWindow(hWnd, nCmdShow);
    pLog->Log("Window Loaded and Displayed!");
    gameData.m_hWnd = hWnd;

    // set up and initialize Direct3D
    CGraphics con(hWnd, cfg.ScreenWidth, cfg.ScreenHeight,bFullscreen);// cfg.FullScreen);
    if(con.InitializeDirectX() == false){
      pLog->Log("Failure initializing DirectX!");
      ::MessageBox(hWnd,"Failed to initialize DirectX", "Error", 0);
      return 0;
    }
    pLog->Log("DirectX Initialized");

    //load framework assets
    if(con.LoadAssetFile(cfg.FrameworkAssetFile) == false){
      pLog->Log("Failure loading " + cfg.FrameworkAssetFile);
      ::MessageBox(hWnd,"Failed to load editor.dat file", "Error", 0);
      return 0;
    }
    else
      pLog->Log(cfg.FrameworkAssetFile + " (frame graphics) was loaded successfully!");

  /*  //display
    // clear the window to a deep blue
    HRESULT hr;
    static bLostDevice = false;
    if(FAILED(hr = con.m_d3ddev->TestCooperativeLevel())){
    
      //device is lost...cannot be reset...exit Render() function
      if(hr == D3DERR_DEVICELOST){
        if(bLostDevice == false){
          bLostDevice = true;
          con.DeleteLost();    //delete one time per failure
        }
        return 0; 
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
        return 0;
      }
    }
    if(FAILED(hr = con.m_d3ddev->TestCooperativeLevel())){
      
      //device is lost...cannot be reset...exit Render() function
      if(hr == D3DERR_DEVICELOST){
        if(bLostDevice == false){
          bLostDevice = true;
          con.DeleteLost();    //delete one time per failure
        }
        return 0;
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
        return 0;
      }
    }
    con.m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    con.m_d3ddev->BeginScene();    // begins the 3D scene
    GRAPHIC_IMAGE gi;
    con.BeginRender();
    gi = con.GetSprite(1000);
    con.RenderGraphicModulate(gi, 0, 0, 255, 255, 255);
    con.m_d3ddev->EndScene();    // ends the 3D scene
    hr = con.m_d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
*/
    //load game play assets
    if(con.LoadAssetFile(cfg.GamePlayAssetFile) == false){
      pLog->Log("Failure loading " + cfg.GamePlayAssetFile);
      ::MessageBox(hWnd,"Failed to load assets.dat file", "Error", 0);
      return 0;
    }
    else
      pLog->Log(cfg.GamePlayAssetFile + " (game play graphics) was loaded successfully!");

    gTimerKey.Initialize();
    gTimerKey.ResetTimer();
    mouse.SetHandle(hWnd);
    gTimerFPS.Initialize();
    
    //game timer for update
    CTimer timerGame;
    timerGame.Initialize();

    //define events for changing game states
    //*************************************************************************
    g_pStateIntro->AddTransitionEvent(EVENT_GO_MAIN, g_pStateMain);
    g_pStateMain->AddTransitionEvent(EVENT_GO_PLAY1, g_pStatePlay1);
    //g_pStateMain->AddTransitionEvent(EVENT_GO_HIGH_SCORE, g_pStateScore);
    g_pStateMain->AddTransitionEvent(EVENT_GO_HELP, g_pStateHelp);
    g_pStateMain->AddTransitionEvent(EVENT_GO_CREDITS, g_pStateCredits);
    g_pStatePlay1->AddTransitionEvent(EVENT_GO_MAIN, g_pStateMain);
    g_pStateHelp->AddTransitionEvent(EVENT_GO_MAIN, g_pStateMain);
    g_pStateCredits->AddTransitionEvent(EVENT_GO_QUIT, g_pStateQuit);
    //g_pStateScore->AddTransitionEvent(EVENT_GO_MAIN, g_pStateMain);
    g_pCurrent = g_pStateIntro;

    //************************************** S O U N D ************************
    //initialize sound manager
    CAudioManager *pAudio = CAudioManager::Instance();

    //load sound asset file
    if(pAudio->LoadAssetFile(cfg.GamePlayAssetFile) == false){
      pLog->Log("Failure loading " + cfg.GamePlayAssetFile);//assets.dat audio files!");
      ::MessageBox(hWnd,"Failed to load assets.dat audio files!", "Error", 0);
    }
    else
      pLog->Log(cfg.GamePlayAssetFile + " (audio) was loaded successfully!");
        
    if(pAudio->IsValidSound() == true)
      pLog->Log("Audio system initialized (size)", pAudio->Size());
    else
      pLog->Log("Audio failure!");

    //log all audio files    
    if(cfg.LogDebugInfo == true){
      pLog->Log("************************** Audio Files Loaded **************");
      for(int i = 0; i < pAudio->Size(); i++){
        pLog->Log(pAudio->GetFilename(i)); 
      }
      pLog->Log("");
    }

    // enter the main loop
    //************************************** M A I N  L O O P *****************
    MSG msg;
    pLog->Log("Entering Main Control Loop");
    float timeDiff = 0.0f;
    
    while(g_bRunning)
    {
      DWORD starting_point = GetTickCount();

      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
          if (msg.message == WM_QUIT)
              break;

          TranslateMessage(&msg);
          DispatchMessage(&msg);
      }

      //manage frame count determination
      gLoopCount++;
      if(gTimerFPS.getTimer(1.0) == true){
        gameData.m_FPS = gLoopCount;
        gLoopCount = 0;   
        gSecondCount++;
        if(gSecondCount > 30){ //log every 30 seconds
          gSecondCount = 0;
          if(cfg.LogDebugInfo == true)
            pLog->Log("FPS",gameData.m_FPS);
        }
      }

      //stores mouse button status for use in other classes
      gameData.m_bLeftMouseDown =  mouse.LeftButtonDown();
      gameData.m_bRightMouseDown = mouse.RightButtonDown();
      
      //update
      g_pLast = g_pCurrent;
      g_pNext = g_pCurrent->Update(timerGame.getTimeDifference(), gameData, cfg, con);

      if(g_pNext == g_pStateQuit)
        g_bRunning = false;
      else if(NULL != g_pNext)
	    {
        if(g_pNext != g_pLast){
          g_pLast->Deactivate(gameData);
          g_pCurrent = g_pNext;
          g_pCurrent->Activate(gameData);
        }
        //g_pLast->Deactivate(CGameData &data);
        //g_pCurrent->Deactivate(CGameData &data);
        //g_pCurrent = g_pNext;
        //g_pNext->Activate(CGameData &data);
      }  
      
      //render
      g_pCurrent->Render(con, gameData, cfg);

      // check the 'escape' key
      if(g_bRunning == false){
        gExitProgram = true;
        PostMessage(hWnd, WM_DESTROY, 0, 0);
      }
    }
    pLog->Log("Exited main loop");

    // clean up DirectX and COM
    con.CleanupDirectX();
    Shutdown();

    pLog->Log("DirectX Cleaned Up");
    pLog->Log("Shutdown complete!");
    pLog->Log("***************************************");
    pLog->Log(" Program Terminated Normally");
    pLog->Log("***************************************");

    return msg.wParam;
}

/***********************************************************************************
  WindowProc - Main message handler
***********************************************************************************/
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static TRACKMOUSEEVENT tme;

  switch(message)
  {
  case WM_CREATE:
    g_pStateIntro->Initialize();
    g_pStateMain->Initialize();
    g_pStateControl->Initialize();
    g_pStateCredits->Initialize();
    //g_pStateScore->Initialize();
    g_pStatePlay1->Initialize();
    //g_pStatePlay2->Initialize();
    //g_pStatePlay3->Initialize();
    //g_pStatePlay4->Initialize();

    //#define _WIN32_WINNT 0x0500 added before <windows.h> in order to get this to work
    tme.cbSize = sizeof(tme);
    tme.dwFlags = 0x00000002;//TME_LEAVE;
    tme.hwndTrack = hWnd;
    TrackMouseEvent(&tme);
    return 0;

  case WM_MOUSEMOVE:
	  TrackMouseEvent(&tme);
	  return 0;

  case WM_LBUTTONDOWN:
    mouse.SetLeftButtonDown(true);
    return 0;

  case WM_LBUTTONUP:
    mouse.SetLeftButtonDown(false);
    return 0;

  case WM_RBUTTONDOWN:
    mouse.SetRightButtonDown(true);
    return 0;

  case WM_RBUTTONUP:
    mouse.SetRightButtonDown(false);
    return 0;

  case WM_MOUSELEAVE:
    mouse.SetLeftButtonDown(false);
    mouse.SetRightButtonDown(false);
    return 0;

  case WM_DESTROY:
      {
          PostQuitMessage(0);
          return 0;
      } break;
  }

  return DefWindowProc (hWnd, message, wParam, lParam);
}

//Shutdown everything NOT DirectX
void Shutdown(){
  delete g_pStateControl;
  delete g_pStateMain;
  delete g_pStateIntro;
  delete g_pStateCredits;
  delete g_pStateHelp;
  //delete g_pStateScore;
  delete g_pStateQuit;
  delete g_pStatePlay1;
  //delete g_pStatePlay2;
  //delete g_pStatePlay3;
  //delete g_pStatePlay4;

}