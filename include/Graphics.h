/*************************************************************************************************
 * GRAPHICS.H                                                  Copyright (c) 2003 Eamonn Doherty *
 *                                                                                               *
 * Direct3D code                                                                                 *
 *************************************************************************************************/
//#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <sstream>
#include "CSprite.h"
#include "CLog.h"
#include "CGlobal.h"
#include "EmitterList.h"
#include "PyroD3D9.h"

//Custom vertex format
const DWORD D3DFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

//Custom vertex
struct TLVERTEX
{
    float x;
    float y;
    float z;
    float rhw;
    D3DCOLOR color;
    float u;
    float v;
};

//file data

//font constants
const int F_A12 = 0;  //Arial
const int F_A12B = 1;
const int F_A14 = 2;
const int F_A14B = 3;
const int F_A16 = 4;
const int F_A16B = 5;
const int F_V12 = 6;  //Verdana
const int F_V12B = 7;
const int F_V14 = 8;
const int F_V14B = 9;
const int F_V16 = 10;
const int F_V16B = 11;
const int F_V20 = 12;
const int F_V20B = 13;
const int C_NUMBER_FONTS = 14;

const int GF_MAX_FILES = 50;
const int GF_SPLASH = 0;
const int GF_BUTTONS = 2;

class Graphics{
public:
  Graphics();
	// SMJ 11/8/2007 Completey overhauled and/or added functions.
  int			InitD3D (HWND hWnd, bool bWindowedMode = false);
  D3DFORMAT		FindDepthStencilFormat( ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType );
  void			CreateDXFonts();

  int CloseD3D ();
  int ClearBuffer (int red, int green, int blue, int alpha = 255);
  int Present();
  int BeginDrawing ();
  int EndDrawing ();
  void LoadTexture(std::string fileName, int fileID);
  void BlitD3D (IDirect3DTexture9 *texture, RECT *rDest, D3DCOLOR vertexcolor = 0xFFFFFFFF,float fRotate = 0);
  void DrawSprite (IDirect3DTexture9 *texture, RECT *rDest, RECT *rSource, float fRotate = 0, D3DCOLOR vertexcolor = 0xFFFFFFFF);
  void BlitExD3D (IDirect3DTexture9 *texture, RECT *rDest, D3DCOLOR *vertexcolors, float rotate = 0);
  void Draw2DText(std::string sText, int fontType, int posX, int posY, int red, int green, int blue, int alpha = 255);
  void Draw2DTextValue(std::string sText, float value, int fontType, int posX, int posY, int red, int green, int blue, int alpha = 255);
  void Draw2DTextValue(std::string sText, std::ostringstream value, int fontType, int posX, int posY, int red, int green, int blue, int alpha = 255);
  void Draw2DTextValue(std::string sText, long value, int fontType, int posX, int posY, int red, int green, int blue, int alpha = 255);
  void DrawCircle(float x, float y, int red, int green, int blue, float radius);
  void DrawLine(float x1, float y1,float x2, float y2, int red, int green, int blue);
  void DrawRect(float x1, float y1,float x2, float y2, int red, int green, int blue);
  RECT LoadRect(float left, float top, float right, float bottom);
  IDirect3DTexture9 * GetTexturePointer(int fileID);//{return m_pFile[fileID];}
  int GetScreenCenterX();
  int GetScreenCenterY();
  int GetScreenWidth();
  int GetScreenHeight();
  void SetScreenWidthHeight(int w, int h);
  void Draw2DValue(int value,int fontType, int posX, int posY, int red, int green, int blue, int alpha);
  void Draw2DValue(float value,int fontType, int posX, int posY, int red, int green, int blue, int alpha);
  void RenderGraphic(int positionX, int positionY, GRAPHIC_IMAGE gi);
  void RenderGraphicModulate(int positionX, int positionY, GRAPHIC_IMAGE gi, int red, int green, int blue);
  HRESULT ScreenGrab(char *fileName);
  void InitializePyro();
  void BeginPyro(float x, float y, int type);
  void EndPyro();
  void ClosePyro();

private:
	// SMJ 11/8/2007 Renamed.
  IDirect3D9*					  m_pD3D;				//Direct3D 9 interface
  IDirect3DDevice9*			m_pD3DDevice;		//Graphics adapter interface
	D3DPRESENT_PARAMETERS	m_D3DPresentParams;	//Direct3D present parameters
  D3DCAPS9					    m_D3DCaps;			//Capabilities of graphics adapter
	ID3DXSprite*				  m_pSprite;			// The drawing surface
	bool						      m_bWindowed;		// true for windowed mode
  IDirect3DVertexBuffer9* vertexBuffer;//Vertex buffer for drawing quads
  ID3DXFont *m_Font[C_NUMBER_FONTS];
	int m_screenWidth;
  int m_screenHeight;
  int m_screenCenterX;
  int m_screenCenterY;
  IDirect3DTexture9 * m_pFile[GF_MAX_FILES];
	int	m_TextureCount;
  int m_fileCount;

  //particles
  PyroParticles::IPyroParticleLibrary *g_pParticleLibrary;
  PyroParticles::PyroGraphics::IDevice *pPyroGraphics;
  PyroParticles::CEmitterList *g_pEmitters;
  //PyroParticles::IPyroParticleEmitter *pEmitter;
  const char *pEmitterName;
  //PyroParticles::IPyroFile *pPyroFile;
  
  PyroParticles::IPyroFile *pInitialExplosionFile;
  PyroParticles::IPyroFile *pSmokeFile;
  bool m_bParticlesEnabled;

	float Time;
	float Phase;
  DWORD	g_StartTime;



};

#endif