/**********************************************************
 GRAPHICS.CPP - Written by Chuck Bolin
 Thanks to http://www.toymaker.info/Games/html/sprites.html
 for the tutorials on 2D sprites with DirectX

 Save sprites with ImageComposer as PNG, check Transparent
 color, set threshold to 127
 *********************************************************/
#include "..\includes\Graphics.h"
#include "..\includes\Log.h"
extern CGlobal g_Global;

Graphics::Graphics(){
	for (int i = 0; i < GF_MAX_FILES; i++)
	{
    m_pFile[i] = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name :	InitD3D () (Public)
// Desc :	Performs a simple, non-enumerated, initialization of Direct3D
//-----------------------------------------------------------------------------
int Graphics::InitD3D (HWND hWnd, bool bWindowedMode)
{
  HRESULT               hRet;
  D3DCAPS9              Caps;
	D3DDISPLAYMODE        CurrentMode;
	m_bWindowed			= bWindowedMode;

	m_TextureCount = 0;

  // First of all create our D3D Object
  m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
  if (!m_pD3D) 
  {
    MessageBox( hWnd, "No compatible Direct3D object could be created.", "Fatal Error!", MB_OK | MB_ICONSTOP | MB_APPLMODAL );
    return false;
  } // End if failure

  // Fill out a simple set of present parameters
  ZeroMemory( &m_D3DPresentParams, sizeof(D3DPRESENT_PARAMETERS) );

  // Select back buffer format etc
	m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &CurrentMode);
	
	D3DFORMAT adapterFormat = (m_bWindowed) ? CurrentMode.Format : D3DFMT_X8R8G8B8;

	m_D3DPresentParams.Windowed						= m_bWindowed;
	m_D3DPresentParams.BackBufferFormat		= CurrentMode.Format;
  m_D3DPresentParams.BackBufferWidth		= (m_bWindowed) ? 0 : CurrentMode.Width;
  m_D3DPresentParams.BackBufferHeight		= (m_bWindowed) ? 0 : CurrentMode.Height;
	m_D3DPresentParams.BackBufferCount		= 1;
  m_D3DPresentParams.SwapEffect					= D3DSWAPEFFECT_DISCARD;
  m_D3DPresentParams.hDeviceWindow			= hWnd;
  m_D3DPresentParams.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	m_D3DPresentParams.Windowed						= bWindowedMode;
	m_D3DPresentParams.AutoDepthStencilFormat		= FindDepthStencilFormat( D3DADAPTER_DEFAULT, CurrentMode, D3DDEVTYPE_HAL );
	m_D3DPresentParams.EnableAutoDepthStencil		= true;
	m_D3DPresentParams.FullScreen_RefreshRateInHz = (m_bWindowed) ? 0 : CurrentMode.RefreshRate;
    
	// Set Creation Flags
	unsigned long ulFlags = 0;// D3DCREATE_SOFTWARE_VERTEXPROCESSING;

  // Check if Hardware T&L is available
  ZeroMemory( &Caps, sizeof(D3DCAPS9) );
  m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps );
  

  if( Caps.VertexProcessingCaps != 0 )
      ulFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
  else
      ulFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

  // Attempt to create a HAL device
  if( FAILED( hRet = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, ulFlags, &m_D3DPresentParams, &m_pD3DDevice ) ) ) 
  {
      MessageBox( hWnd, "Could not create a valid HAL Direct3D device object.\r\n\r\n" 
                          "The system will now attempt to create a device utilising the 'Reference Rasterizer' (D3DDEVTYPE_REF)",
                          "Fatal Error!", MB_OK | MB_ICONINFORMATION | MB_APPLMODAL) ;
      
      // Find REF depth buffer format
      m_D3DPresentParams.AutoDepthStencilFormat = FindDepthStencilFormat( D3DADAPTER_DEFAULT, CurrentMode, D3DDEVTYPE_REF );

      // Check if Hardware T&L is available
      ZeroMemory( &Caps, sizeof(D3DCAPS9) );
      ulFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
      m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &Caps );
      if ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

      // Attempt to create a REF device
      if( FAILED( hRet = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, ulFlags, &m_D3DPresentParams, &m_pD3DDevice ) ) ) 
      {
          MessageBox( hWnd, "Could not create a valid REF Direct3D device object.\r\n\r\nThe system will now exit.",
                            "Fatal Error!", MB_OK | MB_ICONSTOP | MB_APPLMODAL );

          // Failed
          return false;
      
      } // End if Failure (REF)

  } // End if Failure (HAL)


	//Set vertex shader
    m_pD3DDevice->SetVertexShader(NULL);
    m_pD3DDevice->SetFVF (D3DFVF_TLVERTEX);

    //Create vertex buffer and set as stream source
    m_pD3DDevice->CreateVertexBuffer(sizeof(TLVERTEX) * 4, NULL, D3DFVF_TLVERTEX, D3DPOOL_MANAGED,
                                  &vertexBuffer, NULL);
    m_pD3DDevice->SetStreamSource (0, vertexBuffer, 0, sizeof(TLVERTEX));

    //Setup rendering states
    m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	// Create the fonts
	CreateDXFonts();


  if(FAILED( D3DXCreateSprite(m_pD3DDevice, &m_pSprite)) )
  {
   // g_Log.Write("Sprite creation failure...");
    return false;
  }


    // Success!!
    return true;
}


