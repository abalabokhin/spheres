
#include <math.h>
#include <iostream>
#include <set>
#include "SphereWorld.h"

#define RadiusEpselone 0.001f

SphereWorld::SphereWorld(float maxSphereRadius, float sphereMaxKoordPerAxis) 
	:	maxSphereRadius(maxSphereRadius), maxCellNumber(static_cast<int>(sphereMaxKoordPerAxis / maxSphereRadius))
{}


SphereWorld::~SphereWorld(void)
{
}

void SphereWorld::GenerateCover(CoverBuilder & builder) {
	/// груба€ проверка - верна только дл€ рандомно сгенерированных сфер - если сфер в каждой €чейке > 100, то считаем что покрытие посто€нно.
	bool allCellsFull = true;
	for (int i = 0; i < 1000; i++) {
		if (cells[i].size() < 100) {
			allCellsFull = false;
			break;
		}
	}

	if (allCellsFull) {
		builder.ClearAllPlanes();
		builder.FinishCovering();
		return;	
	}

	for (int i = 0; i < 1000; i++) {
		int numberX = i / 100;
		int numberY = (i / 10) % 10;
		int numberZ = i % 10 ;

		float planeX1 = 0.1f * numberX;
		float planeX2 = 0.1f * (numberX + 1);
		float planeY1 = 0.1f * numberY;
		float planeY2 = 0.1f * (numberY + 1);
		float planeZ1 = 0.1f * numberZ;
		float planeZ2 = 0.1f * (numberZ + 1);

		for (auto sphere = cells[i].begin(); sphere != cells[i].end(); ++sphere) {
			float r2 = (*sphere)->r * (*sphere)->r;
			if (r2 > ((*sphere)->x - planeX1) * ((*sphere)->x - planeX1))
				builder.AddSquareToXPlane(numberX, (*sphere)->y, (*sphere)->z, 0.7f * sqrt(r2 - ((*sphere)->x - planeX1) * ((*sphere)->x - planeX1)));
			if (r2 > ((*sphere)->x - planeX2) * ((*sphere)->x - planeX2))
				builder.AddSquareToXPlane(numberX + 1, (*sphere)->y, (*sphere)->z, 0.7f * sqrt(r2 - ((*sphere)->x - planeX2) * ((*sphere)->x - planeX2)));

			if (r2 > ((*sphere)->y - planeY1) * ((*sphere)->y - planeY1))
				builder.AddSquareToYPlane(numberY, (*sphere)->x, (*sphere)->z, 0.7f * sqrt(r2 - ((*sphere)->y - planeY1) * ((*sphere)->y - planeY1)));
			if (r2 > ((*sphere)->y - planeY2) * ((*sphere)->y - planeY2))
				builder.AddSquareToYPlane(numberY + 1, (*sphere)->x, (*sphere)->z, 0.7f * sqrt(r2 - ((*sphere)->y - planeY2) * ((*sphere)->y - planeY2)));

			if (r2 > ((*sphere)->z - planeZ1) * ((*sphere)->z - planeZ1))
				builder.AddSquareToZPlane(numberZ, (*sphere)->x, (*sphere)->y, 0.7f * sqrt(r2 - ((*sphere)->z - planeZ1) * ((*sphere)->z - planeZ1)));
			if (r2 > ((*sphere)->z - planeZ2) * ((*sphere)->z - planeZ2))
				builder.AddSquareToZPlane(numberZ + 1, (*sphere)->x, (*sphere)->y, 0.7f * sqrt(r2 - ((*sphere)->z - planeZ2) * ((*sphere)->z - planeZ2)));
		}
	}
	builder.FinishCovering();
}

inline int SphereIncludesOther(SphereKoords * s1, SphereKoords * s2) {
	float s = sqrt((s1->x - s2->x) * (s1->x - s2->x) + (s1->y - s2->y) * (s1->y - s2->y) + (s1->z - s2->z) * (s1->z - s2->z));
	if (s <= abs(s1->r - s2->r) + RadiusEpselone)
		return s1->r > s2->r ? 1 : 2;
	return 0;
}

bool SphereWorld::CompareSphereInCell(int x, int y, int z, SphereKoords * sphere) {
	std::vector<SphereKoords *> & cell = cells[x * 100 + y * 10 + z];
	for (auto i = cell.begin(); i != cell.end();) {
		int intersectingResult = SphereIncludesOther(sphere, *i);
		if (intersectingResult == 2)
			return false;
		if (intersectingResult == 1) {
			i = cell.erase(i);
			continue;
		}
		++i;
	}
	return true;
}

