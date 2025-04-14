// В одном из CPP-файлов (например, Bitmap_stb.cpp) определите реализации stb:
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Bitmap.hpp"
#include "FileSys/FileMan.hpp"
#include "FileSys/File.hpp"
#include <string.h>
#include <stdio.h>
#include <math.h>

// Если нужны функции сравнения строк без учета регистра в Windows:
#ifndef _WIN32
#include <strings.h>
#define _stricmp strcasecmp
#endif

// Конструктор по умолчанию
BitmapT::BitmapT() : SizeX(0), SizeY(0)
{
}

// Конструктор, загружающий изображение из файла с использованием stb_image.
// Он загружает изображение в RGBA формате (4 канала) независимо от исходного.
BitmapT::BitmapT(const char* FileName) : SizeX(0), SizeY(0)
{
    if (FileName == NULL)
        throw LoadErrorT();

    int width, height, channels;
    // Загрузка изображения с принудительным числом каналов = 4 (RGBA)
    unsigned char* imgData = stbi_load(FileName, &width, &height, &channels, 4);
    if (!imgData)
        throw LoadErrorT();

    SizeX = static_cast<unsigned int>(width);
    SizeY = static_cast<unsigned int>(height);
    // Предполагается, что Data – некий контейнер с методом PushBackEmpty(size_t)
    Data.PushBackEmpty(SizeX * SizeY);

    // stbi_load возвращает пиксели в формате: R, G, B, A (по порядку)
    // В Cafu используется формат: (A << 24) | (B << 16) | (G << 8) | R.
    for (unsigned int i = 0; i < SizeX * SizeY; i++)
    {
        uint32_t r = imgData[i * 4 + 0];
        uint32_t g = imgData[i * 4 + 1];
        uint32_t b = imgData[i * 4 + 2];
        uint32_t a = imgData[i * 4 + 3];

        Data[i] = (a << 24) | (b << 16) | (g << 8) | r;
    }

    stbi_image_free(imgData);
}

// Сохранение изображения на диск с использованием stb_image_write.
// Поддерживаются форматы .png, .bmp, .tga, .jpg/.jpeg.
// При сохранении преобразуем данные из внутреннего формата (uint32_t в формате A|B|G|R)
// в массив байтов в формате RGBA, который ожидают stb_image_write.
bool BitmapT::SaveToDisk(const char* FileName) const
{
    if (FileName == NULL)
        return false;

    size_t FileNameLength = strlen(FileName);
    if (FileNameLength < 5)
        return false;

    // Создаем временный буфер для хранения пикселей в формате RGBA.
    unsigned char* imgData = new unsigned char[SizeX * SizeY * 4];
    for (unsigned int i = 0; i < SizeX * SizeY; i++)
    {
        uint32_t pix = Data[i];
        // Извлекаем компоненты согласно тому, как они упакованы: (A << 24) | (B << 16) | (G << 8) | R.
        imgData[i * 4 + 0] = pix & 0xFF;             // R
        imgData[i * 4 + 1] = (pix >> 8) & 0xFF;        // G
        imgData[i * 4 + 2] = (pix >> 16) & 0xFF;       // B
        imgData[i * 4 + 3] = (pix >> 24) & 0xFF;       // A
    }

    bool result = false;
    // Определяем формат по расширению файла (без учета регистра).
    if (_stricmp(&FileName[FileNameLength - 4], ".png") == 0)
    {
        // stbi_write_png: последний параметр — число байтов в строке.
        result = stbi_write_png(FileName, SizeX, SizeY, 4, imgData, SizeX * 4) != 0;
    }
    else if (_stricmp(&FileName[FileNameLength - 4], ".bmp") == 0)
    {
        result = stbi_write_bmp(FileName, SizeX, SizeY, 4, imgData) != 0;
    }
    else if (_stricmp(&FileName[FileNameLength - 4], ".tga") == 0)
    {
        result = stbi_write_tga(FileName, SizeX, SizeY, 4, imgData) != 0;
    }
    else if ((_stricmp(&FileName[FileNameLength - 4], ".jpg") == 0) ||
             (_stricmp(&FileName[FileNameLength - 5], ".jpeg") == 0))
    {
        // Для JPEG можно задать качество (например, 95).
        result = stbi_write_jpg(FileName, SizeX, SizeY, 4, imgData, 95) != 0;
    }
    else
    {
        result = false;
    }

    delete[] imgData;
    return result;
}