//-----------------------------------------------------------------------------
// Name : FindDepthStencilFormat ()		(private)
// Desc : This function simply determines the best depth format that is
//        available for the specified mode.
// Note : No tests for stencil active depth buffers are made.
//-----------------------------------------------------------------------------
D3DFORMAT Graphics::FindDepthStencilFormat( ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType )
{

    // Test for 24 bith depth buffer
    if (SUCCEEDED( m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D32 )))
    {
        if (SUCCEEDED( m_pD3D->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D32 ))) return D3DFMT_D32;
    
    } // End if 32bpp Available

    // Test for 24 bit depth buffer
    if (SUCCEEDED( m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D24X8 )))
    {
        if (SUCCEEDED( m_pD3D->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D24X8 ))) return D3DFMT_D24X8;
    
    } // End if 24bpp Available

    // Test for 16 bit depth buffer
    if (SUCCEEDED( m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D16 )))
    {
        if (SUCCEEDED( m_pD3D->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D16 ))) return D3DFMT_D16;
    
    } // End if 16bpp Available

    // No depth buffer supported
    return D3DFMT_UNKNOWN;

}


void Graphics::CreateDXFonts()
{
  //load 12 different fonts for Arial and Verdana, 12-16, normal and bold
    D3DXCreateFont(m_pD3DDevice, 12, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_Font[F_A12] );
    D3DXCreateFont(m_pD3DDevice, 12, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_Font[F_A12B] );
    D3DXCreateFont(m_pD3DDevice, 14, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_Font[F_A14] );
    D3DXCreateFont(m_pD3DDevice, 14, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_Font[F_A14B] );
    D3DXCreateFont(m_pD3DDevice, 16, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_Font[F_A16] );
    D3DXCreateFont(m_pD3DDevice, 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_Font[F_A16B] );
    D3DXCreateFont(m_pD3DDevice, 12, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_Font[F_V12] );
    D3DXCreateFont(m_pD3DDevice, 12, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_Font[F_V12B] );
    D3DXCreateFont(m_pD3DDevice, 14, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_Font[F_V14] );
    D3DXCreateFont(m_pD3DDevice, 14, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_Font[F_V14B] );
    D3DXCreateFont(m_pD3DDevice, 16, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_Font[F_V16] );    
    D3DXCreateFont(m_pD3DDevice, 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_Font[F_V16B] );

    D3DXCreateFont(m_pD3DDevice, 20, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_Font[F_V20] );
    D3DXCreateFont(m_pD3DDevice, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, TEXT("Verdana"), &m_Font[F_V20B] );

}
void Graphics::Draw2DText(std::string sText, int fontType, int posX, int posY, int red, int green, int blue, int alpha){
  D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, red, green, blue);
  int width = 0;
  int fontHeight = 0;

  if(fontType == 0 || fontType == 1 || fontType == 6 || fontType == 7)
    fontHeight = 12;
  else if(fontType == 2 || fontType == 3 || fontType == 8 || fontType == 9)
    fontHeight = 14;
  else if(fontType == 4 || fontType == 5 || fontType == 10 || fontType == 11)
    fontHeight = 16;
  else if(fontType == 12 || fontType == 13)
    fontHeight = 20;

  width = sText.size() * fontHeight;
  RECT rct;
  rct.left = posX;
  rct.top = posY;
  rct.right = rct.left + width;
  rct.bottom = rct.top + fontHeight;
  m_Font[fontType]->DrawText(NULL, sText.c_str(), -1, &rct, 0, fontColor);  
}

