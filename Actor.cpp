#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include "GameController.h"
#include "GameWorld.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////
// Base Class

Actor::Actor(StudentWorld* access, int imageID, int startX, int startY,
	     Direction dir, double size, unsigned int depth)
  : GraphObject(imageID, startX, startY, dir, size, depth), ptrWorld(nullptr)
{status = true; setVisible(true);}

// Auxiliary Functions
void Actor::changeStatus(bool condition)
{status = condition;}

void Actor::sound(const int soundCode)
{GameController::getInstance().playSound(soundCode);}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// FRACKMAN & PEOPLE /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

// Agent class
Agent::Agent(StudentWorld* access, int imageID, int startX, int startY,
	     Direction dir, unsigned int hitPoints)
  : Actor(access, imageID, startX, startY, dir, 1.0, 0)
{}
/////////////////////////////////////////////////////////////////////////////////////

FrackMan::FrackMan(StudentWorld* access)
  : Agent(access, IID_PLAYER, 30,60 , right, 10)
{
  changeStudentWorldPtr(access);
  m_health = 10;
  ammo = 5;
  m_sonars = 1;
  m_gold = 0;
}

void FrackMan::doSomething()
{
  // Check if FrackMan is still alive
  if (getWorld()->getLives() <= 0)
    return;

  ///////////////////////////////////////////////////////////////////////////////////
  // IF FRACKMAN'S IMAGE OVERLAPS DIRT OBJECTS

  for (int y = getY(); y != getY() + 4; y++)
    for (int x = getX(); x != getX() + 4; x++)
      {
	if (getWorld()->isThereDirt(getX(), getY()))
	  sound(SOUND_DIG);
	else if (getWorld()->isThereDirt(getX() + 3, getY() + 3))
	  sound(SOUND_DIG);
	else if (getWorld()->isThereDirt(getX() + 3, getY()))
	  sound(SOUND_DIG);
	getWorld()->deleteDirt(x, y);
      }
  ///////////////////////////////////////////////////////////////////////////////////
  // IF USER PRESSES A KEY

  int ch;
  if (getWorld()->getKey(ch) == true)
  {    
    // User hit a key this tick!
    switch(ch)
    { 
      case KEY_PRESS_ESCAPE:
	changeStatus(false);
	break;

      case KEY_PRESS_SPACE:
	if (ammo > 0)
	{
	  sound(SOUND_PLAYER_SQUIRT);
	  switch(getDirection())
	  {
	    case right:
	      getWorld()->fire(getX()+4,getY(),right);
	      break;
	    case left:
	      getWorld()->fire(getX()-4,getY(),left);
	      break;
	    case up:
	      getWorld()->fire(getX(),getY()+4,up);
	      break;
            case down:
	      getWorld()->fire(getX(),getY()-4, down);
	      break;
	    default:
	      break;
	  }
	}
	break;

      case KEY_PRESS_LEFT:
	if (getDirection() != left)
	{
	  setDirection(left);
	  break;
	}
        if (getX() - 1 < 0)
	{
	  moveTo(getX(),getY());
	  break;
	}
	if(getWorld()->boulderIsPresent(getX()-1, getY()))
	  break;
	moveTo(getX() - 1, getY());
	break;

      case KEY_PRESS_RIGHT:
	if (getDirection() != right)
	{
	  setDirection(right);
	  break;
	}
	if (getX() + 1 > 60)
	{
	  moveTo(getX(),getY());
	  break;
	}
	if (getWorld()->boulderIsPresent(getX()+1, getY()))
	  break;
	moveTo(getX() + 1, getY());
	break;

      case KEY_PRESS_UP:
	if (getDirection() != up)
	  {
	    setDirection(up);
	    break;
	  }
	if (getY() + 1 > 60)
	{
	  moveTo(getX(),getY());
	  break;
	}
	if (getWorld()->boulderIsPresent(getX(), getY()+1))
	  break;
	moveTo(getX(), getY() + 1);
	break;

      case KEY_PRESS_DOWN:
	if (getDirection() != down)
	  {
	    setDirection(down);
	    break;
	  }
	if (getY() - 1 < 0)
	{
	  moveTo(getX(),getY());
	  break;
	}
	if (getWorld()->boulderIsPresent(getX(), getY()-1))
	  break;
	moveTo(getX(), getY() - 1);
	break;

      case KEY_PRESS_TAB:
	if (m_gold > 0)
	  getWorld()->dropGold(getX(),getY());
	break;
      case 'z':
      case 'Z':
	if (m_sonars > 0)
	  getWorld()->radar(getX(),getY());
	break;
      default:
	break;
    }
  }
}

void FrackMan::annoyed()
{
  m_health -= 2;
  if (m_health <= 0)
  {
    changeStatus(false);
    sound(SOUND_PLAYER_GIVE_UP);
  }
}

