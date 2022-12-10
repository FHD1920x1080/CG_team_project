#include"road_generator.h"

RoadMap::RoadMap(int input) : max_size{ input }
{
	list_size = 0;
}

RoadMap::~RoadMap()
{
	RoadBlock* target;
	while (true)
	{
		target = start;
		start = start->next;
		delete target;
		if (start == nullptr)
			break;
	}
}

void RoadMap::add_block(RoadBlock* a)
{
	if (start == nullptr)
	{
		start = a;
		tail = a;
		start->prev = nullptr;
		list_size++;
	}
	else if (start == tail) {
		start->next = a;
		tail = a;
		tail->prev = start;
		tail->obj.Pos.z = tail->prev->obj.front() + tail->obj.Scale.z / 2;
		list_size++;
	}
	else
	{
		a->prev = tail;
		tail->next = a;
		tail = a;
		tail->obj.Pos.z = tail->prev->obj.front() + tail->obj.Scale.z / 2;
		tail->next = nullptr;
		list_size++;
	}

	if (list_size > max_size) {
		RoadBlock* temp;
		temp = start;
		start = start->next;
		start->prev = nullptr;
		delete temp;
	}
}

void RoadMap::showall(unsigned int* modelLocation)
{
	RoadBlock* t;
	t = start;
	while (true)
	{
		t->obj.show(modelLocation);
		t = t->next;
		if (t == nullptr) break;
	}
	std::cout<<std::endl;
}

