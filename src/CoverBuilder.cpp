#include "CoverBuilder.h"

#define MINIMUM_PLANE_KOORD_STEP 0.01f
#define MAX_PLANE_INT_KOORD 100

CoverBuilder::CoverBuilder(void)
	: isSieve(false)
{
	FillPlanes(planesX);
	FillPlanes(planesY);
	FillPlanes(planesZ);
}


CoverBuilder::~CoverBuilder(void)
{
}

void CoverBuilder::ClearAllPlanes() {
	for (auto i = 0; i < 11; ++i) {
		planesX[i].clear();
		planesY[i].clear();
		planesZ[i].clear();
	}
}
/// TODO - передават положение камеры для настояцего вычисления
/// взять плоскости исходя из того что мы находимся в точке -1, -1, -1
bool CoverBuilder::GetApropriateCellsNumbers(std::set<int> & result) {
	if (isSieve)
		return false;
	
	result.insert(cellsInPlanes[0].begin(), cellsInPlanes[0].end());
	result.insert(cellsInPlanes[2].begin(), cellsInPlanes[2].end());
	result.insert(cellsInPlanes[4].begin(), cellsInPlanes[4].end());
	return true;
}

void CoverBuilder::AddCellsIntoPlaneX(std::set<int> & plane, int x) {
	for (int y = 0; y < 10; ++y) {
		for (int z = 0; z < 10; ++z) {
			int cellNumber = x * 100 + y * 10 + z;
			plane.insert(cellNumber);
		}
	}
}

void CoverBuilder::AddCellsIntoPlaneY(std::set<int> & plane, int y) {
	for (int x = 0; x < 10; ++x) {
		for (int z = 0; z < 10; ++z) {
			int cellNumber = x * 100 + y * 10 + z;
			plane.insert(cellNumber);
		}
	}
}

void CoverBuilder::AddCellsIntoPlaneZ(std::set<int> & plane, int z) {
	for (int x = 0; x < 10; ++x) {
		for (int y = 0; y < 10; ++y) {
			int cellNumber = x * 100 + y * 10 + z;
			plane.insert(cellNumber);
		}
	}
}

void CoverBuilder::FinishCovering() {
	for (int i = 0; i < 10; ++i) {
		AddCellsIntoPlaneX(cellsInPlanes[0], i);
		if (planesX[i].empty())
			break;
	}
	
	AddCellsIntoPlaneX(cellsInPlanes[0], 9);
	for (int i = 10; i > 0; --i) {
		AddCellsIntoPlaneX(cellsInPlanes[1], i - 1);
		if (planesX[i].empty())
			break;
	}

	AddCellsIntoPlaneY(cellsInPlanes[0], 0);
	for (int i = 0; i < 10; ++i) {
		AddCellsIntoPlaneY(cellsInPlanes[2], i);
		if (planesY[i].empty())
			break;
	}

	AddCellsIntoPlaneY(cellsInPlanes[0], 9);
	for (int i = 10; i > 0; --i) {
		AddCellsIntoPlaneY(cellsInPlanes[3], i - 1);
		if (planesY[i].empty())
			break;
	}

	AddCellsIntoPlaneZ(cellsInPlanes[0], 0);
	for (int i = 0; i < 10; ++i) {
		AddCellsIntoPlaneZ(cellsInPlanes[4], i);
		if (planesZ[i].empty())
			break;
	}

	AddCellsIntoPlaneZ(cellsInPlanes[0], 9);
	for (int i = 10; i > 0; --i) {
		AddCellsIntoPlaneZ(cellsInPlanes[5], i - 1);
		if (planesZ[i].empty())
			break;
	}

	for (int i = 0; i < 6; ++i) {
		if (cellsInPlanes[i].size() == 1000) {
			isSieve = true;
			return;
		}
	}
}

void CoverBuilder::AddSquareToXPlane(int planeNumber, float centreX, float centreY, float radius) {
	RemoveSquareFromPlane(planesX[planeNumber], centreX, centreY, radius);
}

void CoverBuilder::AddSquareToYPlane(int planeNumber, float centreX, float centreY, float radius) {
	RemoveSquareFromPlane(planesY[planeNumber], centreX, centreY, radius);
}

void CoverBuilder::AddSquareToZPlane(int planeNumber, float centreX, float centreY, float radius) {
	RemoveSquareFromPlane(planesZ[planeNumber], centreX, centreY, radius);
}

void CoverBuilder::RemoveSquareFromPlane(std::set<PointKoords> & plane, float centreX, float centreY, float radius) {
	if (radius < MINIMUM_PLANE_KOORD_STEP)
		return;

	int normalizedX1 = (int((centreX - radius) / MINIMUM_PLANE_KOORD_STEP));
	int normalizedX2 = (int((centreX + radius) / MINIMUM_PLANE_KOORD_STEP));
	int normalizedY1 = (int((centreY - radius) / MINIMUM_PLANE_KOORD_STEP));
	int normalizedY2 = (int((centreY + radius) / MINIMUM_PLANE_KOORD_STEP));
	for (int x = normalizedX1; x < normalizedX2; ++x)
		for (int y = normalizedY1; y < normalizedY2; ++y)
			plane.erase(PointKoords(x, y));
}

void CoverBuilder::FillPlanes(std::set<PointKoords> * planes) {
	for (int i = 0; i <= 10; i++) {
		for (int x = 0; x <= MAX_PLANE_INT_KOORD; ++x) {
			for (int y = 0; y <= MAX_PLANE_INT_KOORD; ++y)
			{
				planes[i].insert(PointKoords(x, y));
			}
		}
	}
}
