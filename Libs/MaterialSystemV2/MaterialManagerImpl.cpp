#include "MaterialManagerImpl.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

MaterialManagerImplT::MaterialManagerImplT() = default;

MaterialManagerImplT::~MaterialManagerImplT() {
    for (auto &[name, mat]: Materials) {
        delete mat;
    }
}

MaterialT *MaterialManagerImplT::RegisterMaterial(const MaterialT &Mat) {
    auto it = Materials.find(Mat.Name);
    if (it != Materials.end()) {
        return it->second;
    }

    MaterialT *newMat = new MaterialT(Mat);
    Materials[Mat.Name] = newMat;
    return newMat;
}

std::vector<MaterialT *> MaterialManagerImplT::RegisterMaterialFromFile(const fs::path &filePath) {
    std::vector<MaterialT *> newMaterials;

    if (std::find(LoadedMaterialFiles.begin(), LoadedMaterialFiles.end(), filePath) != LoadedMaterialFiles.end()) {
        return newMaterials;
    }

    try {
        std::ifstream file(filePath);
        json data = json::parse(file);

        if (data.is_array()) {
            for (auto &matData: data) {
                MaterialT *mat = new MaterialT();
                mat->Name = matData.value("name", "unnamed");
                if (data.contains("albedo_color")) {
                    auto &albedo = data["albedo_color"];
                    mat->AlbedoColor = glm::vec3(
                        albedo[0].get<float>(),
                        albedo[1].get<float>(),
                        albedo[2].get<float>()
                    );
                }

                mat->MetallicFactor = data.value("metallic", 0.0f);
                mat->RoughnessFactor = data.value("roughness", 1.0f);
                mat->AlphaCutoff = data.value("alpha_cutoff", 0.5f);

                if (data.contains("emissive_color")) {
                    auto &emissive = data["emissive_color"];
                    mat->EmissiveColor = glm::vec3(
                        emissive[0].get<float>(),
                        emissive[1].get<float>(),
                        emissive[2].get<float>()
                    );
                }

                if (data.contains("textures")) {
                    auto &textures = data["textures"];
                    mat->Maps.Albedo = textures.value("albedo", "");
                    mat->Maps.Normal = textures.value("normal", "");
                    mat->Maps.Metallic = textures.value("metallic", "");
                    mat->Maps.Roughness = textures.value("roughness", "");
                    mat->Maps.AmbientOcclusion = textures.value("ao", "");
                    mat->Maps.Emissive = textures.value("emissive", "");
                }

                if (data.contains("physics")) {
                    auto &physics = data["physics"];
                    mat->Physics.BlocksMovement = physics.value("blocks_movement", true);
                    mat->Physics.IsLadder = physics.value("is_ladder", false);
                    mat->Physics.IsTrigger = physics.value("is_trigger", false);
                }

                std::string surfaceType = data.value("surface_type", "none");
                if (surfaceType == "stone") mat->Surface = MaterialT::ST_Stone;
                else if (surfaceType == "metal") mat->Surface = MaterialT::ST_Metal;
                else if (surfaceType == "glass") mat->Surface = MaterialT::ST_Glass;
                else if (surfaceType == "liquid") mat->Surface = MaterialT::ST_Liquid;
                else if (surfaceType == "plastic") mat->Surface = MaterialT::ST_Plastic;
                else if (surfaceType == "sand") mat->Surface = MaterialT::ST_Sand;
                else if (surfaceType == "wood") mat->Surface = MaterialT::ST_Wood;
                else mat->Surface = MaterialT::ST_None;

                mat->TwoSided = data.value("two_sided", false);
                mat->IsTransparent = data.value("transparent", false);

                if (!Materials[mat->Name]) {
                    Materials[mat->Name] = mat;
                    newMaterials.push_back(mat);
                } else {
                    delete mat; // material already exists
                }
            }
        } else {
            MaterialT *mat = new MaterialT();
            mat->Name = data.value("name", "unnamed");

            if (data.contains("albedo_color")) {
                auto &albedo = data["albedo_color"];
                mat->AlbedoColor = glm::vec3(
                    albedo[0].get<float>(),
                    albedo[1].get<float>(),
                    albedo[2].get<float>()
                );
            }

            mat->MetallicFactor = data.value("metallic", 0.0f);
            mat->RoughnessFactor = data.value("roughness", 1.0f);
            mat->AlphaCutoff = data.value("alpha_cutoff", 0.5f);

            if (data.contains("emissive_color")) {
                auto &emissive = data["emissive_color"];
                mat->EmissiveColor = glm::vec3(
                    emissive[0].get<float>(),
                    emissive[1].get<float>(),
                    emissive[2].get<float>()
                );
            }

            if (data.contains("textures")) {
                auto &textures = data["textures"];
                mat->Maps.Albedo = textures.value("albedo", "");
                mat->Maps.Normal = textures.value("normal", "");
                mat->Maps.Metallic = textures.value("metallic", "");
                mat->Maps.Roughness = textures.value("roughness", "");
                mat->Maps.AmbientOcclusion = textures.value("ao", "");
                mat->Maps.Emissive = textures.value("emissive", "");
            }

            if (data.contains("physics")) {
                auto &physics = data["physics"];
                mat->Physics.BlocksMovement = physics.value("blocks_movement", true);
                mat->Physics.IsLadder = physics.value("is_ladder", false);
                mat->Physics.IsTrigger = physics.value("is_trigger", false);
            }

            std::string surfaceType = data.value("surface_type", "none");
            if (surfaceType == "stone") mat->Surface = MaterialT::ST_Stone;
            else if (surfaceType == "metal") mat->Surface = MaterialT::ST_Metal;
            else if (surfaceType == "glass") mat->Surface = MaterialT::ST_Glass;
            else if (surfaceType == "liquid") mat->Surface = MaterialT::ST_Liquid;
            else if (surfaceType == "plastic") mat->Surface = MaterialT::ST_Plastic;
            else if (surfaceType == "sand") mat->Surface = MaterialT::ST_Sand;
            else if (surfaceType == "wood") mat->Surface = MaterialT::ST_Wood;
            else mat->Surface = MaterialT::ST_None;

            mat->TwoSided = data.value("two_sided", false);
            mat->IsTransparent = data.value("transparent", false);

            if (!Materials[mat->Name]) {
                Materials[mat->Name] = mat;
                newMaterials.push_back(mat);
            } else {
                delete mat; // material already exists
            }
        }

        LoadedMaterialFiles.push_back(filePath);
    } catch (const std::exception &e) {
        std::cerr << "Error loading material from " << filePath << ": " << e.what() << std::endl;
    }

    return newMaterials;
}

std::vector<MaterialT *> MaterialManagerImplT::RegisterMaterialsFromDir(const fs::path &dirPath, bool recursive) {
    std::vector<MaterialT *> newMaterials;

    if (!fs::exists(dirPath)) return newMaterials;

    try {
        for (const auto &entry: fs::directory_iterator(dirPath)) {
            if (entry.is_directory() && recursive) {
                auto mats = RegisterMaterialsFromDir(entry.path(), true);
                newMaterials.insert(newMaterials.end(), mats.begin(), mats.end());
            } else if (entry.is_regular_file() && entry.path().extension() == ".json") {
                auto mats = RegisterMaterialFromFile(entry.path());
                newMaterials.insert(newMaterials.end(), mats.begin(), mats.end());
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error scanning directory " << dirPath << ": " << e.what() << std::endl;
    }

    return newMaterials;
}

bool MaterialManagerImplT::HasMaterial(const std::string &name) const {
    return Materials.find(name) != Materials.end();
}

MaterialT *MaterialManagerImplT::GetMaterial(const std::string &name) const {
    auto it = Materials.find(name);
    return it != Materials.end() ? it->second : nullptr;
}
