#ifndef CTRAIN_H
#define CTRAIN_H

#include <vector>
#include <cmath>
#include "CFizzix.h"

struct WAYPOINT{
  float x;
  float y;
  float angle;
};

class CTrain{
public:
  CTrain();
  void AddWaypoint(float x, float y, float angle);  //adds waypoints to m_waypoint
  void Update(double timeDifference);               //updates position
  void ResetWaypoints();                            //clears waypoints
  void Start(); //start motion
  void Stop();  //end motion
  void Pause(); //pause motion
  void Reset(); //reset position
  void SetVelocity(float vel);
  float GetPosX(){return m_x;}
  float GetPosY(){return m_y;}
  float GetAngle(){return m_angle;}
  int GetState(){return m_state;}
  int GetWaypoint(){return m_wpIndex;}
  int GetNumberWaypoints();
  float GetWaypointX(int index);
  float GetWaypointY(int index);
  WAYPOINT GetWaypoint(int index);

private:
  float m_x;
  float m_y;
  float m_angle;
  float m_velocity;
  float m_dir;          //1 or 0 1 is positive for 2D 0 is negative for 2D
  int m_state;          //0=do nothing 1=run 2 = pause 3 = stop

  std::vector<WAYPOINT> m_waypoint; //stores waypoints
  int m_wpIndex;                    //index to m_waypoint


};

#endif