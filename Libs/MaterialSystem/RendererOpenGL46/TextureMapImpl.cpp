/*
Cafu Engine, http://www.cafu.de/
Copyright (c) Carsten Fuchs and other contributors.
This project is licensed under the terms of the MIT license.
*/

/**********************************/
/*** Texture Map Implementation ***/
/**********************************/

// Required for Windows includes
#if defined(_WIN32) && defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <GL/glew.h>
#include <vector>
#include <string>

#include "TextureMapImpl.hpp"
#include "RendererImpl.hpp"
#include "Bitmap/Bitmap.hpp"


inline bool isPowerOf2(unsigned long i) {
    return i > 0 && (i & (i - 1)) == 0;
}


/*********************/
/*** TextureMap2DT ***/
/*********************/

TextureMap2DT::TextureMap2DT(const MapCompositionT &MapComp_)
    : Source(MC),
      MapComp(MapComp_),
      Bitmap(nullptr),
      Data(nullptr),
      SizeX(0),
      SizeY(0),
      BytesPerPixel(0),
      OpenGLObject(0),
      InitCounter(0) {
}


TextureMap2DT::TextureMap2DT(char *Data_, unsigned long SizeX_, unsigned long SizeY_, char BytesPerPixel_,
                             bool MakePrivateCopy, const MapCompositionT &McForFiltersAndWrapping)
    : Source(MakePrivateCopy ? RawPtrOwn : RawPtrExt),
      MapComp(McForFiltersAndWrapping),
      Bitmap(nullptr),
      Data(MakePrivateCopy ? new char[SizeX_ * SizeY_ * BytesPerPixel_] : Data_),
      SizeX(SizeX_),
      SizeY(SizeY_),
      BytesPerPixel(BytesPerPixel_),
      OpenGLObject(0),
      InitCounter(0) {
    if (MakePrivateCopy)
        memcpy(Data, Data_, SizeX_ * SizeY_ * BytesPerPixel_);
}


TextureMap2DT::TextureMap2DT(BitmapT *Bitmap_, bool MakePrivateCopy, const MapCompositionT &McForFiltersAndWrapping)
    : Source(MakePrivateCopy ? BitmapPtrOwn : BitmapPtrExt),
      MapComp(McForFiltersAndWrapping),
      Bitmap(MakePrivateCopy ? new BitmapT(*Bitmap_) : Bitmap_),
      Data(nullptr),
      SizeX(0),
      SizeY(0),
      BytesPerPixel(0),
      OpenGLObject(0),
      InitCounter(0) {
}


bool TextureMap2DT::IsCreatedFromMapComp(const MapCompositionT &MC_) {
    return Source == MC && MapComp == MC_;
}


TextureMap2DT::~TextureMap2DT() {
    switch (Source) {
        case MC: delete Bitmap;
            break;
        case RawPtrExt: break;
        case RawPtrOwn: delete[] Data;
            break;
        case BitmapPtrExt: break;
        case BitmapPtrOwn: delete Bitmap;
            break;
    }

    if (InitCounter == RendererImplT::GetInstance().GetInitCounter() && OpenGLObject != 0) {
        glDeleteTextures(1, &OpenGLObject);
    }
}


unsigned int TextureMap2DT::GetSizeX() {
    switch (Source) {
        case MC:
            if (!Bitmap) Bitmap = MapComp.GetBitmap();
            return Bitmap->SizeX;

        case RawPtrExt:
        case RawPtrOwn:
            return SizeX;

        case BitmapPtrExt:
        case BitmapPtrOwn:
            return Bitmap->SizeX;
    }

    return 0;
}


unsigned int TextureMap2DT::GetSizeY() {
    switch (Source) {
        case MC:
            if (!Bitmap) Bitmap = MapComp.GetBitmap();
            return Bitmap->SizeY;

        case RawPtrExt:
        case RawPtrOwn:
            return SizeY;

        case BitmapPtrExt:
        case BitmapPtrOwn:
            return Bitmap->SizeY;
    }

    return 0;
}


