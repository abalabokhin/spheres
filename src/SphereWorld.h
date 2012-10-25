#pragma once

#include "windows.h"
#include <vector>
#include <list>
#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"

#include "CoverBuilder.h"

class SphereWorld
{
public:
	SphereWorld(float maxSphereRadius = 0.1f, float sphereMaxKoordPerAxis = 1.0f);
	void GenerateCover(CoverBuilder & coverBuilder);
	void AddSphereKoord(SphereKoords * kandidate);
	void AddSphereWorld(SphereWorld & kandidate);
	void SimpleAddSphereWorld(SphereWorld & kandidate);
	void SphereWorld::Clear();
	void AddSphereWorldInSeveralThreads(SphereWorld & kandidate, int threadAmount);
	void GenerateMatrixInCells(long matrixInCells[1000][2], mat4 * matrixStore);
	//void GenerateMatrixInCells(std::vector<M3DMatrix44f> * matrixInCells);
	//std::vector<SphereKoords *> GetAllSpheres();
	int GetCurrentSize();
	void AddCell(std::vector<SphereKoords *> & out, int x, int y, int z, std::vector<SphereKoords *> const & in);
	~SphereWorld(void);
	bool CompareSphereInCell(int x, int y, int z, SphereKoords * sphere);

	std::vector<SphereKoords *> cells[1000];
	float maxSphereRadius;
	int maxCellNumber;
};

void clearWorlds(SphereWorld * sphereWorlds, int sphereWorldNumber);

struct SphereWorldWorkerData {
	SphereWorldWorkerData(int sphereWorldNumber = 0, SphereWorld * worlds = nullptr, int sphereNumber = 0, SphereKoords * spheres = nullptr)
		:	sphereWorldNumber(sphereWorldNumber), worlds(worlds), sphereNumber(sphereNumber), spheres(spheres)
	{}
	int sphereWorldNumber;
	SphereWorld * worlds;
	long sphereNumber;
	SphereKoords * spheres;
};

DWORD WINAPI SphereWorldWorker(LPVOID lpArg);
