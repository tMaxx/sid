#pragma once
#include <thread>
#include <chrono>
#include "zwierzak.h"
#include "stats.h"

typedef void (*THREADROUTINE)(void*);

class world final
{
private:
	world() = delete;
	void WorldINIT(class stats*,bool);
	void WorldRESTART();
	void WorldRoutine(void);

	class stats *meStats;

	class zwierzak *IamTheSid;

	std::thread *meThread;

	void _chkchrono();
	void _worldChronoWait(bool,int,bool*); //interruptable

	void _statsCmp(int);

	bool _item_inrange(int,int,char);
	void _additem(char);

	Vc2D _shorterWayAlgorithm();
	Vc2D _weirdWayAlgorithm();

	void _loopTrimVector(Vc2D&);

	void _intTrimVector(Vc2D&);

public:
	world(class stats*);
	~world(void);

	void LaunchMe();
};
void world::WorldINIT(class stats * _stats, bool _releaseMutex = true)
{
	meStats=_stats;
	meStats->sMutex->lock();
	meStats->sRunmodeD = (meStats->sRunmode |= statF_thPSE);
	IamTheSid = new class zwierzak((class statsd*)meStats);

	meStats->sPositionD = (struct stats::sItmD*) \
								((meStats->sPosition = (meStats->sitmAll[0]) = \
								new stats::sItm(Vc2D(statG_width_half, statG_height_half), 'p')));
	meStats->sGotoD     = (struct stats::sItmD*) \
								((meStats->sGoto     = (meStats->sitmAll[1]) = \
								new stats::sItm(Vc2D(statG_width_half, statG_height_half), 'g')));

	for (int i = 0; i < 10; ++i)
		_additem('c');
	_additem('l');

	if(_releaseMutex)
		meStats->sMutex->unlock();
}
world::world(class stats * _stats)
{	world::WorldINIT(_stats);	}

world::~world()
{
	if(meThread->joinable())
	{
		meStats->sRunmode |= statF_thKIL | statF_thPSE;
		meStats->sDelayRunning = false;
	}
	if(meThread->joinable())
		meThread->join();
	delete meThread;
	delete IamTheSid;
	delete meStats;
}

void world::LaunchMe()
{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpmf-conversions"
	meThread = new std::thread((THREADROUTINE)&world::WorldRoutine,this);
	#pragma GCC diagnostic pop
}

