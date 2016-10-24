#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include "GameConstants.h"
class StudentWorld;
/////////////////////////////////////////////////////////////////////////////////////
// Base Class

class Actor : public GraphObject
{
 public:
  Actor(StudentWorld* access, int imageID, int startX, int startY, Direction dir,
  	double size, unsigned int depth);

  // Base functions
  virtual void doSomething() = 0;
  virtual void annoyed(){}

  // Accessors
  StudentWorld* getWorld() const {return ptrWorld;}
  bool alive() const {return status;}
  int X() const {return getX();}
  int Y() const {return getY();}

  // Mutators
  void changeStatus(bool condition);
  void changeStudentWorldPtr(StudentWorld* Pointer)
  {ptrWorld = Pointer;}

  // Functions used for invariants
  virtual bool boulderObjects() const {return false;}
  virtual bool oilObjects() const {return false;}
  virtual bool nuggetObjects() const {return false;}
  virtual bool protesterObject()  const {return false;}
  virtual void quit(bool condition){};

  // Auxiliary Functions
  void sound(const int soundCode);
  bool inRangeWithFrackMan(double const condition);
  virtual void illuminate() {};
  virtual void useDirection(int changeDir){}
  virtual ~Actor(){}
  
 private:
  bool status;
  StudentWorld* ptrWorld;
};
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Agents

class Agent : public Actor
{
 public:
  Agent(StudentWorld* access, int imageID, int startX, int startY,
	Direction dir, unsigned int hitPoints);
  virtual void doSomething() = 0;
  virtual ~Agent(){}
};

/////////////////////////////////////////////////////////////////////////////////////

class FrackMan : public Agent
{
 public:
  FrackMan(StudentWorld* access);
  void doSomething();
  void annoyed();

  // Inventory
  int getSonars() const {return m_sonars;}
  int getNuggets() const {return m_gold;}
  int getAmmo() const {return ammo;}
  int health() const {return m_health;}

  // Update Inventory
  void addSonars(const int amount) {m_sonars += amount;}
  void addAmmo(const int amount) {ammo += amount;}
  void addNuggets(const int amount) {m_gold += amount;}
  
  ~FrackMan();
 private:
  int m_health;
  int ammo;
  int m_sonars;
  int m_gold;
};

class Protester : public Agent
{
 public:
  Protester(StudentWorld* access, int imageID, int ticksToWait);
  virtual void doSomething();

  // Part of doSomething body
  bool waiting();
  bool annoyFrackMan();
  bool sightfrackman();
  void chasefrackman();
  void changeDirection();
  void intersection();
  bool move();
  
  // Leave-the-oil-field state
  void quit(bool condition);
  bool quitting(){return leaving_condition;}
  void leaving();
  
  virtual void findsNugget();
  virtual void annoyed();

  // Auxiliary functions
  virtual bool protesterObject() const {return true;}
  void useDirection(int changeDir);
  ~Protester();
 private:
  int m_health;
  bool leaving_condition;
  int counter;
  int intervals;
  StudentWorld* studentworld;
  int onedirection;
  bool initializer;
  int countShout;
  int countPerpendicular;
  int tracker;
  GraphObject::Direction facing;
};


class HardcoreProtester : public Protester
{
 public:
  HardcoreProtester(StudentWorld* access, int imageID, int ticksToWait);
  bool protesterObject() {return true;}
  ~HardcoreProtester();
 private:
  int m_health = 20;
};

/////////////////////////////////////////////////////////////////////////////////////
// Squirt

class Squirt : public Actor
{
 public:
  Squirt(StudentWorld* access, int startX, int startY, Direction dir);
  void doSomething();
  ~Squirt();

 private:
  int countDistance;
  StudentWorld* studentworld;
};

/////////////////////////////////////////////////////////////////////////////////////
// Dirt

class Dirt : public Actor
{
 public:
  Dirt(StudentWorld* access, int imageID, int startX, int startY);
  void doSomething();
  ~Dirt();
};

/////////////////////////////////////////////////////////////////////////////////////
// Boulder

class Boulder : public Actor
{
 public:
  Boulder(StudentWorld* access, int startX, int startY);
  void doSomething();
  bool stable() const;
  bool waiting();
  bool falling();
  void kill();
  bool boulderObjects() const {return true;}
  ~Boulder();

 private:
  StudentWorld* studentworld;
  int counter;
  bool unstable;
};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Activating Objects

class ActivatingObject : public Actor
{
 public:
  ActivatingObject(StudentWorld* world, int imageID, int startX, int startY,
		   Direction dir);
  virtual void doSomething() = 0;
  bool inRangeWithFrackMan(double const condition);
  void timeToLive();
  bool timeUp();
  void gotGoodie(const int update);
  virtual ~ActivatingObject(){}

 private:
  int counter;
  int timeLeft;
  StudentWorld* studentworld;
};

/////////////////////////////////////////////////////////////////////////////////////


class Oil : public ActivatingObject
{
 public:
  Oil(StudentWorld* access, int startX, int startY);
  void doSomething();
  bool checkFrackMan();
  bool oilObjects() const {return true;}
  void illuminate() {setVisible(true); alreadySeen = true;}
  ~Oil();
 private:
  bool alreadySeen;
};

class Sonar : public ActivatingObject
{
 public:
  Sonar(StudentWorld* access);
  void doSomething();
  ~Sonar();
  
 private:
  StudentWorld* studentworld;
};

class Nuggets : public ActivatingObject
{
 public:
  Nuggets(StudentWorld* access, int startX, int startY);
  bool nuggetObjects () const {return true;}
  void doSomething();
  void illuminate() {setVisible(true); alreadySeen = true;}
  ~Nuggets() {};
 private:
  bool alreadySeen;
};

class BribeNuggets : public ActivatingObject
{
 public:
  BribeNuggets(StudentWorld* access, int startX, int startY);
  void doSomething();
  ~BribeNuggets() {}
 private:
  StudentWorld* studentworld;
  int counter;

};

class WaterPool : public ActivatingObject
{
 public:
  WaterPool(StudentWorld* access, int startX, int startY);
  void doSomething();
  ~WaterPool();
 private:
  StudentWorld* studentworld;
};

#endif // ACTOR_H_