GLuint TextureMap2DT::GetOpenGLObject() {
    if (InitCounter < RendererImplT::GetInstance().GetInitCounter()) {
        const GLint InternalFormat = MapComp.GetNoCompression() ? GL_RGBA8 : GL_COMPRESSED_RGBA;

        switch (Source) {
            case MC:
            case BitmapPtrExt:
            case BitmapPtrOwn: {
                if (Source == MC && !Bitmap) Bitmap = MapComp.GetBitmap();

                BitmapT *ScaledBitmap = nullptr;
                const unsigned long MaxSize = TextureMapManagerImplT::Get().GetMaxTextureSize();

                // For modern OpenGL, we don't strictly need power-of-2 textures
                // but we'll still respect MaxTextureSize
                if (Bitmap->SizeX <= MaxSize && Bitmap->SizeY <= MaxSize) {
                    ScaledBitmap = Bitmap;
                } else {
                    ScaledBitmap = new BitmapT(*Bitmap);

                    unsigned long NewX = Bitmap->SizeX;
                    unsigned long NewY = Bitmap->SizeY;

                    if (!MapComp.GetNoScaleDown()) {
                        while (NewX > MaxSize) NewX = MaxSize;
                        while (NewY > MaxSize) NewY = MaxSize;
                    }

                    ScaledBitmap->Scale(NewX, NewY);
                }

                glGenTextures(1, &OpenGLObject);
                glBindTexture(GL_TEXTURE_2D, OpenGLObject);

                // Set texture parameters first (can be done before uploading texture data)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                MapComp.GetWrapModeS() == MapCompositionT::Repeat
                                    ? GL_REPEAT
                                    : MapComp.GetWrapModeS() == MapCompositionT::Clamp
                                          ? GL_CLAMP_TO_EDGE
                                          : GL_CLAMP_TO_EDGE);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                MapComp.GetWrapModeT() == MapCompositionT::Repeat
                                    ? GL_REPEAT
                                    : MapComp.GetWrapModeT() == MapCompositionT::Clamp
                                          ? GL_CLAMP_TO_EDGE
                                          : GL_CLAMP_TO_EDGE);

                GLenum minFilter = GL_LINEAR;
                switch (MapComp.GetMinFilter()) {
                    case MapCompositionT::Nearest: minFilter = GL_NEAREST;
                        break;
                    case MapCompositionT::Linear: minFilter = GL_LINEAR;
                        break;
                    case MapCompositionT::Nearest_MipMap_Nearest: minFilter = GL_NEAREST_MIPMAP_NEAREST;
                        break;
                    case MapCompositionT::Nearest_MipMap_Linear: minFilter = GL_NEAREST_MIPMAP_LINEAR;
                        break;
                    case MapCompositionT::Linear_MipMap_Nearest: minFilter = GL_LINEAR_MIPMAP_NEAREST;
                        break;
                    case MapCompositionT::Linear_MipMap_Linear: minFilter = GL_LINEAR_MIPMAP_LINEAR;
                        break;
                }
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

                GLenum magFilter = MapComp.GetMagFilter() == MapCompositionT::Nearest ? GL_NEAREST : GL_LINEAR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

                // Upload texture data
                glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ScaledBitmap->SizeX, ScaledBitmap->SizeY,
                             0, GL_RGBA, GL_UNSIGNED_BYTE, &ScaledBitmap->Data[0]);

                // Generate mipmaps if needed
                if (MapComp.GetMinFilter() >= MapCompositionT::Nearest_MipMap_Nearest) {
                    glGenerateMipmap(GL_TEXTURE_2D);
                }

                if (ScaledBitmap != Bitmap) {
                    delete ScaledBitmap;
                }
                break;
            }

            case RawPtrExt:
            case RawPtrOwn: {
                glGenTextures(1, &OpenGLObject);
                glBindTexture(GL_TEXTURE_2D, OpenGLObject);

                // Set texture parameters
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                MapComp.GetWrapModeS() == MapCompositionT::Repeat
                                    ? GL_REPEAT
                                    : MapComp.GetWrapModeS() == MapCompositionT::Clamp
                                          ? GL_CLAMP_TO_EDGE
                                          : GL_CLAMP_TO_EDGE);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                MapComp.GetWrapModeT() == MapCompositionT::Repeat
                                    ? GL_REPEAT
                                    : MapComp.GetWrapModeT() == MapCompositionT::Clamp
                                          ? GL_CLAMP_TO_EDGE
                                          : GL_CLAMP_TO_EDGE);

                GLenum minFilter = GL_LINEAR;
                switch (MapComp.GetMinFilter()) {
                    case MapCompositionT::Nearest: minFilter = GL_NEAREST;
                        break;
                    case MapCompositionT::Linear: minFilter = GL_LINEAR;
                        break;
                    case MapCompositionT::Nearest_MipMap_Nearest: minFilter = GL_NEAREST_MIPMAP_NEAREST;
                        break;
                    case MapCompositionT::Nearest_MipMap_Linear: minFilter = GL_NEAREST_MIPMAP_LINEAR;
                        break;
                    case MapCompositionT::Linear_MipMap_Nearest: minFilter = GL_LINEAR_MIPMAP_NEAREST;
                        break;
                    case MapCompositionT::Linear_MipMap_Linear: minFilter = GL_LINEAR_MIPMAP_LINEAR;
                        break;
                }
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

                GLenum magFilter = MapComp.GetMagFilter() == MapCompositionT::Nearest ? GL_NEAREST : GL_LINEAR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

                // Upload texture data
                GLenum format = BytesPerPixel == 3 ? GL_RGB : GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, SizeX, SizeY,
                             0, format, GL_UNSIGNED_BYTE, Data);

                // Generate mipmaps if needed
                if (MapComp.GetMinFilter() >= MapCompositionT::Nearest_MipMap_Nearest) {
                    glGenerateMipmap(GL_TEXTURE_2D);
                }
                break;
            }
        }

        InitCounter = RendererImplT::GetInstance().GetInitCounter();
    }

    return OpenGLObject;
}


