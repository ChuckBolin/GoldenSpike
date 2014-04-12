#include "..\include\CAudioManager.h"

CAudioManager *CAudioManager::pInstance = 0;

CAudioManager *CAudioManager::Instance(){
  if(CAudioManager::pInstance == 0)
    CAudioManager::pInstance = new CAudioManager;

  //else
  return CAudioManager::pInstance;
}

//initialize sound system
CAudioManager::CAudioManager(){
  m_bValidAudio = false;

  if( FSOUND_Init(44000,64,0) == false ){
		m_bValidAudio = false;
    return;
  }
  else{
		m_bValidAudio = true;
    m_MaxChannels = FSOUND_GetMaxChannels();
  }
}

//close sound system if it initialized correctly
CAudioManager::~CAudioManager(){
  if(m_bValidAudio == true){

    for(int i = 0; i < m_AudioClip.size(); i++)
      FSOUND_Sample_Free(m_AudioClip[i].handle);

    FSOUND_Close();
  }
}

void CAudioManager::PlaySoundClip(int id, unsigned int mode){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id){
      if(mode == 0)
        FSOUND_Sample_SetMode(m_AudioClip[i].handle, FSOUND_LOOP_OFF);
      else
        FSOUND_Sample_SetMode(m_AudioClip[i].handle, FSOUND_LOOP_NORMAL);
        
      //m_AudioClip[i].channelPlaying = FSOUND_PlaySound(FSOUND_FREE, m_AudioClip[i].handle);
      m_AudioClip[i].channelPlaying = FSOUND_PlaySound(id, m_AudioClip[i].handle);

    }
  }
}

void CAudioManager::PauseAll(){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(FSOUND_IsPlaying(m_AudioClip[i].channelPlaying) == true)
      FSOUND_SetPaused(m_AudioClip[i].channelPlaying, true);
  }
}

void CAudioManager::UnPauseAll(){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(FSOUND_GetPaused(m_AudioClip[i].channelPlaying) == true)
      FSOUND_SetPaused(m_AudioClip[i].channelPlaying, false);
  }
}

int CAudioManager::GetVolume(int id){
  if(m_bValidAudio == false)
    return 0;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id && IsPlaying(id)==true){
      return FSOUND_GetVolume(m_AudioClip[i].channelPlaying);
    }
  }
  return 0;
}

void CAudioManager::SetVolume(int id, int vol){
  if(m_bValidAudio == false)
    return;
  if(vol < 0)
    vol = 0;
  if(vol > 255)
    vol = 255;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id && IsPlaying(id) == true){
      if(!FSOUND_SetVolumeAbsolute(m_AudioClip[i].channelPlaying, vol)){
        return;
      }
      else
        return;
    }
  }//for
}

bool CAudioManager::IsPlaying(int id){
  if(m_bValidAudio == false)
    return false;

  FSOUND_SAMPLE *handle;
  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id && m_AudioClip[i].channelPlaying > -1){
      if(FSOUND_IsPlaying(m_AudioClip[i].channelPlaying)==true){
        return true;
      }
      else{
        m_AudioClip[i].channelPlaying = -1;
        return false;
      }
    }//if
  }//for
 
  return false;
}

void CAudioManager::KillAll(){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(FSOUND_IsPlaying(i)==true){
      StopSoundClip(i);
    }
    else{
    }
  }

  for(int i = 0; i < m_AudioClip.size(); i++){
    StopStream(i);
  }


  //FSOUND_StopSound(FSOUND_ALL);
  //::Sleep(2000);
}

bool CAudioManager::IsValidSound(){
  return m_bValidAudio;
}

int  CAudioManager::GetMaxChannels(){
  return m_MaxChannels;
}

void CAudioManager::LoadSample(int id, std::string filename){
  if(m_bValidAudio == false)
    return;

  AUDIO_VEC temp;
  FSOUND_SAMPLE *handle;
  handle=FSOUND_Sample_Load (id,filename.c_str(),FSOUND_HW3D, 0, 0);// | FSOUND_LOOP_NORMAL,0,0);
  //FSOUND_2D | FSOUND_LOOP_NORMAL
  if(handle == NULL){ //error
    return;
  }
  else{               //good
    temp.AudioID = id;
    temp.handle = handle;
    temp.filename = filename;
    temp.channelPlaying = -1;
    m_AudioClip.push_back(temp);
  }
}

void CAudioManager::StopSoundClip(int id){
  if(FSOUND_IsPlaying(id)==true)
    FSOUND_StopSound(id);
  return;

  if(m_bValidAudio == false)
    return;
  FSOUND_SAMPLE *handle;
  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id){
      for(int j=0; j < GetMaxChannels(); j++){
        handle = FSOUND_Sample_Get(j);
        
        if(handle == m_AudioClip[i].handle){
          if(j > -1)
            FSOUND_StopSound(j - 1);
        }
      }//for
    }//if
  }
}

