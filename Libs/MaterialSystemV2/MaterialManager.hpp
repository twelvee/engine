#ifndef CAFU_MATSYSV2_MATERIAL_MANAGER_INTERFACE_HPP_INCLUDED
#define CAFU_MATSYSV2_MATERIAL_MANAGER_INTERFACE_HPP_INCLUDED

#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <memory>

class MaterialT;

namespace fs = std::filesystem;

class MaterialManagerI {
public:
    virtual ~MaterialManagerI() = default;

    // ========== Material Registration ==========

    /**
     * Registers a single material instance
     * @param mat Material to register
     * @return Pointer to registered material (existing if name conflict)
     */
    virtual MaterialT *RegisterMaterial(const MaterialT &mat) = 0;

    /**
     * Loads materials from a JSON file
     * @param filePath Path to JSON material file
     * @return List of newly registered materials
     */
    virtual std::vector<MaterialT *> RegisterMaterialFromFile(const fs::path &filePath) = 0;

    /**
     * Scans directory for JSON material files
     * @param dirPath Directory to scan
     * @param recursive Whether to scan subdirectories
     * @return List of newly registered materials
     */
    virtual std::vector<MaterialT *> RegisterMaterialsFromDir(
        const fs::path &dirPath,
        bool recursive = true
    ) = 0;

    // ========== Material Access ==========

    /**
     * Gets all registered materials
     * @return Map of material name to material pointer
     */
    virtual const std::unordered_map<std::string, MaterialT *> &GetAllMaterials() const = 0;

    /**
     * Checks if material exists
     * @param name Material name to check
     * @return True if material exists
     */
    virtual bool HasMaterial(const std::string &name) const = 0;

    /**
     * Gets material by name
     * @param name Material name to find
     * @return Pointer to material or nullptr if not found
     */
    virtual MaterialT *GetMaterial(const std::string &name) const = 0;

    // ========== Utility Methods ==========

    /**
     * Gets material or returns fallback if not found
     * @param name Material name to find
     * @param fallback Fallback material name
     * @return Found material or fallback (throws if both not found)
     */
    virtual MaterialT *GetMaterialOrDefault(
        const std::string &name,
        const std::string &fallback = "default"
    ) const = 0;
};

extern std::unique_ptr<MaterialManagerI> MaterialManager;

#endif
