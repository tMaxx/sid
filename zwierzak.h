#include "statsd.h"

class zwierzak final
{
private:
	zwierzak() = delete;

	void setMove(unsigned int&,unsigned int);

	Vc2D GetNearest(char);
	Vc2D GetFirst(char);

	Vc2D GetRandomPt();
	int GetOfRadius();

	struct statsd *Stats;

	unsigned int Runmode;
	Vc2D LongGoto;

public:
	zwierzak(struct statsd * _stats) : Stats(_stats){}
	~zwierzak(){}

	void SidAI();
};


void zwierzak::SidAI()
{
	//First, copy for local (not nec. faster, just easier for typing)
	Runmode = Stats->sRunmodeD;

	//Check for automatic mode
	if(Runmode & statF_smAUT)
		Runmode |= statF_smDET | statF_smMID;
	//Remove 'teleport', not yet sure if we want to use it
	Runmode &= ~statF_smMVE;

	if((Stats->sGotoD->PosD).GetDistance(Stats->sPositionD->PosD) <= 3)
		setMove(Runmode, 0);

	//Health
	if( ((Runmode & statF_smDET) && ((Stats->sHealthD) < 400)) || \
						(!(Runmode & statF_smDET) && (Runmode & statF_Shcrt)) )
	{
		if(!(Runmode & statF_sLIFE))
			(Stats->sGotoD->PosD) = GetNearest('l');
		setMove(Runmode,statF_sLIFE);
		goto SidAI_EndRun;
	}

	//Happiness
	if( ((Runmode & statF_smDET) && ((Stats->sHappinessD) < 200)) || \
						(!(Runmode & statF_smDET) && (Runmode & statF_Siemo)) )
	{
		if(!(Runmode & statF_sMOAR))
			(Stats->sGotoD->PosD) = GetNearest('c');
		setMove(Runmode,statF_sMOAR);
		goto SidAI_EndRun;
	}

	setMove(Runmode,statF_sWHTV);


	if(Runmode & statF_smMID)
	{
		//create tempint
		int tempInt = GetOfRadius();
		if((tempInt) && ((Stats->sPositionD->PosD) != (Stats->sitmAllD[(tempInt)]->PosD)))
		{
			(Stats->sGotoD->PosD) = (Stats->sitmAllD[(tempInt)]->PosD);
			goto SidAI_OmmitMID;
		}
	}

	//Choose between collecting & random target on map
	if((Stats->sGotoD->PosD).GetDistance(Stats->sPositionD->PosD) <= 3)
	{
	if((RanDev()%8)>=1)
		(Stats->sGotoD->PosD) = GetFirst('c');
	else
		(Stats->sGotoD->PosD) = GetRandomPt();
	}

	SidAI_OmmitMID:
	//Shall we use teleport?
	if((Runmode & statF_smRND))
	{
	if((Runmode & statF_sWHTV) && ((RanDev()%200)==1))
		Runmode |= statF_smMVE;
	else
		Runmode &= ~statF_smMVE;
	}

	SidAI_EndRun:
	Stats->sRunmodeD = Runmode;
}

void zwierzak::setMove(unsigned int &set,unsigned int val)
{
	set &= ~(statF_MStep);
	set |= val;
}

Vc2D zwierzak::GetNearest(char type)
{
	bool wasisdas = true;
	int itm;
	if(type=='l')
		itm = statsd::sItmD::LifeCountD;
	else
		itm = statsd::sItmD::CollectCountD;
	Vc2D ret = Vc2D(-1337,-1337),tmp;
	for (int all = statsd::sItmD::MaxNoD ; (all >= 2) && (itm > 0) ; --all)
		if((Stats->sitmAllD[all]->TypeD)==type)
		{
			if(wasisdas)
			{
				ret = (Stats->sitmAllD[all]->PosD);
				wasisdas = false;
			}
			else if(((((Stats->sitmAllD[all]->PosD) - (Stats->sPositionD->PosD))).Length()) < \
														((ret - (Stats->sPositionD->PosD)).Length()))
				ret = Stats->sitmAllD[all]->PosD;
			itm--;
		}


	return (wasisdas?(Stats->sPositionD->PosD):ret);
}

Vc2D zwierzak::GetFirst(char type)
{
	for (int i = 2; i <= statsd::sItmD::MaxNoD; ++i)
		if((Stats->sitmAllD[i]->TypeD)==type)
			return (Stats->sitmAllD[i]->PosD);
	return (Stats->sPositionD->PosD);
}

Vc2D zwierzak::GetRandomPt()
{
	return Vc2D(RanDev() % (statG_width+1),RanDev() % (statG_height+1));
}

int zwierzak::GetOfRadius()
{
	for (int i = 2; i <= statsd::sItmD::MaxNoD; ++i)
		if(((Stats->sitmAllD[i]->TypeD)=='c') && \
			(((Stats->sPositionD->PosD).GetDistance(Stats->sitmAllD[i]->PosD))<=20))
			return i;
	return 0;
}