void world::WorldRoutine(void)
{
	//lock mutex
	meStats->sMutex->lock();
	//inform we're now running
	(meStats->sRunmode) |= statF_thRUN;

	//moves
	Vc2D funcTmpVec;
	unsigned int funcTmpFlags;

	//if we shall not be killed
	while(!((meStats->sRunmode) & statF_thKIL))
	{
		//Check for reset
		if((meStats->sRunmode) & statF_RESET)
			WorldRESTART();
		else //we're skipping this round, sync purposes
		//if not paused or dead, run routine
		if(!((meStats->sRunmode) & statF_thPSE) && !((meStats->sRunmode) & statF_sDEAD))
		{ //run this town

			//1. Sid AI
			IamTheSid->SidAI();

			//2. Entry stats sync
			meStats->SyncStats(true);

			//3. Control and decisioning
			//3.1 Item control
			if((meStats->sRunmode) & statF_Mitem)
			{
				if((meStats->sRunmode) & statF_itADl)
					_additem('l');
				else if((meStats->sRunmode) & statF_itADc)
					_additem('c');
				else if((meStats->sRunmode) & statF_itRMl)
					meStats->ItemDelFront('l');
				else if((meStats->sRunmode) & statF_itRMc)
					meStats->ItemDelFront('c');
				funcTmpFlags |= statF_Mitem;
			}

			//3.2 Moving
			//3.2.1 Create vector
			if((meStats->sRunmode) & statF_wmSHW)
				funcTmpVec = _shorterWayAlgorithm();
			else if((meStats->sRunmode) & statF_wmWEW)
				funcTmpVec = _weirdWayAlgorithm();
			else
				funcTmpVec = meStats->sGoto->Pos - meStats->sPosition->Pos;

			//3.2.2 Move this town
			if((meStats->sRunmode) & statF_smMVE)
				meStats->sPosition->Pos += funcTmpVec;
			else
				meStats->sPosition->Pos += funcTmpVec.NormalizeMath();

			//3.2.x Trim vector
			if((meStats->sRunmode) & (statF_wmSHW | statF_wmWEW))
				_loopTrimVector(meStats->sPosition->Pos);
			else
				_intTrimVector(meStats->sPosition->Pos);

			//3.3 Stats calc
			_statsCmp(funcTmpVec.GetXnY());

			//3.4 Higher Ministry of Flags, Switches and Checks
			//3.4.1 Flags
			if ((meStats->sHealth <= statG_healthzero) || (meStats->sHappiness <= statG_happizero))
				meStats->sRunmode |= statF_sDEAD;
			else
				funcTmpFlags |= statF_sDEAD;

			if (meStats->sHealth <= statG_healthcrit)
				meStats->sRunmode |= statF_Shcrt;
			else
				funcTmpFlags |= statF_Shcrt;

			if(meStats->sHappiness <= statG_happicrit)
				meStats->sRunmode |= statF_Siemo;
			else
				funcTmpFlags |= statF_Siemo;

			if( (meStats->sPosition->Pos.x == 0) || \
				(meStats->sPosition->Pos.y == 0) || \
				(meStats->sPosition->Pos.x == statG_width) || \
				(meStats->sPosition->Pos.y == statG_height) )
					meStats->sRunmode |= statF_Sedge;
			else
				funcTmpFlags |= statF_Sedge;
			meStats->sRunmode &= ~funcTmpFlags;
			funcTmpFlags = 0;

			//3.4.2 Checks
			//Collectbles
			if ((stats::sItm::CollectCount < statI_collMaxNO) && ((RanDev()%6)==2))
				_additem('c');
			else if ((stats::sItm::CollectCount < (statI_collMaxNO * 3) ) && ((RanDev()%3000)==1270))
				_additem('c');
			//Life
			if ((stats::sItm::LifeCount < statI_lifeMaxNO) && ((RanDev()%2)==0))
				_additem('l');
			else if ((stats::sItm::LifeCount < (statI_lifeMaxNO * 3)) && ((RanDev()%statI_lifeProbGen)==(statI_lifeProbGen>>1)))
				_additem('l');

			//4. Final stats sync
			meStats->SyncStats();

		}//th not paused nor zwierzak dead

		if((meStats->sRunmode |= statF_wUPD8)&statF_wdlCH)
			world::_chkchrono();
		meStats->sDelayRunning = true;
		meStats->sMutex->unlock();
		_worldChronoWait((meStats->sRunmode) & statF_wFAST, meStats->sDelay, &(meStats->sDelayRunning));
		meStats->sMutex->lock();
		meStats->sDelayRunning = false;
	}

	//and we're closing down
	(meStats->sRunmode) &= ~(statF_thRUN);
	meStats->sMutex->unlock();
}

void world::WorldRESTART()
{
	delete IamTheSid;
	delete meStats;
	WorldINIT(new class stats, false);
	meStats->sRunmode |= statF_thRUN;
}



void world::_chkchrono()
{
	if((meStats->sDelay >= 5) && !((meStats->sRunmode) & statF_wdlUD))
	{
		if(meStats->sDelay != 5)
			(meStats->sDelay) >>= 1;
		else
			(meStats->sDelay) = 1;
	}
	else if((meStats->sDelay < 2560) && ((meStats->sRunmode) & statF_wdlUD))
	{
		if(meStats->sDelay != 1)
			(meStats->sDelay) <<= 1;
		else
			(meStats->sDelay) = 5;
	}
	(meStats->sRunmode) &= ~statF_Mdlay;
}
void world::_worldChronoWait(bool immexit, int howlong, bool *bptr)
{
	if(immexit) //exit on fast mode
		return;
	if(howlong<=40) //just wait and exit
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(howlong));
		return;
	}
	else
		for (int i = (howlong/40) ; (i > 0)&&(*bptr) ; --i)
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
}


void world::_statsCmp(int moves)
{
	static short unsigned int flipflop = 1;

	if( (flipflop++ % 3) == 0 )
		meStats->sHealth--;
	if( ((flipflop % 5) == 0 ) || ((moves == 0)&&((flipflop % 2) == 0)) )
		meStats->sHappiness--;

	for(int i = 2; i <= stats::sItm::MaxNo; ++i)
		if(((meStats->sitmAll[i]->Type) == 'c') && \
			(((meStats->sitmAll[i]->Pos).GetDistance(meStats->sPosition->Pos)) <= statI_collRadius))
		{
			if( (((meStats->sHappiness += statI_collPts))) >= statG_happimax )
			{
				meStats->sHappiness = statG_happimax;
				meStats->sHealth++;
			}

			meStats->sScore++;

			meStats->ItemDelNo(i);
		}
		else if(((meStats->sitmAll[i]->Type) == 'l') && \
			(((meStats->sitmAll[i]->Pos).GetDistance(meStats->sPosition->Pos)) <= statI_lifeRadius))
		{

			if((((meStats->sHealth += 45))) >= statG_healthmax)
			{
				meStats->sHealth = statG_healthmax;
				meStats->sHappiness++;
			}
			else
				meStats->sHappiness--;

			if((flipflop%10)==9)
				meStats->sScore++;

			if((RanDev() % 200) == 12)
				meStats->ItemDelNo(i);
		}

	++(meStats->sScore) += moves;
}


