/*
Cafu Engine, http://www.cafu.de/
Copyright (c) Carsten Fuchs and other contributors.
This project is licensed under the terms of the MIT license.
*/

#include <math.h>
#include <stdio.h>

#include <cmath>

#include "Bitmap.hpp"
#include "FileSys/FileMan.hpp"
#include "FileSys/File.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifndef _WIN32
#include <string.h>
#define _stricmp strcasecmp
#endif

const char FileNotFoundBitmapData[128 * 16] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

BitmapT::BitmapT() : SizeX(0), SizeY(0) {
}

static uint32_t ReadTgaPixel(cf::FileSys::InFileI *TgaFile, char ImagePixelSize) {
    char Blue;
    TgaFile->Read(&Blue, sizeof(Blue));
    char Green;
    TgaFile->Read(&Green, sizeof(Green));
    char Red;
    TgaFile->Read(&Red, sizeof(Red));
    char Alpha;
    if (ImagePixelSize == 32) TgaFile->Read(&Alpha, sizeof(Alpha));
    else Alpha = char(0xFF);

    const uint32_t b = Blue;
    const uint32_t g = Green;
    const uint32_t r = Red;
    const uint32_t a = Alpha;

    return (a << 24) + (b << 16) + (g << 8) + r;
}

BitmapT::BitmapT(const char *FileName) /*throw (LoadErrorT)*/ : SizeX(0), SizeY(0) {
    if (FileName == NULL) throw LoadErrorT();

    const size_t FileNameLength = strlen(FileName);

    if (FileNameLength < 5) throw LoadErrorT();

    if (_stricmp(&FileName[FileNameLength - 4], ".bmp") == 0 ||
        _stricmp(&FileName[FileNameLength - 4], ".png") == 0 ||
        _stricmp(&FileName[FileNameLength - 4], ".jpg") == 0 ||
        _stricmp(&FileName[FileNameLength - 5], ".jpeg") == 0) {
        cf::FileSys::InFileI *File = cf::FileSys::FileMan->OpenRead(FileName);
        if (File == NULL) throw LoadErrorT();

        uint64_t FileSize = File->GetSize();
        ArrayT<unsigned char> FileData;
        FileData.PushBackEmpty(static_cast<unsigned long>(FileSize));
        File->Read(reinterpret_cast<char *>(&FileData[0]), static_cast<unsigned long>(FileSize));
        cf::FileSys::FileMan->Close(File);

        int width, height, channels;
        unsigned char *img_data = stbi_load_from_memory(&FileData[0], static_cast<unsigned long>(FileSize), &width, &height, &channels, 4);

        if (!img_data) throw LoadErrorT();

        SizeX = width;
        SizeY = height;
        Data.PushBackEmpty(SizeX * SizeY);

        for (unsigned int i = 0; i < SizeX * SizeY; i++) {
            uint32_t r = img_data[i * 4 + 0];
            uint32_t g = img_data[i * 4 + 1];
            uint32_t b = img_data[i * 4 + 2];
            uint32_t a = img_data[i * 4 + 3];
            Data[i] = (a << 24) | (b << 16) | (g << 8) | r;
        }

        stbi_image_free(img_data);
    } else if (_stricmp(&FileName[FileNameLength - 4], ".tga") == 0) {
        cf::FileSys::InFileI *TgaFile = cf::FileSys::FileMan->OpenRead(FileName);
        if (TgaFile == NULL) throw LoadErrorT();

        char SizeOfIDField;
        TgaFile->Read(&SizeOfIDField, sizeof(SizeOfIDField));
        char ColorMapType;
        TgaFile->Read(&ColorMapType, sizeof(ColorMapType));
        char ImageTypeCode;
        TgaFile->Read(&ImageTypeCode, sizeof(ImageTypeCode));

        if (ColorMapType != 0) {
            cf::FileSys::FileMan->Close(TgaFile);
            throw LoadErrorT();
        }

        if (ImageTypeCode != 2 && ImageTypeCode != 10) {
            cf::FileSys::FileMan->Close(TgaFile);
            throw LoadErrorT();
        }

        for (unsigned int i = 0; i < 5; i++) {
            char Dummy;
            TgaFile->Read(&Dummy, sizeof(Dummy));
        }

        unsigned short OriginX;
        TgaFile->Read((char *) &OriginX, sizeof(OriginX));
        unsigned short OriginY;
        TgaFile->Read((char *) &OriginY, sizeof(OriginY));
        unsigned short SizeX_;
        TgaFile->Read((char *) &SizeX_, sizeof(SizeX_));
        unsigned short SizeY_;
        TgaFile->Read((char *) &SizeY_, sizeof(SizeY_));
        char ImagePixelSize;
        TgaFile->Read(&ImagePixelSize, sizeof(ImagePixelSize));
        char ImageDescriptionByte;
        TgaFile->Read(&ImageDescriptionByte, sizeof(ImageDescriptionByte));

        SizeX = SizeX_;
        SizeY = SizeY_;

        if (ImagePixelSize != 32 && ImagePixelSize != 24) {
            cf::FileSys::FileMan->Close(TgaFile);
            throw LoadErrorT();
        }

        for (int i = 0; i < SizeOfIDField; i++) {
            char Dummy;
            TgaFile->Read(&Dummy, sizeof(Dummy));
        }

        Data.PushBackEmpty(SizeX * SizeY);

        if (ImageTypeCode == 2) {
            for (unsigned int PosY = 0; PosY < SizeY; PosY++)
                for (unsigned int PosX = 0; PosX < SizeX; PosX++) {
                    const unsigned int DestPosX = (ImageDescriptionByte & 0x10) == 0 ? PosX : SizeX - PosX - 1;
                    const unsigned int DestPosY = (ImageDescriptionByte & 0x20) == 0 ? SizeY - PosY - 1 : PosY;

                    Data[DestPosX + DestPosY * SizeX] = ReadTgaPixel(TgaFile, ImagePixelSize);
                }
        } else // ImageTypeCode==10
        {
            for (unsigned int PosY = 0; PosY < SizeY; PosY++)
                for (unsigned int PosX = 0; PosX < SizeX; PosX++) {
                    unsigned char RepetitionCount;
                    TgaFile->Read((char *) &RepetitionCount, sizeof(RepetitionCount));

                    const bool IsRunLengthPacket = (RepetitionCount & 0x80) != 0;
                    RepetitionCount = (RepetitionCount & 0x7F) + 1;

                    uint32_t Pixel = 0;
                    if (IsRunLengthPacket) Pixel = ReadTgaPixel(TgaFile, ImagePixelSize);

                    for (unsigned long RepNr = 0; RepNr < RepetitionCount; RepNr++) {
                        if (!IsRunLengthPacket) Pixel = ReadTgaPixel(TgaFile, ImagePixelSize);

                        unsigned int DestPosX = (ImageDescriptionByte & 0x10) == 0 ? PosX : SizeX - PosX - 1;
                        unsigned int DestPosY = (ImageDescriptionByte & 0x20) == 0 ? SizeY - PosY - 1 : PosY;

                        Data[DestPosX + DestPosY * SizeX] = Pixel;

                        PosX++;
                    }

                    PosX--;
                }
        }

        cf::FileSys::FileMan->Close(TgaFile);
    } else throw LoadErrorT();
}