/***********************/
/*** TextureMapCubeT ***/
/***********************/

const GLenum TextureMapCubeT::CubeTargets[6] =
{
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};


const std::string TextureMapCubeT::CubeSuffixes[6] =
{
    "_px",
    "_py",
    "_pz",
    "_nx",
    "_ny",
    "_nz"
};


std::string TextureMapCubeT::GetFullCubeMapString(std::string BaseString, unsigned long SuffixNr) {
    for (std::string::size_type i = BaseString.find("#"); i != std::string::npos; i = BaseString.find("#"))
        BaseString.replace(i, 1, TextureMapCubeT::CubeSuffixes[SuffixNr]);

    return BaseString;
}


TextureMapCubeT::TextureMapCubeT(const MapCompositionT &MapComp_)
    : Source(Files),
      MapComp(MapComp_),
      SizeX(0),
      SizeY(0),
      BytesPerPixel(0),
      OpenGLObject(0),
      InitCounter(0) {
    for (int i = 0; i < 6; i++) Bitmap[i] = nullptr;
    for (int i = 0; i < 6; i++) Data[i] = nullptr;
}


TextureMapCubeT::TextureMapCubeT(char *Data_[6], unsigned long SizeX_, unsigned long SizeY_, char BytesPerPixel_,
                                 bool MakePrivateCopy, const MapCompositionT &McForFiltersAndWrapping)
    : Source(MakePrivateCopy ? RawPtrOwn : RawPtrExt),
      MapComp(McForFiltersAndWrapping),
      SizeX(SizeX_),
      SizeY(SizeY_),
      BytesPerPixel(BytesPerPixel_),
      OpenGLObject(0),
      InitCounter(0) {
    for (int i = 0; i < 6; i++) Bitmap[i] = nullptr;

    for (int i = 0; i < 6; i++)
        Data[i] = MakePrivateCopy ? new char[SizeX_ * SizeY_ * BytesPerPixel_] : Data_[i];

    if (MakePrivateCopy)
        for (int i = 0; i < 6; i++)
            memcpy(Data[i], Data_[i], SizeX_ * SizeY_ * BytesPerPixel_);
}


TextureMapCubeT::TextureMapCubeT(BitmapT *Bitmap_[6], bool MakePrivateCopy,
                                 const MapCompositionT &McForFiltersAndWrapping)
    : Source(MakePrivateCopy ? BitmapPtrOwn : BitmapPtrExt),
      MapComp(McForFiltersAndWrapping),
      SizeX(0),
      SizeY(0),
      BytesPerPixel(0),
      OpenGLObject(0),
      InitCounter(0) {
    for (int i = 0; i < 6; i++)
        Bitmap[i] = MakePrivateCopy ? new BitmapT(*Bitmap_[i]) : Bitmap_[i];
    for (int i = 0; i < 6; i++)
        Data[i] = nullptr;
}


bool TextureMapCubeT::IsCreatedFromMapComp(const MapCompositionT &MC_) {
    return Source == Files && MapComp == MC_;
}