FrackMan::~FrackMan()
{}

/////////////////////////////////////////////////////////////////////////////////////
// Squirt

Squirt::Squirt(StudentWorld* access, int startX, int startY, Direction dir)
  : Actor(access, IID_WATER_SPURT, startX,startY , dir, 1.0, 1)
{countDistance = 0; studentworld = access;}

void Squirt::doSomething()
{
  if (countDistance == 4)
  {
    changeStatus(false);
    setVisible(false);
  }
  
  if (studentworld->squirtHitProtester(getX(), getY()))
  {
	  changeStatus(false); setVisible(false);
  }

  countDistance++;
  switch(getDirection())
  {
    case right:
      if (getX() + 1 > 60 || studentworld->boulderIsPresent(getX()+1, getY())
	  || !(studentworld->noDirt(getX()+1,getY())))
	break;
      moveTo(getX() + 1, getY());
      break;
      
    case left:
      if (getX() - 1 < 0 || studentworld->boulderIsPresent(getX()-1, getY())
	  || !(studentworld->noDirt(getX()-1,getY())))
	break;
      moveTo(getX() - 1, getY());
      break;

    case up:
      if (getY() + 1 > 60 || studentworld->boulderIsPresent(getX(), getY()+1)
	  || !(studentworld->noDirt(getX(),getY()+1)))
	break;
      moveTo(getX(), getY()+1);
      break;

    case down:
      if (getY() - 1 < 0 || studentworld->boulderIsPresent(getX(), getY()-1)
	  || !(studentworld->noDirt(getX(),getY()-1)))
	break;
      moveTo(getX(), getY()-1);
      break;
  }
}

Squirt::~Squirt()
{}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// PROTESTERS /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// PROTESTERS

Protester::Protester(StudentWorld* access, int imageID, int ticksToWait)
  : Agent(access, IID_PROTESTER, 60,60 , left, 5)
{
  studentworld = access;
  counter = 0;
  countShout = 15;
  intervals = ticksToWait;
  m_health = 5;
  countPerpendicular = 0;
  leaving_condition = false;
  initializer = false;
  facing = left;
  tracker = 0;

  onedirection = 0;
  while (onedirection < 8)
    onedirection = rand() % 60;
}

void Protester::doSomething()
{
  if (!alive())
    return;

  if (quitting())
  {
    leaving();
    return;
  }

  if (waiting())
    return;
  
  countShout++;
  if (countShout >= 15)
    if (annoyFrackMan())
    {
      countShout = 0;
      sound(SOUND_PROTESTER_YELL);
      return;
    }
  if (countShout < 15)
    return;

  if (initializer == false)
  {
    if (onedirection <= 0)
    {
      initializer = true;
    }
    else
      moveTo(getX() - 1, getY());
    onedirection--;
    return;
  }

  // Protester sees FrackMan
  if (sightfrackman())
  {
    chasefrackman();
    onedirection = 0;
    return;
  }

  changeDirection();
  intersection();

  if (!move())
    onedirection = 0;
}

void Protester::useDirection(int changeDir)
{
  switch (changeDir)
  {
    case 1:
      facing = right;
      break;
    case 2:
      facing = left;
      break;
    case 3:
      facing = down;
      break;
    case 4:
      facing = up;
      break;
    default:
      break;
  }
}

void Protester::quit(bool condition)
{  
  leaving_condition = condition;
  sound(SOUND_PROTESTER_GIVE_UP);
}

bool Protester::waiting()
{
  counter++;
  // Protester rests until counter reaches interval
  if (counter != intervals)
    return true;

  countPerpendicular++;
  counter = 0; // Reset timer
  return false;
}

// Leave-the-oil-field state
void Protester::leaving()
{
  if (getX() == 60 && getY() == 60)
  {
    setVisible(false);
    changeStatus(false);
    return;
  }

  tracker = rand() % 2;
  if (getX() + 1 <= 60 && tracker == 0)
    {setDirection(right);
      moveTo(getX() + 1, getY()); tracker = 2;}
  if (getY() + 1 <= 60 && tracker == 1)
    {setDirection(up);
      moveTo(getX(), getY() + 1); tracker = 1;}
}

bool Protester::annoyFrackMan()
{
  if (studentworld->protesterHitfrackman())
    return true;
  else
    return false;
}

void Protester::findsNugget()
{
  leaving_condition = true;
  leaving();
}

void Protester::annoyed()
{
  if (leaving_condition == true)
    return;

  m_health = m_health - 2; // Caused by squirt
  if (m_health <= 0)
    quit(true);
}

bool Protester::sightfrackman()
{return studentworld->frackmanSighting(getX(),getY());}

