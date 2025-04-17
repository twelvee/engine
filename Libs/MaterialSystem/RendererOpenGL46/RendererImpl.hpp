/*
Cafu Engine, http://www.cafu.de/
Copyright (c) Carsten Fuchs and other contributors.
This project is licensed under the terms of the MIT license.
*/

/*******************************/
/*** Renderer Implementation ***/
/*******************************/

#ifndef CAFU_MATSYS_RENDERER_IMPLEMENTATION_HPP_INCLUDED
#define CAFU_MATSYS_RENDERER_IMPLEMENTATION_HPP_INCLUDED

#include <ConsoleCommands/Console.hpp>

#include "../Renderer.hpp"
#include "Templates/Array.hpp"
#include "Math3D/Matrix.hpp"
#include "../Common/DepRelMatrix.hpp"
#include "../Expression.hpp"


class ShaderT;
class TextureMap2DT;


class RendererImplT : public MatSys::RendererI {
public:
    // RendererI implementation.
    bool IsSupported() const override;

    bool DoesSupportCompressedSHL() const override;

    bool DoesSupportUncompressedSHL() const override;

    int GetPreferenceNr() const override;

    void Initialize() override;

    void Release() override;

    static void DebugOutputCallback(GLenum source, GLenum type, GLuint id,
                                        GLenum severity, GLsizei length,
                                        const GLchar* message, const void* userParam);

    ArrayT<ShaderT*>& DGetShaderRepository() override;

    const char *GetDescription() const override;

    // OBSOLETE void SetBaseDir(const std::string& BaseDir_);
    // OBSOLETE const std::string& GetBaseDir();
    MatSys::RenderMaterialT *RegisterMaterial(const MaterialT *Material) const override;

    const MaterialT *GetMaterialFromRM(MatSys::RenderMaterialT *RenderMaterial) const override;

    unsigned long GetAmbientShaderIDFromRM(MatSys::RenderMaterialT *RenderMaterial) const override;

    unsigned long GetLightShaderIDFromRM(MatSys::RenderMaterialT *RenderMaterial) const override;

    void FreeMaterial(MatSys::RenderMaterialT *RenderMaterial) override;

    void BeginFrame(double Time) override;

    void EndFrame() override;

    void PreCache() override;

    void SetCurrentRenderAction(RenderActionT RA) override;

    RenderActionT GetCurrentRenderAction() const override;

    void SetGenPurposeRenderingParam(unsigned long Index, float Value) override;

    void SetGenPurposeRenderingParam(unsigned long Index, int Value) override;

    float GetGenPurposeRenderingParamF(unsigned long Index) const override;

    int GetGenPurposeRenderingParamI(unsigned long Index) const override;

    void SetCurrentAmbientLightColor(float r, float g, float b) override;

    float *GetCurrentAmbientLightColor() override;

    const float *GetCurrentAmbientLightColor() const override;

    void SetCurrentLightSourcePosition(float x, float y, float z) override;

    float *GetCurrentLightSourcePosition() override;

    const float *GetCurrentLightSourcePosition() const override;

    void SetCurrentLightSourceRadius(float r) override;

    float &GetCurrentLightSourceRadius() override;

    const float &GetCurrentLightSourceRadius() const override;

    void SetCurrentLightSourceDiffuseColor(float r, float g, float b) override;

    float *GetCurrentLightSourceDiffuseColor() override;

    const float *GetCurrentLightSourceDiffuseColor() const override;

    void SetCurrentLightSourceSpecularColor(float r, float g, float b) override;

    float *GetCurrentLightSourceSpecularColor() override;

    const float *GetCurrentLightSourceSpecularColor() const override;

    void SetCurrentEyePosition(float x, float y, float z) override;

    float *GetCurrentEyePosition() override;

    const float *GetCurrentEyePosition() const override;

    void PushLightingParameters() override;

    void PopLightingParameters() override;

    void ClearColor(float r, float g, float b, float a) override;

    void Flush() override;

    const MatrixT &GetMatrix(MatrixNameT MN) const override;

    const MatrixT &GetMatrixInv(MatrixNameT MN) const override;

    const MatrixT &GetMatrixModelView() const override;

    void SetMatrix(MatrixNameT MN, const MatrixT &Matrix) override;

    void Translate(MatrixNameT MN, float x, float y, float z) override;

    void Scale(MatrixNameT MN, float scale) override;

    void RotateX(MatrixNameT MN, float angle) override;