BitmapT::BitmapT(unsigned int Width, unsigned int Height, const uint32_t *Buffer)
    : SizeX(Width),
      SizeY(Height) {
    const unsigned long int SizeXY = SizeX * SizeY;

    Data.PushBackEmpty(SizeXY);

    if (Buffer != NULL)
        for (unsigned long int PixelNr = 0; PixelNr < SizeXY; PixelNr++)
            Data[PixelNr] = Buffer[PixelNr];
}

BitmapT BitmapT::GetBuiltInFileNotFoundBitmap() {
    BitmapT FNF;

    FNF.SizeX = 128;
    FNF.SizeY = 16;

    for (unsigned int PixelNr = 0; PixelNr < 128 * 16; PixelNr++)
        FNF.Data.PushBack(FileNotFoundBitmapData[PixelNr] ? 0xFFFC5454 : 0xFFA80000);

    return FNF;
}

void BitmapT::GetPixel(unsigned int x, unsigned int y, int &r, int &g, int &b) const {
    const unsigned long RGBA = Data[x + y * SizeX];

    r = (RGBA >> 0) & 0xFF;
    g = (RGBA >> 8) & 0xFF;
    b = (RGBA >> 16) & 0xFF;
}

void BitmapT::SetPixel(unsigned int x, unsigned int y, int r, int g, int b) {
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    const unsigned long r_ = r;
    const unsigned long g_ = g;
    const unsigned long b_ = b;

    Data[x + y * SizeX] &= 0xFF000000;
    Data[x + y * SizeX] |= (b_ << 16) + (g_ << 8) + r_;
}

