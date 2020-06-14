#pragma once
#include "valve_sdk/math/Vector.hpp"
#include <vector>



class LegitBacktrack
{
public:
	struct Tick
	{
		Tick(int Tickcount, Vector Head)
		{
			tick = Tickcount;
			head = Head;
		}
		int tick = -1;
		Vector head;
	};

private:
	std::vector<Tick> tick[64];
};