TextureMapCubeT::~TextureMapCubeT() {
    switch (Source) {
        case Files: for (int i = 0; i < 6; i++) delete Bitmap[i];
            break;
        case RawPtrExt: break;
        case RawPtrOwn: for (int i = 0; i < 6; i++) delete[] Data[i];
            break;
        case BitmapPtrExt: break;
        case BitmapPtrOwn: for (int i = 0; i < 6; i++) delete Bitmap[i];
            break;
    }

    if (InitCounter == RendererImplT::GetInstance().GetInitCounter() && OpenGLObject != 0) {
        glDeleteTextures(1, &OpenGLObject);
    }
}


unsigned int TextureMapCubeT::GetSizeX() {
    switch (Source) {
        case Files:
            if (!Bitmap[0]) {
                Bitmap[0] = MapCompositionT(GetFullCubeMapString(MapComp.GetString(), 0), MapComp.GetBaseDir()).
                        GetBitmap();
            }
            return Bitmap[0]->SizeX;

        case RawPtrExt:
        case RawPtrOwn:
            return SizeX;

        case BitmapPtrExt:
        case BitmapPtrOwn:
            return Bitmap[0]->SizeX;
    }

    return 0;
}


unsigned int TextureMapCubeT::GetSizeY() {
    switch (Source) {
        case Files:
            if (!Bitmap[0]) {
                Bitmap[0] = MapCompositionT(GetFullCubeMapString(MapComp.GetString(), 0), MapComp.GetBaseDir()).
                        GetBitmap();
            }
            return Bitmap[0]->SizeY;

        case RawPtrExt:
        case RawPtrOwn:
            return SizeY;

        case BitmapPtrExt:
        case BitmapPtrOwn:
            return Bitmap[0]->SizeY;
    }

    return 0;
}


