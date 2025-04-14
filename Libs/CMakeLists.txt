cmake_minimum_required(VERSION 3.9)

project("Cafu Libraries")

add_subdirectory(SceneGraph)
add_subdirectory(FileSys)
add_subdirectory(MaterialSystem)
add_subdirectory(SoundSystem)
add_subdirectory(ClipSys)
add_subdirectory(Math3D)
add_subdirectory(TextParser)
add_subdirectory(Terrain)
add_subdirectory(Bitmap)
add_subdirectory(Models)
add_subdirectory(Plants)
add_subdirectory(GuiSys)
add_subdirectory(GameSys)

# find_package(ZLIB REQUIRED)

#[[add_library(cfsLib STATIC
    Bitmap/Bitmap.cpp Bitmap/jdatasrc.cpp
    ConsoleCommands/ConVar.cpp # ConsoleCommands/ConFunc.cpp
    #ConsoleCommands/ConsoleInterpreterImpl.cpp ConsoleCommands/ConsoleInterpreter_LuaBinding.cpp
    ConsoleCommands/Console.cpp # ConsoleCommands/Console_Lua.cpp ConsoleCommands/ConsoleComposite.cpp
        ConsoleCommands/ConsoleStdout.cpp ConsoleCommands/ConsoleStringBuffer.cpp ConsoleCommands/ConsoleWarningsOnly.cpp ConsoleCommands/ConsoleFile.cpp
    Fonts/Font.cpp Fonts/FontTT.cpp MapFile.cpp TypeSys.cpp VarVisitorsLua.cpp
        UniScriptState.cpp PhysicsWorld.cpp DebugLog.cpp
    FileSys/FileManImpl.cpp FileSys/FileSys_LocalPath.cpp FileSys/FileSys_ZipArchive_GV.cpp FileSys/File_local.cpp FileSys/File_memory.cpp FileSys/Password.cpp
)]]

add_library(cfsLib STATIC
        Bitmap/Bitmap.cpp
        Bitmap/jdatasrc.cpp
        ConsoleCommands/ConVar.cpp
        ConsoleCommands/ConFunc.cpp
        ConsoleCommands/ConsoleInterpreterImpl.cpp
        ConsoleCommands/ConsoleInterpreter_LuaBinding.cpp
        ConsoleCommands/Console.cpp
        ConsoleCommands/Console_Lua.cpp
        ConsoleCommands/ConsoleComposite.cpp
        ConsoleCommands/ConsoleStdout.cpp
        ConsoleCommands/ConsoleStringBuffer.cpp
        ConsoleCommands/ConsoleWarningsOnly.cpp
        ConsoleCommands/ConsoleFile.cpp
        Fonts/Font.cpp
        Fonts/FontTT.cpp
        FileSys/FileManImpl.cpp
        FileSys/FileSys_LocalPath.cpp
        FileSys/FileSys_ZipArchive_GV.cpp
        FileSys/File_local.cpp
        FileSys/File_memory.cpp
        FileSys/Password.cpp
        MainWindow/glfwLibrary.cpp
        MainWindow/glfwMainWindow.cpp
        MainWindow/glfwMonitor.cpp
        MainWindow/glfwWindow.cpp
        MapFile.cpp
        Models/Loader.cpp
        Models/Loader_ase.cpp
        Models/Loader_cmdl.cpp
        Models/Loader_dlod.cpp
        Models/Loader_dummy.cpp
        Models/Loader_lwo.cpp
        Models/Loader_md5.cpp
        Models/Loader_mdl.cpp
        Models/Loader_mdl_hl1.cpp
        Models/Loader_mdl_hl2.cpp
        Models/Loader_mdl_hl2_vtf.cpp
        Models/AnimExpr.cpp
        Models/AnimPose.cpp
        Models/Model_cmdl.cpp
        Models/ModelManager.cpp
        Network/Network.cpp
        Network/State.cpp
        ParticleEngine/ParticleEngineMS.cpp
        PlatformAux.cpp
        Terrain/Terrain.cpp
        TextParser/TextParser.cpp
        Plants/Tree.cpp
        Plants/PlantDescription.cpp
        Plants/PlantDescrMan.cpp
        Util/Util.cpp
        Win32/Win32PrintHelp.cpp
        DebugLog.cpp
        PhysicsWorld.cpp
        TypeSys.cpp
        UniScriptState.cpp
        Variables.cpp
        VarVisitorsLua.cpp
)

target_include_directories(cfsLib PRIVATE ${minizip_SOURCE_DIR}/include "${bullet_SOURCE_DIR}/src")
target_link_libraries(cfsLib PRIVATE minizip glfw)
