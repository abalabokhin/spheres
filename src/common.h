#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <string>
#include <cassert>
#include <cmath>
#include <ctime>

#include "windows.h"

#define VERT_POSITION  0
#define VERT_NORMAL    1

#define FRAG_OUTPUT0   0

typedef float float2[2];
typedef float float3[3];
typedef float float4[4];

#if defined(_MSC_VER)
typedef __int8           int8_t;
typedef unsigned __int8  uint8_t;
typedef __int16          int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32          int32_t;
typedef unsigned __int32 uint32_t;

// кажется MSVC не знает про snprintf, но там есть _snprintf
#define snprintf _snprintf

#elif defined(__MINGW32__)
#include <stdint.h>

#else
#error You must compile this code with MSVC or MinGW!
#endif

#include "Logger.h"

struct SphereKoords {
	float x;
	float y;
	float z;
	float r;
};

// загрузка файла целиком в память
bool LoadFile(const char *fileName, bool binary, uint8_t **buffer, uint32_t *size);
// считывание данных о всех сферах из файла
int readSpheresDataFromFile(const std::string & inputFileName, SphereKoords * allSpheres, long maxSpheresNumber);
// рандомная генерация сфер - используется при тестах.
void generateRandomSpheres(SphereKoords * allSpheres, long maxSpheresNumber);
// сохранение массива байтов в картинку в качестве bmp файла.
bool SaveBMP ( BYTE* Buffer, int width, int height, long paddedsize, char const * bmpfile );

#endif /* COMMON_H */