void CAudioManager::ListAll(){
  if(m_bValidAudio == false)
    return;
  
  for(int i = 0; i < m_AudioClip.size(); i++){
    std::cout << "Clip: (" << i << ") " << m_AudioClip[i].AudioID
              << ", " << m_AudioClip[i].handle << ", " 
              << m_AudioClip[i].filename << std::endl;
  }  
}

int CAudioManager::Size(){
  return m_AudioClip.size();
}

std::string CAudioManager::GetFilename(int id){
  if(m_bValidAudio == false)
    return "";
  
  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id){
      //std::string name = FSOUND_Sample_GetName(m_AudioClip[i].filename);// .handle);
      std::string name = m_AudioClip[i].filename;
      return name;
    }
  }  
  return "";
}

void CAudioManager::Update(double timeDifference){
 // CLog *pLog = CLog::Instance();
  if(m_bValidAudio == false)
    return;
 // pLog->Log("audio 1");
  int volume = 0;

  for(int i = 0; i < m_AudioClip.size(); i++){
    
    //fade sound out
    if(m_AudioClip[i].bFadeout == true){// && IsPlaying(id) == true){
      volume = GetVolume(i);
      if(volume > 0){
        volume -= m_AudioClip[i].fadeStep;// 5;
        SetVolume(i, volume);
      }
      else{
        m_AudioClip[i].bFadeout = false;
        StopSoundClip(i);
      }
    }

    //fade sound int
    if(m_AudioClip[i].bFadein == true){// && IsPlaying(i) == false){
      volume = GetVolume(i);
      if(volume < 255){
        volume += m_AudioClip[i].fadeStep;// 5;
        SetVolume(i, volume);
      }
      else
        m_AudioClip[i].bFadein = false;
    }


  }//for
  FSOUND_Update();
}

void CAudioManager::Fadeout(int id, int targetVolume, float time){
  if(m_bValidAudio == false)
    return;

  int currentVolume = 0;
  double timeStep = 0;
  double volumeStep = 0;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id && IsPlaying(id) == true){
      m_AudioClip[i].bFadeout = true;
      
      m_AudioClip[i].targetVolume = targetVolume;
      currentVolume = GetVolume(i);
      if(targetVolume < currentVolume && time > 0){
        timeStep = time/ 0.1;
        volumeStep = (double)(currentVolume - targetVolume) / timeStep;
        m_AudioClip[i].fadeStep = volumeStep;
        m_AudioClip[i].bFadein = true;
      }

    }
  }//for
}

void CAudioManager::Fadein(int id, int targetVolume,  float time){
  if(m_bValidAudio == false)
    return;

  int currentVolume = 0;
  double timeStep = 0;
  double volumeStep = 0;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id){// && IsPlaying(id) == true){
      if(IsPlaying(i) == false){
        PlaySoundClip(i, 0);
        SetVolume(i, 0);
      }
      m_AudioClip[i].targetVolume = targetVolume;
      currentVolume = GetVolume(i);
      if(targetVolume > currentVolume && time > 0){
        timeStep = time/ 0.1;
        volumeStep = (double)(targetVolume - currentVolume) / timeStep;
        m_AudioClip[i].fadeStep = volumeStep;
        m_AudioClip[i].bFadein = true;
      }
    }
  }//for
}

void CAudioManager::HoldVolume(int id){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_AudioClip.size(); i++){
    if(m_AudioClip[i].AudioID == id){// && IsPlaying(id) == true){
      m_AudioClip[i].bFadein = false;
      m_AudioClip[i].bFadeout = false;
    }
  }
}


//***************************************************
// STREAMS   STREAMS   STREAMS    STREAMS   STREAMS
//***************************************************

void CAudioManager::LoadStream(int id, std::string filename){
  if(m_bValidAudio == false)
    return;

  STREAM temp;
  FSOUND_STREAM *handle = NULL;
  handle = FSOUND_Stream_Open(filename.c_str(),FSOUND_2D,0,0);
  if(handle == NULL){ //error
    return;
  }
  else{               //good
    temp.AudioID = id;
    temp.handle = handle;
    temp.filename = filename;
    //temp.channelPlaying = -1;
    temp.streamSizeBytes = FSOUND_Stream_GetLength(handle);
    temp.streamSizeMs = FSOUND_Stream_GetLengthMs(handle);
    m_Stream.push_back(temp);
  }
}

//count = 0 is infinte
void CAudioManager::PlayStream(int id, int count){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_Stream.size(); i++){
    if(m_Stream[i].AudioID == id){
      FSOUND_Stream_SetLoopCount(m_Stream[i].handle, count);
      FSOUND_Stream_Play(m_Stream[i].AudioID, m_Stream[i].handle);
    }
  }
}

void CAudioManager::StopStream(int id){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_Stream.size(); i++){
    if(m_Stream[i].AudioID == id){
      FSOUND_Stream_Stop(m_Stream[i].handle);
    }
  }
}

