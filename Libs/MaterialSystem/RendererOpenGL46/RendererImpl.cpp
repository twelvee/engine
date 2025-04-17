/*
Cafu Engine, http://www.cafu.de/
Copyright (c) Carsten Fuchs and other contributors.
This project is licensed under the terms of the MIT license.
*/

/*******************************/
/*** Renderer Implementation ***/
/*******************************/

#ifdef _MSC_VER
#include <windows.h>
#endif
#include <cstdio>
#include <stdlib.h>
#include <GL/glew.h>

#include "RendererImpl.hpp"
#include "../Common/OpenGLState.hpp"
#include "RenderMaterial.hpp"
#include "Shader.hpp"
#include "TextureMapImpl.hpp"

#include "../Common/OpenGLEx.hpp"
#include "ConsoleCommands/Console.hpp"
#include "Templates/Array.hpp"


using namespace MatSys;


RendererImplT &RendererImplT::GetInstance() {
    static RendererImplT Renderer;

    return Renderer;
}


RendererImplT::RendererImplT()
    : CurrentRenderAction(AMBIENT),
      CurrentRenderMaterial(NULL),
      LockCurrentRM(false),
      CurrentShader(NULL),
      CurrentLightMap(NULL),
      CurrentSHLLookupMap(NULL),
      InitCounter(1),
      MatrixModelView(Matrix[WORLD_TO_VIEW], Matrix[MODEL_TO_WORLD]) {
    for (unsigned long SHLMapNr = 0; SHLMapNr < 16; SHLMapNr++)
        CurrentSHLMaps[SHLMapNr] = NULL;

    for (unsigned long MN = 0; MN < END_MARKER; MN++) {
        Matrix[MN].Age = 1;
        MatrixInv[MN].SetSourceMatrix(&Matrix[MN]);
    }

    ExpressionSymbols.GenFloat.PushBackEmpty(32);
    ExpressionSymbols.GenInt.PushBackEmpty(32);
}


bool RendererImplT::IsSupported() const {
    // Initialize GLEW if not already done
    static bool glewInitialized = false;
    if (!glewInitialized) {
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
#ifdef DEBUG
            Console->Print(cf::va("%s (%u): GLEW initialization failed: %s\n", __FILE__, __LINE__,
                                  glewGetErrorString(err)));
#endif
            return false;
        }
        glewInitialized = true;
    }

    // Check OpenGL version
    const char *version = (const char *) glGetString(GL_VERSION);
#ifdef DEBUG
    Console->Print(cf::va("%s (%u): OpenGL version: %s\n", __FILE__, __LINE__, version ? version : "NULL"));
#endif

    if (!version) return false;

    // Parse version string (e.g. "4.6.0 NVIDIA 456.71")
    int major = 0, minor = 0;
    if (sscanf(version, "%d.%d", &major, &minor) != 2)
        return false;

    if (major < 4 || (major == 4 && minor < 6)) {
#ifdef DEBUG
        Console->Print(cf::va("%s (%u): OpenGL 4.6 not supported\n", __FILE__, __LINE__));
#endif
        return false;
    }

    return true;
}


bool RendererImplT::DoesSupportCompressedSHL() const {
    return true; // Modern OpenGL supports compressed textures
}


bool RendererImplT::DoesSupportUncompressedSHL() const {
    return true;
}


int RendererImplT::GetPreferenceNr() const {
    return 1000;
}