void Protester::chasefrackman()
{
  setDirection(facing);

  switch(facing)
  {
    case right:
      moveTo(getX() + 1, getY());
      break;
    case left:
      moveTo(getX() - 1, getY());
      break;
    case up:
      moveTo(getX(),getY() + 1);
      break;
    case down:
      moveTo(getX(),getY()-1);
      break;
    default:
      break;
  }
}

void Protester::changeDirection()
{
  onedirection--;
  if (onedirection > 0)
    return;
  else
  {
    int newdirection = rand() % 4;
    switch(newdirection)
    {
      case 0:
	if (studentworld->checkRight(getX()+1,getY()))
	  {setDirection(right); facing = right;}
	break;
      case 1:
	if (studentworld->checkLeft(getX()-1,getY()))
	  {setDirection(left); facing = left;}
	break;
      case 2:
	if (studentworld->checkUp(getX(),getY()+1))
	  {setDirection(up); facing = up;}
	break;
      case 3:
	if (studentworld->checkDown(getX(),getY()-1))
	  {setDirection(down); facing = down;}
	break;
      default:
	break;
    }
    while (onedirection < 8)
      onedirection = rand() % 60;
  }
}

void Protester::intersection()
{
  if (countPerpendicular < 200)
    return;

  int chance = rand() % 2;
  switch(facing)
  {
    case left:
    case right:
      {
	if (studentworld->checkUp(getX(),getY()+1) &&
	    studentworld->checkDown(getX(),getY()-1))
	{
	  if (chance == 1)
	    setDirection(up);
	  else
	    setDirection(down);
	}
	else if (studentworld->checkUp(getX(),getY()+1))
	  setDirection(up);
	else if (studentworld->checkDown(getX(),getY()-1))
	  setDirection(down);
	else
	  break;
      }
    case down:	  
    case up:
      {
	if (studentworld->checkRight(getX()+1,getY()) &&
	    studentworld->checkLeft(getX()-1,getY()))
        {
	  if (chance == 1)
	    setDirection(right);
	  else
	    setDirection(left);
	}
	else if (studentworld->checkRight(getX()+1,getY()))
	  setDirection(right);
	else if (studentworld->checkLeft(getX()-1,getY()))
	  setDirection(left);
	else
	  break;
      }
    default:
      break;
  }
  
  while (onedirection < 8)
    onedirection = rand() % 60;

  countPerpendicular = 0;
}

bool Protester::move()
{
  switch(getDirection())
  {
    case right:
      if (studentworld->checkRight(getX()+1,getY()))
	{moveTo(getX()+1,getY()); return true;}
      break;
      
    case left:
      if (studentworld->checkLeft(getX()-1,getY()))
	{moveTo(getX()-1,getY()); return true;}
      break;
      
    case up:
      if (studentworld->checkUp(getX(),getY()+1))
	{moveTo(getX(),getY()+1); return true;}
      break;
      
    case down:
      if (studentworld->checkDown(getX(),getY()-1))
	{moveTo(getX(),getY()-1); return true;}
      break;
    default:
      break;
  }
  return false;
}

Protester::~Protester()
{}
/////////////////////////////////////////////////////////////////////////////////////
// HARDCORE PROTESTERS

HardcoreProtester::HardcoreProtester(StudentWorld* access, int imageID,
				     int ticksToWait)
  : Protester(access, IID_HARD_CORE_PROTESTER, ticksToWait)
{
  m_health = 20;
}

HardcoreProtester::~HardcoreProtester()
{}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// RESOURCES /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Dirt

Dirt::Dirt(StudentWorld* access, int imageID, int startX, int startY)
  : Actor(access, IID_DIRT, startX,startY , right, .25, 3)
{}

void Dirt::doSomething()
{}

Dirt::~Dirt()
{}

/////////////////////////////////////////////////////////////////////////////////////
// Boulder

Boulder::Boulder(StudentWorld* access, int startX, int startY)
  : Actor(access, IID_BOULDER, startX,startY , down, 1.0, 1)
{
  bool unstable = false;
  studentworld = access;
  counter = 0; // count to 30 for waiting phase
  for (int y = getY(); y != getY() + 4; y++)
    for (int x = getX(); x != getX() + 4; x++)
      access->deleteDirt(x,y);
}

void Boulder::doSomething()
{
  if (!alive())
    return;

  if (!stable())
  {
    unstable = true;
    changeStatus(waiting());
	if (!alive())
	{
		setVisible(false);
		return;
	}
  }
}

bool Boulder::stable() const
{
  if (unstable == true)
    return false; // Boulder had been unstable before
  if (studentworld->dirtBeneathBoulder(getX(),getY()-1))
    return true; // Stable boulder
  else
    return false; // Unstable boulder
}

bool Boulder::waiting()
{
  if (counter <= 30)
  {
    counter++;
    return true;
  }
  else
    return falling();
}

