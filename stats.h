#pragma once
#include <mutex>
#include "statsd.h"

class stats final : public statsd
{
public:
	stats(void);
	~stats(void);

	static class stats * MePtr; //self-pointer

	//WORLD CONSTANS
	#define statG_width_half  (statG_width >> 1)
	#define statG_height_half (statG_height >> 1)
	#define statG_healthmax   1000
	#define statG_healthcrit  300
	#define statG_healthzero  0
	#define statG_happicrit   (statG_healthcrit * 1.25)
	#define statG_happimax    (statG_healthmax << 1)
	#define statG_happizero   -300
	#define statG_itemsmax    75
	//WORLD CONSTANS

	//ITEM CONSTANS
	#define statI_collRadius  5
	#define statI_collMaxNO	  (statG_itemsmax >> 3)
	#define statI_collProbGen 3000
	#define statI_collPts	  15
	#define statI_lifeRadius  25
	#define statI_lifeMaxNO	  (statG_itemsmax >> 6)
	#define statI_lifeProbGen 18000
	//ITEM CONSTANS

	//FLAGS
	//THread
	#define statF_thRUN  0x00010000
						//thread is present
	#define statF_thPSE  0x00020000
						//thread is paused
	#define statF_thKIL  0x00040000
						//halt & kill yrself immediately, we're shutting down
	//World
	#define statF_wUPD8  0x00080000
						//world was updated
	#define statF_wFAST  0x00100000
						//fast mode - 1ms delay
	//DeLay
	#define statF_wdlCH  0x00200000
						//chrono/delay will be changed
	#define statF_wdlUD  0x00400000
						//chrono/delay goes up(1) or down (0)
	//World Managing
	#define statF_wmSHW  0x00800000
						//shorter way algorithm
	#define statF_wmWEW  0x01000000
						//weird way agorithm
	//ITems
	#define statF_itADl  0x08000000
						//add life item
	#define statF_itADc  0x10000000
						//add collect item
	#define statF_itRMl  0x20000000
						//remove random life item
	#define statF_itRMc  0x40000000
						//remove random collect item
	//RESET world
	#define statF_RESET  0x80000000
						//reset the world
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
	//sid moved upper!
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
	//masks for CoMparison
	#define statF_Mdlay (statF_wdlUD | statF_wdlCH)
						//for chrono delay check
	#define statF_Mitem (statF_itADl | statF_itADc | statF_itRMl | statF_itRMc)
						//for items
	#define statF_Mexor (statF_llama | statF_thPSE | statF_wFAST | statF_wmWEW | statF_wmSHW | \
								statF_smRND | statF_smDET | statF_smAUT | statF_smMVE | statF_smMID)
	//FLAGS


	std::mutex *sMutex; //lock o' death

	unsigned int sRMrequest, sRunmode; //bit masks

	unsigned int sDelay; //world thread sleep, in milliseconds
	bool sDelayRunning; //wts, is currently waiting

	int sHealth, sHappiness; //stats (health, h-ness)
	unsigned long long int sScore; //points earned (should one be willing to set records, switch to double)

	struct sItm : sItmD
	{
		static short int MaxNo;
		static bool WasChanged; //was anything changed
		static short int LifeCount;
		static short int CollectCount;
		bool ItmChanged; //was this item changed
		Vc2D Pos;
		char Type; //Position, Goto, Life, Collectibles

		sItm(Vc2D p,char t)
		{	PosD=Pos = p; TypeD=Type = t;	WasChanged = ItmChanged = true; MaxNoD = (MaxNo++);
			if(Type=='l')LifeCountD = (LifeCount++);else if(Type=='c')CollectCountD = (CollectCount++);}
		~sItm()
		{	if(Type=='l')LifeCountD = (LifeCount--);else if(Type=='c')CollectCountD = (CollectCount--);
			MaxNoD = (MaxNo--); WasChanged = true;}
		void Up(){ if(Pos==PosD)return; Pos = PosD; stats::sItm::WasChanged = ItmChanged = true; }
		void Down(){ if(Pos==PosD)return; PosD = Pos; stats::sItm::WasChanged = ItmChanged = true; }
	} *sitmAll[statG_itemsmax], *sPosition, *sGoto;
	void ItemPushBack(Vc2D,char);
	void ItemDelBack();
	void ItemDelFront(char);
	void ItemDelNo(int);
	void SyncStats(bool);

	void flagsSyncAll();
	unsigned int flagsSyncUpper();
};
short int stats::sItm::MaxNo = -1;
bool stats::sItm::WasChanged = true;
class stats * stats::MePtr = 0;
short int stats::sItm::LifeCount = 0;
short int stats::sItm::CollectCount = 0;