void RendererImplT::Initialize() {
    // Initialize GLEW
    glewExperimental = GL_TRUE; // Needed for core profile
    GLenum err = glewInit();
    if (err != GLEW_OK) {
#ifdef DEBUG
        Console->Print(cf::va("%s (%u): GLEW initialization failed: %s\n", __FILE__, __LINE__,
                              glewGetErrorString(err)));
#endif
        return;
    }

    if (GLEW_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(DebugOutputCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glGenVertexArrays(1, &m_defaultVAO);
    glBindVertexArray(m_defaultVAO);

    GLfloat quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_screenQuadVAO);
    glGenBuffers(1, &m_screenQuadVBO);

    glBindVertexArray(m_screenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));

    glBindVertexArray(0);

    // Matrices
    MatrixT ProjMat;
    glGetFloatv(GL_PROJECTION_MATRIX, &ProjMat.m[0][0]);

    SetMatrix(MODEL_TO_WORLD, MatrixT());
    SetMatrix(WORLD_TO_VIEW, MatrixT());
    SetMatrix(PROJECTION, ProjMat);

    // Uniform buffer for matrices
    glGenBuffers(1, &m_matrixUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_matrixUBO);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(MatrixT), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_matrixUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLenum glErr = glGetError();
    if (glErr != GL_NO_ERROR) {
#ifdef DEBUG
        Console->Print(cf::va("%s (%u): OpenGL error after initialization: %d\n", __FILE__, __LINE__, glErr));
#endif
    }

    // Print renderer info
#ifdef DEBUG
    Console->Print("Renderer initialized successfully\n");
    Console->Print(cf::va("OpenGL version: %s\n", glGetString(GL_VERSION)));
    Console->Print(cf::va("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION)));
    Console->Print(cf::va("Vendor: %s\n", glGetString(GL_VENDOR)));
    Console->Print(cf::va("Renderer: %s\n", glGetString(GL_RENDERER)));
#endif
}

ArrayT<ShaderT*>& RendererImplT::DGetShaderRepository() {
    return GetShaderRepository();
}

void RendererImplT::Release() {
    // Clean up OpenGL resources
}

void RendererImplT::DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                        const GLchar *message, const void *userParam) {
    if (type == GL_DEBUG_TYPE_ERROR) {
        Console->Print(cf::va("GL ERROR: %s\n", message));
    }
}


const char *RendererImplT::GetDescription() const {
    return "OpenGL 4.6 Renderer";
}


RenderMaterialT *RendererImplT::RegisterMaterial(const MaterialT *Material) const {
    if (Material == NULL) return NULL;
    return new RenderMaterialT(Material);
}


const MaterialT *RendererImplT::GetMaterialFromRM(MatSys::RenderMaterialT *RenderMaterial) const {
    if (RenderMaterial == NULL) return NULL;

    return RenderMaterial->Material;
}


unsigned long RendererImplT::GetAmbientShaderIDFromRM(MatSys::RenderMaterialT *RenderMaterial) const {
    if (RenderMaterial == NULL) return 0;

    for (unsigned long ShaderNr = 0; ShaderNr < GetShaderRepository().Size(); ShaderNr++)
        if (RenderMaterial->AmbientShader == GetShaderRepository()[ShaderNr])
            return ShaderNr;

    return 0xFFFFFFFF; // Should never get here!
}


unsigned long RendererImplT::GetLightShaderIDFromRM(MatSys::RenderMaterialT *RenderMaterial) const {
    if (RenderMaterial == NULL) return 0;

    for (unsigned long ShaderNr = 0; ShaderNr < GetShaderRepository().Size(); ShaderNr++)
        if (RenderMaterial->LightShader == GetShaderRepository()[ShaderNr])
            return ShaderNr;

    return 0xFFFFFFFF; // Should never get here!
}


void RendererImplT::FreeMaterial(RenderMaterialT *RenderMaterial) {
    // Trying to delete the currently active material? Deactivate it first.
    if (RenderMaterial == CurrentRenderMaterial) {
        LockCurrentMaterial(false);
        SetCurrentMaterial(NULL);
    }

    // The user code cannot simply delete directly, because the pointer is probably from a different heap (the "exe/dll boundary").
    // (And of course it could not do easily the above check.)
    delete RenderMaterial;
}


void RendererImplT::BeginFrame(double Time) {
    ExpressionSymbols.Time = float(Time);

    OpenGLStateT::GetInstance()->ColorMask(true, true, true, true);
    OpenGLStateT::GetInstance()->DepthMask(true);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void RendererImplT::PreCache() {
    // Pre-compile and cache shaders
    if (CurrentShader != NULL) CurrentShader->Deactivate();
    for (unsigned long ShaderNr = 0; ShaderNr < GetShaderRepository().Size(); ShaderNr++) {
        GetShaderRepository()[ShaderNr]->Activate();
        GetShaderRepository()[ShaderNr]->Deactivate();
    }
    if (CurrentShader != NULL) CurrentShader->Activate();

    // Set up OpenGL state for precaching
    OpenGLStateT::GetInstance()->Disable(GL_BLEND);
    OpenGLStateT::GetInstance()->Disable(GL_ALPHA_TEST);
    OpenGLStateT::GetInstance()->DepthFunc(GL_LEQUAL);
    OpenGLStateT::GetInstance()->ColorMask(true, true, true, true);
    OpenGLStateT::GetInstance()->DepthMask(false);

    // Pre-cache textures
    const ArrayT<TextureMapImplT *> &TexMapRepository = TextureMapManagerImplT::Get().GetTexMapRepository();

    for (unsigned long TexNr = 0; TexNr < TexMapRepository.Size(); TexNr++) {
        unsigned long TexNr_ = TexMapRepository.Size() - TexNr - 1;
        GLuint TexObject = TexMapRepository[TexNr_]->GetOpenGLObject();

        if (dynamic_cast<TextureMap2DT *>(TexMapRepository[TexNr_])) {
            OpenGLStateT::GetInstance()->BindTexture(GL_TEXTURE_2D, TexObject);
        } else if (dynamic_cast<TextureMapCubeT *>(TexMapRepository[TexNr_])) {
            OpenGLStateT::GetInstance()->BindTexture(GL_TEXTURE_CUBE_MAP, TexObject);
        }

#ifdef DEBUG
        GLenum Error=glGetError();

        if (Error!=GL_NO_ERROR)
            Console->Print(cf::va("glGetError()==%i\n", Error));
#endif
    }
}


void RendererImplT::SetCurrentRenderAction(RenderActionT RA) {
    if (RA == STENCILSHADOW) {
        glClear(GL_STENCIL_BUFFER_BIT);
    }

    CurrentRenderAction = RA;
    SetCurrentShader();
}

void RendererImplT::EndFrame() {
}

void RendererImplT::RenderMesh(const MeshT &Mesh) {
    OpenGLStateT::GetInstance()->LoadMatrix(OpenGLStateT::PROJECTION, GetDepRelMatrix(PROJECTION));
    OpenGLStateT::GetInstance()->LoadMatrix(OpenGLStateT::MODELVIEW, GetDepRelMatrixModelView());

    if (CurrentRenderMaterial == NULL || CurrentShader == NULL) return;

    // For OpenGL 4.6, we use vertex array objects and shaders here
    CurrentShader->RenderMesh(Mesh);
}


void RendererImplT::SetGenPurposeRenderingParam(unsigned long Index, float Value) {
    if (Index >= ExpressionSymbols.GenFloat.Size()) return;

    ExpressionSymbols.GenFloat[Index] = Value;
}


void RendererImplT::SetGenPurposeRenderingParam(unsigned long Index, int Value) {
    if (Index >= ExpressionSymbols.GenInt.Size()) return;

    ExpressionSymbols.GenInt[Index] = Value;
}


float RendererImplT::GetGenPurposeRenderingParamF(unsigned long Index) const {
    if (Index >= ExpressionSymbols.GenFloat.Size()) return 0.0;

    return ExpressionSymbols.GenFloat[Index];
}


int RendererImplT::GetGenPurposeRenderingParamI(unsigned long Index) const {
    if (Index >= ExpressionSymbols.GenInt.Size()) return 0;

    return ExpressionSymbols.GenInt[Index];
}


void RendererImplT::SetCurrentAmbientLightColor(float r, float g, float b) {
    ExpressionSymbols.AmbientLightColor[0] = r;
    ExpressionSymbols.AmbientLightColor[1] = g;
    ExpressionSymbols.AmbientLightColor[2] = b;
}


float *RendererImplT::GetCurrentAmbientLightColor() {
    return ExpressionSymbols.AmbientLightColor;
}


const float *RendererImplT::GetCurrentAmbientLightColor() const {
    return ExpressionSymbols.AmbientLightColor;
}


void RendererImplT::SetCurrentLightSourcePosition(float x, float y, float z) {
    CurrentLightingParams.LightSourcePosition[0] = x;
    CurrentLightingParams.LightSourcePosition[1] = y;
    CurrentLightingParams.LightSourcePosition[2] = z;
}


float *RendererImplT::GetCurrentLightSourcePosition() {
    return CurrentLightingParams.LightSourcePosition;
}


const float *RendererImplT::GetCurrentLightSourcePosition() const {
    return CurrentLightingParams.LightSourcePosition;
}


void RendererImplT::SetCurrentLightSourceRadius(float r) {
    CurrentLightingParams.LightSourceRadius = r;
}


float &RendererImplT::GetCurrentLightSourceRadius() {
    return CurrentLightingParams.LightSourceRadius;
}


const float &RendererImplT::GetCurrentLightSourceRadius() const {
    return CurrentLightingParams.LightSourceRadius;
}


void RendererImplT::SetCurrentLightSourceDiffuseColor(float r, float g, float b) {
    CurrentLightingParams.LightSourceDiffuseColor[0] = r;
    CurrentLightingParams.LightSourceDiffuseColor[1] = g;
    CurrentLightingParams.LightSourceDiffuseColor[2] = b;
}


float *RendererImplT::GetCurrentLightSourceDiffuseColor() {
    return CurrentLightingParams.LightSourceDiffuseColor;
}


const float *RendererImplT::GetCurrentLightSourceDiffuseColor() const {
    return CurrentLightingParams.LightSourceDiffuseColor;
}


void RendererImplT::SetCurrentLightSourceSpecularColor(float r, float g, float b) {
    CurrentLightingParams.LightSourceSpecularColor[0] = r;
    CurrentLightingParams.LightSourceSpecularColor[1] = g;
    CurrentLightingParams.LightSourceSpecularColor[2] = b;
}


float *RendererImplT::GetCurrentLightSourceSpecularColor() {
    return CurrentLightingParams.LightSourceSpecularColor;
}


const float *RendererImplT::GetCurrentLightSourceSpecularColor() const {
    return CurrentLightingParams.LightSourceSpecularColor;
}


void RendererImplT::SetCurrentEyePosition(float x, float y, float z) {
    CurrentLightingParams.EyePosition[0] = x;
    CurrentLightingParams.EyePosition[1] = y;
    CurrentLightingParams.EyePosition[2] = z;
}


float *RendererImplT::GetCurrentEyePosition() {
    return CurrentLightingParams.EyePosition;
}


const float *RendererImplT::GetCurrentEyePosition() const {
    return CurrentLightingParams.EyePosition;
}


void RendererImplT::PushLightingParameters() {
    LightingParamsStack.PushBack(CurrentLightingParams);
}


void RendererImplT::PopLightingParameters() {
    if (LightingParamsStack.Size() > 0) {
        CurrentLightingParams = LightingParamsStack[LightingParamsStack.Size() - 1];
        LightingParamsStack.DeleteBack();
    }
}

RendererImplT::RenderActionT RendererImplT::GetCurrentRenderAction() const {
    return CurrentRenderAction;
}

void RendererImplT::ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}


void RendererImplT::Flush() {
    glFlush();
}


/**************************************************************************************************************/
/*** BEGIN of matrix-related code.  This code happens to be common across all OpenGL-based renderers.       ***/
/**************************************************************************************************************/

const MatrixT &RendererImplT::GetMatrix(MatrixNameT MN) const {
    // Matrix[MN].Update();

    return Matrix[MN].Matrix;
}


const MatrixT &RendererImplT::GetMatrixInv(MatrixNameT MN) const {
    MatrixInv[MN].Update();

    return MatrixInv[MN].Matrix;
}


const MatrixT &RendererImplT::GetMatrixModelView() const {
    MatrixModelView.Update();

    return MatrixModelView.Matrix;
}


const DepRelMatrixT &RendererImplT::GetDepRelMatrix(MatrixNameT MN) const {
    // Matrix[MN].Update();

    return Matrix[MN];
}


const DepRelMatrixT &RendererImplT::GetDepRelMatrixInv(MatrixNameT MN) const {
    MatrixInv[MN].Update();

    return MatrixInv[MN];
}


const DepRelMatrixT &RendererImplT::GetDepRelMatrixModelView() const {
    MatrixModelView.Update();

    return MatrixModelView;
}


void RendererImplT::SetMatrix(MatrixNameT MN, const MatrixT &Matrix_) {
    Matrix[MN].Matrix = Matrix_;
    Matrix[MN].Age++;
}


void RendererImplT::Translate(MatrixNameT MN, float x, float y, float z) {
    Matrix[MN].Matrix.Translate_MT(x, y, z);
    Matrix[MN].Age++;
}


void RendererImplT::Scale(MatrixNameT MN, float scale) {
    Matrix[MN].Matrix.Scale_MS(scale, scale, scale);
    Matrix[MN].Age++;
}


void RendererImplT::RotateX(MatrixNameT MN, float angle) {
    Matrix[MN].Matrix.RotateX_MR(angle);
    Matrix[MN].Age++;
}


void RendererImplT::RotateY(MatrixNameT MN, float angle) {
    Matrix[MN].Matrix.RotateY_MR(angle);
    Matrix[MN].Age++;
}


void RendererImplT::RotateZ(MatrixNameT MN, float angle) {
    Matrix[MN].Matrix.RotateZ_MR(angle);
    Matrix[MN].Age++;
}


void RendererImplT::PushMatrix(MatrixNameT MN) {
    MatrixStack[MN].PushBack(Matrix[MN].Matrix);
}


void RendererImplT::PopMatrix(MatrixNameT MN) {
    const unsigned long StackSize = MatrixStack[MN].Size();

    if (StackSize > 0) {
        Matrix[MN].Matrix = MatrixStack[MN][StackSize - 1];
        MatrixStack[MN].DeleteBack();
        Matrix[MN].Age++;
    }
}

/************************************************************************************************************/
/*** END of matrix-related code.  This code happens to be common across all OpenGL-based renderers.       ***/
/************************************************************************************************************/


void RendererImplT::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}


