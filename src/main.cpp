#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"
#include "SphereWorld.h"

// индекс шейдерной программы
static GLuint shaderProgram = 0;
// сфера
static Mesh     mesh;
// главная камера
static Camera     mainCamera;

// количество сфер после урезаний в SphereWorlds
long REDUCED_NUM_SPHERES = 0;
// матрицы для всех реальных (оставшихся после урезки) сфер для отрисовки 
mat4 * spheres;

// глобальный класс, рассчитывающий покрытие ячеек куба (1x1x1) сферами, чтобы не отрисовывать те сферы которые мы не видим
CoverBuilder coverBuilder;
// первая и последняя матрица сферы в каждой из 1000 ячеек. Номера соттветствуют массиву spheres.
long matrixInCells[1000][2];

std::string vertexShader = 
"#version 330 core\n"
"#define VERT_POSITION 0\n"
"#define VERT_NORMAL   1\n"

"layout(location = VERT_POSITION) in vec3 position;\n"
"layout(location = VERT_NORMAL)   in vec3 normal;\n"

"uniform struct Transform\n"
"{\n"
"	mat4 model;\n"
"	mat4 viewProjection;\n"
"} transform;\n"

"out vec3 fragmentColor;\n"

"void main(void)\n"
"{\n"

"	vec4 vertex   = transform.model * vec4(position, 1.0);\n"

"	float NdotL = max(dot(normalize(normal), vec3(-0.4082, -0.4082, -0.8165)), 0.0);\n"
"	fragmentColor = vec3(0.0, 0.0, 1.0) * NdotL;\n"

"	gl_Position = transform.viewProjection * vertex;\n"
"}\n";

std::string fragmentShader = 
"#version 330 core \n"

"#define FRAG_OUTPUT0 0 \n"

"in vec3 fragmentColor; \n"

"out vec4 color; \n"

"void main(void)\n"
"{\n"
"	color = vec4(fragmentColor, 1.0);\n"
"}\n";


// инициализаця OpenGL
bool GLWindowInit(const GLWindow *window)
{
	ASSERT(window);

	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, window->width, window->height);

	// параметры OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	// создадим и загрузим шейдерную программу
	if ((shaderProgram = ShaderProgramCreateFromMemory(vertexShader, fragmentShader)) == 0)
		return false;

	// собираем созданную и загруженную шейдерную программу
	if (!ShaderProgramLink(shaderProgram))
		return false;

	// сделаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	// проверка шейдерной программы на корректность
	if (!ShaderProgramValidate(shaderProgram))
		return false;

	// создадим примитив для сферы
	MeshCreateSphere(mesh, vec3(0.0f, 0.0f, 0.0f), 1.0f);
	
	// создадим и настроим камеру
	CameraCreate(mainCamera, -1.0f, -1.0f, -1.0f);
	CameraOrthographic(mainCamera, -0.5f, 0.5f, -0.5f, 0.5f, -10.0f, 10.0f);
	CameraRotate(mainCamera, (float)(-math_degrees * asin((double)1/sqrt(3.0))), 135.0f, 0.0f);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	MeshDestroy(mesh);

	ShaderProgramDestroy(shaderProgram);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/// вычисление произведения матрицы камеры и матрицы проекции
	CameraSetup(shaderProgram, mainCamera);
	/// ячейки, сферы из которых мы будем рендерить.
	std::set<int> cellsToRender;
	if (!coverBuilder.GetApropriateCellsNumbers(cellsToRender)) {
		/// рендер всех ячеек
		for(int i = 0; i < REDUCED_NUM_SPHERES; ++i) {
			MeshSetup(shaderProgram, spheres[i]);
			MeshRender(mesh);
		}
	} else {
		/// рендер только выбранных ячеек
		for (auto i = cellsToRender.begin(); i != cellsToRender.end(); ++i) {
			long * cell = matrixInCells[*i];
			for (int matrixNumber = cell[0]; matrixNumber < cell[1]; ++matrixNumber) {
				MeshSetup(shaderProgram, spheres[matrixNumber]);
				MeshRender(mesh);
			}
		}
	}

	// проверка на ошибки
	//OPENGL_CHECK_FOR_ERRORS();
}

