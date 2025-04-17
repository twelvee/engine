#include "RendererImpl.hpp"
#include "TextureMapImpl.hpp"
#include "ConsoleCommands/Console.hpp"
#include "FileSys/FileMan.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int __stdcall DllMain(void * /*hInstance*/, unsigned long /*Reason*/, void * /*Reserved*/) {
    return TRUE;
}

#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
    #define DLL_EXPORT extern "C"
    #define __stdcall
#endif


cf::ConsoleI *Console = NULL;
cf::FileSys::FileManI *cf::FileSys::FileMan = NULL;

DLL_EXPORT MatSysV2::RendererI * __stdcall GetRenderer(cf::ConsoleI *Console_, cf::FileSys::FileManI *FileMan_) {
    Console = Console_;
    cf::FileSys::FileMan = FileMan_;

    return &RendererImplT::GetInstance();
}


DLL_EXPORT MatSysV2::TextureMapManagerI * __stdcall GetTextureMapManager() {
    return &TextureMapManagerImplT::Get();
}