GLuint TextureMapCubeT::GetOpenGLObject() {
    if (InitCounter < RendererImplT::GetInstance().GetInitCounter()) {
        const GLint InternalFormat = MapComp.GetNoCompression() ? GL_RGBA8 : GL_COMPRESSED_RGBA;

        switch (Source) {
            case Files:
            case BitmapPtrExt:
            case BitmapPtrOwn: {
                glGenTextures(1, &OpenGLObject);
                glBindTexture(GL_TEXTURE_CUBE_MAP, OpenGLObject);

                for (unsigned long SideNr = 0; SideNr < 6; SideNr++) {
                    if (Source == Files && !Bitmap[SideNr]) {
                        Bitmap[SideNr] = MapCompositionT(GetFullCubeMapString(MapComp.GetString(), SideNr),
                                                         MapComp.GetBaseDir()).GetBitmap();
                    }

                    BitmapT *ScaledBitmap = nullptr;
                    const unsigned long MaxSize = TextureMapManagerImplT::Get().GetMaxTextureSize();

                    if (Bitmap[SideNr]->SizeX <= MaxSize && Bitmap[SideNr]->SizeY <= MaxSize) {
                        ScaledBitmap = Bitmap[SideNr];
                    } else {
                        ScaledBitmap = new BitmapT(*Bitmap[SideNr]);

                        unsigned long NewX = Bitmap[SideNr]->SizeX;
                        unsigned long NewY = Bitmap[SideNr]->SizeY;

                        if (!MapComp.GetNoScaleDown()) {
                            while (NewX > MaxSize) NewX = MaxSize;
                            while (NewY > MaxSize) NewY = MaxSize;
                        }

                        ScaledBitmap->Scale(NewX, NewY);
                    }

                    glTexImage2D(CubeTargets[SideNr], 0, InternalFormat,
                                 ScaledBitmap->SizeX, ScaledBitmap->SizeY,
                                 0, GL_RGBA, GL_UNSIGNED_BYTE, &ScaledBitmap->Data[0]);

                    if (ScaledBitmap != Bitmap[SideNr]) {
                        delete ScaledBitmap;
                    }
                }

                // Set texture parameters
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

                GLenum minFilter = GL_LINEAR;
                switch (MapComp.GetMinFilter()) {
                    case MapCompositionT::Nearest: minFilter = GL_NEAREST;
                        break;
                    case MapCompositionT::Linear: minFilter = GL_LINEAR;
                        break;
                    case MapCompositionT::Nearest_MipMap_Nearest: minFilter = GL_NEAREST_MIPMAP_NEAREST;
                        break;
                    case MapCompositionT::Nearest_MipMap_Linear: minFilter = GL_NEAREST_MIPMAP_LINEAR;
                        break;
                    case MapCompositionT::Linear_MipMap_Nearest: minFilter = GL_LINEAR_MIPMAP_NEAREST;
                        break;
                    case MapCompositionT::Linear_MipMap_Linear: minFilter = GL_LINEAR_MIPMAP_LINEAR;
                        break;
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);

                GLenum magFilter = MapComp.GetMagFilter() == MapCompositionT::Nearest ? GL_NEAREST : GL_LINEAR;
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);

                // Generate mipmaps if needed
                if (MapComp.GetMinFilter() >= MapCompositionT::Nearest_MipMap_Nearest) {
                    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                }
                break;
            }

            case RawPtrExt:
            case RawPtrOwn: {
                glGenTextures(1, &OpenGLObject);
                glBindTexture(GL_TEXTURE_CUBE_MAP, OpenGLObject);

                for (unsigned long SideNr = 0; SideNr < 6; SideNr++) {
                    GLenum format = BytesPerPixel == 3 ? GL_RGB : GL_RGBA;
                    glTexImage2D(CubeTargets[SideNr], 0, InternalFormat,
                                 SizeX, SizeY,
                                 0, format, GL_UNSIGNED_BYTE, Data[SideNr]);
                }

                // Set texture parameters
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

                GLenum minFilter = GL_LINEAR;
                switch (MapComp.GetMinFilter()) {
                    case MapCompositionT::Nearest: minFilter = GL_NEAREST;
                        break;
                    case MapCompositionT::Linear: minFilter = GL_LINEAR;
                        break;
                    case MapCompositionT::Nearest_MipMap_Nearest: minFilter = GL_NEAREST_MIPMAP_NEAREST;
                        break;
                    case MapCompositionT::Nearest_MipMap_Linear: minFilter = GL_NEAREST_MIPMAP_LINEAR;
                        break;
                    case MapCompositionT::Linear_MipMap_Nearest: minFilter = GL_LINEAR_MIPMAP_NEAREST;
                        break;
                    case MapCompositionT::Linear_MipMap_Linear: minFilter = GL_LINEAR_MIPMAP_LINEAR;
                        break;
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);

                GLenum magFilter = MapComp.GetMagFilter() == MapCompositionT::Nearest ? GL_NEAREST : GL_LINEAR;
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);

                // Generate mipmaps if needed
                if (MapComp.GetMinFilter() >= MapCompositionT::Nearest_MipMap_Nearest) {
                    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                }
                break;
            }
        }

        InitCounter = RendererImplT::GetInstance().GetInitCounter();
    }

    return OpenGLObject;
}


/******************************/
/*** TextureMapManagerImplT ***/
/******************************/

TextureMapManagerImplT &TextureMapManagerImplT::Get() {
    static TextureMapManagerImplT *TMMI = new TextureMapManagerImplT();
    return *TMMI;
}


void TextureMapManagerImplT::SetMaxTextureSize(unsigned long MaxSize) {
    MaxTextureMapSize = MaxSize;
}


unsigned long TextureMapManagerImplT::GetMaxTextureSize() const {
    return MaxTextureMapSize;
}


MatSys::TextureMapI *TextureMapManagerImplT::GetTextureMap2D(const MapCompositionT &MC) {
    return GetTextureMap2DInternal(MC);
}


MatSys::TextureMapI *TextureMapManagerImplT::GetTextureMap2D(char *Data, unsigned long SizeX, unsigned long SizeY,
                                                             char BytesPerPixel, bool MakePrivateCopy,
                                                             const MapCompositionT &McForFiltersAndWrapping) {
    return GetTextureMap2DInternal(Data, SizeX, SizeY, BytesPerPixel, MakePrivateCopy, McForFiltersAndWrapping);
}


MatSys::TextureMapI *TextureMapManagerImplT::GetTextureMap2D(BitmapT *Bitmap, bool MakePrivateCopy,
                                                             const MapCompositionT &McForFiltersAndWrapping) {
    return GetTextureMap2DInternal(Bitmap, MakePrivateCopy, McForFiltersAndWrapping);
}


