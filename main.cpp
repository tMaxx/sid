#include "disp.h"
#include "world.h"

int main()
{
	class world * w = new class world (new class stats);
	class disp  * d = new class disp (stats::MePtr);

	w->LaunchMe();
	d->LaunchMe();

	delete d;
	delete w;

	return EXIT_SUCCESS;
}