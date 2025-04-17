#ifndef CAFU_MATSYSV2_MESH_HPP_INCLUDED
#define CAFU_MATSYSV2_MESH_HPP_INCLUDED

#include <vector>
#include <glm/glm.hpp>

namespace MatSysV2 {
    class Mesh {
    public:
        enum class PrimitiveType {
            Points,
            Lines,
            LineStrip,
            LineLoop,
            Triangles,
            TriangleStrip,
            TriangleFan
        };

        enum class WindingOrder {
            Clockwise,
            CounterClockwise
        };

        struct Vertex {
            glm::vec4 position{0.0f, 0.0f, 0.0f, 1.0f};

            glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

            glm::vec2 texCoord{0.0f, 0.0f};

            glm::vec3 normal{0.0f, 0.0f, 1.0f};

            glm::vec3 tangent{1.0f, 0.0f, 0.0f};
            glm::vec3 bitangent{0.0f, 1.0f, 0.0f};

            glm::vec4 customAttributes{0.0f};

            Vertex(const glm::vec3 &pos = glm::vec3(0.0f),
                   const glm::vec2 &uv = glm::vec2(0.0f),
                   const glm::vec3 &norm = glm::vec3(0.0f, 0.0f, 1.0f))
                : position(pos, 1.0f), texCoord(uv), normal(norm) {
            }
        };

        struct CreateInfo {
            PrimitiveType primitiveType = PrimitiveType::Triangles;
            WindingOrder windingOrder = WindingOrder::CounterClockwise;
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices; // Optional index buffer
        };

        explicit Mesh(const CreateInfo &createInfo)
            : m_primitiveType(createInfo.primitiveType),
              m_windingOrder(createInfo.windingOrder),
              m_vertices(createInfo.vertices),
              m_indices(createInfo.indices) {
        }

        PrimitiveType GetPrimitiveType() const { return m_primitiveType; }
        WindingOrder GetWindingOrder() const { return m_windingOrder; }
        const std::vector<Vertex> &GetVertices() const { return m_vertices; }
        const std::vector<uint32_t> &GetIndices() const { return m_indices; }

        bool IsIndexed() const { return !m_indices.empty(); }

    private:
        PrimitiveType m_primitiveType;
        WindingOrder m_windingOrder;
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
    };
} // namespace MatSysV2

#endif