void BitmapT::GetPixel(unsigned int x, unsigned int y, int &r, int &g, int &b, int &a) const {
    const uint32_t RGBA = Data[x + y * SizeX];

    r = (RGBA >> 0) & 0xFF;
    g = (RGBA >> 8) & 0xFF;
    b = (RGBA >> 16) & 0xFF;
    a = (RGBA >> 24) & 0xFF;
}

void BitmapT::SetPixel(unsigned int x, unsigned int y, int r, int g, int b, int a) {
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (a < 0) a = 0;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    if (a > 255) a = 255;

    const uint32_t r_ = r;
    const uint32_t g_ = g;
    const uint32_t b_ = b;
    const uint32_t a_ = a;

    Data[x + y * SizeX] = (a_ << 24) + (b_ << 16) + (g_ << 8) + r_;
}

void BitmapT::GetPixel(unsigned int x, unsigned int y, float &r, float &g, float &b) const {
    const uint32_t RGBA = Data[x + y * SizeX];

    r = ((RGBA >> 0) & 0xFF) / 255.0f;
    g = ((RGBA >> 8) & 0xFF) / 255.0f;
    b = ((RGBA >> 16) & 0xFF) / 255.0f;
}

void BitmapT::SetPixel(unsigned int x, unsigned int y, float r, float g, float b) {
    if (r < 0.0) r = 0.0;
    if (g < 0.0) g = 0.0;
    if (b < 0.0) b = 0.0;

    if (r > 1.0) r = 1.0;
    if (g > 1.0) g = 1.0;
    if (b > 1.0) b = 1.0;

    const uint32_t r_ = (uint32_t) (r * 255.0);
    const uint32_t g_ = (uint32_t) (g * 255.0);
    const uint32_t b_ = (uint32_t) (b * 255.0);

    Data[x + y * SizeX] &= 0xFF000000;
    Data[x + y * SizeX] |= (b_ << 16) + (g_ << 8) + r_;
}

void BitmapT::GetPixel(unsigned int x, unsigned int y, float &r, float &g, float &b, float &a) const {
    const uint32_t RGBA = Data[x + y * SizeX];

    r = ((RGBA >> 0) & 0xFF) / 255.0f;
    g = ((RGBA >> 8) & 0xFF) / 255.0f;
    b = ((RGBA >> 16) & 0xFF) / 255.0f;
    a = ((RGBA >> 24) & 0xFF) / 255.0f;
}

void BitmapT::SetPixel(unsigned int x, unsigned int y, float r, float g, float b, float a) {
    if (r < 0.0) r = 0.0;
    if (g < 0.0) g = 0.0;
    if (b < 0.0) b = 0.0;
    if (a < 0.0) a = 0.0;

    if (r > 1.0) r = 1.0;
    if (g > 1.0) g = 1.0;
    if (b > 1.0) b = 1.0;
    if (a > 1.0) a = 1.0;

    const uint32_t r_ = (uint32_t) (r * 255.0);
    const uint32_t g_ = (uint32_t) (g * 255.0);
    const uint32_t b_ = (uint32_t) (b * 255.0);
    const uint32_t a_ = (uint32_t) (a * 255.0);

    Data[x + y * SizeX] = (a_ << 24) + (b_ << 16) + (g_ << 8) + r_;
}

