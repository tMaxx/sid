#pragma once
static_assert(__cplusplus > 199711L, "Program requires C++11 capable compiler");
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "stats.h"

class disp final
{
private:
	disp() = delete;
	//DISPLAY VARIABLES
	//vector
	const Vc2D dFrameOffset = Vc2D(10,10);
	//state
	unsigned int dRunmode,dUsrChange;
	//DISPLAY VARIABLES

	//DISPLAY SHAPES
	const sf::Shape dbgSidRect = sf::Shape::Rectangle(dFrameOffset.x, dFrameOffset.y, (dFrameOffset.x+statG_width), (dFrameOffset.y+statG_height), sf::Color(124,252,50), 2.f, sf::Color(255,255,255));
	//fonts
	sf::Font dfontMonospace, dfontNormal, dfontBigger;
	//strings
	sf::String dtxtLLama, dtxtZwierzak, dtxtTextsAll, dtxtMonospace;
	//llama image
	sf::Image dimgtLLama;
	//llama image=>sprite
	sf::Sprite dtheLLama;
	//DISPLAY SHAPES

	struct dsPoints
	{
		static unsigned short int MaxNo;
		sf::Shape Sh; //since Sh[] is impossible...
	}dshpAll[statG_itemsmax];

	class stats * dStats;

	sf::RenderWindow * App;

	void update();

	void redraw();

	void procevents();

	void refflags();

	void convflagschar(unsigned int, std::ostringstream*);

	void closeme();

public:
	disp(class stats*);
	~disp();

	void LaunchMe();
};
unsigned short int disp::dsPoints::MaxNo = 1;

disp::disp(class stats *_stats = 0)
{
	dStats = _stats;
	dRunmode = dUsrChange = 0;

	App = new sf::RenderWindow(sf::VideoMode(500,320,32), "I am the LLama", sf::Style::Close, sf::WindowSettings());

	if(!dimgtLLama.LoadFromFile("llama.png"))
		throw dimgtLLama;
	dimgtLLama.SetSmooth(false);
	dtheLLama.SetImage(dimgtLLama);
	dtheLLama.SetCenter(-6,-2);

	if(!dfontMonospace.LoadFromFile("mono.ttf",15))
		dfontMonospace = sf::Font::GetDefaultFont();
	if(!dfontNormal.LoadFromFile("norm.ttf",15))
		dfontNormal = sf::Font::GetDefaultFont();
	if(!dfontBigger.LoadFromFile("norm.ttf",25))
		dfontBigger = sf::Font::GetDefaultFont();


	dtxtZwierzak = sf::String("LLamaNator 0.5h",dfontBigger);
	dtxtZwierzak.SetScale(.7f, .7f);
	dtxtZwierzak.Move(320.f, 4.f);

	dtxtTextsAll = sf::String("",dfontNormal);
	dtxtTextsAll.SetScale(.5f, .5f);
	dtxtTextsAll.Move(320.f, 35.f);

	dtxtMonospace = sf::String("",dfontMonospace);
	dtxtMonospace.SetScale(.5f,.5f);
	dtxtMonospace.Move(320.f, 142.f);

	dtxtLLama = sf::String("<'l\n ll\n llama~\n || ||\n '' ''",dfontMonospace);
	dtxtLLama.SetScale(.55f, .55f);
	dtxtLLama.Move(410.f, 240.f);
	dtxtLLama.SetStyle(sf::String::Bold);

	//Goto point
	dshpAll[1].Sh = sf::Shape::Circle(Vc2D::ToSFML(dFrameOffset),1.5f,sf::Color(255,50,50));
}

disp::~disp()
{
	dStats = 0;
	delete App;
}

void disp::LaunchMe()
{
	while(App->IsOpened())
	{
		procevents();

		App->Clear(sf::Color(56,56,56));

		if(stats::MePtr!=0)
		{
			if(dStats!=stats::MePtr)
				dStats = stats::MePtr;
			//else if(
				dStats->sMutex->lock();//)
			//{
			update();
			dStats->sMutex->unlock();
			//}else
			//	refflags();
		}

		redraw();

		App->Display();
	}
}

void disp::update()
{
	std::ostringstream ss;
	static bool dtheLLamaFlip = true; //1: right, 0:left

	//Position vectors & points
	if((stats::sItm::WasChanged)||(disp::dsPoints::MaxNo!=stats::sItm::MaxNo))
	{
		dtheLLama.SetPosition(Vc2D::ToSFML(dStats->sPosition->Pos));
		//flip the llama!
		if((dStats->sGoto->Pos.x > dStats->sPosition->Pos.x))
		{	if(!dtheLLamaFlip)
			{
				dtheLLama.FlipX(false);
				dtheLLamaFlip = true;
		}	}
		else
		{	if(dtheLLamaFlip)
			{
				dtheLLama.FlipX(true);
				dtheLLamaFlip = false;
		}	}

		(dshpAll[1].Sh).SetPosition(Vc2D::ToSFML(dStats->sGoto->Pos));

		disp::dsPoints::MaxNo = stats::sItm::MaxNo;

		for (int i = 2; i <= disp::dsPoints::MaxNo; ++i)
			if(dStats->sitmAll[i]->ItmChanged){
				if((dStats->sitmAll[i]->Type) == 'l')
				dshpAll[i].Sh = sf::Shape::Circle(Vc2D::ToSFML(dStats->sitmAll[i]->Pos + dFrameOffset),25, \
																		sf::Color(0,255,0,100), 1.f, sf::Color(0,140,0));
				else if((dStats->sitmAll[i]->Type) == 'c')
					dshpAll[i].Sh = sf::Shape::Circle(Vc2D::ToSFML(dStats->sitmAll[i]->Pos + dFrameOffset),1.5f, \
																		sf::Color(0,80,0), 3.5f, sf::Color(0,102,0));
				((dStats->sitmAll[i]->ItmChanged))=false;
			}
	}

	//Stats update
	ss << "G:" << (dStats->sGoto->Pos) << " P:" << (dStats->sPosition->Pos) << \
		"\nHealth: " << dStats->sHealth << "/1000\nFondy: " << dStats->sHappiness << \
		"\nScore:  " << dStats->sScore << "\nDelay:  " << ((dRunmode & statF_wFAST)?0:dStats->sDelay) << " ms" << \
		"\n\nCurrent flags:\n\n\n\nFlags to set:\n";
	//Set the text
	dtxtTextsAll.SetText(ss.str());
	//clear stream
	ss.str("");

	//Runmode sync
	dStats->sRMrequest = dUsrChange;
	dRunmode = dStats->flagsSyncUpper();
	disp::convflagschar(dRunmode,&ss);
	ss<<"\n\n";
	disp::convflagschar(dUsrChange,&ss);
	dUsrChange = 0;
	dtxtMonospace.SetText(ss.str());
}

