#include "StudentWorld.h"
#include "GraphObject.h"
#include "Actor.h"
#include "GameConstants.h"
#include "GameController.h"
#include <string>
#include <list>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{return new StudentWorld(assetDir);}

void StudentWorld::generateDirtField()
{
  int r,c;

  // Initialize the dirt field
  for (r = 0; r <= 63; r++)
    for (c = 0; c <= 63; c++)
      area[r][c] = new Dirt(this, IID_DIRT, c,r);   

  // Clear the top row
  for (r = 60; r <= 63; r++)
    for (c = 0; c <= 63; c++)
    {
      delete area[r][c];
      area[r][c] = nullptr;
    }
  
  // Generate mine-shaft
  for (r = 4; r < 60; r++)
    for (c = 30; c <= 33; c++)
    {
      delete area[r][c];
      area[r][c] = nullptr;
    }
}

void StudentWorld::deleteDirt(const int x, const int y)
{
  delete area[y][x];
  area[y][x] = nullptr;
}

bool StudentWorld::isThereDirt(const int x, const int y)
{
  // If there is a nullptr, then there is no dirt
  if (area[y][x] == nullptr)
    return false;
  else
    return true;
}

bool StudentWorld::dirtBeneathBoulder(const int x, const int y)
{
  int dirtless = 0;

  for (int i = 0; i < 4; i++)
  {
    if (!isThereDirt(x + i, y))
      dirtless++;
  }
  if (dirtless == 4)
    return false;
  else
    return true;
}