void BitmapT::ApplyGamma(float Gamma) {
    char GammaLookup[256];

    for (unsigned int ValueNr = 0; ValueNr < 256; ValueNr++) {
        float Value = std::pow(static_cast<float>(ValueNr) / 255.0f, 1.0f / Gamma) * 255.0f;

        if (Value < 0.0f) Value = 0.0f;
        if (Value > 255.0f) Value = 255.0f;

        GammaLookup[ValueNr] = char(Value + 0.49f);
    }

    for (unsigned long i = 0; i < Data.Size(); i++) {
        char Alpha = char(Data[i] >> 24);
        char Blue = char(Data[i] >> 16);
        char Green = char(Data[i] >> 8);
        char Red = char(Data[i] >> 0);

        const uint32_t NewAlpha = Alpha;
        const uint32_t NewBlue = GammaLookup[Blue];
        const uint32_t NewGreen = GammaLookup[Green];
        const uint32_t NewRed = GammaLookup[Red];

        Data[i] = (NewAlpha << 24) + (NewBlue << 16) + (NewGreen << 8) + NewRed;
    }
}

void BitmapT::Scale(unsigned int NewSizeX, unsigned int NewSizeY) {
    if (NewSizeX >= 1 && NewSizeX != SizeX) {
        BitmapT NewBitmap;

        NewBitmap.SizeX = NewSizeX;
        NewBitmap.SizeY = SizeY;
        NewBitmap.Data.PushBackEmpty(NewSizeX * SizeY);

        for (unsigned int nx = 0; nx < NewSizeX; nx++) {
            const double Ratio = double(SizeX) / double(NewSizeX);
            const double ox1 = double(nx) * Ratio;
            const double ox2 = double(nx + 1) * Ratio;
            const unsigned int ox1_floor = nx * SizeX / NewSizeX;
            const unsigned int ox2_ceil = ((nx + 1) * SizeX + (NewSizeX - 1)) / NewSizeX;
            const double ox1_frac = ox1 - ox1_floor;
            const double ox2_frac = ox2 - (ox2_ceil - 1);

            for (unsigned int oy = 0; oy < SizeY; oy++) {
                double TotalR = 0.0;
                double TotalG = 0.0;
                double TotalB = 0.0;
                double TotalA = 0.0;

                for (unsigned int ox = ox1_floor; ox < ox2_ceil; ox++) {
                    double Weight;

                    if (ox1_floor != ox2_ceil - 1) {
                        if (ox == ox1_floor) Weight = (1.0 - ox1_frac) / Ratio;
                        else if (ox == ox2_ceil - 1) Weight = ox2_frac / Ratio;
                        else Weight = 1.0 / Ratio;
                    } else Weight = 1.0;

                    int r, g, b, a;
                    GetPixel(ox, oy, r, g, b, a);

                    TotalR += r * Weight;
                    TotalG += g * Weight;
                    TotalB += b * Weight;
                    TotalA += a * Weight;
                }

                NewBitmap.SetPixel(nx, oy, int(TotalR + 0.49), int(TotalG + 0.49), int(TotalB + 0.49),
                                   int(TotalA + 0.49));
            }
        }

        SizeX = NewBitmap.SizeX;
        SizeY = NewBitmap.SizeY;
        Data = NewBitmap.Data;
    }

    if (NewSizeY >= 1 && NewSizeY != SizeY) {
        BitmapT NewBitmap;

        NewBitmap.SizeX = SizeX;
        NewBitmap.SizeY = NewSizeY;
        NewBitmap.Data.PushBackEmpty(SizeX * NewSizeY);

        for (unsigned int ny = 0; ny < NewSizeY; ny++) {
            const double Ratio = double(SizeY) / double(NewSizeY);
            const double oy1 = double(ny) * Ratio;
            const double oy2 = double(ny + 1) * Ratio;
            const unsigned int oy1_floor = ny * SizeY / NewSizeY;
            const unsigned int oy2_ceil = ((ny + 1) * SizeY + (NewSizeY - 1)) / NewSizeY;
            const double oy1_frac = oy1 - oy1_floor;
            const double oy2_frac = oy2 - (oy2_ceil - 1);

            for (unsigned int ox = 0; ox < SizeX; ox++) {
                double TotalR = 0.0;
                double TotalG = 0.0;
                double TotalB = 0.0;
                double TotalA = 0.0;

                for (unsigned int oy = oy1_floor; oy < oy2_ceil; oy++) {
                    double Weight;

                    if (oy1_floor != oy2_ceil - 1) {
                        if (oy == oy1_floor) Weight = (1.0 - oy1_frac) / Ratio;
                        else if (oy == oy2_ceil - 1) Weight = oy2_frac / Ratio;
                        else Weight = 1.0 / Ratio;
                    } else Weight = 1.0;

                    int r, g, b, a;
                    GetPixel(ox, oy, r, g, b, a);

                    TotalR += r * Weight;
                    TotalG += g * Weight;
                    TotalB += b * Weight;
                    TotalA += a * Weight;
                }

                NewBitmap.SetPixel(ox, ny, int(TotalR + 0.49), int(TotalG + 0.49), int(TotalB + 0.49),
                                   int(TotalA + 0.49));
            }
        }

        SizeX = NewBitmap.SizeX;
        SizeY = NewBitmap.SizeY;
        Data = NewBitmap.Data;
    }
}