void disp::redraw()
{
	App->Draw(dbgSidRect);
	App->Draw(dtxtZwierzak);
	App->Draw(dtxtTextsAll);
	App->Draw(dtxtMonospace);
	if(dRunmode & statF_llama)
		App->Draw(dtxtLLama);
	for (int i = disp::dsPoints::MaxNo; i >= 1; --i)
		App->Draw(dshpAll[i].Sh);
	App->Draw(dtheLLama);
}

void disp::procevents()
{
	sf::Event AppEvent;

	while (App->GetEvent(AppEvent))
	{
		switch(AppEvent.Type){

		// Close window : exit
		case sf::Event::Closed:
			disp::closeme();
			App->Close();
			return;
			break;


		case sf::Event::KeyPressed:
			switch(AppEvent.Key.Code){

			// Take a screenshot
			case sf::Key::F10:
				{
				sf::Image Screen;
				Screen = (App->Capture());
				Screen.SetSmooth(false);
				Screen.SaveToFile("zrzut.png");
				}break;

			//Quit
			case sf::Key::F4:
				if (AppEvent.Key.Alt)
				{
					disp::closeme();
					App->Close();
					return;
				}
				break;
			case sf::Key::Q:
				if (AppEvent.Key.Control)
				{
					disp::closeme();
					App->Close();
					return;
				}
				break;

			default:
				break;
			}
			break;

		case sf::Event::KeyReleased:
			switch(AppEvent.Key.Code)
			{
			//WORLD MANAGING
			//Thread
			case sf::Key::Up:
				(((dUsrChange &= statF_Mdlay))) |= (statF_wdlCH | statF_wdlUD);
				break;
			case sf::Key::Down:
				(((dUsrChange &= statF_Mdlay))) |= statF_wdlCH;
				break;
			case sf::Key::Space:
				dUsrChange |= statF_thPSE;
				break;
			case sf::Key::B:
				if(AppEvent.Key.Control && AppEvent.Key.Shift)
					dUsrChange |= statF_wFAST;
				break;
			case sf::Key::N:
				if(AppEvent.Key.Control && AppEvent.Key.Shift)
					dUsrChange |= statF_RESET;
				break;
			//Items
			case sf::Key::Z: //add life
				dUsrChange |= statF_itADl;
				break;
			case sf::Key::X: //add collectb
				dUsrChange |= statF_itADc;
				break;
			case sf::Key::C: //remove life
				dUsrChange |= statF_itRMl;
				break;
			case sf::Key::V: //remove collect
				dUsrChange |= statF_itRMc;
				break;
			//Movement
			case sf::Key::Period: //shorter way alg.
				dUsrChange |= statF_wmSHW;
				break;
			case sf::Key::Comma: //weird way alg.
				dUsrChange |= statF_wmWEW;
				break;
			//WORLD MANAGING

			//SID MANAGEMENT
			case sf::Key::A:
				dUsrChange |= statF_smAUT;
				break;
			case sf::Key::D:
				dUsrChange |= statF_smDET;
				break;
			case sf::Key::S:
				dUsrChange |= statF_smRND;
				break;
			case sf::Key::F:
				dUsrChange |= statF_smMID;
				break;
			case sf::Key::L:
				dUsrChange |= statF_llama;
				break;
			//SID MANAGEMENT

			default:
				break;
			}
			break;

		default:
			break;
		}
	}
}


void disp::refflags()
{
	std::ostringstream ss;
	disp::convflagschar(dRunmode,&ss);
	ss<<"\n\n";
	disp::convflagschar(dUsrChange,&ss);
	dtxtMonospace.SetText(ss.str());
}

void disp::convflagschar(unsigned int n, std::ostringstream * ss)
{
	unsigned int mask = 0x80000000;
	unsigned char space = 97; //'a'
	do
	{
		*ss << (char)(((n & mask)?(space-32):space));
		if(space==(112))
		{
			*ss<<"\n";
			space=97;
		}else if(space++%4==0)
			*ss<<" ";

	}
	while (mask>>=1);
}

void disp::closeme()
{
	dStats->sMutex->lock();
	dStats->sRunmode |= statF_thKIL | statF_thPSE;
	dStats->sDelayRunning = false;
	dStats->sMutex->unlock();
}