    void RotateY(MatrixNameT MN, float angle) override;

    void RotateZ(MatrixNameT MN, float angle) override;

    void PushMatrix(MatrixNameT MN) override;

    void PopMatrix(MatrixNameT MN) override;

    void SetViewport(int x, int y, int width, int height) override;

    void GetViewport(int viewport[4]) override;

    void SetSelectionBuffer(unsigned long Size, unsigned int *Buffer) override;

    unsigned long SetPickingRenderMode(PickingRenderModeT PRM) override;

    void InitNameStack() override;

    void LoadName(unsigned long Name) override;

    void PushName(unsigned long Name) override;

    void PopName() override;

    void SetCurrentMaterial(MatSys::RenderMaterialT *RenderMaterial) override;

    MatSys::RenderMaterialT *GetCurrentMaterial() const override;

    void LockCurrentMaterial(bool LockCM) override;

    void SetCurrentLightMap(MatSys::TextureMapI *LightMap) override;

    void SetCurrentLightDirMap(MatSys::TextureMapI *LightDirMap) override;

    void SetCurrentSHLMaps(const ArrayT<MatSys::TextureMapI *> &SHLMaps) override;

    void SetCurrentSHLLookupMap(MatSys::TextureMapI *SHLLookupMap) override;

    void RenderMesh(const MatSys::MeshT &Mesh) override;


    // Internal interface.
    MatSys::RenderMaterialT *GetCurrentRenderMaterial() const;

    TextureMap2DT *GetCurrentLightMap() const;

    TextureMap2DT *GetCurrentSHLMap(unsigned long Index) const;

    TextureMap2DT *GetCurrentSHLLookupMap() const;

    unsigned long GetInitCounter() const;

    const DepRelMatrixT &GetDepRelMatrix(MatrixNameT MN) const;

    const DepRelMatrixT &GetDepRelMatrixInv(MatrixNameT MN) const;

    const DepRelMatrixT &GetDepRelMatrixModelView() const;

    const ExpressionT::SymbolsT &GetExpressionSymbols() const { return ExpressionSymbols; }

    static RendererImplT &GetInstance();

private:
    struct LightingParamsT {
        float LightSourcePosition[3];
        float LightSourceRadius;
        float LightSourceDiffuseColor[3];
        float LightSourceSpecularColor[3];
        float EyePosition[3];
    };

    /// Based on the CurrentRenderAction and the CurrentRenderMaterial, set the CurrentShader.
    /// Called from SetCurrentRenderAction() and SetCurrentMaterial().
    void SetCurrentShader();

    // std::string              BaseDir;
    RenderActionT CurrentRenderAction;
    MatSys::RenderMaterialT *CurrentRenderMaterial;
    bool LockCurrentRM;
    ShaderT *CurrentShader;
    TextureMap2DT *CurrentLightMap;
    TextureMap2DT *CurrentSHLMaps[16];
    TextureMap2DT *CurrentSHLLookupMap;
    LightingParamsT CurrentLightingParams;
    ArrayT<LightingParamsT> LightingParamsStack;

    GLuint m_defaultVAO;
    GLuint m_screenQuadVAO;
    GLuint m_screenQuadVBO;
    GLuint m_matrixUBO;
    unsigned long InitCounter;


    // The original source/parent matrices. Each matrix has an associated age, and they grow older whenever they are modified.
    DepRelMatrixT Matrix[END_MARKER];
    ///< The current MODEL_TO_WORLD, WORLD_TO_VIEW and PROJECITON matrices. All other matrices eventually depend on these.
    ArrayT<MatrixT> MatrixStack[END_MARKER]; ///< The storage for the matrix stack.

    // Dependent matrices. They all depend in some way or another on the elements of the Matrix array above.
    // A dependent matrix is updated whenever its source matrix (or one of its source matrices) has grown older
    // than it was when the dependent matrix was last created.
    mutable InverseMatrixT MatrixInv[END_MARKER];
    ///< The inverses of the above matrices. MatrixInv[MN] depends on Matrix[MN].
    mutable ProductMatrixT MatrixModelView;
    ///< The model-to-view matrix. It depends on the Matrix[MODEL_TO_WORLD] and Matrix[WORLD_TO_VIEW] matrices.

    /// This is where the symbol values for material expressions are stored.
    ExpressionT::SymbolsT ExpressionSymbols;


    /// Private constructor for the Singleton pattern.
    RendererImplT();
};

#endif