char *BitmapT::GetPalettedImage() const {
    char *PaletteImage = new char[256 * 3 + SizeX * SizeY];

    const int netsize = 256;
    const int maxnetpos = netsize - 1;
    const int netbiasshift = 4;
    const int lengthcount = 3 * SizeX * SizeY;
    const int samplefac = 1;

    const int intbiasshift = 16;
    const int intbias = ((int) 1) << intbiasshift;
    const int gammashift = 10;
    const int betashift = 10;
    const int beta = intbias >> betashift;
    const int betagamma = intbias << (gammashift - betashift);

    const int initrad = netsize >> 3;
    const int radiusbiasshift = 6;
    const int radiusbias = ((int) 1) << radiusbiasshift;
    const int initradius = initrad * radiusbias;
    const int radiusdec = 30;

    const int alphabiasshift = 10;
    const int initalpha = ((int) 1) << alphabiasshift;

    const int radbiasshift = 8;
    const int radbias = ((int) 1) << radbiasshift;
    const int alpharadbshift = alphabiasshift + radbiasshift;
    const int alpharadbias = ((int) 1) << alpharadbshift;

    int network[netsize][4];
    int netindex[256];
    int bias[netsize];
    int freq[netsize];
    int radpower[initrad];

    for (int i = 0; i < netsize; i++) {
        network[i][0] = (i << (netbiasshift + 8)) / netsize;
        network[i][1] = (i << (netbiasshift + 8)) / netsize;
        network[i][2] = (i << (netbiasshift + 8)) / netsize;
        freq[i] = intbias / netsize;
        bias[i] = 0;
    } {
        const int alphadec = 30 + ((samplefac - 1) / 3);
        unsigned long PixelNr = 0;
        int samplepixels = lengthcount / (3 * samplefac);
        int delta = samplepixels / 100;
        int alpha = initalpha;
        int radius = initradius;
        int rad = radius >> radiusbiasshift;
        const int step = (lengthcount % 499) != 0
                             ? 499
                             : (lengthcount % 491) != 0
                                   ? 491
                                   : (lengthcount % 487) != 0
                                         ? 487
                                         : 503;

        if (rad <= 1) rad = 0;
        for (int i = 0; i < rad; i++)
            radpower[i] = alpha * (((rad * rad - i * i) * radbias) / (rad * rad));

        for (int i = 0; i < samplepixels;) {
            int b = ((Data[PixelNr] >> 16) & 0xFF) << netbiasshift;
            int g = ((Data[PixelNr] >> 8) & 0xFF) << netbiasshift;
            int r = ((Data[PixelNr] >> 0) & 0xFF) << netbiasshift;

            int bestd = ~(((int) 1) << 31);
            int bestbiasd = bestd;
            int bestpos = -1;
            int bestbiaspos = bestpos; {
                int *p = bias;
                int *f = freq;

                for (int netnum = 0; netnum < netsize; netnum++) {
                    int *n = network[netnum];
                    int dist = abs(n[0] - b) + abs(n[1] - g) + abs(n[2] - r);
                    int biasdist = dist - ((*p) >> (intbiasshift - netbiasshift));
                    int betafreq = *f >> betashift;

                    if (dist < bestd) {
                        bestd = dist;
                        bestpos = netnum;
                    }
                    if (biasdist < bestbiasd) {
                        bestbiasd = biasdist;
                        bestbiaspos = netnum;
                    }

                    *f++ -= betafreq;
                    *p++ += (betafreq << gammashift);
                }
            }

            freq[bestpos] += beta;
            bias[bestpos] -= betagamma;

            int *n = network[bestbiaspos];
            *n -= (alpha * (*n - b)) / initalpha;
            n++;
            *n -= (alpha * (*n - g)) / initalpha;
            n++;
            *n -= (alpha * (*n - r)) / initalpha;

            if (rad) {
                int lo = bestbiaspos - rad;
                if (lo < -1) lo = -1;
                int hi = bestbiaspos + rad;
                if (hi > netsize) hi = netsize;
                int j = bestbiaspos + 1;
                int k = bestbiaspos - 1;
                int *q = radpower;

                while ((j < hi) || (k > lo)) {
                    int a = (*(++q));

                    if (j < hi) {
                        int *p = network[j];
                        *p -= (a * (*p - b)) / alpharadbias;
                        p++;
                        *p -= (a * (*p - g)) / alpharadbias;
                        p++;
                        *p -= (a * (*p - r)) / alpharadbias;
                        j++;
                    }

                    if (k > lo) {
                        int *p = network[k];
                        *p -= (a * (*p - b)) / alpharadbias;
                        p++;
                        *p -= (a * (*p - g)) / alpharadbias;
                        p++;
                        *p -= (a * (*p - r)) / alpharadbias;
                        k--;
                    }
                }
            }

            PixelNr += step;
            while (PixelNr >= SizeX * SizeY) PixelNr -= SizeX * SizeY;

            i++;
            if ((i % delta) == 0) {
                alpha -= alpha / alphadec;
                radius -= radius / radiusdec;
                rad = radius >> radiusbiasshift;
                if (rad <= 1) rad = 0;
                for (int j = 0; j < rad; j++)
                    radpower[j] = alpha * (((rad * rad - j * j) * radbias) / (rad * rad));
            }
        }
    }

    for (int i = 0; i < netsize; i++) {
        for (int j = 0; j < 3; j++) {
            int temp = (network[i][j] + (1 << (netbiasshift - 1))) >> netbiasshift;
            if (temp > 255) temp = 255;
            network[i][j] = temp;
        }
        network[i][3] = i;
    }

    for (int i = 2; i >= 0; i--)
        for (int j = 0; j < netsize; j++)
            PaletteImage[j * 3 + i] = network[j][2 - i];

    int previouscol = 0;
    int startpos = 0;

    for (int i = 0; i < netsize; i++) {
        int *p = network[i];
        int smallpos = i;
        int smallval = p[1];

        for (int j = i + 1; j < netsize; j++)
            if (network[j][1] < smallval) {
                smallpos = j;
                smallval = network[j][1];
            }

        if (i != smallpos) {
            int *q = network[smallpos];
            int j;

            j = q[0];
            q[0] = p[0];
            p[0] = j;
            j = q[1];
            q[1] = p[1];
            p[1] = j;
            j = q[2];
            q[2] = p[2];
            p[2] = j;
            j = q[3];
            q[3] = p[3];
            p[3] = j;
        }

        if (smallval != previouscol) {
            netindex[previouscol] = (startpos + i) >> 1;
            for (int j = previouscol + 1; j < smallval; j++) netindex[j] = i;
            previouscol = smallval;
            startpos = i;
        }
    }

    netindex[previouscol] = (startpos + maxnetpos) >> 1;
    for (int j = previouscol + 1; j < 256; j++) netindex[j] = maxnetpos;

    for (unsigned long PixelNr = 0; PixelNr < Data.Size(); PixelNr++) {
        int b = (Data[PixelNr] >> 16) & 0xFF;
        int g = (Data[PixelNr] >> 8) & 0xFF;
        int r = (Data[PixelNr] >> 0) & 0xFF;

        int bestd = 1000;
        int best = -1;
        int i = netindex[g];
        int j = i - 1;

        while ((i < netsize) || (j >= 0)) {
            if (i < netsize) {
                int *p = network[i];
                int dist = p[1] - g;
                if (dist >= bestd) i = netsize;
                else {
                    i++;
                    if (dist < 0) dist = -dist;
                    dist += abs(p[0] - b);
                    if (dist < bestd) {
                        dist += abs(p[2] - r);
                        if (dist < bestd) {
                            bestd = dist;
                            best = p[3];
                        }
                    }
                }
            }

            if (j >= 0) {
                int *p = network[j];
                int dist = g - p[1];
                if (dist >= bestd) j = -1;
                else {
                    j--;
                    if (dist < 0) dist = -dist;
                    dist += abs(p[0] - b);
                    if (dist < bestd) {
                        dist += abs(p[2] - r);
                        if (dist < bestd) {
                            bestd = dist;
                            best = p[3];
                        }
                    }
                }
            }
        }

        PaletteImage[3 * 256 + PixelNr] = best;
    }

    return PaletteImage;
}