bool Boulder::falling()
{
  sound(SOUND_FALLING_ROCK);

  // If boulder runs to bottom of the map
  if (getY() == 0)
    return false;

  // If boulder runs into another boulder
  if (!(studentworld->boulderIsPresent(getX(), getY()-1)))
    return false;

  // If boulder runs into dirt
  if (studentworld->dirtBeneathBoulder(getX(),getY()-1))
    return false;

  moveTo(getX(),getY()-1);
  kill();
  return true;
}

void Boulder::kill()
{studentworld->pulverize(getX(),getY());}

Boulder::~Boulder()
{}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Activating Objects

ActivatingObject::ActivatingObject(StudentWorld* access, int imageID,
			             int startX, int startY, Direction dir)
  : Actor(access, imageID, startX, startY, dir, 1.0, 2), counter(1)
{timeLeft = 0; studentworld = access;}

bool ActivatingObject::inRangeWithFrackMan(double const condition)
{
  int fx = studentworld->frackmanX();
  int fy = studentworld->frackmanY();
  double d = studentworld->distance(getX(),getY() , fx,fy);

  if (d <= condition)
    return true;
  else
    return false;
}

void ActivatingObject::timeToLive()
{
  const int current_level_number = studentworld->getLevel();
  const int value = 300 - 10 * current_level_number;

  // Initialize duration for water pools and sonar kits
  timeLeft = std::max(100, value);
}


bool ActivatingObject::timeUp()
{
  if (counter >= timeLeft)
    return false; // Object dies
  else
    counter++;
  return true; // Object continues to live
}

void ActivatingObject::gotGoodie(const int update)
{
  setVisible(false);
  sound(SOUND_GOT_GOODIE);
  changeStatus(false);
  studentworld->updateInventory(update);
}
/////////////////////////////////////////////////////////////////////////////////////
// Oil

Oil::Oil(StudentWorld* access, int startX, int startY)
  : ActivatingObject(access, IID_BARREL, startX,startY, right)
{
  setVisible(false);
  alreadySeen = false;
}

void Oil::doSomething()
{
  if (!alive())
    return;

  setVisible(checkFrackMan());
}

bool Oil::checkFrackMan()
{ 
  if (alreadySeen == false)
    if (inRangeWithFrackMan(4))
    {
      alreadySeen = true;
      return true;
    }
  
  if (inRangeWithFrackMan(3))
  {
    changeStatus(false);
    sound(SOUND_FOUND_OIL);
    return false;
  }
 
  if (alreadySeen == false)
    return false;
  else
    return true;
}

Oil::~Oil()
{}
/////////////////////////////////////////////////////////////////////////////////////
// Nuggets

Nuggets::Nuggets(StudentWorld* access, int startX, int startY)
  : ActivatingObject(access, IID_GOLD, startX,startY, right)
{
  alreadySeen = false;
  setVisible(false);
}

void Nuggets::doSomething()
{
  if (!alive())
    return;

  if (alreadySeen == false)
    if (inRangeWithFrackMan(4))
    {
      setVisible(true);
      alreadySeen = true;
      return;
    }

  if (inRangeWithFrackMan(3))
    gotGoodie(1);
}


// Nuggets pickable by protesters
BribeNuggets::BribeNuggets(StudentWorld* access, int startX, int startY)
  : ActivatingObject(access, IID_GOLD, startX,startY, right)
{counter = 0; setVisible(true); studentworld = access;}

void BribeNuggets::doSomething()
{
  if (!(alive()))
      return;
      
  counter++;
  if (counter == 100)
  {
    setVisible(false);
    changeStatus(false);
  }

  if (studentworld->bribeProtester(getX(),getY()))
  {
    sound(SOUND_PROTESTER_FOUND_GOLD);
    setVisible(false);
    changeStatus(false);
  }
}


/////////////////////////////////////////////////////////////////////////////////////
// Water Pool

WaterPool::WaterPool(StudentWorld* access, int startX, int startY)
  : ActivatingObject(access, IID_WATER_POOL, startX, startY, right)
{timeToLive(); studentworld = access;}

void WaterPool::doSomething()
{
  if (!alive())
    return;
  changeStatus(timeUp());
  
  if (inRangeWithFrackMan(3))
    gotGoodie(5);
}

WaterPool::~WaterPool()
{}
/////////////////////////////////////////////////////////////////////////////////////
// Sonar

Sonar::Sonar(StudentWorld* access)
  : ActivatingObject(access, IID_SONAR, 0,60 , right)
{timeToLive(); studentworld = access;}

void Sonar::doSomething()
{
  if (!alive())
    return;
  
  changeStatus(timeUp());

  if (inRangeWithFrackMan(3))
    gotGoodie(2);
}

Sonar::~Sonar()
{}
