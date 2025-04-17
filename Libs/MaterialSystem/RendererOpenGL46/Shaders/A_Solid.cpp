/*
Cafu Engine, http://www.cafu.de/
Copyright (c) Carsten Fuchs and other contributors.
This project is licensed under the terms of the MIT license.
*/

/**************/
/*** Shader ***/
/**************/

#if defined(_WIN32) && defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <GL/glew.h>
#include <string>
#include <vector>
#include <MaterialSystem/Common/OpenGLEx.hpp>

#include "../../Common/OpenGLState.hpp"
#include "../RendererImpl.hpp"
#include "../RenderMaterial.hpp"
#include "../Shader.hpp"
#include "../../Mesh.hpp"

using namespace MatSys;

// Structure to store Mesh resources todo: move it to renderer?
struct MeshGLResourcesT {
    GLuint VAO;
    GLuint VBO;
    bool initialized;
};

class Shader_A_Solid : public ShaderT {
private:
    GLuint Program;
    unsigned long InitCounter;
    std::unordered_map<const MeshT *, MeshGLResourcesT> meshResources;

    GLuint CompileShader(GLenum type, const char *source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            printf("Shader compilation error: %s\n", infoLog);
        }
        return shader;
    }

    void Initialize() {
        const char *testVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 ourColor;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";

        const char *testFragmentShaderSource = R"(
    #version 330 core
    in vec3 ourColor;
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
)";

        const char *vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 ModelViewProjMatrix;

void main()
{
    gl_Position = ModelViewProjMatrix * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
    )";

        const char *fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseTexture;