bool BitmapT::SaveToDisk(const char *FileName) const {
    if (FileName == NULL) return false;

    const size_t FileNameLength = strlen(FileName);
    if (FileNameLength < 5) return false;

    ArrayT<unsigned char> img_data;
    img_data.PushBackEmpty(SizeX * SizeY * 4);

    for (unsigned int i = 0; i < SizeX * SizeY; i++) {
        img_data[i * 4 + 0] = (Data[i] >> 0) & 0xFF; // R
        img_data[i * 4 + 1] = (Data[i] >> 8) & 0xFF; // G
        img_data[i * 4 + 2] = (Data[i] >> 16) & 0xFF; // B
        img_data[i * 4 + 3] = (Data[i] >> 24) & 0xFF; // A
    }

    if (_stricmp(&FileName[FileNameLength - 4], ".bmp") == 0) {
        return stbi_write_bmp(FileName, SizeX, SizeY, 4, &img_data[0]) != 0;
    } else if (_stricmp(&FileName[FileNameLength - 4], ".png") == 0) {
        return stbi_write_png(FileName, SizeX, SizeY, 4, &img_data[0], SizeX * 4) != 0;
    } else if (_stricmp(&FileName[FileNameLength - 4], ".jpg") == 0 ||
               _stricmp(&FileName[FileNameLength - 5], ".jpeg") == 0) {
        ArrayT<unsigned char> rgb_data;
        rgb_data.PushBackEmpty(SizeX * SizeY * 3);

        for (unsigned int i = 0; i < SizeX * SizeY; i++) {
            rgb_data[i * 3 + 0] = img_data[i * 4 + 0]; // R
            rgb_data[i * 3 + 1] = img_data[i * 4 + 1]; // G
            rgb_data[i * 3 + 2] = img_data[i * 4 + 2]; // B
        }

        return stbi_write_jpg(FileName, SizeX, SizeY, 3, &rgb_data[0], 95) != 0;
    } else if (_stricmp(&FileName[FileNameLength - 4], ".tga") == 0) {
        return stbi_write_tga(FileName, SizeX, SizeY, 4, &img_data[0]) != 0;
    }

    return false;
}
