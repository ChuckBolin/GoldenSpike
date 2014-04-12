#include "../include/CTrain.h"

CTrain::CTrain(){
  m_x = 0;
  m_y = 0;
  m_angle = 0;
  m_velocity = 30;
  m_dir = 1;
  m_state = 1;
  m_wpIndex = -1;

}

void CTrain::AddWaypoint(float x, float y, float angle){  //adds waypoints to m_waypoint
  WAYPOINT temp;
  temp.x = x;
  temp.y = y;
  temp.angle = angle;
  m_waypoint.push_back(temp);
}

WAYPOINT CTrain::GetWaypoint(int index){
  WAYPOINT temp;
  temp.x = 0;
  temp.y = 0;
  temp.angle = 0;
  if(index < 0 || index > m_waypoint.size())
    return temp;
  temp = m_waypoint[index];
  return temp;
}

void CTrain::Update(double timeDifference){
  CFizzix *pFizzix = CFizzix::Instance();
  
  //important to reset position to match waypoint after waypoint indexes
  static reset = false;

  //1st time here
  if(m_waypoint.size() < 1)
    return;


  //setup 1st time
  if(m_wpIndex == -1){
    m_wpIndex = 0;
    m_x = m_waypoint[m_wpIndex].x;
    m_y = m_waypoint[m_wpIndex].y;
    m_angle = m_waypoint[m_wpIndex].angle; 
  }

  int nextIndex = m_wpIndex + 1;
  if(nextIndex > m_waypoint.size() - 1){
    m_wpIndex = -1;
    return;
  }

  //current waypoint position and angle
  float currentX = m_waypoint[m_wpIndex].x;
  float currentY = m_waypoint[m_wpIndex].y;  
  float currentAngle = m_waypoint[m_wpIndex].angle;

  //next waypoint position and scale
  float nextX = m_waypoint[nextIndex].x;
  float nextY = m_waypoint[nextIndex].y;
  float nextAngle = m_waypoint[nextIndex].angle;  

  //orientation of train
  m_angle = currentAngle;
  float dx = nextX - currentX;
  float dy = nextY - currentY;
  float vel = m_velocity * timeDifference;
  float offset = 40;// 10;
  float targetAngle = pFizzix->CalcGraphAngle(m_x, -m_y, m_waypoint[nextIndex].x, -m_waypoint[nextIndex].y);
  m_x = m_x + cos(currentAngle) * vel;
  m_y = m_y + sin(currentAngle) * vel;
  if(m_x > m_waypoint[m_wpIndex].x - offset && m_x < m_waypoint[m_wpIndex].x + offset &&//23
     m_y > m_waypoint[m_wpIndex].y - offset && m_y < m_waypoint[m_wpIndex].y + offset){
    m_x = m_waypoint[m_wpIndex].x;
    m_y = m_waypoint[m_wpIndex].y;
    m_wpIndex++;
  }
}

/*
  float angle;
  CFIZZIX_POINT pt1;
  CFIZZIX_POINT pt2;
  CFIZZIX_POINT pt3;
  float targetAngle;
  
  
  if(m_wpIndex == 0){
    m_x = m_waypoint[m_wpIndex].x;
    m_y = m_waypoint[m_wpIndex].y;
    m_angle = m_wayPoint[m_wpIndex].angle;
    m_wpIndex++;
  }
  else{
    targetAngle = pFizzix->CalcGraphAngle(m_x, -m_y, m_waypoint[m_wpIndex + 1].x, -m_waypoint[m_wpIndex + 1].y);
    m_angle = targetAngle;
    m_x = m_x + cos(m_angle) * vel;
    m_y = m_y + sin(m_angle) * vel;
    if(m_x > m_waypoint[m_wpIndex].x - 30 && m_x < m_waypoint[m_wpIndex].x + 30 &&
      m_y > m_waypoint[m_wpIndex].y - 30 && m_y < m_waypoint[m_wpIndex].y + 30){
        targetAngle = pFizzix->CalcGraphAngle(m_x, -m_y, m_waypoint[m_wpIndex + 1].x, -m_waypoint[m_wpIndex + 1].y);
        m_wpIndex ++;
        if(m_wpIndex > (int)m_waypoint.size()){
          m_state = 0;
          m_wpIndex = 0;
        }
    }
  }
}
*/

/*
void CTrain::Update(double timeDifference){
  CFizzix *pFizzix = CFizzix::Instance();

  float angle;
  CFIZZIX_POINT pt1;
  CFIZZIX_POINT pt2;
  CFIZZIX_POINT pt3;
  float targetAngle;
  float vel = m_velocity * timeDifference;
  if(m_wpIndex == 0){
    m_x = m_waypoint[m_wpIndex].x;
    m_y = m_waypoint[m_wpIndex].y;
    m_wpIndex++;
  }
  else{
    targetAngle = pFizzix->CalcGraphAngle(m_x, -m_y, m_waypoint[m_wpIndex + 1].x, -m_waypoint[m_wpIndex + 1].y);
    m_angle = targetAngle;
    m_x = m_x + cos(m_angle) * vel;
    m_y = m_y + sin(m_angle) * vel;
    if(m_x > m_waypoint[m_wpIndex].x - 30 && m_x < m_waypoint[m_wpIndex].x + 30 &&
      m_y > m_waypoint[m_wpIndex].y - 30 && m_y < m_waypoint[m_wpIndex].y + 30){
        targetAngle = pFizzix->CalcGraphAngle(m_x, -m_y, m_waypoint[m_wpIndex + 1].x, -m_waypoint[m_wpIndex + 1].y);
        m_wpIndex ++;
        if(m_wpIndex > (int)m_waypoint.size()){
          m_state = 0;
          m_wpIndex = 0;
        }
    }
  }
}
*/
void CTrain::ResetWaypoints(){
  m_waypoint.clear();
}

void CTrain::Start(){m_state = 1;} //start motion
void CTrain::Stop(){m_state = 2;}  //end motion
void CTrain::Pause(){m_state = 3;} //pause motion
void CTrain::Reset(){m_state = 0;m_wpIndex = -1;} //reset position
  
void CTrain::SetVelocity(float vel){
  m_velocity = vel;
}

int CTrain::GetNumberWaypoints(){
  return m_waypoint.size();
}

float CTrain::GetWaypointX(int index){
  if(index < 0 || index > m_waypoint.size())
    return 0.0f;

  return m_waypoint[index].x;
}

float CTrain::GetWaypointY(int index){
  if(index < 0 || index > m_waypoint.size())
    return 0.0f;

  return m_waypoint[index].y;
}
