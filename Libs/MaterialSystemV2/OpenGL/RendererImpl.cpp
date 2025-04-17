#include "RendererImpl.hpp"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

// ==============================================
// Core Renderer Functions
// ==============================================

bool RendererImplT::IsSupported() const {
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    return (major >= 4 && minor >= 6);
}

void RendererImplT::Initialize() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    glEnable(GL_DEPTH_TEST);
}

void RendererImplT::Release() {
    Materials.clear();
}

const char* RendererImplT::GetDescription() const {
    return "OpenGL 4.6 Renderer (PBR)";
}

// ==============================================
// Material Management
// ==============================================

MatSysV2::RenderMaterialT* RendererImplT::RegisterMaterial(const MaterialT* Material) {
    if (!Material) return nullptr;

    auto it = Materials.find(Material);
    if (it != Materials.end()) {
        return it->second.get();
    }

    auto renderMat = std::make_unique<MatSysV2::RenderMaterialT>(Material);
    auto* rawPtr = renderMat.get();
    Materials[Material] = std::move(renderMat);
    return rawPtr;
}

void RendererImplT::FreeMaterial(MatSysV2::RenderMaterialT* RenderMaterial) {
    if (!RenderMaterial) return;

    if (CurrentMaterial == RenderMaterial) {
        CurrentMaterial = nullptr;
    }

    for (auto it = Materials.begin(); it != Materials.end(); ++it) {
        if (it->second.get() == RenderMaterial) {
            Materials.erase(it);
            break;
        }
    }
}

void RendererImplT::SetCurrentMaterial(MatSysV2::RenderMaterialT* RenderMaterial) {
    CurrentMaterial = RenderMaterial;
    if (RenderMaterial) {
        // todo: set the shader
    }
}

MatSysV2::RenderMaterialT* RendererImplT::GetCurrentMaterial() const {
    return CurrentMaterial;
}

// ==============================================
// Frame Control
// ==============================================

void RendererImplT::BeginFrame(double Time) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererImplT::EndFrame() {
    // swapbuffers
}

void RendererImplT::ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void RendererImplT::Flush() {
    glFlush();
}

// ==============================================
// Lighting State
// ==============================================

void RendererImplT::SetCurrentRenderAction(RenderActionT RA) {
    CurrentRenderAction = RA;
}

MatSysV2::RendererI::RenderActionT RendererImplT::GetCurrentRenderAction() const {
    return CurrentRenderAction;
}

void RendererImplT::SetCurrentAmbientLightColor(float r, float g, float b) {
    AmbientLightColor = {r, g, b};
}

const float* RendererImplT::GetCurrentAmbientLightColor() const {
    return glm::value_ptr(AmbientLightColor);
}

void RendererImplT::SetCurrentLightSourcePosition(float x, float y, float z) {
    LightSourcePosition = {x, y, z};
}

const float* RendererImplT::GetCurrentLightSourcePosition() const {
    return glm::value_ptr(LightSourcePosition);
}

void RendererImplT::SetCurrentLightSourceRadius(float r) {
    LightSourceRadius = r;
}

float RendererImplT::GetCurrentLightSourceRadius() const {
    return LightSourceRadius;
}

// ==============================================
// Matrix Operations
// ==============================================

void RendererImplT::SetMatrix(MatrixNameT MN, const glm::mat4& Matrix) {
    switch (MN) {
        case MODEL_TO_WORLD: ModelToWorld = Matrix; break;
        case WORLD_TO_VIEW: WorldToView = Matrix; break;
        case PROJECTION: Projection = Matrix; break;
        default: break;
    }
}

const glm::mat4& RendererImplT::GetMatrix(MatrixNameT MN) const {
    switch (MN) {
        case MODEL_TO_WORLD: return ModelToWorld;
        case WORLD_TO_VIEW: return WorldToView;
        case PROJECTION: return Projection;
        default: return ModelToWorld; // Fallback
    }
}

// ==============================================
// Viewport
// ==============================================

void RendererImplT::SetViewport(int x, int y, int width, int height) {
    Viewport[0] = x;
    Viewport[1] = y;
    Viewport[2] = width;
    Viewport[3] = height;
    glViewport(x, y, width, height);
}

void RendererImplT::GetViewport(int viewport[4]) {
    memcpy(viewport, Viewport, sizeof(int) * 4);
}

// ==============================================
// Mesh Rendering
// ==============================================

void RendererImplT::RenderMesh(const MatSysV2::MeshT& Mesh) {
    if (!CurrentMaterial) return;
    // todo: get material shader and render with selected shader (use shader repository?)
}