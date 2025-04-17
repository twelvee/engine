#ifndef CAFU_MATSYSV2_RENDERER_HPP_INCLUDED
#define CAFU_MATSYSV2_RENDERER_HPP_INCLUDED

#include <glm/glm.hpp>

class ShaderT;
template<class T>
class ArrayT;
class MaterialT;
template<class T>
class Matrix4x4T;
typedef Matrix4x4T<float> Matrix4x4fT;


namespace MatSysV2 {
    class MeshT;
    class RenderMaterialT;

    class RendererI {
    public:
        enum RenderActionT { AMBIENT, STENCILSHADOW, LIGHTING };

        // --- Core Renderer Setup ---
        virtual bool IsSupported() const = 0;

        virtual void Initialize() = 0;

        virtual void Release() = 0;

        virtual const char *GetDescription() const = 0;

        // --- Material Management ---
        virtual RenderMaterialT *RegisterMaterial(const MaterialT *Material) = 0;

        virtual void FreeMaterial(RenderMaterialT *RenderMaterial) = 0;

        virtual void SetCurrentMaterial(RenderMaterialT *RenderMaterial) = 0;

        virtual RenderMaterialT *GetCurrentMaterial() const = 0;

        // --- Frame Control ---
        virtual void BeginFrame(double Time) = 0;

        virtual void EndFrame() = 0;

        virtual void ClearColor(float r, float g, float b, float a) = 0;

        virtual void Flush() = 0;

        // --- Lighting ---
        virtual void SetCurrentRenderAction(RenderActionT RA) = 0;

        virtual RenderActionT GetCurrentRenderAction() const = 0;

        virtual void SetCurrentAmbientLightColor(float r, float g, float b) = 0;

        virtual const float *GetCurrentAmbientLightColor() const = 0;

        virtual void SetCurrentLightSourcePosition(float x, float y, float z) = 0;

        virtual const float *GetCurrentLightSourcePosition() const = 0;

        virtual void SetCurrentLightSourceRadius(float r) = 0;

        virtual float GetCurrentLightSourceRadius() const = 0;

        // --- Matrices ---
        enum MatrixNameT { MODEL_TO_WORLD, WORLD_TO_VIEW, PROJECTION };

        virtual void SetMatrix(MatrixNameT MN, const glm::mat4 &Matrix) = 0;

        virtual const glm::mat4 &GetMatrix(MatrixNameT MN) const = 0;

        // --- Viewport ---
        virtual void SetViewport(int x, int y, int width, int height) = 0;

        virtual void GetViewport(int viewport[4]) = 0;

        // --- Geometry Rendering ---
        virtual void RenderMesh(const MeshT &Mesh) = 0;

        virtual ~RendererI() = default;
    };


    extern RendererI *Renderer;
}

#endif
