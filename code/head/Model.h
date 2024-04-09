#pragma once

#include <iostream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Material.h"
#include "BoneInformation.h"
#include "Animation.h"
#include "Exception.h"

namespace Rendering
{
    template<typename VertexType>
    class Model
    {
    public:
        /*
            When constructing a model, grid array space should be pre allocated.
            When constructing a Mesh, vertex and index data space should be pre allocated.
        */
        Model(int meshMaxNum = 1)
        {
            if (meshMaxNum < 0 || meshMaxNum > MAX_VECTOR_RESERVE_SIZE)
                Exception::ThrowException(LEVEL_Warning, "size out of range");

            meshs.reserve(meshMaxNum);
        }

        bool CheckValid()
        {
            ComputeVINum();
            return (vertexNum > 0 && indexNum > 0);
        }

        std::vector<Mesh<VertexType>>& GetMeshs()
        {
            return meshs;
        }

        MaterialManager& GetMaterialManager()
        {
            return materialManager;
        }
        std::vector<UString>& GetTextFilePaths()
        {
            return materialManager.GetTextureFilePaths();
        }
        std::map<UString, int>& GetTextMap()
        {
            return materialManager.GetTextureMap();
        }

        std::map<UString, BoneInfo>& GetBoneMap()
        {
            return boneInformation.boneMap;
        }
        int& GetBoneCount()
        {
            return boneInformation.boneCount;
        }
        std::vector<Animation>& GetAnimations()
        {
            return animations;
        }
        void SetHaveBone(bool _haveBone)
        {
            haveBone = _haveBone;
        }
        bool HaveBone()const
        {
            return haveBone;
        }

        AssimpNodeData& GetNodeData()
        {
            return nodeLayout;
        }
        std::map<UString, DirectX::XMFLOAT4X4>& GetTransforms()
        {
            return transforms;
        }

        UString& GetDirectory()
        {
            return directory;
        }
        UString& GetName()
        {
            return name;
        }
        void SetName(UString _name)
        {
            name = _name;
        }

        int GetVertexNum()const
        {
            return vertexNum;
        }
        int GetIndexNum()const
        {
            return indexNum;
        }
        
        void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY _topology)
        {
            topology = _topology;
        }
        D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology()
        {
            return topology;
        }

        // Release the space of the model by releasing grid data
        void FreeSpace()
        {
            FreeVector(meshs);
            FreeVector(materialManager.GetMaterials());
        }

        // Calculate the number of vertices and indexes
        void ComputeVINum()
        {
            vertexNum = 0;
            indexNum = 0;
            for (int i = 0; i < meshs.size(); ++i)
            {
                vertexNum += meshs[i].GetVertexNumber();
                indexNum += meshs[i].GetIndexNumber();
            }
        }

        // Obtain the geometric index description array of the model
        std::vector<GeometryDesc> GetGeoDesc()const
        {
            return Mesh<VertexType>::GetGeometryDescs(meshs);
        }

        /*
            Obtain geometric index description arrays for multiple models.
            This function can be used to support unified uploading of geometric data from multiple models, 
            and more continuous use of memory is beneficial for performance.
        */
        static std::vector<GeometryDesc> GetGeoDescs(const std::vector<Model<VertexType>>& models)
        {
            int c = 0;
            for (int i = 0; i < models.size(); ++i)
            {
                c += models[i].meshs.size();
            }

            std::vector<GeometryDesc> geoDesc(c);

            for (int i = 0; i < models.size(); ++i)
            {
                for (int j = 0; j < models[i].meshs.size(); ++j)
                {
                    geoDesc[i] = models[i].meshs[j].GetGeoDesc();
                }
            }
            return geoDesc;
        }

        static bool HasBones(const aiScene* scene) {
            if (!scene || !scene->mRootNode) {
                return false;
            }

            for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
                const aiMesh* mesh = scene->mMeshes[i];
                if (mesh->mNumBones > 0) {
                    return true;
                }
            }

