#pragma once

struct GridPosition
{
	int x;
	int z;
};

struct tile_path
{
	int x, z;
	int gCost;
	int hCost;
	int fCost() const { return gCost + hCost; }
	GridPosition parent;
};