void world::_additem(char type)
{
	int x,y, delta;

	if(type=='c')
		delta = statI_collRadius;
	else if(type=='l')
		delta = statI_lifeRadius;

	for(int i=0;i<=100;++i)										  //2=2*1 - minumum borders
		if(_item_inrange( x = (( RanDev() % (statG_width - (delta << 1)) ) + delta) , \
						  y = (( RanDev() % (statG_height- (delta << 1)) ) + delta),  type))
			continue;
		else
		{
			meStats->ItemPushBack(Vc2D(x,y),type);
			break;
		}
}

bool world::_item_inrange(int x, int y, char type = 0)
{
	unsigned short int range, delta = 0;
	if(type == 'c')
		delta = statI_collRadius;
	else if(type=='l')
		delta = statI_lifeRadius;
	for (int i = 2; i <= stats::sItm::MaxNo; ++i)
	{

		if((meStats->sitmAll[i]->Type) == 'c')
			range = (delta + statI_collRadius);
		else if((meStats->sitmAll[i]->Type) == 'l')
			range = (delta + statI_lifeRadius);
		else
			range = delta + 10;

		if( ( (meStats->sitmAll[i]->Pos).GetDistance(x,y) ) <= range )
			return true;
	}
	return false;
}


Vc2D world::_shorterWayAlgorithm()
{
	Vc2D  pos = (meStats->sPosition->Pos), gt = (meStats->sGoto->Pos), \
					v1 = (gt - pos), v2, v3, v4, ret;

	if(pos.y <= statG_height_half)
	{
		v2 = (gt.AddTemp(0,-(statG_height+1)) - pos);
		if(pos.x <= statG_width_half)
		{
			v3 = (gt.AddTemp(-(statG_width+1),0) - pos);
			v4 = (gt.AddTemp(-(statG_width+1),-(statG_height+1)) - pos);
		}
		else
		{
			v3 = (gt.AddTemp((statG_width+1),0) - pos);
			v4 = (gt.AddTemp((statG_width+1),-(statG_height+1)) - pos);
	}	}
	else
	{
		v2 = (gt.AddTemp(0,(statG_height+1)) - pos);
		if(pos.x <= statG_width_half)
		{
			v3 = (gt.AddTemp(-(statG_width+1),0) - pos);
			v4 = (gt.AddTemp(-(statG_width+1),(statG_height+1)) - pos);
		}
		else
		{
			v3 = (gt.AddTemp((statG_width+1),0) - pos);
			v4 = (gt.AddTemp((statG_width+1),(statG_height+1)) - pos);
	}	}

	//decisional
	if(v1.Length() < v2.Length())
		ret = v1;
	else
		ret = v2;
	if(ret.Length() > v3.Length())
		ret = v3;
	if(ret.Length() > v4.Length())
		ret = v4;
	return ret;
}
Vc2D world::_weirdWayAlgorithm()
{
	//Uses point-to-point relation
	int ch1,ch2;
	Vc2D ret, pos = (meStats->sPosition->Pos), gt = (meStats->sGoto->Pos);

	//x coords
	if(pos.x == gt.x)
		ret.x = 0;
	else
	{
		if( abs(ch1 = (gt.x - pos.x)) < abs(ch2 = ((gt.x + statG_width) - pos.x)) )
			ret.x = ch1;
		else
			ret.x = ch2;
		if( abs(ret.x) > abs(ch1 = ((gt.x - statG_width) - pos.x)) )
			ret.x = ch1;
	}
	//y coords
	if(pos.y == gt.y)
		ret.y = 0;
	else
	{
		if( abs(ch1 = (gt.y - pos.y)) < abs(ch2 = ((gt.y + statG_height) - pos.y)) )
			ret.y = ch1;
		else
			ret.y = ch2;
		if( abs(ret.y) > abs(ch1 = ((gt.y - statG_height) - pos.y)) )
			ret.y = ch1;
	}

	return ret;
}
void world::_loopTrimVector(Vc2D & vec)
{
	vec.x=(((int)vec.x)%(statG_width+1));
	if(vec.x<0) vec.x+=(1+statG_width);
	vec.y=(((int)vec.y)%(statG_height+1));
	if(vec.y<0) vec.y+=(1+statG_height);
}
void world::_intTrimVector(Vc2D & vec)
{
	vec.x=((int)vec.x);
	vec.y=((int)vec.y);
}