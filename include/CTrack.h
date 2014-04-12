#ifndef CTRACK_H
#define CTRACK_H

class CTrack{
public:
  CTrack();
  CTrack(long tx, long ty, int pid, int bid, int tid, int rid);
  void Update(double timeDifference, double timeFactor);
  //sprite ID
  long x;
  long y;
  int profileID;
  int ballastID;
  int tieID;
  int railID;
  int spriteID;     //current ID
  double timeAccum; //accumulates time during construction
  double timeStep;  //state changes each time timeStep is reached
  int state; //0=profile, 1=ballast, 2= tie, 3=rail
  bool complete; //true = construction complete
  int uniqueID; //each track has a unique ID
  //this is for delayed construction of a track
  bool m_bBegin;       //true means it may be updated (constructed) false is ignore
  double m_countDown;  //this timer count down before construction begins
};

#endif