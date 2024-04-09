#include "Material.h"

namespace Rendering
{
	PBR_Material_Data::PBR_Material_Data()
	{
        baseColor.value = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		normal.value = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		emissiveColor.value = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		emissiveIntensity.value = 1.0f;
		metallic.value = 0.0f;
		roughness.value = 0.0f;
		ambient.value = 1.0f;
		opacity.value = 1.0f;
	}

	PBR_Material::PBR_Material()
	{
		for (int i = 0; i < 8; ++i)
		{
			mapIndex[i] = -1;
		}
	}

    void MaterialManager::AddDefaultMaterial()
    {
        materials.push_back(PBR_Material());
        materials.back().name = "default";
        defaultMaterialIndex = materials.size() - 1;
    }

    int MaterialManager::GetDefaultMaterialIndex()
    {
        if (defaultMaterialIndex == -1)
            Exception::ThrowException(LEVEL_Warning, "don't have default materia");
        return defaultMaterialIndex;
    }

    void MaterialManager::SetMaterialReserve(int materialCount)
    {
        if (materialCount <= 0 || materialCount > MAX_VECTOR_RESERVE_SIZE)
        {
            Exception::ThrowException(LEVEL_Warning, "Count out of range");
        }
        materials.reserve(materialCount);
    }

    /*
        Query whether the map path is loaded into the map.
        If it is recorded, return its index in the map array. Otherwise, return -1
    */
    int MaterialManager::FindTexturePathIndex(UString path)
    {
        int index = -1;
        if (textMap.find(path) != textMap.end())
        {
            index = textMap[path];
        }
        return index;
    }

    // Add a texture and return its index in the array
    int MaterialManager::AddTextureFilePath(UString path)
    {
        textFilePaths.push_back(path);
        textMap[path] = textFilePaths.size() - 1;
        return textFilePaths.size() - 1;
    }

    /*
        Load a specific type of texture from a certain material in Assymp.
        For simplicity, the maximum number of textures a material can reference for a specific type is 1,
        so this function returns at most the first texture of a specific type.
    */
    UString MaterialManager::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
    {
        if (mat->GetTextureCount(type) >= 1)
        {
            aiString str;
            mat->GetTexture(type, 0, &str);
            return str.C_Str();
        }
        else
        {
            // Indicates that the material does not have this type of texture
            return "NULL";
        }
    }

    /*
    Process texture paths in materials,
        1. Add its relative path to the model file path and change it to an absolute path
        2. Unified use of "\\" as a hierarchical division
        3. Change the texture suffix to. dds
    */
    UString MaterialManager::PrcessFilePath(UString modelPath, UString texturePath)
    {
        // Change "texture/baseColor.png" to "texture\\baseColor.png"
        std::replace(texturePath.GetStr().begin(), texturePath.GetStr().end(), '/', '\\');

        int textureFormatSize = -1;
        auto pos = texturePath.GetStr().rfind('.');
        if (pos == std::wstring::npos)
        {
            Exception::ThrowException(LEVEL_Error, "Texture path is faild, don't have . char");
        }
        else
        {
            textureFormatSize = texturePath.GetStr().size() - (pos + 1);
        }

        // Change "texture\\baseColor.xxx" to "texture\\baseColor.dds"
        texturePath.GetStr().replace(pos + 1, textureFormatSize, "dds");

        // Change "texture\\baseColor.dds" to "D\\Model\\texture\\baseColor.dds"
        return modelPath + "\\" + texturePath;
    }

