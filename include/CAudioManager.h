//****************************************************************************
// Class: CAudioManager
// Purpose: Manages FMOD Library for Chuck's Game, September 23, 2007
// Websites used as references:
//		http://www.fmod.org/docs/HTML/	FMOD Organization
//		http://www.robthebloke.org/		Tutorials
//
//  FMOD 3 is the tried and true engine that started it all!
//  FMOD 3 is still supported and is mature (fmod has shipped in hundreds
//  of titles that have sold millions of units) and has all the basic audio 
//  features needed for your title!
//  http://www.fmod.org/index.php/release/version/fmodapi375win.zip
//
//link to fmod.lib
//add fmod.dll to current folder for distributions
//Revision:
//  March 28, 2008  Added fade in and fade out features  CB
//  March 29, 2008  Add stream support
//  June 16, 2008   Loaded assets from file.
// 
//  For small sounds such as sound effects that you want to trigger 
//  multiple times at once (for example a gunshot), then you will
//  want to use Sound. 
//  If the file is going to be big, and doesnt need to be played 
//  multiple times at once (note the next option cannot be played 
//  multiple times at once), then the other choice is to use Stream.
//****************************************************************************

//CAudioManager.h
#ifndef CAUDIOMANAGER_H
#define CAUDIOMANAGER_H

#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <string>
#include <cstring>
#include <vector>
#include "fmod.h"
#include "fmod_errors.h"
#include "CTimer.h"
#include "CLog.h"
#include "CFileReader.h"

struct AUDIO_VEC{
  int AudioID;
  FSOUND_SAMPLE *handle;
  std::string filename;
  int channelPlaying;
  bool bFadeout;
  bool bFadein;
  double fadeStep; //volume inc/dec per 0.1 sec update() call
  int targetVolume;//volume to fade in to or out to
};

struct STREAM{
  int AudioID;
  FSOUND_STREAM *handle;
  std::string filename;
  int streamSizeMs;
  unsigned int streamSizeBytes;
};

struct STREAM_CLIP{
  int ClipID;
  int AudioID;
  int beginMs;
  int endMs;
};

struct VECTOR3D{
  float x;
  float y;
  float z;
};

//file loading
struct SOUND_FILE_DATA{
  int soundID;
  std::string filename;
  std::string description;
};

class CAudioManager{
public:
  static CAudioManager *Instance();
  bool IsValidSound();
  int GetMaxChannels();
  void LoadSample(int id, std::string filename);
  void PlaySoundClip(int id, unsigned int mode);
  void StopSoundClip(int id);
  void KillAll();
  void ListAll();
  bool IsPlaying(int id);
  std::string GetFilename(int id);
  void PauseAll();
  void UnPauseAll();
  void SetVolume(int id, int vol);
  int GetVolume(int id);
  int Size();
  bool LoadAssetFile(std::string filename);

  //added 3.28.08 - Chuck Bolin
  void Update(double timeDifference);
  void Fadeout(int id, int targetVolume,  float time);
  void Fadein(int id, int targetVolume,  float time);
  void HoldVolume(int id);

  //stream stuff
  void LoadStream(int id, std::string filename);
  void PlayStream(int id, int count );
  void StopStream(int id);
  int GetNumberStreams();
  unsigned int GetStreamPositionBytes(int id);
  int GetStreamPositionTime(int id);
  unsigned int GetStreamLengthBytes(int id);
  int GetStreamLengthTime(int id);
  void SetStreamPositionBytes(int id, unsigned int position);
  void SetStreamPositionTime(int id, int position);
  void AddStreamClip(int clipID, int audioID, int beginMs, int endMs);
  void PlayStreamClip(int clipID);
 // void SetListener(const float *pos, const float *vel,float fx, float fy, float fz, float tx, float ty, float tz);
protected:
  CAudioManager();
  ~CAudioManager();

private:
  static CAudioManager *pInstance;
  bool m_bValidAudio;
  int m_MaxChannels;
  std::vector<AUDIO_VEC> m_AudioClip;
  std::vector<STREAM> m_Stream;
  std::vector<STREAM_CLIP> m_StreamClip;
  const float *m_pPosition3D;
  const float *m_pVelocity3D;
  std::vector<SOUND_FILE_DATA> m_assetSoundData;//stores loaded asset data
};

#endif

