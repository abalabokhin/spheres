#include <fstream>
#include "common.h"
#include "Logger.h"

bool LoadFile(const char *fileName, bool binary, uint8_t **buffer, uint32_t *size)
{
	ASSERT(fileName);
	ASSERT(buffer);
	ASSERT(size);

	FILE     *input;
	uint32_t fileSize, readed;

	const char mode[] = {'r', binary ? 'b' : 't', '\0'};

	if ((input = fopen(fileName, mode)) == NULL)
	{
		LOG_ERROR("Opening file '%s'\n", fileName);
		return false;
	}

	fseek(input, 0, SEEK_END);
	fileSize = (uint32_t)ftell(input);
	rewind(input);

	if (fileSize == 0)
	{
		LOG_ERROR("Empty file '%s'\n", fileName);
		fclose(input);
		return false;
	}

	*buffer = new uint8_t[fileSize];
	ASSERT(*buffer);

	readed = fread(*buffer, 1, fileSize, input);

	fclose(input);

	if (readed != fileSize)
	{
		LOG_ERROR("Reading file '%s'\n", fileName);
		delete[] *buffer;
		return false;
	}

	*size = fileSize;

	return true;
}

int readSpheresDataFromFile(const std::string & inputFileName, SphereKoords * allSpheres, long maxSpheresNumber) 
{
	std::ifstream inputStream(inputFileName);
	LOG_DEBUG ("start reading data from file %s\n", inputFileName.c_str());
	int i = 0;
	char ch;
    while (inputStream && i < maxSpheresNumber) {
		inputStream >> allSpheres[i].x;
		inputStream >> ch;
		inputStream >> allSpheres[i].y;
		inputStream >> ch;
		inputStream >> allSpheres[i].z;
		inputStream >> ch;
		inputStream >> allSpheres[i].r;
		if (!inputStream)
			continue;
		i++;
		if (i % 1000000 == 0) {
			LOG_DEBUG ("%i sphere koords were read\n", i);		
		}
    }
	return i;
}

void generateRandomSpheres(SphereKoords * allSpheres, long maxSpheresNumber) 
{
	std::ofstream out("input2.txt");
	for(int i = 0; i < maxSpheresNumber; i++) {
		allSpheres[i].x = ((rand() % 1000)) * 0.001f;
		allSpheres[i].z = ((rand() % 1000)) * 0.001f;
		allSpheres[i].y = ((rand() % 1000)) * 0.001f;
		allSpheres[i].r = ((rand() % 900) + 100) / 10000.0f;
		out << allSpheres[i].x << ", " << allSpheres[i].y << ", " << allSpheres[i].z << ", " << allSpheres[i].r << std::endl;
	}
}

bool SaveBMP ( BYTE* Buffer, int width, int height, long paddedsize, char const * bmpfile )
{
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;
	
	// andinitialize them to zero
	memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
	memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );
	
	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits
	
	// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;	
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;     
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

	// now we open the file to write to
	HANDLE file = CreateFileA ( bmpfile , GENERIC_WRITE, FILE_SHARE_READ,
		 NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( file == NULL )
	{
		CloseHandle ( file );
		return false;
	}
	
	// write file header
	unsigned long bwritten;
	if ( WriteFile ( file, &bmfh, sizeof ( BITMAPFILEHEADER ), &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}
	// write infoheader
	if ( WriteFile ( file, &info, sizeof ( BITMAPINFOHEADER ), &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}
	// write image data
	if ( WriteFile ( file, Buffer, paddedsize, &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}
	
	// and close file
	CloseHandle ( file );

	return true;
}