void RendererImplT::GetViewport(int viewport[4]) {
    glGetIntegerv(GL_VIEWPORT, viewport);
}


void RendererImplT::SetSelectionBuffer(unsigned long Size, unsigned int *Buffer) {
    glSelectBuffer(Size, Buffer);
}


unsigned long RendererImplT::SetPickingRenderMode(PickingRenderModeT PRM) {
    switch (PRM) {
        case RENDER: return glRenderMode(GL_RENDER);
        case SELECT: return glRenderMode(GL_SELECT);
        default: return glRenderMode(GL_FEEDBACK);
    }
}


void RendererImplT::InitNameStack() {
    glInitNames();
}


void RendererImplT::LoadName(unsigned long Name) {
    glLoadName(Name);
}


void RendererImplT::PushName(unsigned long Name) {
    glPushName(Name);
}


void RendererImplT::PopName() {
    glPopName();
}


void RendererImplT::SetCurrentShader() {
    if (CurrentRenderMaterial == NULL) return;

    switch (CurrentRenderAction) {
        case AMBIENT:
            if (CurrentShader != CurrentRenderMaterial->AmbientShader) {
                if (CurrentShader != NULL) CurrentShader->Deactivate();
                CurrentShader = CurrentRenderMaterial->AmbientShader;
                if (CurrentShader != NULL) CurrentShader->Activate();
                // else Warning("CurrentShader==NULL");
            }
            break;

        case STENCILSHADOW:
            if (CurrentShader != GetStencilShadowVolumesShader()) {
                if (CurrentShader != NULL) CurrentShader->Deactivate();
                CurrentShader = GetStencilShadowVolumesShader();
                if (CurrentShader != NULL) CurrentShader->Activate();
                // else Warning("CurrentShader==NULL");
            }
            break;

        case LIGHTING:
            if (CurrentShader != CurrentRenderMaterial->LightShader) {
                if (CurrentShader != NULL) CurrentShader->Deactivate();
                CurrentShader = CurrentRenderMaterial->LightShader;
                if (CurrentShader != NULL) CurrentShader->Activate();
                // else Warning("CurrentShader==NULL");
            }
            break;
    }
}