void Graphics::Draw2DTextValue(std::string sText, std::ostringstream value, int fontType, int posX, int posY, int red, int green, int blue, int alpha){
  std::ostringstream oss;
  std::string text;
  oss << sText << ":" << value;
  text = oss.str();
  D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, red, green, blue);
  int width = 0;
  int fontHeight = 0;

  if(fontType == 0 || fontType == 1 || fontType == 6 || fontType == 7)
    fontHeight = 12;
  else if(fontType == 2 || fontType == 3 || fontType == 8 || fontType == 9)
    fontHeight = 14;
  else if(fontType == 4 || fontType == 5 || fontType == 10 || fontType == 11)
    fontHeight = 16;
  else if(fontType == 12 || fontType == 13)
    fontHeight = 20;

  width = sText.size() * fontHeight * 20;
  RECT rct;
  rct.left = posX;
  rct.top = posY;
  rct.right = rct.left + width;
  rct.bottom = rct.top + fontHeight;
  m_Font[fontType]->DrawText(NULL, text.c_str(), -1, &rct, 0, fontColor);  

}

void Graphics::Draw2DTextValue(std::string sText, float value,int fontType, int posX, int posY, int red, int green, int blue, int alpha){
  std::ostringstream oss;
  std::string text;
  oss << sText << ":" << value;
  text = oss.str();
  D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, red, green, blue);
  int width = 0;
  int fontHeight = 0;

  if(fontType == 0 || fontType == 1 || fontType == 6 || fontType == 7)
    fontHeight = 12;
  else if(fontType == 2 || fontType == 3 || fontType == 8 || fontType == 9)
    fontHeight = 14;
  else if(fontType == 4 || fontType == 5 || fontType == 10 || fontType == 11)
    fontHeight = 16;
  else if(fontType == 12 || fontType == 13)
    fontHeight = 20;

  width = sText.size() * fontHeight * 20;
  RECT rct;
  rct.left = posX;
  rct.top = posY;
  rct.right = rct.left + width;
  rct.bottom = rct.top + fontHeight;
  m_Font[fontType]->DrawText(NULL, text.c_str(), -1, &rct, 0, fontColor);  
}

