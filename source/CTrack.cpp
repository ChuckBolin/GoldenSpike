#include "../include/CTrack.h"

CTrack::CTrack(){
  x = 0;
  y = 0;
  profileID = -1;//state = 0
  ballastID = -1;//state = 1 
  tieID = -1;    //state = 2
  railID = -1;   //state = 3
  spriteID = -1;
  timeAccum = 0; 
  timeStep = 2.0;
  state = 0; //start with profile
  complete = false;
  uniqueID = -1;

  m_countDown = 0;
  m_bBegin = false;
}

CTrack::CTrack(long tx, long ty, int pid, int bid, int tid, int rid){
  x = tx;
  y = ty;
  profileID = pid;
  ballastID = bid;
  tieID = tid;
  railID = rid;
  spriteID = -1;
  timeAccum = 0; 
  timeStep = 2.0;
  state = 0; //start with profile
  complete = false;

  m_countDown = 0;
  m_bBegin = false;
}

void CTrack::Update(double timeDifference, double timeFactor){
  if(complete == true)
    void;
  
  //manages delay prior to start of construction
  if(m_countDown > 0){
    m_countDown -= (timeDifference * timeFactor);
    if(m_countDown < 0)
      m_countDown = 0;
  }
  else
    timeAccum += timeDifference * timeFactor;

  //change sprite being displayed
  if(timeAccum > 1.0f){//(timeStep/timeFactor)){//time for state change  /*m_bBegin == true && */ 
    timeAccum = 0;
    state++;
    switch(state){
    case 0:
      spriteID = profileID;
      break;
    case 1:
      spriteID = ballastID;
      break;
    case 2:
      spriteID = tieID;
      break;
    case 3:
      spriteID = railID;
      complete = true;
      break;
    }
  }
}
