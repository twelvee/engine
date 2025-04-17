#ifndef CAFU_MATSYSV2_MATERIAL_MANAGER_IMPLEMENTATION_HPP_INCLUDED
#define CAFU_MATSYSV2_MATERIAL_MANAGER_IMPLEMENTATION_HPP_INCLUDED

#include "MaterialManager.hpp"
#include "Material.hpp"
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

class MaterialManagerImplT : public MaterialManagerI {
public:
    MaterialManagerImplT();

    ~MaterialManagerImplT();

    // Основные методы
    MaterialT *RegisterMaterial(const MaterialT &Mat) override;

    std::vector<MaterialT *> RegisterMaterialFromFile(const fs::path &filePath) override;

    std::vector<MaterialT *> RegisterMaterialsFromDir(const fs::path &dirPath, bool recursive = true) override;

    const std::unordered_map<std::string, MaterialT *> &GetAllMaterials() const override { return Materials; }

    bool HasMaterial(const std::string &name) const override;

    MaterialT *GetMaterial(const std::string &name) const override;

private:
    std::unordered_map<std::string, MaterialT *> Materials;
    std::vector<fs::path> LoadedMaterialFiles;

    // Запрет копирования
    MaterialManagerImplT(const MaterialManagerImplT &) = delete;

    void operator=(const MaterialManagerImplT &) = delete;
};

#endif