void Graphics::Draw2DTextValue(std::string sText, long value,int fontType, int posX, int posY, int red, int green, int blue, int alpha){
  std::ostringstream oss;
  std::string text;
  oss << sText << ":" << value;
  text = oss.str();
  D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, red, green, blue);
  int width = 0;
  int fontHeight = 0;

  if(fontType == 0 || fontType == 1 || fontType == 6 || fontType == 7)
    fontHeight = 12;
  else if(fontType == 2 || fontType == 3 || fontType == 8 || fontType == 9)
    fontHeight = 14;
  else if(fontType == 4 || fontType == 5 || fontType == 10 || fontType == 11)
    fontHeight = 16;
  else if(fontType == 12 || fontType == 13)
    fontHeight = 20;

  width = sText.size() * fontHeight * 20;
  RECT rct;
  rct.left = posX;
  rct.top = posY;
  rct.right = rct.left + width;
  rct.bottom = rct.top + fontHeight;
  m_Font[fontType]->DrawText(NULL, text.c_str(), -1, &rct, 0, fontColor);  
}
void Graphics::Draw2DValue(float value,int fontType, int posX, int posY, int red, int green, int blue, int alpha){
  std::ostringstream oss;
  std::string text;
  oss << value;
  text = oss.str();
  D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, red, green, blue);
  int width = 0;
  int fontHeight = 0;
  if(fontType < 0 || fontType > 11)
    fontType = 0;

  if(fontType == 0 || fontType == 1 || fontType == 6 || fontType == 7)
    fontHeight = 12;
  else if(fontType == 2 || fontType == 3 || fontType == 8 || fontType == 9)
    fontHeight = 14;
  else if(fontType == 4 || fontType == 5 || fontType == 10 || fontType == 11)
    fontHeight = 16;

  width = text.size() * fontHeight;
  RECT rct;
  rct.left = posX;
  rct.top = posY;
  rct.right = rct.left + width;
  rct.bottom = rct.top + fontHeight;
  m_Font[fontType]->DrawText(NULL, text.c_str(), -1, &rct, 0, fontColor);  
}

void Graphics::Draw2DValue(int value,int fontType, int posX, int posY, int red, int green, int blue, int alpha){
  std::ostringstream oss;
  std::string text;
  oss << value;
  text = oss.str();
  D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, red, green, blue);
  int width = 0;
  int fontHeight = 0;
  if(fontType < 0 || fontType > 11)
    fontType = 0;

  if(fontType == 0 || fontType == 1 || fontType == 6 || fontType == 7)
    fontHeight = 12;
  else if(fontType == 2 || fontType == 3 || fontType == 8 || fontType == 9)
    fontHeight = 14;
  else if(fontType == 4 || fontType == 5 || fontType == 10 || fontType == 11)
    fontHeight = 16;

  width = text.size() * fontHeight;
  RECT rct;
  rct.left = posX;
  rct.top = posY;
  rct.right = rct.left + width;
  rct.bottom = rct.top + fontHeight;
  m_Font[fontType]->DrawText(NULL, text.c_str(), -1, &rct, 0, fontColor);  
}


