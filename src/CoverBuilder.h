#pragma once

#include <set>

struct PointKoords {
	PointKoords (int x, int y) 
		: x(x), y(y)
	{}
	int x;
	int y;
	bool operator < (const PointKoords & right) const {
		if (x < right.x)
			return true;
		if (x > right.x)
			return false;
		return y < right.y;
	}
};

class CoverBuilder
{
public:
	CoverBuilder(void);
	~CoverBuilder(void);

	void ClearAllPlanes();

	bool GetApropriateCellsNumbers(std::set<int> & result);
	void FinishCovering();

	void AddSquareToXPlane(int planeNumber, float centreX, float centreY, float radius);
	void AddSquareToYPlane(int planeNumber, float centreX, float centreY, float radius);
	void AddSquareToZPlane(int planeNumber, float centreX, float centreY, float radius);
private:
	void AddCellsIntoPlaneX(std::set<int> & plane, int x);
	void AddCellsIntoPlaneY(std::set<int> & plane, int y);
	void AddCellsIntoPlaneZ(std::set<int> & plane, int z);
	void FillPlanes(std::set<PointKoords> * planes);
	void RemoveSquareFromPlane(std::set<PointKoords> & plane, float centreX, float centreY, float radius);

	std::set<PointKoords> planesX[11];
	std::set<PointKoords> planesY[11];
	std::set<PointKoords> planesZ[11];

	std::set<int> cellsInPlanes[6];
	bool isSieve;
};