            return false;
        }

        static const aiScene* LoadModelScene(UString path, Assimp::Importer& importer)
        {
            unsigned int option =
                aiProcess_ConvertToLeftHanded |
                aiProcessPreset_TargetRealtime_MaxQuality;

            unsigned int option2 =
                aiProcess_CalcTangentSpace | 
                aiProcess_GenSmoothNormals | 
                aiProcess_Triangulate;

            const aiScene* scene = importer.ReadFile(path.GetStr(), option);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                Exception::ThrowException(LEVEL_Error, "Load model fai");
            }

            return scene;
        }

        static int FindModelType(const aiScene* scene)
        {
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                Exception::ThrowException(LEVEL_Error, "Load model fai");
                return -1;
            }

            bool haveBone = HasBones(scene);

            // Skeleton animation not supported returns 0, otherwise returns 1
            if (!haveBone)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }

        // Load all materials in the scene into the model
        static void LoadAssimpMaterial(const aiScene* scene, Model<VertexType>& model, bool pressPath = true)
        {
            MaterialManager& materialManagerT = model.GetMaterialManager();
            materialManagerT.SetMaterialReserve(scene->mNumMaterials + 1);

            aiMaterial* material;
            for (int i = 0; i < scene->mNumMaterials; ++i)
            {
                material = scene->mMaterials[i];
                
                if(pressPath)
                    materialManagerT.AddMaterial(material, model.GetDirectory());
                else
                    materialManagerT.AddMaterial(material, "");
            }

            materialManagerT.AddDefaultMaterial();
        }

        static void CalcTransformMatr(const AssimpNodeData* node,
            DirectX::XMFLOAT4X4 parentTransform, std::map<UString, DirectX::XMFLOAT4X4>& transforms)
        {
            UString nodeName = node->name;
            DirectX::XMFLOAT4X4 nodeTransform =
                node->transformation;

            DirectX::XMFLOAT4X4 globalTransformation =
                MatrXMatr(nodeTransform, parentTransform);

            transforms[nodeName] = globalTransformation;

            // Calculate the final transformation matrix for all child nodes
            for (int i = 0; i < node->childrenCount; ++i)
            {
                CalcTransformMatr(&node->children[i], globalTransformation, transforms);
            }            
        }

        static void LoadMeshTransform(std::vector<Mesh<VertexType>>& meshs,
            std::map<UString, DirectX::XMFLOAT4X4>& transforms)
        {
            for (int i = 0; i < meshs.size(); ++i)
            {
                UString meshName = meshs[i].GetName();
                if (transforms.find(meshName) != transforms.end())
                {
                    meshs[i].GetTransform() = transforms[meshName];
                }
                else
                {
                    meshs[i].GetTransform() = Identity4X4();
                    //Exception::ThrowException(LEVEL_Error, "Error");s
                }
            }
        }

        // Load boneless models
        static bool LoadModel(UString path, const aiScene* scene, Model<Vertex_G>& model, bool pressPath = true)
        {
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                Exception::ThrowException(LEVEL_Error, "Load model fai");
                return false;
            }

            model.GetName() = scene->mName.C_Str();
            model.GetDirectory() = path.GetStr().substr(0, path.GetStr().find_last_of('\\'));
            model.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            AssimpNodeData::ReadHeirarchyData(model.GetNodeData(), scene->mRootNode);
            CalcTransformMatr(&model.GetNodeData(), Identity4X4(), model.GetTransforms());

            LoadAssimpMaterial(scene, model);
            model.SetHaveBone(false);

            model.GetMeshs().reserve(scene->mNumMeshes);
            ProcessNode(scene->mRootNode, scene, model);
            LoadMeshTransform(model.GetMeshs(), model.GetTransforms());

            model.ComputeVINum();
            return true;
        }

        static void printTransform(const DirectX::XMFLOAT4X4& matr)
        {
            for (int i = 0; i < 4; ++i)
            {
                std::cout << std::endl;
                for (int j = 0; j < 4; ++j)
                {
                    std::cout << matr(i,j) << " ";
                }
            }
        }

        // Process Assimp scene nodes and load them into the boneless model version
        static void ProcessNode(aiNode* node, const aiScene* scene, Model<Vertex_G>& model)
        {
            for (int i = 0; i < node->mNumMeshes; ++i)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                ProcessMesh(mesh, scene, model);
            }

            for (int i = 0; i < node->mNumChildren; ++i)
            {
                ProcessNode(node->mChildren[i], scene, model);
            }
        }

        // Process the Assimp scene mesh and load it into the boneless model version
        static void ProcessMesh(aiMesh* mesh, const aiScene* scene, Model<Vertex_G>& model)
        {
            Mesh<Vertex_G>& resMesh = AddElementVector(model.GetMeshs());

            resMesh.SetName(mesh->mName.C_Str());
            resMesh.Reserve(mesh->mNumVertices);

            // Read mesh vertices
            Vertex_G vertex;

            for (UINT64 i = 0; i < mesh->mNumVertices; ++i)
            {
                vertex.InitToDefaultValue();

                vertex.position = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                vertex.normal = DirectX::XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                if (mesh->mTextureCoords[0])
                {
                    vertex.texCoords = DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }
                else
                {
                    vertex.texCoords = DirectX::XMFLOAT2(0.0f, 0.0f);
                }
                if (mesh->mTangents != NULL)
                {
                    vertex.tangent = DirectX::XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                    vertex.bitangent = DirectX::XMFLOAT3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
                }

                resMesh.vertexs.push_back(vertex);
            }

            // Read mesh vertices
            aiFace face;
            int indexNumT = 0;

            for (UINT64 i = 0; i < mesh->mNumFaces; ++i)
            {
                face = mesh->mFaces[i];
                indexNumT += face.mNumIndices;
            }
            resMesh.Reserve(-1, indexNumT);

            for (UINT64 i = 0; i < mesh->mNumFaces; ++i)
            {
                face = mesh->mFaces[i];
                for (UINT64 j = 0; j < face.mNumIndices; ++j)
                {
                    if (face.mNumIndices != 3)
                        Exception::ThrowException(LEVEL_Error, "Error");
                    resMesh.AddIndex(face.mIndices[j]);
                }
            }

            // Set the material of the grid. If there is no corresponding material, use the default material
            if (mesh->mMaterialIndex >= 0)
            {
                resMesh.SetPBRMaterilaIndex(mesh->mMaterialIndex);
            }
            else
            {
                resMesh.SetPBRMaterilaIndex(model.GetMaterialManager().GetDefaultMaterialIndex());
            }
        }

        // Loading Skeleton Models
        static bool LoadModel(UString path, const aiScene* scene, Model<Vertex_GB>& model, bool pressPath = true)
        {
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                Exception::ThrowException(LEVEL_Error, "Load model fai");
                return false;
            }

            model.GetName() = scene->mName.C_Str();
            model.GetDirectory() = path.GetStr().substr(0, path.GetStr().find_last_of('\\'));
            model.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            LoadAssimpMaterial(scene, model, pressPath);
            model.SetHaveBone(true);

            model.GetMeshs().reserve(scene->mNumMeshes);
            ProcessNode(scene->mRootNode, scene, model);

            model.ComputeVINum();

            LoadAnimations(scene, model);

            return true;
        }

        static void ProcessNode(aiNode* node, const aiScene* scene, Model<Vertex_GB>& model)
        {
            for (int i = 0; i < node->mNumMeshes; ++i)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                ProcessMesh(mesh, scene, model);
            }

            for (int i = 0; i < node->mNumChildren; ++i)
            {
                ProcessNode(node->mChildren[i], scene, model);
            }
        }

        static void ProcessMesh(aiMesh* mesh, const aiScene* scene, Model<Vertex_GB>& model)
        {
            Mesh<Vertex_GB>& resMesh = AddElementVector(model.GetMeshs());

            resMesh.SetName(mesh->mName.C_Str());
            resMesh.Reserve(mesh->mNumVertices);

            Vertex_GB vertex;
            vertex.InitToDefaultValue();

            for (UINT64 i = 0; i < mesh->mNumVertices; ++i)
            {
                vertex.position = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                vertex.normal = DirectX::XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                if (mesh->mTextureCoords[0])
                {
                    vertex.texCoords = DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }
                else
                {
                    vertex.texCoords = DirectX::XMFLOAT2(0.0f, 0.0f);
                }
                vertex.tangent = DirectX::XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                vertex.bitangent = DirectX::XMFLOAT3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

                resMesh.vertexs.push_back(vertex);
            }

            aiFace face;
            int indexNumT = 0;

            for (UINT64 i = 0; i < mesh->mNumFaces; ++i)
            {
                face = mesh->mFaces[i];
                indexNumT += face.mNumIndices;
            }
            resMesh.Reserve(-1, indexNumT);

            for (UINT64 i = 0; i < mesh->mNumFaces; ++i)
            {
                face = mesh->mFaces[i];
                for (UINT64 j = 0; j < face.mNumIndices; ++j)
                {
                    resMesh.AddIndex(face.mIndices[j]);
                }
            }

            if (mesh->mMaterialIndex >= 0)
            {
                resMesh.SetPBRMaterilaIndex(mesh->mMaterialIndex);
            }
            else
            {
                resMesh.SetPBRMaterilaIndex(model.GetMaterialManager().GetDefaultMaterialIndex());
            }

            // Load bone data for the model
            ExtractBoneWeightForVertices(mesh, scene, resMesh, model);
        }

        // Load bone data for the model
        static void ExtractBoneWeightForVertices(aiMesh* mesh, const aiScene* scene, Mesh<Vertex_GB>& resMesh, Model<Vertex_GB>& model)
        {
            auto& boneInfoMap = model.GetBoneMap();
            auto& boneCounter = model.GetBoneCount();

            for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
            {
                int boneID = -1;
                UString boneName = mesh->mBones[boneIndex]->mName.C_Str();

                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    BoneInfo newBoneInfo;
                    newBoneInfo.id = boneCounter;
                    newBoneInfo.offset =
                        ConvertAssimpMatrixToDirectX(
                            mesh->mBones[boneIndex]->mOffsetMatrix);

                    boneInfoMap[boneName] = newBoneInfo;
                    boneID = boneCounter;
                    boneCounter += 1;
                }
                else 
                {
                    boneID = boneInfoMap[boneName].id;
                }

                auto weights = mesh->mBones[boneIndex]->mWeights;
                int numWeights = mesh->mBones[boneIndex]->mNumWeights;

                for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
                {
                    int vertexId = weights[weightIndex].mVertexId;
                    float weight = weights[weightIndex].mWeight;

                    if (boneID > 96)
                    {
                        Exception::ThrowException(LEVEL_Error, "Model bone number out of 96");
                    }
                    
                    resMesh.GetVertexs()[vertexId].SetBoneInfo(boneID, weight);
                }
            }
        }

        static bool LoadAnimations(const aiScene* scene, Model<Vertex_GB>& model)
        {
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                Exception::ThrowException(LEVEL_Error, "Load animation file fail");
                return false;
            }

            for (int i = 0; i < scene->mNumAnimations; ++i)
            {
                Animation animaiton = Animation(scene, model.GetBoneMap(), model.GetBoneCount(), i);
                model.GetAnimations().push_back(std::move(animaiton));
            }
            return true;
        }

    private:
        // Model information
        UString name;
        UString directory;
        D3D12_PRIMITIVE_TOPOLOGY topology;

        // mesh information
        std::vector<Mesh<VertexType>> meshs;
        int vertexNum = 0;
        int indexNum = 0;

        // material information
        MaterialManager materialManager;

        AssimpNodeData nodeLayout;
        std::map<UString, DirectX::XMFLOAT4X4> transforms;

        // bone and animation
        bool haveBone = false;
        BoneInformation boneInformation;

        std::vector<Animation> animations;
    };
}