//Close Direct3D
int Graphics::CloseD3D()
{
  CLog *pLog = CLog::Instance();

	// Release the textures
	for (int i = 0; i < m_TextureCount; i++)
	{
		if (m_pFile[i])
		{
			m_pFile[i]->Release();
			m_pFile[i] = NULL;
		}
	}
  pLog->Log("Textures released!");
  
  //pLog->Log("C_NUMBER_FONTS", C_NUMBER_FONTS);
  for(int i = 0; i < C_NUMBER_FONTS; i++){
    //pLog->Log("i", i);

    if(m_Font[i]){
      m_Font[i]->Release();
      //pLog->Log("i released", i);
    }
  }
  pLog->Log("Fonts released!");

  //Release vertex buffer
  if (vertexBuffer)	{
    vertexBuffer->Release ();
		vertexBuffer = NULL;
	}
  pLog->Log("Vertex buffer released!");

  //Release device
  if (m_pD3DDevice){
    m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
  pLog->Log("Device released!");

  //Release m_pD3D object
  if (m_pD3D){
		m_pD3D->Release();
		m_pD3D = NULL;
	}
  pLog->Log("m_pD3D object released!");
  if(m_pSprite){
    m_pSprite->Release();
	  m_pSprite = NULL;
	}

  //Successfully closed Direct3D
  return TRUE;
}

void Graphics::DrawLine(float x1, float y1,float x2, float y2, int red, int green, int blue){
  
  LPD3DXLINE line;
  D3DXCreateLine(m_pD3DDevice, &line);
  D3DXVECTOR2 lines[] = {D3DXVECTOR2(x1, y1), D3DXVECTOR2(x2, y2)};
  line->Begin();
  line->Draw(lines, 2, D3DCOLOR_ARGB(255, red, green, blue));
  line->End();
  line->Release();
}

void Graphics::DrawRect(float x1, float y1,float x2, float y2, int red, int green, int blue){
     
  LPD3DXLINE line;
  D3DXCreateLine(m_pD3DDevice, &line);
  D3DXVECTOR2 lines[5];// = {D3DXVECTOR2(x1, y1), D3DXVECTOR2(x2, y2)};
  line->Begin();
  lines[0].x = x1; lines[0].y = y1;
  lines[1].x = x2; lines[1].y = y1;
  lines[2].x = x2; lines[2].y = y2;
  lines[3].x = x1; lines[3].y = y2;
  lines[4].x = x1; lines[4].y = y1;
  line->Draw(lines, 5, D3DCOLOR_ARGB(255, red, green, blue));
  line->End();
  line->Release();

}

//draws circle
void Graphics::DrawCircle(float x, float y, int red, int green, int blue, float radius){

  const int NUMPOINTS = 24;//24
  const float PI = 3.14159;
  struct line_vertex{
      float x, y, z, rhw;
      DWORD color;       
  };
  line_vertex circle[NUMPOINTS + 1];
 
  float theta;
  float wedgeAngle;
  wedgeAngle = (float)((2 * PI)/NUMPOINTS);
  for (int i = 0; i <= NUMPOINTS; i++){
    theta = i * wedgeAngle;
    circle[i].x = (float)(x + radius * cos(theta));
    circle[i].y = (float)(y - radius * sin(theta));
    circle[i].z = 0;
    circle[i].rhw = 1.0f;
    circle[i].color = D3DCOLOR_XRGB(red, green, blue);
  }

  LPD3DXLINE line;
  D3DXCreateLine(m_pD3DDevice, &line);
  D3DXVECTOR2 lines[2];
  for (int i = 0; i < NUMPOINTS; i++){
        lines[0].x = circle[i].x;
        lines[0].y = circle[i].y;
        lines[1].x = circle[i + 1].x;
        lines[1].y = circle[i + 1].y;
        line->Begin();
        line->Draw(lines, 2, D3DCOLOR_ARGB(255, red, green, blue));
        line->End();
  }
  line->Release();
}

//Clear backbuffer
int Graphics::ClearBuffer (int red, int green, int blue, int alpha){ 
  return m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(alpha, red, green, blue), 0.0f, 0);
}

//Present
int Graphics::Present () {
  return m_pD3DDevice->Present (NULL, NULL, NULL, NULL);
}
   
//Begin drawing
int Graphics::BeginDrawing () { return m_pD3DDevice->BeginScene (); }

//End drawing
int Graphics::EndDrawing () { return m_pD3DDevice->EndScene (); }