stats::stats(void)
{
	sMutex = new std::mutex();
	sMutex->lock();
	sDelay = 160;
	sDelayRunning = false;
	sScoreD = sScore = 0;
	sRunmodeD = sRunmode = statF_wUPD8 & statF_sDEAD;
	sRMrequest = 0;
	sHealthD = sHealth = statG_healthmax;
	sHappinessD = sHappiness = statG_healthmax / 2;

	sitmAllD = (struct sItmD**)sitmAll;

	stats::sItm::MaxNo = -1;
	stats::sItm::WasChanged = true;
	stats::sItm::LifeCount = 0;
	stats::sItm::CollectCount = 0;
	statsd::sItmD::MaxNoD = -1;
	statsd::sItmD::LifeCountD = 0;
	statsd::sItmD::CollectCountD = 0;

	MePtr = this;
	sMutex->unlock();
}

stats::~stats(void)
{
	MePtr = 0;
	stats::sItm::MaxNo = -1;
	stats::sItm::WasChanged = false;

	if(!(sRunmode&statF_RESET))
		sMutex->lock();

	for (int i = 0; i <= stats::sItm::MaxNo; ++i)
		delete sitmAll[i];
	delete sMutex;
}

void stats::ItemPushBack(Vc2D p,char t)
{
	if(((t!='l')&&(t!='c'))||(stats::sItm::MaxNo >= (statG_itemsmax - 1)))
		return;
	sitmAll[stats::sItm::MaxNo] = new stats::sItm(p, t);
}
void stats::ItemDelBack()
{
	if(stats::sItm::MaxNo < 2)
		return;
	delete sitmAll[stats::sItm::MaxNo];
}
void stats::ItemDelFront(char type)
{
	if(((type!='l')&&(type!='c'))||(stats::sItm::MaxNo <=1))
		return;

	for (int i = 2; i <= stats::sItm::MaxNo; ++i)
		if(sitmAll[i]->Type == type)
		{
			ItemDelNo(i);
			return;
		}
}
void stats::ItemDelNo(int id)
{
	if((id>stats::sItm::MaxNo)||(id<2))
		return;
	delete sitmAll[id];
	for (; id <= stats::sItm::MaxNo; ++id)
		(sitmAll[id] = sitmAll[(id+1)])->ItmChanged = true;
}

void stats::SyncStats(bool ispos=false)
{
	sGoto->Up();
	flagsSyncAll();
	if(ispos)return;
	for (int i = 0; i <= stats::sItm::MaxNo; ++i)
		sitmAll[i]->Down();
	sHealthD = sHealth;
	sHappinessD = sHappiness;
	sRunmodeD = sRunmode;
	sScoreD = sScore;
	sItmD::MaxNoD = stats::sItm::MaxNo;
	sItmD::LifeCountD = stats::sItm::LifeCount;
	sItmD::CollectCountD = stats::sItm::CollectCount;
}

void stats::flagsSyncAll()
{
	sRunmode &= ~statF_MSall;
	sRunmode |= (sRunmodeD & statF_MSall);
	sRunmodeD = sRunmode;
}

unsigned int stats::flagsSyncUpper()
{
	unsigned int ret;
	sRunmode ^= (sRMrequest & statF_Mexor);
	//delay
	if(sRMrequest & statF_Mdlay)
	{
		sRunmode &= ~statF_Mdlay;
		sRunmode |= (sRMrequest & statF_Mdlay);
	}
	ret = (sRunmode |= (sRMrequest & ~statF_Mdlay & ~statF_Mexor));
	sRunmodeD = sRunmode &= ~statF_wUPD8;
	sRMrequest = 0;
	return ret;
}