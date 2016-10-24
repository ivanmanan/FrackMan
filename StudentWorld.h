#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>

class Actor;
class FrackMan;
class Protester;
class HardcoreProtester;
class Sonar;

class Boulder;
class Nuggets;
class BribeNuggets;
class Oil;

class StudentWorld : public GameWorld
{
 public:
  StudentWorld(std::string assetDir)
    : GameWorld(assetDir){}
  virtual int init();
  virtual int move();
  virtual void cleanUp();

  // Dirt
  void generateDirtField();
  void deleteDirt(const int x, const int y);
  bool dirtBeneathBoulder(const int x, const int y);
  bool isThereDirt(const int x, const int y);
  bool noDirt(const int x, const int y);

  // FrackMan's auxiliary functions
  int frackmanX() const;
  int frackmanY() const;
  void updateInventory(const int indicator);
  void fire(const int x, const int y, GraphObject::Direction dir);
  void dropGold(const int x, const int y);
  void radar(const int x, const int y);

  // Distances
  double distance(const int x1, const int y1, const int x2, const int y2);
  bool checkDistance(const int x1, const int y1);
  bool distanceIsGood(const int x1, const int y1, const int x2, const int y2,
		      const double condition);

  // Distribute resources
  void generateNuggetsOil(const int numberOfNO, const int indicator);
  void generateBoulders(const int numberOfBoulders);
  void generateWS();

  // Distribute protesters
  void generateProtesters();

  // Invariant
  bool boulderIsPresent(const int x, const int y);

  // Protester's auxiliary functions
  bool frackmanSighting(const int px, const int py);
  bool horizontalDB(const int left, const int right, const int vert);
  bool verticalDB(const int down, const int up, const int horiz);
  bool checkRight(const int px, const int py);
  bool checkLeft(const int px, const int py);
  bool checkUp(const int px, const int py);
  bool checkDown(const int px, const int py);

  // Conflict functions
  void pulverize(const int bx, const int by);
  bool protesterHitfrackman();
  bool bribeProtester(const int nx, const int ny);
  bool squirtHitProtester(const int x, const int y);
  
  // Screen text
  void screenText();

  // Destructor
  ~StudentWorld();

  // Auxiliary Functions
  void decOil();
  bool probabilityIsGood(const int chance);

 private:
  FrackMan* frackman;
  Actor* area[64][64];
  std::list<Actor*> la;

  // Counters
  int oilBarrels;
  int PROBABILITY_OF_WS;
  int counterForProtester;
  int timeForNewProtester;
  int MAX_NUMBER_OF_PROTESTERS;
  int numberOfProtestersMade;
  int protesterIntervals;
  int PROBABILITY_OF_HARDCORE;
};

#endif // STUDENTWORLD_H_