void SphereWorld::AddCell(std::vector<SphereKoords *> & out, int sphereCellX, int sphereCellY, int sphereCellZ, std::vector<SphereKoords *> const & in) {
	std::vector<SphereKoords *> const & cell = in;
	for (std::vector<SphereKoords *>::const_iterator sphereI = cell.begin(); sphereI != cell.end(); ++sphereI) {
		bool needsAdding = true;
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				for (int k = -1; k <= 1; k++) {
					int cellX = sphereCellX + i;
					int cellY = sphereCellY + j;
					int cellZ = sphereCellZ + k;

					if (cellX < 0 || cellX >= maxCellNumber || cellY < 0 || cellY >= maxCellNumber || cellZ < 0 || cellZ >= maxCellNumber)
						break;

					if (!CompareSphereInCell(cellX, cellY, cellZ, *sphereI)) {
						needsAdding = false;
						break;
					}
				}
				if (!needsAdding)
					break;
			}
			if (!needsAdding)
				break;
		}
		if (needsAdding) {
			out.push_back(*sphereI);
		}
	}
}

void SphereWorld::AddSphereKoord(SphereKoords * kandidate) {
	int sphereCellX = min(9, static_cast<int>(kandidate->x / maxSphereRadius));
	int sphereCellY = min(9, static_cast<int>(kandidate->y / maxSphereRadius));
	int sphereCellZ = min(9, static_cast<int>(kandidate->z / maxSphereRadius));

	bool needsAdding = true;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			for (int k = -1; k <= 1; k++) {
				int cellX = sphereCellX + i;
				int cellY = sphereCellY + j;
				int cellZ = sphereCellZ + k;

				if (cellX < 0 || cellX >= maxCellNumber || cellY < 0 || cellY >= maxCellNumber || cellZ < 0 || cellZ >= maxCellNumber)
					break;

				if (!CompareSphereInCell(cellX, cellY, cellZ, kandidate)) {
					needsAdding = false;
					break;
				}
			}
			if (!needsAdding)
				break;
		}
		if (!needsAdding)
			break;
	}
	if (needsAdding) {
		cells[sphereCellX * 100 + sphereCellY * 10 + sphereCellZ].push_back(kandidate);
	}
}

void SphereWorld::AddSphereWorld(SphereWorld & right) {
	std::vector<SphereKoords *> temporaryCells[1000];

	for (int sphereCellX = 0; sphereCellX < 10; sphereCellX++) {
		for (int sphereCellY = 0; sphereCellY < 10; sphereCellY++) {
			for (int sphereCellZ = 0; sphereCellZ < 10; sphereCellZ++) {
				std::vector<SphereKoords *> & cell = right.cells[sphereCellX * 100 + sphereCellY * 10 + sphereCellZ];
				AddCell(temporaryCells[sphereCellX * 100 + sphereCellY * 10 + sphereCellZ], sphereCellX, sphereCellY, sphereCellZ, cell);
			}
		}
	}

	for (int i = 0; i < 1000; i++) {
		cells[i].insert(cells[i].end(), temporaryCells[i].begin(), temporaryCells[i].end());
	}
}

void SphereWorld::SimpleAddSphereWorld(SphereWorld & right) {
	for (int i = 0; i < 1000; i++) {
		cells[i].insert(cells[i].end(), right.cells[i].begin(), right.cells[i].end());
	}
}

void SphereWorld::Clear() {
	for (int i = 0; i < 1000; i++) {
		cells[i].clear();
	}
}

enum CellManagerResult {STOP, WAIT, DO};

struct CriticalSectionLocker {
	CriticalSectionLocker(CRITICAL_SECTION * lock)
		:  lock(lock)
	{
		EnterCriticalSection(lock);
	}
	~CriticalSectionLocker()
	{
		LeaveCriticalSection(lock);
	}
	CRITICAL_SECTION * lock;
};

class CellController {
public:
	CellController () {
		InitializeCriticalSection(&lock);
		for (int i = 0; i < 100; i++) {
			freeColumns.insert(i);
		}
	}
	
	CellManagerResult GetFreeColumn(int & x, int & y) {
		CriticalSectionLocker locker(&lock);
		if (freeColumns.empty())
			return STOP;

		for (auto freeColumn = freeColumns.begin(); freeColumn != freeColumns.end(); ++freeColumn) {
			bool goodColumn = true;
			int kandidateColumnX = *freeColumn / 10;
			int kandidateColumnY = *freeColumn % 10;
			for (auto columnInWork = columnsInWork.begin(); columnInWork != columnsInWork.end(); ++columnInWork) {
				int columnInWorkX = *columnInWork / 10;
				int columnInWorkY = *columnInWork % 10;

				if (abs(kandidateColumnX - columnInWorkX) <= 2 && abs(kandidateColumnY - columnInWorkY) <= 2)  {
					goodColumn = false;
					break;
				}
			}
			if (goodColumn) {
				columnsInWork.insert(*freeColumn);
				freeColumns.erase(*freeColumn);
				x = kandidateColumnX;
				y = kandidateColumnY;
				return DO;
			}
		}
		return WAIT;
	}
	
	void FinishColumn(int x, int y) {
		CriticalSectionLocker locker(&lock);
		columnsInWork.erase(x*10 + y);
	}
	