void RendererImplT::SetCurrentMaterial(RenderMaterialT *RenderMaterial) {
    // Only do this if the current render material has not been locked.
    if (LockCurrentRM) return;

    CurrentRenderMaterial = RenderMaterial;

    // Based on the CurrentRenderAction and the CurrentRenderMaterial, set the CurrentShader.
    SetCurrentShader();
}


MatSys::RenderMaterialT *RendererImplT::GetCurrentMaterial() const {
    return GetCurrentRenderMaterial();
}


void RendererImplT::LockCurrentMaterial(bool LockCM) {
    LockCurrentRM = LockCM;
}


void RendererImplT::SetCurrentLightMap(TextureMapI *LightMap) {
    CurrentLightMap = (TextureMap2DT *) LightMap;
}


void RendererImplT::SetCurrentLightDirMap(TextureMapI * /*LightDirMap*/) {
    // We never actually use the LightDirMaps in this renderer.
    // CurrentLightDirMap=(TextureMap2DT*)LightDirMap;
}


void RendererImplT::SetCurrentSHLMaps(const ArrayT<TextureMapI *> &SHLMaps) {
    for (unsigned long SHLMapNr = 0; SHLMapNr < 16; SHLMapNr++)
        CurrentSHLMaps[SHLMapNr] = SHLMapNr < SHLMaps.Size() ? (TextureMap2DT *) SHLMaps[SHLMapNr] : NULL;
}


void RendererImplT::SetCurrentSHLLookupMap(TextureMapI *SHLLookupMap) {
    CurrentSHLLookupMap = (TextureMap2DT *) SHLLookupMap;
}


RenderMaterialT *RendererImplT::GetCurrentRenderMaterial() const {
    return CurrentRenderMaterial;
}


TextureMap2DT *RendererImplT::GetCurrentLightMap() const { return CurrentLightMap; }

TextureMap2DT *RendererImplT::GetCurrentSHLMap(unsigned long Index) const {
    return Index < 16 ? CurrentSHLMaps[Index] : NULL;
}

TextureMap2DT *RendererImplT::GetCurrentSHLLookupMap() const { return CurrentSHLLookupMap; }


unsigned long RendererImplT::GetInitCounter() const {
    return InitCounter;
}