    // Read Assymp material aiMaterial and convert it to PBR_Material
    void MaterialManager::AddMaterial(aiMaterial* material, UString modelPath, bool prcessFilePath)
    {
        materials.emplace_back();
        PBR_Material& pbr_materila = materials.back();
        pbr_materila.name = material->GetName().C_Str();

        UString textFilePath;           // Record the texture path in the material
        UString textFilePathT;          // Record the processed texture path
        int index;                      // Record the index of textures in an array
        aiVector3D aiVe3;
        aiColor4D color4;
        float fValue;

        /*
            baseColor Attribute Item.
            First, obtain the original path of this type of texture map
        */
        textFilePath = LoadMaterialTextures(material, aiTextureType_BASE_COLOR);
        if (textFilePath.GetStr() != "NULL")
        {
            // If there is a corresponding texture path, process it first
            if (prcessFilePath)
                textFilePathT = PrcessFilePath(modelPath, textFilePath);
            else
                textFilePathT = textFilePath;

            // The index of the processed texture path in the array after query processing
            index = FindTexturePathIndex(textFilePathT);
            if (index == -1)
            {
                // If the texture does not exist in the array, add it to the array and record its index
                index = AddTextureFilePath(textFilePathT);
            }

            // Record index data and mark unused value data
            pbr_materila.mapIndex[0] = index;
            pbr_materila.data.baseColor.useValue = 0;
        }
        else
        {
            // If the material does not contain this type of map, try to obtain the attribute values
            if (AI_SUCCESS == material->Get(AI_MATKEY_BASE_COLOR, color4))
            {
                // If the material contains such attribute values, it is set as value data; otherwise, the value data is set to default
                pbr_materila.data.baseColor.value = ConvertAiColorToXMFLOAT4(color4);
            }
            // Mark using value data instead of texture
            pbr_materila.data.baseColor.useValue = 1;
        }

        // normal
        textFilePath = LoadMaterialTextures(material, aiTextureType_NORMALS);
        if (textFilePath.GetStr() != "NULL")
        {
            if (prcessFilePath)
                textFilePathT = PrcessFilePath(modelPath, textFilePath);
            else
                textFilePathT = textFilePath;

            index = FindTexturePathIndex(textFilePathT);
            if (index == -1)
            {
                index = AddTextureFilePath(textFilePathT);
            }

            pbr_materila.mapIndex[1] = index;
            pbr_materila.data.normal.useValue = 0;
        }
        else
        {
            // If there is no normal map, use the normal calculated from the vertices and do not query the values in the material
            pbr_materila.data.normal.useValue = 1;
        }
        
        // emissiveColor
        textFilePath = LoadMaterialTextures(material, aiTextureType_EMISSION_COLOR);
        std::string s = "NULL";
        if(textFilePath.GetC_Str() == s)
            textFilePath = LoadMaterialTextures(material, aiTextureType_EMISSIVE);
        if (textFilePath.GetStr() != "NULL")
        {
            if (prcessFilePath)
                textFilePathT = PrcessFilePath(modelPath, textFilePath);
            else
                textFilePathT = textFilePath;

            index = FindTexturePathIndex(textFilePathT);
            if (index == -1)
            {
                index = AddTextureFilePath(textFilePathT);
            }

            pbr_materila.mapIndex[2] = index;
            pbr_materila.data.emissiveColor.useValue = 0;
        }
        else
        {
            if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, color4))
            {
                pbr_materila.data.emissiveColor.value =
                    DirectX::XMFLOAT3(color4.r, color4.g, color4.b);
            }
            pbr_materila.data.emissiveColor.useValue = 1;
        }

        // emissiveIntensity
        if (AI_SUCCESS == material->Get(AI_MATKEY_EMISSIVE_INTENSITY, fValue))
        {
            pbr_materila.data.emissiveIntensity.value = fValue;
        }
        else
        {
            pbr_materila.data.emissiveIntensity.value = 1.0f;
            pbr_materila.data.emissiveIntensity.useValue = 1;
        }

        // meatllic
        textFilePath = LoadMaterialTextures(material, aiTextureType_METALNESS);
        if (textFilePath.GetStr() != "NULL")
        {
            if (prcessFilePath)
                textFilePathT = PrcessFilePath(modelPath, textFilePath);
            else
                textFilePathT = textFilePath;

            index = FindTexturePathIndex(textFilePathT);
            if (index == -1)
            {
                index = AddTextureFilePath(textFilePathT);
            }

            pbr_materila.mapIndex[4] = index;
            pbr_materila.data.metallic.useValue = 0;
        }
        else
        {
            if (AI_SUCCESS == material->Get(AI_MATKEY_METALLIC_FACTOR, fValue))
            {
                pbr_materila.data.metallic.value = fValue;
            }
            pbr_materila.data.metallic.useValue = 1;
        }

        // roughness
        textFilePath = LoadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS);
        if (textFilePath.GetStr() != "NULL")
        {
            if (prcessFilePath)
                textFilePathT = PrcessFilePath(modelPath, textFilePath);
            else
                textFilePathT = textFilePath;

            index = FindTexturePathIndex(textFilePathT);
            if (index == -1)
            {
                index = AddTextureFilePath(textFilePathT);
            }

            pbr_materila.mapIndex[5] = index;
            pbr_materila.data.roughness.useValue = 0;
        }
        else
        {
            if (AI_SUCCESS == material->Get(AI_MATKEY_ROUGHNESS_FACTOR, fValue))
            {
                pbr_materila.data.roughness.value = fValue;
            }
            pbr_materila.data.roughness.useValue = 1;
        }

        // AO
        textFilePath = LoadMaterialTextures(material, aiTextureType_LIGHTMAP);
        if (textFilePath.GetStr() != "NULL")
        {
            if (prcessFilePath)
                textFilePathT = PrcessFilePath(modelPath, textFilePath);
            else
                textFilePathT = textFilePath;

            index = FindTexturePathIndex(textFilePathT);
            if (index == -1)
            {
                index = AddTextureFilePath(textFilePathT);
            }

            pbr_materila.mapIndex[6] = index;
            pbr_materila.data.ambient.useValue = 0;
        }
        else
        {
            pbr_materila.data.ambient.value = 0.4;
            pbr_materila.data.ambient.useValue = 1;
        }

        // opacity
        textFilePath = LoadMaterialTextures(material, aiTextureType_OPACITY);
        if (textFilePath.GetStr() != "NULL")
        {
            if (prcessFilePath)
                textFilePathT = PrcessFilePath(modelPath, textFilePath);
            else
                textFilePathT = textFilePath;

            index = FindTexturePathIndex(textFilePathT);
            if (index == -1)
            {
                index = AddTextureFilePath(textFilePathT);
            }

            pbr_materila.mapIndex[7] = index;
            pbr_materila.data.opacity.useValue = 0;
        }
        else
        {
            if (AI_SUCCESS == material->Get(AI_MATKEY_OPACITY, fValue))
            {
                pbr_materila.data.opacity.value = fValue;
            }
            pbr_materila.data.opacity.useValue = 1;
        }
    }

    std::vector<UString>& MaterialManager::GetTextureFilePaths()
    {
        return textFilePaths;
    }
    std::map<UString, int>& MaterialManager::GetTextureMap()
    {
        return textMap;
    }
    std::vector<PBR_Material>& MaterialManager::GetMaterials()
    {
        return materials;
    }
}