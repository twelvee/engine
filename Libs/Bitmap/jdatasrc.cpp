// stb_datasrc.cpp
/*
    Реализация загрузки изображений через stb_image с использованием пользовательских колбэков для
    cf::FileSys::InFileI. Эта реализация заменяет jdatasrc.c из jpeglib для работы с вашим файловым
    интерфейсом.

    Cafu Engine, http://www.cafu.de/
    Copyright (c) Carsten Fuchs and other contributors.
    Licensed under the MIT license.
*/

#include "stb_image.h"  // Реализация должна быть определена в отдельном файле: см. ниже.
#include "FileSys/File.hpp"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//--------------------------------------------------------------------
// Реализация колбэков для stb_image с использованием cf::FileSys::InFileI.
//--------------------------------------------------------------------

// Функция для чтения данных. user — указатель на объект InFileI.
static int stb_fs_read(void *user, char *data, int size)
{
    cf::FileSys::InFileI* file = reinterpret_cast<cf::FileSys::InFileI*>(user);
    // Read() возвращает число прочитанных байт.
    return static_cast<int>(file->Read(data, static_cast<uint32_t>(size)));
}

// Функция для пропуска данных. n — число байт для пропуска.
static void stb_fs_skip(void *user, int n)
{
    cf::FileSys::InFileI* file = reinterpret_cast<cf::FileSys::InFileI*>(user);
    // Используем метод Seek, передавая смещение относительно текущей позиции.
    file->Seek(n, cf::FileSys::FileI::FROM_CURRENT_POS);
}

// Функция для проверки конца файла.
static int stb_fs_eof(void *user)
{
    cf::FileSys::InFileI* file = reinterpret_cast<cf::FileSys::InFileI*>(user);
    // Предполагаем, что метод GetPos() возвращает текущую позицию, а GetSize() — общий размер файла.
    return (file->GetPos() >= file->GetSize()) ? 1 : 0;
}

// Заполняем структуру колбэков для stb_image.
static stbi_io_callbacks stb_callbacks = {
    stb_fs_read,
    stb_fs_skip,
    stb_fs_eof
};

//
// Функция-загрузчик, использующая stbi_load_from_callbacks.
// Если req_comp задано (например, 4 для RGBA), то изображение будет преобразовано в этот формат.
// Возвращает указатель на буфер с пикселями (формат по умолчанию — 8 бит на канал),
// который нужно потом освободить вызовом stbi_image_free().
//
unsigned char* stbi_FileSys_load(cf::FileSys::InFileI* file, int *x, int *y, int *comp, int req_comp)
{
    if (!file)
        return nullptr;
    // При необходимости можно сделать "rewind" файла, если это требуется логикой.
    // Например: file->Seek(0, cf::FileSys::FileI::FROM_BEGINNING);
    return stbi_load_from_callbacks(&stb_callbacks, file, x, y, comp, req_comp);
}