bool StudentWorld::noDirt(const int x, const int y)
{
  for (int i = y; i != y + 4; i++)
    for (int j = x; j != x + 4; j++)
    {
      if (isThereDirt(j,i))
	return false;
    }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// FrackMan's auxiliary functions
int StudentWorld::frackmanX() const
{return frackman->getX();}

int StudentWorld::frackmanY() const
{return frackman->getY();}

void StudentWorld::updateInventory(const int indicator)
{
  const int SQUIRTS = 5;
  const int SONARS = 2;
  const int NUGGETS = 1;
  switch(indicator)
  {
    case SQUIRTS:
      frackman->addAmmo(SQUIRTS);
      increaseScore(100);
      break;
    case SONARS:
      frackman->addSonars(SONARS);
      increaseScore(75);
      break;
    case NUGGETS:
      frackman->addNuggets(NUGGETS);
      increaseScore(10);
      break;
    default:
      break;
  }
}

// Fire squirt gun
void StudentWorld::fire(const int x, const int y, GraphObject::Direction dir)
{
  if (x < 60 && x >= 0 && noDirt(x,y) && !boulderIsPresent(x,y))
    la.push_front(new Squirt(this,x,y,dir));

  frackman->addAmmo(-1);
}

// Drop nugget
void StudentWorld::dropGold(const int x, const int y)
{
  la.push_front(new BribeNuggets(this,x,y));
  frackman->addNuggets(-1);
}

// Illuminate hidden objects
void StudentWorld::radar(const int x, const int y)
{
  int ox, oy;
  double d = 0;
  
  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
  {
	  if ((*p)->oilObjects() || (*p)->nuggetObjects())
	  {
		ox = (*p)->X();
		oy = (*p)->Y();
		d = distance(x, y, ox, oy);
		if (d <= 12)
		  (*p)->illuminate();
	  }
  } 
  frackman->addSonars(-1);
}
/////////////////////////////////////////////////////////////////////////////////////
// Distances
double StudentWorld::distance(const int x1, const int y1,
			      const int x2, const int y2)
{
  return sqrt(pow(static_cast<double>(x1 - x2), 2) +
	      pow(static_cast<double>(y1 - y2), 2));  
}

bool StudentWorld::checkDistance(const int x1, const int y1)
{
	int x = 0;
	int y = 0;

  // Checks distance between boulders, nuggets, and oil
  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
  {
    if ((*p)->oilObjects() || (*p)->nuggetObjects() || (*p)->boulderObjects())
      x = (*p)->X();
      y = (*p)->Y();
      if (!(distanceIsGood(x1,y1 , x,y, 6)))
	return false;
  }
  return true;
}

bool StudentWorld::distanceIsGood(const int x1, const int y1, const int x2,
				  const int y2, const double condition)
{
  double d = distance(x1,y1 , x2,y2);
  if (d > condition)
    return true;
  else
    return false;
}
/////////////////////////////////////////////////////////////////////////////////////
// Distribute boulders, gold, and oil

void StudentWorld::generateNuggetsOil(const int numberOfNO, const int indicator)
{
  int r,c;

  // Distribute nuggets and oil throughout the field
  for (int i = 0; i < numberOfNO; i++)
  {
    for (c = rand() % 60, r = rand() % 56; c == 27 || c == 28 || c == 29
	   || c == 30 || c == 31 || c == 32 || c == 33 || !(checkDistance(c,r));)
    {
      r = rand() % 56;
      c = rand() % 60;
    }
    if (indicator == 1)
      la.push_front(new Nuggets(this, c,r));
    if (indicator == 2)
      la.push_front(new Oil(this, c,r));
  }
}

void StudentWorld::generateBoulders(const int numberOfBoulders)
{
  int r,c;

  // Distribute boulders throughout the field
  for (int i = 0; i < numberOfBoulders; i++)
  {
    for (c = rand() % 60, r = rand() % 56; c == 27 || c == 28 || c == 29 || c == 30 || c == 31 || c == 32 || c == 33 || r < 20 || !(checkDistance(c,r));)
    {
      r = rand() % 56;
      c = rand() % 60;
    }
    la.push_front(new Boulder(this, c,r));
  }
}
/////////////////////////////////////////////////////////////////////////////////////
// Distribute water pools and sonar kits

void StudentWorld::generateWS()
{
  int x,y;
  
  if (probabilityIsGood(PROBABILITY_OF_WS))
  {
    int chance = rand() % 5;
    
    if (chance == 0)
      la.push_front(new Sonar(this)); // Generate sonar
    else
    {
      x = rand() % 60;
      y = rand() % 60;
      for(;;)
      {
	x = rand() % 60;
	y = rand() % 60;
	if (noDirt(x,y))
	  break;
	else
	  continue;
      }
      la.push_front(new WaterPool(this, x,y)); // Generate water pool
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Distribute protesters

void StudentWorld::generateProtesters()
{
  if (numberOfProtestersMade < MAX_NUMBER_OF_PROTESTERS)
  {
    counterForProtester++;
    if (counterForProtester == timeForNewProtester)
    {
      
      if (probabilityIsGood(PROBABILITY_OF_HARDCORE))
	la.push_front(new HardcoreProtester(this, IID_HARD_CORE_PROTESTER, protesterIntervals));
            
      la.push_front(new Protester(this, IID_PROTESTER, protesterIntervals));
      counterForProtester = 0;
      numberOfProtestersMade++;
    }
  }
  else
    return;
}

/////////////////////////////////////////////////////////////////////////////////////
// Invariants

bool StudentWorld::boulderIsPresent(const int x, const int y)
{
  int bx, by;
  double d = 0;
  
  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
  {
    if ((*p)->boulderObjects())
    {
      bx = (*p)->X();
      by = (*p)->Y();
      d = distance(x,y , bx,by);

      if (d <= 3)
	return true;
    }
    else
      continue;
  }
  return false;
}

// Auxiliary Functions
void StudentWorld::decOil()
{
  increaseScore(1000);
  oilBarrels--;
}

bool StudentWorld::probabilityIsGood(const int chance)
{
  if (rand() % chance == 0)
    return true;
  else
    return false;

}
/////////////////////////////////////////////////////////////////////////////////////
// Protester auxiliary functions
bool StudentWorld::frackmanSighting(const int px, const int py)
{ 
  list<Actor*>::iterator p;
  for (p = la.begin(); p != la.end(); p++)
  {
    if ((*p)->protesterObject())
      if (px == (*p)->getX() && py == (*p)->getY())
	break;
  }
  if (p == la.end())
    return false;
  
  // FrackMan's coordinates
  const int fx = frackman->getX();
  const int fy = frackman->getY();

  double d = distance(px,py , fx,fy);
  if (d <= 4)
    return false;

  if (px <= fx && py == fy) // FrackMan is horizontal from Protester
  {
    if (horizontalDB(px,fx, py))
    {
      (*p)->useDirection(1);
      return true;
    }
  }
  else if (px >= fx && py == fy)
  {
    if (horizontalDB(fx,px, py))
    {
      (*p)->useDirection(2);
      return true;
    }
  }
  else if (py >= fy && px == fx)
  {
    if (verticalDB(fy,py, px))
    {
      (*p)->useDirection(3);
      return true;
    }
  }
  else if (py <= fy && px == fx)
  {
    if (verticalDB(py,fy, px))
    {
      (*p)->useDirection(4);
      return true;
    }
  }
  else
    return false;
  return false;
}

bool StudentWorld::horizontalDB(const int left, const int right, const int vert)
{
  if (left > right)
    return false;

  for (int x = left; x != right; x++)
  {
    if (!(noDirt(x,vert)))
      return false;
    if (boulderIsPresent(x,vert))
      return false;
  }
  return true;
}

bool StudentWorld::verticalDB(const int down, const int up, const int horiz)
{
  if (down > up)
    return false;

  for (int y = down; y != up; y++)
  {
    if (!(noDirt(horiz,y)))
      return false;
    if (boulderIsPresent(horiz,y))
      return false;
  }
  return true;
}

bool StudentWorld::checkRight(const int px, const int py)
{
  if (px > 60)
    return false;

  if (noDirt(px,py) && !(boulderIsPresent(px,py)))
    return true;
  return false;
}

bool StudentWorld::checkLeft(const int px, const int py)
{
  if (px < 0)
    return false;
  if (noDirt(px,py) && !(boulderIsPresent(px,py)))
    return true;
  return false;
}

bool StudentWorld::checkUp(const int px, const int py)
{
  if (py > 60)
    return false;
  if (noDirt(px,py) && !(boulderIsPresent(px,py)))
    return true;
  return false;
}

bool StudentWorld::checkDown(const int px, const int py)
{
  if (py < 0)
    return false;
  if (noDirt(px,py) && !(boulderIsPresent(px,py)))
    return true;
  return false;
}
/////////////////////////////////////////////////////////////////////////////////////
// Conflict functions

void StudentWorld::pulverize(const int bx, const int by)
{
  double d = distance(bx,by , frackman->X(),frackman->Y());
  if (d <= 3)
  {
    frackman->changeStatus(false);
    decLives();
  }

  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
  {
    if ((*p)->protesterObject())
    {
      d = distance(bx,by , (*p)->getX(), (*p)->getY());
      if (d <= 3)
	(*p)->quit(true);
    }
  }
}

bool StudentWorld::protesterHitfrackman()
{
  int fx = frackman->getX();
  int fy = frackman->getY();
  int px = 0;
  int py = 0;
  double d = 0;
  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
  {
    if ((*p)->protesterObject())
    {
      px = (*p)->getX();
      py = (*p)->getY();
      d = distance(fx,fy , px,py);
      if (d <= 4)
      {
	if (px < fx && py == fy && (*p)->getDirection() == GraphObject::right)
	  {frackman->annoyed();	return true;}
	else if (px > fx && py == fy && (*p)->getDirection() == GraphObject::left)
	  {frackman->annoyed();	return true;}
	else if (py < fy && px == fx && (*p)->getDirection() == GraphObject::up)
	  {frackman->annoyed();	return true;}
	else if (py > fy && px == fx && (*p)->getDirection() == GraphObject::down)
	  {frackman->annoyed();	return true;}
	else
	  return false;
      }
    }
  }
  return false;
}

bool StudentWorld::bribeProtester(const int nx, const int ny)
{
  double d = 0;
  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
  {
    if ((*p)->protesterObject())
    {
      d = distance(nx,ny , (*p)->X(),(*p)->Y());
      if (d <= 3) // Protester picks up nugget
      {
	increaseScore(25);
	(*p)->quit(true); // Protester leaves oil field
	return true;
      }
    }
  }
  return false;
}

bool StudentWorld::squirtHitProtester(const int x, const int y)
{
  double d = 0;
  int count = 0;
  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
  {
    if ((*p)->protesterObject())
    {  
      int px = (*p)->X();
      int py = (*p)->Y();
      d = distance(x,y , px,py);

      if (d <= 3)
      {
	(*p)->annoyed();
	count++;
      }
    }
  }
  if (count > 0)
    return true;
  else
    return false;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// GAME PRODUCTION ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
int StudentWorld::init()
{
  generateDirtField();
  frackman = new FrackMan(this);
  // Generate the number of Boulders, Nuggets, Barrels of Oil
  const int current_level_number = getLevel();
  int B = min(current_level_number / 2 + 2, 6);
  int G = max(5-current_level_number / 2, 2);
  int L = min(2 + current_level_number, 20);

  const int gold = 1;
  const int oil = 2;
  generateBoulders(B);
  generateNuggetsOil(G,gold);
  generateNuggetsOil(L,oil);
  
  // Counters
  oilBarrels = L;

  const int value = 3 - current_level_number/4;
  protesterIntervals = max(0, value);
  
  // Probability to generate water pools, sonar kits, and protesters
  PROBABILITY_OF_WS = current_level_number * 25 + 300;
  timeForNewProtester = max(25, 200 - current_level_number);
  PROBABILITY_OF_HARDCORE = min(90, current_level_number * 10 + 30);
  MAX_NUMBER_OF_PROTESTERS = min<int>(15, 2 + current_level_number * 1.5);
  
  // First protester
  la.push_front(new Protester(this, IID_PROTESTER, protesterIntervals));
  numberOfProtestersMade = 1;
  counterForProtester = 0;
  
  return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{ 
  // Check if player died first
  if (!(frackman->alive()))
  {
    decLives();
    return GWSTATUS_PLAYER_DIED;
  }
  ///////////////////////////////////////////////////////////////////////////////////
  // [Update status text on top of screen]

  screenText();

  ////////////////////////////////////////////////////////////////////////////////// 
  // Call the doSomething function for ea. object

  frackman->doSomething();
  
  if (!(frackman->alive()))
  {
    decLives();
    return GWSTATUS_PLAYER_DIED;
  }
  // Generate water pools, sonar kits, and protesters
  generateWS();
  generateProtesters();

  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
  {
    (*p)->doSomething();

    // Remove dead actors
    if (!(la.empty()) && !((*p)->alive()))
      {	
	if ((*p)->oilObjects())
	  decOil(); // Adjusts score per oil barrel found
      }

  }
  for (list<Actor*>::iterator p = la.begin(); p != la.end(); p++)
    {
      if (!(*p)->alive())
	{
		delete (*p);
	  p = la.erase(p);
	  if (p == la.end())
		  break;
	}
    }
  if (!frackman->alive())
  {
	  decLives();
	  return GWSTATUS_PLAYER_DIED;
  }

  // Check if the game is complete
  if (oilBarrels <= 0)
  {
    GameController::getInstance().playSound(SOUND_FINISHED_LEVEL);
    return GWSTATUS_FINISHED_LEVEL;
  }

  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
  for (int r = 0; r <= 63; r++)
    for (int c = 0; c <= 63; c++)
      delete area[r][c];

  delete frackman;

  while (!la.empty())
  {
    delete la.front();
    la.pop_front();
  }
}

StudentWorld::~StudentWorld()
{
  for (int r = 0; r <= 63; r++)
    for (int c = 0; c <= 63; c++)
      delete area[r][c];

  delete frackman;

  while (!la.empty())
  {
    delete la.front();
    la.pop_front();
  }
}

void StudentWorld::screenText()
{
  ostringstream score;
  score.fill('0');
  score << setw(6) << getScore();

  string Lvl = "  Lvl: ";
  ostringstream level;
  level.fill(' ');
  level << setw(2) << getLevel();

  ostringstream lives;
  lives << "  Lives: " << getLives();

  ostringstream health;
  health.fill(' ');
  health << setw(3) << frackman->health() * 10;

  string wtr = "  Wtr: ";
  ostringstream water;
  water.fill(' ');
  water << setw(2) << frackman->getAmmo();

  string gld = "  Gld: ";
  ostringstream gold;
  gold.fill(' ');
  gold << setw(2) << frackman->getNuggets();
  
  ostringstream sonar;
  sonar.fill(' ');
  sonar << setw(2) << frackman->getSonars();  

  ostringstream oil;
  oil.fill(' ');
  oil << setw(2) << oilBarrels;

  ///////////////////////////////////////////////////////////////////////////////////
  // Final Output

  string gameResult = "Scr: " + score.str() + Lvl + level.str() + lives.str() + 
    "  Htlh: " + health.str() + "%" + wtr + water.str() + gld + gold.str() +
    "  Sonar: " + sonar.str() + "  Oil Left: " + oil.str();
     
  setGameStatText(gameResult);
}