void TextureMapManagerImplT::FreeTextureMap(MatSys::TextureMapI *TM) {
    FreeTextureMap(static_cast<TextureMapImplT *>(TM));
}


TextureMap2DT *TextureMapManagerImplT::GetTextureMap2DInternal(const MapCompositionT &MapComp_) {
    for (unsigned long TMINr = 0; TMINr < TexMapRepository.Size(); TMINr++)
        if (TexMapRepository[TMINr]->IsCreatedFromMapComp(MapComp_)) {
            TexMapRepositoryCount[TMINr]++;
            return static_cast<TextureMap2DT *>(TexMapRepository[TMINr]);
        }

    TextureMap2DT *NewTMI = new TextureMap2DT(MapComp_);
    TexMapRepository.PushBack(NewTMI);
    TexMapRepositoryCount.PushBack(1);
    return NewTMI;
}


TextureMap2DT *TextureMapManagerImplT::GetTextureMap2DInternal(char *Data, unsigned long SizeX, unsigned long SizeY,
                                                               char BytesPerPixel, bool MakePrivateCopy,
                                                               const MapCompositionT &McForFiltersAndWrapping) {
    TextureMap2DT *NewTMI = new TextureMap2DT(Data, SizeX, SizeY, BytesPerPixel, MakePrivateCopy,
                                              McForFiltersAndWrapping);
    TexMapRepository.PushBack(NewTMI);
    TexMapRepositoryCount.PushBack(1);
    return NewTMI;
}


TextureMap2DT *TextureMapManagerImplT::GetTextureMap2DInternal(BitmapT *Bitmap, bool MakePrivateCopy,
                                                               const MapCompositionT &McForFiltersAndWrapping) {
    TextureMap2DT *NewTMI = new TextureMap2DT(Bitmap, MakePrivateCopy, McForFiltersAndWrapping);
    TexMapRepository.PushBack(NewTMI);
    TexMapRepositoryCount.PushBack(1);
    return NewTMI;
}


TextureMapCubeT *TextureMapManagerImplT::GetTextureMapCubeInternal(const MapCompositionT &MapComp_) {
    for (unsigned long TMINr = 0; TMINr < TexMapRepository.Size(); TMINr++)
        if (TexMapRepository[TMINr]->IsCreatedFromMapComp(MapComp_)) {
            TexMapRepositoryCount[TMINr]++;
            return (TextureMapCubeT *) TexMapRepository[TMINr];
        }

    TextureMapCubeT *NewTMI = new TextureMapCubeT(MapComp_);
    TexMapRepository.PushBack(NewTMI);
    TexMapRepositoryCount.PushBack(1);
    return NewTMI;
}


TextureMapCubeT *TextureMapManagerImplT::GetTextureMapCubeInternal(char *Data[6], unsigned long SizeX,
                                                                   unsigned long SizeY, char BytesPerPixel,
                                                                   bool MakePrivateCopy,
                                                                   const MapCompositionT &McForFiltersAndWrapping) {
    TextureMapCubeT *NewTMI = new TextureMapCubeT(Data, SizeX, SizeY, BytesPerPixel, MakePrivateCopy,
                                                  McForFiltersAndWrapping);
    TexMapRepository.PushBack(NewTMI);
    TexMapRepositoryCount.PushBack(1);
    return NewTMI;
}


TextureMapCubeT *TextureMapManagerImplT::GetTextureMapCubeInternal(BitmapT *Bitmap[6], bool MakePrivateCopy,
                                                                   const MapCompositionT &McForFiltersAndWrapping) {
    TextureMapCubeT *NewTMI = new TextureMapCubeT(Bitmap, MakePrivateCopy, McForFiltersAndWrapping);
    TexMapRepository.PushBack(NewTMI);
    TexMapRepositoryCount.PushBack(1);
    return NewTMI;
}


void TextureMapManagerImplT::FreeTextureMap(TextureMapImplT *TM) {
    if (TM == NULL) return;

    for (unsigned long TMINr = 0; TMINr < TexMapRepository.Size(); TMINr++)
        if (TexMapRepository[TMINr] == TM) {
            TexMapRepositoryCount[TMINr]--;

            if (TexMapRepositoryCount[TMINr] == 0) {
                delete TM;

                TexMapRepository.RemoveAt(TMINr);
                TexMapRepositoryCount.RemoveAt(TMINr);
            }

            break;
        }
}