	CRITICAL_SECTION lock;
	std::set<int> freeColumns;
	std::set<int> columnsInWork;
};


struct CellAdderData {
	std::vector<SphereKoords *> * outCells;
	SphereWorld * left;
	SphereWorld * right;
	CellController * controller;
};

DWORD WINAPI CellAdder(LPVOID lpArg) {
	CellAdderData * data = (CellAdderData * ) lpArg;
	std::vector<SphereKoords *> * outCells = data->outCells;
	int x;
	int y;
	CellManagerResult result = data->controller->GetFreeColumn(x, y);
	while (result != STOP) {
		if (result == WAIT) {
			Sleep(100);
			result = data->controller->GetFreeColumn(x, y);
			continue;
		}
		for (int z = 0; z < 10; ++z) {
			data->left->AddCell(*(outCells + x * 100 + y * 10 + z), x, y, z, data->right->cells[x * 100 + y * 10 + z]);
		}
		data->controller->FinishColumn(x, y);
		result = data->controller->GetFreeColumn(x, y);
	}
	return NULL;
}

void SphereWorld::AddSphereWorldInSeveralThreads(SphereWorld & kandidate, int threadAmount) {
	std::vector<SphereKoords *> temporaryCells[1000];
	CellController controller;
	std::vector<HANDLE> hHandles(threadAmount);
	std::vector<CellAdderData > datas(threadAmount);
	for (int threadNumber = 0; threadNumber < threadAmount; ++threadNumber) {
		datas[threadNumber].controller = &controller;
		datas[threadNumber].left = this;
		datas[threadNumber].right = &kandidate;
		datas[threadNumber].outCells = temporaryCells;
		DWORD ThreadId;
		hHandles[threadNumber] = CreateThread(NULL, 0, CellAdder, &datas[threadNumber], 0, &ThreadId);
	}
	for (int i = 0; i < threadAmount; i++) {
		WaitForSingleObject(hHandles[i],INFINITE);
	}
	for (int i = 0; i < 1000; i++) {
		cells[i].insert(cells[i].end(), temporaryCells[i].begin(), temporaryCells[i].end());
	}
}

void SphereWorld::GenerateMatrixInCells(long matrixInCells[1000][2], mat4 * matrixStore) {
	int matrixUniqueNumber = 0;
	for (int i = 0; i < 1000; ++i) {
		std::vector<SphereKoords *> & cell = cells[i];
		matrixInCells[i][0] = matrixUniqueNumber;
		for (int sphereI = 0; sphereI < cell.size(); sphereI++) {
			matrixStore[matrixUniqueNumber] = GLTranslation(vec3(cell[sphereI]->x, cell[sphereI]->y, cell[sphereI]->z)) * GLScale(vec3(cell[sphereI]->r, cell[sphereI]->r, cell[sphereI]->r));
			++matrixUniqueNumber;
		}
		matrixInCells[i][1] = matrixUniqueNumber;
	}
}

int  SphereWorld::GetCurrentSize() {
	int result = 0;
	for (int x = 0; x < maxCellNumber; x++) {
		for (int y = 0; y < maxCellNumber; y++) {
			for (int z = 0; z < maxCellNumber; z++) {
				result += cells[x * 100 + y * 10 + z].size();
			}	
		}	
	}	
	return result;

}

void clearWorlds(SphereWorld * sphereWorlds, int sphereWorldNumber) {
	for (int i = 0; i < sphereWorldNumber; i++)
		sphereWorlds[i].Clear();
}

DWORD WINAPI SphereWorldWorker(LPVOID lpArg) {
	/// adding spheres into worlds
	//std::cout << "=====================================" << std::endl;	
	SphereWorldWorkerData * workerData = (SphereWorldWorkerData *)lpArg;
	int spheresInOneWorld = workerData->sphereNumber/workerData->sphereWorldNumber + 1;
	for (int worldNumber = 0; worldNumber < workerData->sphereWorldNumber; ++worldNumber) {
		for(int i = worldNumber * spheresInOneWorld; i < (worldNumber + 1) * spheresInOneWorld && i < workerData->sphereNumber; i++) {
			workerData->worlds[worldNumber].AddSphereKoord(&workerData->spheres[i]);
			/*if (i % 10000 == 0) {
				std::cout << i << " "<< workerData->worlds[worldNumber].GetCurrentSize() << " " << worldNumber  << " " << std::endl;	
			}*/
		}
	}

	for (int degree = 0; degree < log((double)workerData->sphereWorldNumber) / log(2.0); ++degree) {
		int step = (int) pow(2.0f, (int)degree);
		for (int worldNumber = 0; worldNumber < workerData->sphereWorldNumber; worldNumber += step * 2) {
			workerData->worlds[worldNumber].AddSphereWorld(workerData->worlds[worldNumber + step]);
			//std::cout << "combined " << worldNumber << " " << worldNumber + step << " " << workerData->worlds[worldNumber].GetCurrentSize() << " " << std::endl;	
		}
	}

	return NULL;
}