//*******************************************************************************
// RenderGraphic(...)  - draws defined tile or sprite
//*******************************************************************************
void Graphics::RenderGraphic(int positionX, int positionY, GRAPHIC_IMAGE gi){
  if(gi.fileID > GF_MAX_FILES - 1)
    return;
  
  CLog *pLog = CLog::Instance();
  RECT rect;
  if(gi.frameMax == 0){//non-animated
    rect.left = gi.sourceX;
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  else if(gi.frameMax >0 ){               //animated
    rect.left = gi.sourceX + (gi.width * gi.frameCount);
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  else if(gi.frameMax < 0 ){               //animated
    rect.left = gi.sourceX + (gi.width * abs(gi.frameCount));
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  else{
    rect.left = gi.sourceX;
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  
  m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
//  d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

  D3DXVECTOR2 pos;
  pos.x = positionX;
  pos.y = positionY;
  D3DXVECTOR2 scaling;
  scaling.x = gi.scale;
  scaling.y = gi.scale;
  D3DXVECTOR2 rotcenter;
  rotcenter.x = gi.rotationX * scaling.x;
  rotcenter.y = gi.rotationY * scaling.y;
  D3DXMATRIX mat;
  D3DXMatrixTransformation2D(&mat, NULL, 0.0, &scaling, &rotcenter, gi.rotationAngle, &pos);
  m_pSprite->SetTransform(&mat);  
  
  if(FAILED( m_pSprite->Draw(m_pFile[gi.fileID], &rect, NULL, NULL, D3DCOLOR_ARGB(gi.alpha, 255, 255, 255)) ))
  {
    pLog->Log("Sprite Draw failure!", gi.fileID);
    pLog->Log("Pointer",m_pFile[gi.fileID]);
  }

  D3DXMatrixIdentity(&mat);
  m_pSprite->SetTransform(&mat);
  m_pSprite->End();
 
}

void Graphics::RenderGraphicModulate(int positionX, int positionY, GRAPHIC_IMAGE gi, int red, int green, int blue){
  if(gi.fileID > GF_MAX_FILES - 1)
    return;
 
  RECT rect;
  if(gi.frameMax == 0){//non-animated
    rect.left = gi.sourceX;
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  else if(gi.frameMax >0 ){               //animated
    rect.left = gi.sourceX + (gi.width * gi.frameCount);
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  else if(gi.frameMax < 0 ){               //animated
    rect.left = gi.sourceX + (gi.width * abs(gi.frameCount));
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  else{
    rect.left = gi.sourceX;
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }

/*
  if(gi.frameMax == 0){//non-animated
    rect.left = gi.sourceX;
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  else if(gi.frameMax >0 ){               //animated
    rect.left = gi.sourceX + (gi.width * gi.frameCount);
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }
  else{
    rect.left = gi.sourceX;
    rect.top = gi.sourceY;
    rect.right = rect.left + gi.width;
    rect.bottom = rect.top + gi.height;
  }*/

  
  m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
  m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

  D3DXVECTOR2 pos;
  pos.x = positionX;
  pos.y = positionY;
  D3DXVECTOR2 scaling;
  scaling.x = gi.scale;
  scaling.y = gi.scale;
  D3DXVECTOR2 rotcenter;
  rotcenter.x = gi.rotationX;
  rotcenter.y = gi.rotationY;
  D3DXMATRIX mat;
  D3DXMatrixTransformation2D(&mat, NULL, 0.0, &scaling, &rotcenter, gi.rotationAngle, &pos);
  m_pSprite->SetTransform(&mat);  
  if(FAILED(m_pSprite->Draw(m_pFile[gi.fileID], &rect, NULL, NULL, D3DCOLOR_ARGB(gi.alpha, red, green, blue)) )){
   // g_Log.Write("Sprite Draw failure!", gi.fileID);
  }

  D3DXMatrixIdentity(&mat);
  m_pSprite->SetTransform(&mat);
  m_pSprite->End();

}


//*****************************************************************
// GetTexturePointer(...)
//*****************************************************************
IDirect3DTexture9 * Graphics::GetTexturePointer(int fileID){
  return m_pFile[fileID];
}

//*****************************************************************
// LoadTexture(...)
//*****************************************************************
void Graphics::LoadTexture(std::string fileName, int fileID)
{
    CLog *pLog = CLog::Instance();
    IDirect3DTexture9 *d3dTexture;
    D3DXIMAGE_INFO SrcInfo;			//Optional

    //Use a magenta colorkey
    D3DCOLOR colorkey = 0xFFFF00FF;

    // Load image from file
	if (FAILED(D3DXCreateTextureFromFileEx (m_pD3DDevice, fileName.c_str(), 0, 0, 1, 0, 
          D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 
          colorkey, &SrcInfo, NULL, &d3dTexture)))
    {
      pLog->Log("CreateTexture failed.......");  
      return;// NULL;
    }
   
	//Return the newly made texture
  m_pFile[fileID] = d3dTexture;
	m_TextureCount++;

  pLog->Log("Texture number loaded", fileID);
}


RECT Graphics::LoadRect(float left, float top, float right, float bottom){
  RECT temp;
  temp.left = (long)left;
  temp.top = (long)top;
  temp.right = (long)right;
  temp.bottom = (long)bottom;
  return temp; 
}

int Graphics::GetScreenCenterX(){
  return m_screenCenterX;
}
int Graphics::GetScreenCenterY(){
  return m_screenCenterY;
}
int Graphics::GetScreenWidth(){
  //m_screenWidth = ::GetSystemMetrics(SM_CYSCREEN);
  return m_screenWidth;
}
int Graphics::GetScreenHeight(){
  //m_screenHeight = ::GetSystemMetrics(SM_CXSCREEN);                               
  return m_screenHeight;
}

void Graphics::SetScreenWidthHeight(int w, int h){
 m_screenWidth = w;
 m_screenHeight = h;
 m_screenCenterX = m_screenWidth/2;
 m_screenCenterY = m_screenHeight/2;
}

//captures screen and saves to file
HRESULT Graphics::ScreenGrab(char *fileName){
  HRESULT hr;

  ::D3DDISPLAYMODE mode;
  if(FAILED(hr=m_pD3DDevice->GetDisplayMode(0, &mode)))
    return 100;

  ::LPDIRECT3DSURFACE9 surf;
  if(FAILED(hr=m_pD3DDevice->CreateOffscreenPlainSurface(mode.Width, mode.Height,D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surf, NULL)))
    return 200;

  if(FAILED(hr=m_pD3DDevice->GetFrontBufferData(0, surf))){
    surf->Release();
    return 300;
  }

  hr = ::D3DXSaveSurfaceToFile(fileName, D3DXIFF_JPG, surf, NULL, NULL);
  surf->Release();
  return 0;
}

// Attempt to add particles to the game

void Graphics::InitializePyro(){
	//PyroParticles::PyroGraphics::IDevice *pPyroGraphics = new PyroParticles::CGraphicsDevice_D3D9(g_pd3dDevice, FALSE /* 3D */);
  CLog *pLog = CLog::Instance();
  pLog->Log("step 1");
  try{
    m_bParticlesEnabled = true;//////////test, make false and shoot target
    pPyroGraphics = new PyroParticles::CGraphicsDevice_D3D9(m_pD3DDevice, FALSE );
  }
  catch (PyroParticles::CPyroException &Exception){
		DestroyParticleLibrary(g_pParticleLibrary);
		delete pPyroGraphics;
    pLog->Log("Particles cannot load...disabling particle effects");
    m_bParticlesEnabled = false;
		//MessageBox(NULL, Exception.GetExceptionMessage(), "Error!", MB_OK | MB_ICONEXCLAMATION);
	}
  if(m_bParticlesEnabled == true){
	  pLog->Log("step 2");  
    g_pParticleLibrary = CreateParticleLibrary(PYRO_SDK_VERSION);
  }
  if(m_bParticlesEnabled == true){
	  pLog->Log("step 3");
    g_pParticleLibrary->Init(pPyroGraphics);
  }
  if(m_bParticlesEnabled == true){
    pLog->Log("step 4");
    g_pEmitters = new PyroParticles::CEmitterList();
  }
  if(m_bParticlesEnabled == true){
    pLog->Log("step 5");
    pInitialExplosionFile = g_pParticleLibrary->LoadPyroFile("assets\\effects\\explode1.pyro");//smoke1.pyro");//Pyrodemo.pyro");
    pSmokeFile = g_pParticleLibrary->LoadPyroFile("assets\\effects\\smoke1.pyro");//smoke1.pyro");//Pyrodemo.pyro");
  }
  if(m_bParticlesEnabled == true){
    pLog->Log("step 6");
	  pInitialExplosionFile->CreateTextures();
    pSmokeFile->CreateTextures();
  }
	Time = 0.0f;
	Phase = -100.0f;
  g_StartTime = timeGetTime();
/*
		const PyroParticles::IPyroParticleEmitter *pEmitter = pPyroFile->GetFileEmitter(0);
    //pEmitter = (const) pPyroFile->GetFileEmitter(0);
		pEmitterName = pEmitter->GetName();
*/

}
//x,y is ships center
// type = 0 is explosion, type = 1 is smoke
void Graphics::BeginPyro(float x, float y, int type){
  if(m_bParticlesEnabled == false)
    return;

  if(type < 0 || type > 1)
    type = 1; //smoke is default
  
  if(type == 0){
    const PyroParticles::IPyroParticleEmitter *pEmitter = pInitialExplosionFile->GetFileEmitter(0);
    pEmitterName = pEmitter->GetName();
  }
  else if(type == 1){
    const PyroParticles::IPyroParticleEmitter *pEmitter = pSmokeFile->GetFileEmitter(0);
    pEmitterName = pEmitter->GetName();
  }

  D3DXMATRIX matWorld, matView, matProj;
  D3DXMatrixIdentity(&matWorld);
  D3DXMatrixIdentity(&matView);
  D3DXMatrixTranslation(&matWorld, (float) -(m_screenWidth / 2), (float) -(m_screenHeight / 2), 0.0f);
  D3DXMatrixScaling(&matView, 1.0f, -2.0f, 1.0f);
  D3DXMatrixOrthoLH(&matProj, (float) m_screenWidth, (float) m_screenHeight, -1.0f, 1.0f);

  m_pD3DDevice->SetTransform(D3DTS_WORLD,      &matWorld);
  m_pD3DDevice->SetTransform(D3DTS_VIEW,       &matView);
  m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

  //bool g_bGo = true;
  //here is the magic code to make the effect
  if (Time - Phase > 0.5f)// && g_bGo == true)
  {
	  Phase = Time;

    if(type == 0)//explosion
	    g_pEmitters->Append(pInitialExplosionFile, pEmitterName, Time, x, y);
    else if(type == 1)//smoke
      g_pEmitters->Append(pSmokeFile, pEmitterName, Time, x, y);

    //g_bGo = false;
  }
    
  float PrevTime = Time;
  Time = (timeGetTime() - g_StartTime) * 0.001f;
  float TimeDelta = Time - PrevTime;
  float CurTime = PrevTime;

  while (TimeDelta > 0.0f)
  {
	  float SafeTimeDelta = (TimeDelta >= 0.05f) ? 0.05f : TimeDelta;

	  g_pEmitters->Move(CurTime, SafeTimeDelta);
	  g_pEmitters->Prepare(CurTime, SafeTimeDelta);

	  CurTime += SafeTimeDelta;
	  TimeDelta -= SafeTimeDelta;
  }

  m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
  m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
  m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
  m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
  m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  g_pEmitters->Render(Time);
}

void Graphics::EndPyro(){
  if(m_bParticlesEnabled == false)
    return;

  g_pEmitters->RemoveInactive(Time, 0.1f);
}

void Graphics::ClosePyro(){
  if(m_bParticlesEnabled == false)
    return;

  //pPyroFile->DestroyTextures();
  pInitialExplosionFile->DestroyTextures();
  pSmokeFile->DestroyTextures();

	g_pParticleLibrary->Done();
	delete g_pEmitters;
  //delete pPyroFile;
  delete pInitialExplosionFile;
  delete pSmokeFile;
 	DestroyParticleLibrary(g_pParticleLibrary);
	delete pPyroGraphics;
}