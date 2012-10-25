#pragma once

#include "windows.h"
#include <vector>
#include <list>
#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"

#include "CoverBuilder.h"
/// класс дл€ хранени€ сфер по €чейкам
class SphereWorld
{
public:
	SphereWorld(float maxSphereRadius = 0.1f, float sphereMaxKoordPerAxis = 1.0f);
	/// сгененрировать покрытие сторон каждой €чейки сферами наход€щимис€ внутри каждой €чейки
	void GenerateCover(CoverBuilder & coverBuilder);
	/// добавить сферу
	void AddSphereKoord(SphereKoords * kandidate);
	/// добавить мир с вычислением вхождений одних сфер в другие
	void AddSphereWorld(SphereWorld & kandidate);
	/// добавить мир без вычислени€
	void SimpleAddSphereWorld(SphereWorld & kandidate);
	void SphereWorld::Clear();
	/// добавить мир многопоточно с вычислени€ми вхождений
	void AddSphereWorldInSeveralThreads(SphereWorld & kandidate, int threadAmount);
	/// генераци€ матриц дл€ сфер (перемещение и ресайз) по €чейкам
	void GenerateMatrixInCells(long matrixInCells[1000][2], mat4 * matrixStore);
	/// узнать сколько матриц в мире
	int GetCurrentSize();
	/// добавить €чейку
	void AddCell(std::vector<SphereKoords *> & out, int x, int y, int z, std::vector<SphereKoords *> const & in);
	~SphereWorld(void);
	bool CompareSphereInCell(int x, int y, int z, SphereKoords * sphere);

	std::vector<SphereKoords *> cells[1000];
	float maxSphereRadius;
	int maxCellNumber;
};

void clearWorlds(SphereWorld * sphereWorlds, int sphereWorldNumber);
/// структура дл€ построени€ мира сфер из глобального массива многопоточно
struct SphereWorldWorkerData {
	SphereWorldWorkerData(int sphereWorldNumber = 0, SphereWorld * worlds = nullptr, int sphereNumber = 0, SphereKoords * spheres = nullptr)
		:	sphereWorldNumber(sphereWorldNumber), worlds(worlds), sphereNumber(sphereNumber), spheres(spheres)
	{}
	int sphereWorldNumber;
	SphereWorld * worlds;
	long sphereNumber;
	SphereKoords * spheres;
};
/// поток дл€ построени€ мира
DWORD WINAPI SphereWorldWorker(LPVOID lpArg);
