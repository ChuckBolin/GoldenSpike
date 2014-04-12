//CObject.h
//base class...all other game states are derived from here
#ifndef OBJECT_H
#define OBJECT_H
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>

/*
struct GAME_OBJECT{
  int asset;        //0 = mesh, 1 = splatter
  int meshID;       //unique ID
  int meshBadID;    //ID of mesh used when damaged
  D3DXVECTOR3 pos;
  std::string name;
  float x;          //position
  float y;
  float z;
  float rx;         //rotational angles - orientation
  float ry;
  float rz;
  float collisionDiameter; 
  float direction;
  int behavior;
  bool alive;
  bool dying;
  int spriteID;
  int spriteBadID;
  float turnRate;
  int soundID;
  D3DXVECTOR3 vel;
  int alpha;
  float angle;
  int width;
  int height;
  int left;
  int top;
  int frameCount;
  double totalTime;
  int maxFrames;
  double updateInterval;
  int objectID;
  int layer;
  float radius;
  float speed;
  int mode;
  bool bInAir;
};

struct GAME_OBJECT_TYPE{
  std::string name;
  int objectID;
  int spriteID;
  int badSpriteID;
  int layer;
  float radius;
  int behavior;
  int soundID;
  int frameCount;

};*/

class CObject{
public:
  virtual void Update(double timeDifference) = 0;
//  GAME_OBJECT go;
  virtual void Move(int vx, int vy) = 0;

/*
  void MoveLeft();

  void JumpUp();
  void Duck();
*/
};

#endif //GAME_STATE_OBJECT_H