void main()
{
    FragColor = texture(diffuseTexture, TexCoord) * vec4(ourColor, 1.0);
}
    )";

        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint testVertexShader = CompileShader(GL_VERTEX_SHADER, testVertexShaderSource);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        GLuint testFragmentShader = CompileShader(GL_FRAGMENT_SHADER, testFragmentShaderSource);

        Program = glCreateProgram();
        glAttachShader(Program, vertexShader);
        glAttachShader(Program, fragmentShader);
        glLinkProgram(Program);

        GLint success;
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(Program, 512, NULL, infoLog);
            printf("Shader error: %s\n", infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

public:
    Shader_A_Solid() : Program(0), InitCounter(0) {
    }

    void SetupMeshGLResources(const MeshT &mesh) {
        MeshGLResourcesT &resources = meshResources[&mesh];

        if (resources.initialized) return;

        // Создаем VAO и VBO
        glGenVertexArrays(1, &resources.VAO);
        glGenBuffers(1, &resources.VBO);

        glBindVertexArray(resources.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, resources.VBO);

        // Загружаем данные вершин
        glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.Size() * sizeof(MeshT::VertexT), &mesh.Vertices[0], GL_STATIC_DRAW);

        // Настраиваем атрибуты вершин
        // Position (3 doubles + 1 double w)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(MeshT::VertexT),
                              (void *) offsetof(MeshT::VertexT, Origin));

        // Color (4 floats)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT),
                              (void *) offsetof(MeshT::VertexT, Color));

        // TextureCoord (2 floats)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT),
                              (void *) offsetof(MeshT::VertexT, TextureCoord));

        // LightMapCoord (2 floats)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT),
                              (void *) offsetof(MeshT::VertexT, LightMapCoord));

        // SHLMapCoord (2 floats)
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT),
                              (void *) offsetof(MeshT::VertexT, SHLMapCoord));

        // Normal (3 floats)
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT),
                              (void *) offsetof(MeshT::VertexT, Normal));

        // Tangent (3 floats)
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT),
                              (void *) offsetof(MeshT::VertexT, Tangent));

        // Binormal (3 floats)
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT),
                              (void *) offsetof(MeshT::VertexT, BiNormal));

        glBindVertexArray(0);
        resources.initialized = true;
    }

    const std::string &GetName() const {
        static const std::string Name = "A_Solid";
        return Name;
    }

    char CanHandleAmbient(const MaterialT &Material) const override {
        /*if (Material.NoDraw) return 0;
        if (!Material.UseMeshColors) return 0;

        if (!Material.DiffMapComp.IsEmpty()) return 0;
        if (!Material.LightMapComp.IsEmpty()) return 0;
        if (!Material.LumaMapComp.IsEmpty()) return 0;
        if (!Material.NormMapComp.IsEmpty()) return 0;*/

        return static_cast<char>(255);
    }

    char CanHandleLighting(const MaterialT & /*Material*/) const {
        return 0;
    }

    bool CanHandleStencilShadowVolumes() const {
        return false;
    }

    void Activate() {
        if (InitCounter < RendererImplT::GetInstance().GetInitCounter()) {
            Initialize();
            InitCounter = RendererImplT::GetInstance().GetInitCounter();
        }
        glUseProgram(Program);
    }

    void Deactivate() {
        glUseProgram(0);
    }

    bool NeedsNormals() const {
        return false;
    }

    bool NeedsTangentSpace() const {
        return false;
    }

    bool NeedsXYAttrib() const {
        return false;
    }

    void RenderMesh(const MeshT &Mesh) {
        SetupMeshGLResources(Mesh);
        //Activate();
        const RendererImplT &Renderer = RendererImplT::GetInstance();
        RenderMaterialT *RM = Renderer.GetCurrentRenderMaterial();
        const MaterialT &Material = *(RM->Material);
        const ExpressionT::SymbolsT &Sym = Renderer.GetExpressionSymbols();
        OpenGLStateT *OpenGLState = OpenGLStateT::GetInstance();

        // Получаем значения из материала
        const float AlphaTestValue = Material.AlphaTestValue.Evaluate(Sym).GetAsFloat();
        const float RedValue = Material.RedGen.Evaluate(Sym).GetAsFloat();
        const float GreenValue = Material.GreenGen.Evaluate(Sym).GetAsFloat();
        const float BlueValue = Material.BlueGen.Evaluate(Sym).GetAsFloat();
        const float AlphaValue = Material.AlphaGen.Evaluate(Sym).GetAsFloat();

        // Инициализация шейдеров при необходимости
        if (InitCounter < Renderer.GetInitCounter()) {
            Initialize();
            InitCounter = Renderer.GetInitCounter();
        }

        const MatrixT &modelView = Renderer.GetMatrix(RendererI::WORLD_TO_VIEW);
        const MatrixT &projection = Renderer.GetMatrix(RendererI::PROJECTION);
        MatrixT mvp = projection * modelView;

        GLint mvpLoc = glGetUniformLocation(Program, "ModelViewProjMatrix");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp.m[0][0]);

        GLint texLoc = glGetUniformLocation(Program, "diffuseTexture");
        glUniform1i(texLoc, 0);  // Используем texture unit 0
        glActiveTexture(GL_TEXTURE0);
        GLuint textureID = Renderer.GetCurrentMaterial().GetTextureManager().GetTextureID(Material.DiffMapComp);
        glBindTexture(GL_TEXTURE_2D, RM->Material);

        // Создаем и настраиваем VAO/VBO для меша
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Загружаем данные вершин
        std::vector<float> vertexData;
        for (unsigned long i = 0; i < Mesh.Vertices.Size(); ++i) {
            const MeshT::VertexT& vertex = Mesh.Vertices[i];
            vertexData.push_back(static_cast<float>(vertex.Origin[0]));
            vertexData.push_back(static_cast<float>(vertex.Origin[1]));
            vertexData.push_back(static_cast<float>(vertex.Origin[2]));

            vertexData.push_back(vertex.Color[0]);
            vertexData.push_back(vertex.Color[1]);
            vertexData.push_back(vertex.Color[2]);
            vertexData.push_back(vertex.Color[3]);

            vertexData.push_back(vertex.TextureCoord[0]);
            vertexData.push_back(vertex.TextureCoord[1]);
        }

        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // Рисуем меш
        glDrawArrays(OpenGLStateT::MeshToOpenGLType[Mesh.Type], 0, Mesh.Vertices.Size());

        // Очищаем ресурсы
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glBindVertexArray(0);
    }
};

static Shader_A_Solid A_Solid;
