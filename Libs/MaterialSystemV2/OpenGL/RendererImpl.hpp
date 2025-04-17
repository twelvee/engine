#ifndef CAFU_MATSYSV2_RENDERER_IMPLEMENTATION_HPP_INCLUDED
#define CAFU_MATSYSV2_RENDERER_IMPLEMENTATION_HPP_INCLUDED

#include <unordered_map>

#include "RenderMaterial.hpp"
#include "../Renderer.hpp"
#include "Math3D/Matrix.hpp"


class ShaderT;
class TextureMap2DT;


class RendererImplT : public MatSysV2::RendererI {
public:
    bool IsSupported() const override;

    void Initialize() override;

    void Release() override;

    const char *GetDescription() const override;

    MatSysV2::RenderMaterialT *RegisterMaterial(const MaterialT *Material) override;

    void FreeMaterial(MatSysV2::RenderMaterialT *RenderMaterial) override;

    void SetCurrentMaterial(MatSysV2::RenderMaterialT *RenderMaterial) override;

    MatSysV2::RenderMaterialT *GetCurrentMaterial() const override;

    void BeginFrame(double Time) override;

    void EndFrame() override;

    void ClearColor(float r, float g, float b, float a) override;

    void Flush() override;

    void SetCurrentRenderAction(RenderActionT RA) override;

    RenderActionT GetCurrentRenderAction() const override;

    void SetCurrentAmbientLightColor(float r, float g, float b) override;

    const float *GetCurrentAmbientLightColor() const override;

    void SetCurrentLightSourcePosition(float x, float y, float z) override;

    const float *GetCurrentLightSourcePosition() const override;

    void SetCurrentLightSourceRadius(float r) override;

    float GetCurrentLightSourceRadius() const override;

    void SetMatrix(MatrixNameT MN, const glm::mat4 &Matrix) override;

    const glm::mat4 &GetMatrix(MatrixNameT MN) const override;

    void SetViewport(int x, int y, int width, int height) override;

    void GetViewport(int viewport[4]) override;

    void RenderMesh(const MatSysV2::MeshT &Mesh) override;

private:
    RenderActionT CurrentRenderAction = AMBIENT;
    MatSysV2::RenderMaterialT *CurrentMaterial = nullptr;

    glm::vec3 AmbientLightColor = {0.2f, 0.2f, 0.2f};
    glm::vec3 LightSourcePosition = {0.0f, 0.0f, 0.0f};
    float LightSourceRadius = 10.0f;

    glm::mat4 ModelToWorld = glm::mat4(1.0f);
    glm::mat4 WorldToView = glm::mat4(1.0f);
    glm::mat4 Projection = glm::mat4(1.0f);

    int Viewport[4] = {0, 0, 800, 600};

    // Кэш шейдеров и материалов
    std::unordered_map<const MaterialT *, std::unique_ptr<MatSysV2::RenderMaterialT> > Materials;
};

#endif