// количество боевых потоков для работы со SphereWorlds
#define THREADS_COUNT 4

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
	// константы

	// максимальное число сфер - для выделения буффера памяти
	long MAX_NUM_SPHERES = 100000000;
	// реальное число - определяется при считывании из файла или используется в рандомной генерации.
	long REAL_NUM_SPHERES = 10000000;
	// количество миров сфер, когда мы убираем сферы лежащие в других сферах - нужно разделять все сферы по разным мирам - там добавления новых сфер происходит быстрее.
	int SPHERE_WORLD_NUMBER = 128;
	// количество сфер которое мы обрабатываем полностью, то есть убираем все сферы содержащиеся в других сферах. Так мы бъем все сферы на такие группы и обрабатываем полностью.
	// после этого мы просто склеиваем все такие группы. Это сделано для того что, обрабатывать все 100M долго по времени (>1h), из за этого нам придется отрисовывать на 30% 
	// cфер больше.
	long SPHERES_PROCESSING_STEP = 10000000;

	// имена файлов
	std::string inputFileName = "input2.txt";
    std::string outputTxtFileName = "balabok2.txt";
    std::string outputBmpFileName = "balabok2.bmp";
    std::string fullOutputTxtFileName = "tmp_balabok/spheres_fullOutput.txt";

	CreateDirectory (L"tmp_balabok", NULL);
	LoggerCreate("tmp_balabok/spheres.log");
	// установим зерно генератора случайных чисел
	srand((unsigned int)time(NULL));

	/// create window, start main loop, etc.
	int windowWidht = 1024;
	int windowHeight = 1024;

	if (!GLWindowCreate(L"Spheres", windowWidht, windowHeight))
		return 1;
	/// 
	SphereKoords *allSpheres = new SphereKoords[MAX_NUM_SPHERES];
	/// считывание сфер из файла
	REAL_NUM_SPHERES = readSpheresDataFromFile(inputFileName, allSpheres, MAX_NUM_SPHERES);
	//generateRandomSpheres(allSpheres, REAL_NUM_SPHERES);
	
	SphereWorld * sphereWorlds = new SphereWorld[SPHERE_WORLD_NUMBER];
	SphereWorld finalWorld;
	
	LOG_DEBUG ("Please, be patient. There is a quite long data preprocessing before rendering.\n");
	LOG_DEBUG ("It can take up to 20 minutes for 100M spheres.\n");
	LOG_DEBUG ("Take into account, that time for preprocessing wasn't limited.\n");
	LOG_DEBUG ("By the way, you can see the progress in this file\n");
	auto beginTime = GetTickCount();
	/// вычисление сфер внутри других сфер в несколько потоков по блокам (мирам)
	for (int spheresInitialNumber = 0; spheresInitialNumber < REAL_NUM_SPHERES; spheresInitialNumber += SPHERES_PROCESSING_STEP) {
		long spheresToProcess = spheresInitialNumber + SPHERES_PROCESSING_STEP >= REAL_NUM_SPHERES ? REAL_NUM_SPHERES - spheresInitialNumber : SPHERES_PROCESSING_STEP;
		LOG_DEBUG ("processing spheres from %i to %i, time: %i\n", spheresInitialNumber, spheresInitialNumber + spheresToProcess , GetTickCount() - beginTime);	
		int worldsPerThread = SPHERE_WORLD_NUMBER/THREADS_COUNT;
		SphereWorldWorkerData workersData[THREADS_COUNT];
		HANDLE hHandles[THREADS_COUNT];
		for (int threadNumber = 0; threadNumber < THREADS_COUNT; ++threadNumber) {
			long spheresPerThread = spheresToProcess/THREADS_COUNT;
			workersData[threadNumber] = SphereWorldWorkerData(worldsPerThread, 
				sphereWorlds + threadNumber * worldsPerThread, 
				threadNumber == THREADS_COUNT - 1 ? spheresToProcess - spheresPerThread * (THREADS_COUNT - 1) : spheresPerThread,
				allSpheres + spheresInitialNumber + spheresPerThread * threadNumber);
			DWORD ThreadId;
			hHandles[threadNumber] = CreateThread(NULL, 0, SphereWorldWorker, workersData + threadNumber, 0, &ThreadId);
		}

		for (int i = 0; i < THREADS_COUNT; i++) {
			WaitForSingleObject(hHandles[i],INFINITE);
		}
		/// склейка миров из разных потоков
		for (int degree = (int)(log((double)worldsPerThread) / log(2.0)); degree < log((double)SPHERE_WORLD_NUMBER) / log(2.0); ++degree) {
			int step = (int) pow(2.0f, (int)degree);
			for (int worldNumber = 0; worldNumber < SPHERE_WORLD_NUMBER; worldNumber += step * 2) {
				sphereWorlds[worldNumber].AddSphereWorldInSeveralThreads(sphereWorlds[worldNumber + step], THREADS_COUNT);
			}
		}
		/// простое (без вычислений) добавление сфер из разных миров к финальному миру - сделано для увеличение скорости предобработки.
		finalWorld.SimpleAddSphereWorld(sphereWorlds[0]);
		clearWorlds(sphereWorlds, SPHERE_WORLD_NUMBER);
	}

	LOG_DEBUG("sphere amount after removing the spheres located insides others: %i, time %i\n", finalWorld.GetCurrentSize(), GetTickCount() - beginTime);
	LOG_DEBUG("started checking how spheres cover cube (0,0,0 - 1,1,1)\n");
	/// проверка насколько сферы покрывают все стороны каждой ячейки
	finalWorld.GenerateCover(coverBuilder);
	LOG_DEBUG("covering finished, time %i\n", GetTickCount() - beginTime);
	LOG_DEBUG("creating the main window, etc, and started rendering\n");

	/// generate matrix in cells;
	REDUCED_NUM_SPHERES = finalWorld.GetCurrentSize();
	spheres = new mat4[REDUCED_NUM_SPHERES];
	finalWorld.GenerateMatrixInCells(matrixInCells, spheres);

	/// вектор со временами отрисовки фремов
	std::vector<double> msecsPerFrameList;

	int nSize = windowWidht * windowHeight * 3;
	GLubyte *pixels = new GLubyte [nSize];
	/// старт петли сообщений
	int result = GLWindowMainLoop(msecsPerFrameList, pixels);

	/// сохранение всех данных в файлы
	if (pixels) {
		SaveBMP(pixels, windowWidht, windowHeight, nSize, outputBmpFileName.c_str());	
	}

	double performance = 0;
	double medianeMsecsPerFrame = 0;

	std::ofstream fullResultStream(fullOutputTxtFileName);
	std::ofstream resultStream(outputTxtFileName);

	fullResultStream << "msecs per frame: ";
	for (auto msecsValue = msecsPerFrameList.begin(); msecsValue != msecsPerFrameList.end(); ++msecsValue) {
		medianeMsecsPerFrame += *msecsValue;
		fullResultStream << *msecsValue << " ";
	}
	medianeMsecsPerFrame /= 10.0f;
	fullResultStream << std::endl << "average msecs per frame: " << medianeMsecsPerFrame ;
	performance = REAL_NUM_SPHERES / (1000 * medianeMsecsPerFrame);
	fullResultStream << std::endl << "Mspheres per second: " << performance;
	resultStream << performance << std::endl;

	GLWindowDestroy();
	LoggerDestroy();
	/// TODO: kill all windows handlers and free all the memory.
	return result;
}
