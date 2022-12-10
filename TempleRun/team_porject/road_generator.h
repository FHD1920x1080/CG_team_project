#pragma once
#include<iostream>
#include"Object.h"

struct RoadBlock {
	Object obj;
	RoadBlock* next;
	RoadBlock* prev;
};

class RoadMap{
public:
	RoadMap(int);
	~RoadMap();

	void add_block(RoadBlock*);
	void showall(unsigned int*);

private:
	RoadBlock* start;
	RoadBlock* tail;
	int list_size;
	int max_size;
};

