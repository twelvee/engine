#ifndef CAFU_MATSYSV2_MATERIAL_HPP_INCLUDED
#define CAFU_MATSYSV2_MATERIAL_HPP_INCLUDED

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

// Modern PBR material definition (JSON-based)
class MaterialT {
public:
    // --- PBR Material Properties ---
    struct TextureMaps {
        std::string Albedo;
        std::string Normal;
        std::string Metallic;
        std::string Roughness;
        std::string AmbientOcclusion;
        std::string Emissive;
    };

    // --- Surface Properties ---
    enum SurfaceType {
        ST_None, ST_Stone, ST_Metal,
        ST_Sand, ST_Wood, ST_Liquid,
        ST_Glass, ST_Plastic
    };

    // --- Material Data ---
    std::string Name;
    TextureMaps Maps;

    glm::vec3 AlbedoColor = glm::vec3(1.0f);
    float MetallicFactor = 0.0f;
    float RoughnessFactor = 1.0f;
    glm::vec3 EmissiveColor = glm::vec3(0.0f);

    SurfaceType Surface = ST_None;
    bool TwoSided = false;
    bool IsTransparent = false;
    float AlphaCutoff = 0.5f;

    // --- Physics/Collision ---
    struct PhysicsProperties {
        bool BlocksMovement = true;
        bool IsLadder = false;
        bool IsTrigger = false;
    } Physics;

    // --- Constructors ---
    MaterialT() = default;

    MaterialT(const std::string &jsonPath) {
        // todo: remove it, rely only on material manager
        LoadFromJSON(jsonPath);
    }

    // Serialize to JSON
    json ToJSON() const {
        return {
            {"name", Name},
            {"albedo_color", {AlbedoColor.r, AlbedoColor.g, AlbedoColor.b}},
            {"metallic", MetallicFactor},
            {"roughness", RoughnessFactor},
            {
                "textures", {
                    {"albedo", Maps.Albedo},
                    {"normal", Maps.Normal},
                    {"metallic", Maps.Metallic},
                    {"roughness", Maps.Roughness}
                }
            },
            {
                "physics", {
                    {"blocks_movement", Physics.BlocksMovement},
                    {"is_ladder", Physics.IsLadder}
                }
            }
        };
    }

    // Load from JSON file
    void LoadFromJSON(const std::string &path) {
        std::ifstream file(path);
        json data = json::parse(file);

        Name = data.value("name", "unnamed");
        AlbedoColor = glm::vec3(
            data["albedo_color"][0],
            data["albedo_color"][1],
            data["albedo_color"][2]
        );

        Maps.Albedo = data["textures"].value("albedo", "");
        Maps.Normal = data["textures"].value("normal", "");
        Maps.Metallic = data["textures"].value("metallic", "");

        Physics.BlocksMovement = data["physics"].value("blocks_movement", true);
    }

    bool HasNormalMap() const { return !Maps.Normal.empty(); }
    bool IsOpaque() const { return !IsTransparent; }
};
#endif
