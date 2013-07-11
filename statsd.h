#pragma once
#include <random>
#include "Vector2D.h"

std::random_device RanDev; //pseudorandom number generator

struct statsd
{
	//WORLD CONSTANS
	#define statG_width  300
	#define statG_height 300
	//WORLD CONSTANS

	//FLAGS
	//Sid
	#define statF_sDEAD  0x00000001
						 //[world-side] sid be dead
	#define statF_sLIFE  0x00000002
						 //sid be searching for life refill
	#define statF_sMOAR  0x00000004
						 //sid be collecting collectibles
	#define statF_sWHTV  0x00000008
						 //sid be doing whatever (power saving mode)
	//Sid Managing
	#define statF_smAUT  0x00000010
						 //[usr-s] auto mode
	#define statF_smRND  0x00000020
						 //use random determining
	#define statF_smMVE  0x00000040
						 //sid/world: llama uses teleport!!
	#define statF_smDET  0x00000080
						 //depend on flags(0) or actual readings(1) (estimation)
	#define statF_smMID  0x00000100
						 //[implement later] middle-way collecting
	//purely informative
	#define statF_Sedge  0x00001000
						 //sid at the edge of world
	#define statF_Shcrt  0x00002000
						 //health critical
	#define statF_Siemo  0x00004000
						 //i have depression (I, Emo.)
	#define statF_llama  0x00008000
						 //i am llama
	//Masks
	#define statF_MStep  (statF_sLIFE | statF_sMOAR | statF_sWHTV)
						 //nextstep filter
	#define statF_MSall  (statF_MStep | statF_smAUT | statF_smRND | statF_smMVE | statF_smDET | statF_smMID)
						 //state filter
	//FLAGS

	unsigned int sRunmodeD; //bit masks

	int sHealthD, sHappinessD;
	unsigned long long int sScoreD; //health, h-ness and points earned

	struct sItmD
	{
		static short int MaxNoD;
		static short int LifeCountD;
		static short int CollectCountD;
		Vc2D PosD;
		char TypeD;
	} **sitmAllD, *sPositionD, *sGotoD;

};
short int statsd::sItmD::MaxNoD = -1;
short int statsd::sItmD::LifeCountD = 0;
short int statsd::sItmD::CollectCountD = 0;