int CAudioManager::GetNumberStreams(){
  return m_Stream.size();
}


unsigned int CAudioManager::GetStreamPositionBytes(int id){
  if(m_bValidAudio == false)
    return - 1;

  for(int i = 0; i < m_Stream.size(); i++){
    if(m_Stream[i].AudioID == id){
      return FSOUND_Stream_GetPosition(m_Stream[i].handle);
    }
  }
  return -1;
}

int CAudioManager::GetStreamPositionTime(int id){
  if(m_bValidAudio == false)
    return -1;

  for(int i = 0; i < m_Stream.size(); i++){
    if(m_Stream[i].AudioID == id){
      return FSOUND_Stream_GetTime(m_Stream[i].handle);
    }
  }
  return -1;
}

unsigned int CAudioManager::GetStreamLengthBytes(int id){
  if(m_bValidAudio == false)
    return -1;

  for(int i = 0; i < m_Stream.size(); i++){
    if(m_Stream[i].AudioID == id){
      return m_Stream[i].streamSizeBytes;
    }
  }
  return -1;
}

int CAudioManager::GetStreamLengthTime(int id){
  if(m_bValidAudio == false)
    return -1;

  for(int i = 0; i < m_Stream.size(); i++){
    if(m_Stream[i].AudioID == id){
      return m_Stream[i].streamSizeMs;
      //return FSOUND_Stream_GetLengthMs(m_Stream[i].handle);
    }
  }
  return -1;
}

void CAudioManager::SetStreamPositionBytes(int id, unsigned int position){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_Stream.size(); i++){
    if(m_Stream[i].AudioID == id){
      FSOUND_Stream_SetPosition(m_Stream[i].handle, position);
    }
  }
}

void CAudioManager::SetStreamPositionTime(int id, int position){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_Stream.size(); i++){
    if(m_Stream[i].AudioID == id){
      FSOUND_Stream_SetTime(m_Stream[i].handle, position);
    }
  }
}

void CAudioManager::AddStreamClip(int clipID, int audioID, int beginMs, int endMs){
  if(m_bValidAudio == false)
    return;

  STREAM_CLIP sc;
  sc.AudioID = audioID;
  sc.ClipID = clipID;
  sc.beginMs = beginMs;
  sc.endMs = endMs;
  m_StreamClip.push_back(sc);
}

void CAudioManager::PlayStreamClip(int clipID){
  if(m_bValidAudio == false)
    return;

  for(int i = 0; i < m_StreamClip.size(); i++){
    if(m_StreamClip[i].ClipID == clipID){
      SetStreamPositionTime(m_StreamClip[i].AudioID, m_StreamClip[i].beginMs);
      PlayStream(m_StreamClip[i].AudioID, 0);
    }
  }
}

/*
void CAudioManager::SetListener(const float *pos, const float *vel,float fx, float fy, float fz, float tx, float ty, float tz){
  m_pPosition3D = pos;
  m_pVelocity3D = vel;
  //FSOUND_3D_Listener_SetAttributes(pos, vel, fx, fy, fz, tx, ty, tz);
  int channel1 = FSOUND_PlaySoundEx(1, m_AudioClip[0].handle, NULL, TRUE);
  FSOUND_3D_SetAttributes(1, pos, vel);

}*/

bool CAudioManager::LoadAssetFile(std::string filename){
  CLog *pLog = CLog::Instance();

  //read assets.dat file
  CFileReader cfr(filename);
  if(cfr.IsValid() == false)
    return false; //file is invalid or contains no usable data

  //validate asset data and add to appropriate vectors
  std::string parameter;
  std::string assetType;
  SOUND_FILE_DATA     tempSound;

  for(int i = 0; i < cfr.GetNumberOfLines(); ++i){

    //process only lines with 2 or more terms
    if(cfr.GetNumberOfTermsFromLine(i) > 1){
      assetType = cfr.GetTerm(cfr.GetLineFromFile(i), 1);
     
      if(assetType == "sound" && cfr.GetNumberOfTermsFromLine(i) == 4){
        parameter = cfr.GetTerm(cfr.GetLineFromFile(i), 2);//sound ID
        tempSound.soundID = atoi(parameter.c_str());
        parameter = cfr.GetTerm(cfr.GetLineFromFile(i), 3);//filename
        tempSound.filename = parameter;
        parameter = cfr.GetTerm(cfr.GetLineFromFile(i), 4);//description
        tempSound.description = parameter;
        m_assetSoundData.push_back(tempSound);
        //pLog->Log("****** soundfile added *******");
        //pLog->Log("soundID", tempSound.soundID);
        //pLog->Log("filename", tempSound.filename);
        //pLog->Log("description", tempSound.description);        
      }
    }
  }

  //load sound information
  for(int i = 0; i < m_assetSoundData.size(); ++i){
    LoadSample(m_assetSoundData[i].soundID,
               "assets\\sounds\\" + m_assetSoundData[i].filename);
  }

  return true;
}