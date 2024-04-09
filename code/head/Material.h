#pragma once

#include <map>
#include <vector>

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AuxiliaryD3D.h"
#include "MacroDefine.h"
#include "UString.h"

namespace Rendering
{
    template<typename T>
    class AttributeItem
    {
    public:
        AttributeItem()
        {
            useValue = 1;
        }

        T value;        // Template attribute values
        int useValue;   // Directly use value data, otherwise use texture sampling
    };


    // This class will be uploaded to the constant buffer
    class PBR_Material_Data
    {
    public:
        /*
            PBR Material contain AttributeItem��
                1. baseColor    2. normal       3. emissiveColor    4. emissiveIntensity
                5. metallic     6. roughness    7. ambient          8. opacity
            When the above attributes have corresponding textures, they will be sampled from the texture.
            Otherwise, the attributes will be obtained from the material.
            If they are not included in the material, the default values will be used.
            The normal vector is somewhat special. If there is no corresponding map, the normal vector calculated based on the vertices is used as the value data.
        */
        PBR_Material_Data();

        AttributeItem<DirectX::XMFLOAT4> baseColor;
        AttributeItem<DirectX::XMFLOAT3> normal;
        AttributeItem<DirectX::XMFLOAT3> emissiveColor;
        AttributeItem<float> emissiveIntensity;
        AttributeItem<float> metallic;
        AttributeItem<float> roughness;
        AttributeItem<float> ambient;
        AttributeItem<float> opacity;
    };


    class PBR_Material
    {
    public:
        PBR_Material();

        UString name;
        PBR_Material_Data data;     // The data values of PBR material properties and whether data is used

        /*
            Record the index of the corresponding texture when data is not used
                baseColorMap - 0   normalMap    - 1   emssiveColorMap - 2   emssiveIntensityMap - 3
                metallicMap  - 4   roughnessMap - 5   ambientMap      - 6   opacityMap          - 7
        */
        int mapIndex[8];
    };

    class MaterialManager
    {
    public:
        MaterialManager() {}

        /*
            Some meshes may not have a corresponding material, but rendering must specify a material.
            After importing all the materials of the model,
            this function can be called to add a default material at the end of the material array.
        */
        void AddDefaultMaterial();

        int GetDefaultMaterialIndex();

        void SetMaterialReserve(int materialCount);

        /*
            Query whether the map path is loaded into the map.
            If it is recorded, return its index in the map array. Otherwise, return -1
        */
        int FindTexturePathIndex(UString path);

        // Add a texture and return its index in the array
        int AddTextureFilePath(UString path);

        /*
            Load a specific type of texture from a certain material in Assymp.
            For simplicity, the maximum number of textures a material can reference for a specific type is 1,
            so this function returns at most the first texture of a specific type.
        */
        static UString LoadMaterialTextures(aiMaterial* mat, aiTextureType type);

        /*
        Process texture paths in materials,
            1. Add its relative path to the model file path and change it to an absolute path
            2. Unified use of "\\" as a hierarchical division
            3. Change the texture suffix to. dds
        */
        static UString PrcessFilePath(UString modelPath, UString texturePath);

        // Read Assymp material aiMaterial and convert it to PBR_Material
        void AddMaterial(aiMaterial* material, UString modelPath, bool prcessFilePath = true);

        std::vector<UString>& GetTextureFilePaths();
        std::map<UString, int>& GetTextureMap();
        std::vector<PBR_Material>& GetMaterials();

    private:
        int defaultMaterialIndex = -1;

        std::vector<UString> textFilePaths;	// Absolute path array for texture maps
        std::map<UString, int> textMap;		// Record the index of textures in an array

        std::vector<PBR_Material> materials;        // Material array
    };
}