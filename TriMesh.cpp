#include <chrono>
#include "TriMesh.h"
#include "MeshPainter.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// 一些基础颜色
const glm::vec3 basic_colors[8] = {
	glm::vec3(1.0, 1.0, 1.0), // White
	glm::vec3(1.0, 1.0, 0.0), // Yellow
	glm::vec3(0.0, 1.0, 0.0), // Green
	glm::vec3(0.0, 1.0, 1.0), // Cyan
	glm::vec3(1.0, 0.0, 1.0), // Magenta
	glm::vec3(1.0, 0.0, 0.0), // Red
	glm::vec3(0.0, 0.0, 0.0), // Black
	glm::vec3(0.0, 0.0, 1.0)	 // Blue
};

// 立方体的各个点
const glm::vec3 cube_vertices[8] = {
	glm::vec3(-0.5, -0.5, -0.5),
	glm::vec3(0.5, -0.5, -0.5),
	glm::vec3(-0.5, 0.5, -0.5),
	glm::vec3(0.5, 0.5, -0.5),
	glm::vec3(-0.5, -0.5, 0.5),
	glm::vec3(0.5, -0.5, 0.5),
	glm::vec3(-0.5, 0.5, 0.5),
	glm::vec3(0.5, 0.5, 0.5)};

// 三角形的点
const glm::vec3 triangle_vertices[3] = {
	glm::vec3(-0.5, -0.5, 0.0),
	glm::vec3(0.5, -0.5, 0.0),
	glm::vec3(0.0, 0.5, 0.0)};

// 正方形平面
const glm::vec3 square_vertices[4] = {
	glm::vec3(-0.5, -0.5, 0.0),
	glm::vec3(0.5, -0.5, 0.0),
	glm::vec3(0.5, 0.5, 0.0),
	glm::vec3(-0.5, 0.5, 0.0),
};



TriMesh::~TriMesh() = default;
TriMesh* TriMesh::generateTriMesh_by_aiMesh(aiMesh *mesh, const aiScene *scene,Model* model,bool isbound
                                            ,glm::mat4 totalTransform,const std::string& filename,MeshPainter *painter) {
    // 读取每个mesh的顶点信息
    auto * new_mesh = new TriMesh( model );
    // 读取顶点坐标
    if(mesh->HasPositions())
    {
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D pos = mesh->mVertices[j];
            new_mesh->vertex_positions.push_back(glm::vec3(pos.x, pos.y, pos.z));
        }
    }
    // 读取顶点颜色
    if(mesh->HasVertexColors(0))
    {
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiColor4D color = mesh->mColors[0][j];
            new_mesh->vertex_colors.push_back(glm::vec3(color.r, color.g, color.b));
        }
    }
    // 读取顶点法向量
    if (mesh->HasNormals())
    {
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D normal = mesh->mNormals[j];
            new_mesh->vertex_normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
        }
    }
    // 读取顶点纹理坐标
    if(mesh->HasTextureCoords(0))
    {
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D texture = mesh->mTextureCoords[0][j];
            new_mesh->vertex_textures.push_back(glm::vec2(texture.x, texture.y));
        }
    }
    else{
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            new_mesh->vertex_textures.push_back(glm::vec2(0.0, 0.0));
        }
    }
    // 获取光照信息
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D aiAmbient, aiDiffuse, aiSpecular;
    float shininess;
    material->Get(AI_MATKEY_COLOR_AMBIENT, aiAmbient);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuse);
    material->Get(AI_MATKEY_COLOR_SPECULAR, aiSpecular);
    material->Get(AI_MATKEY_SHININESS, shininess);
    new_mesh->setAmbient(glm::vec4(aiAmbient.r, aiAmbient.g, aiAmbient.b, 1.0));
    new_mesh->setDiffuse(glm::vec4(aiDiffuse.r, aiDiffuse.g, aiDiffuse.b, 1.0));
    new_mesh->setSpecular(glm::vec4(aiSpecular.r, aiSpecular.g, aiSpecular.b, 1.0));
    new_mesh->setShininess(shininess);
    new_mesh->setTranslation(model->getPosition());
    new_mesh->setScale(model->getScale());
    new_mesh->setRotation(model->getRotation());
    // 读取包围盒信息
    if(isbound) {
        // 存储包围盒信息
        glm::vec4 min = glm::vec4(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z,1.0);
        min = glm::mat4(new_mesh->getModelMatrix()*totalTransform) * min;
        glm::vec4 max = glm::vec4(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z,1.0);
        max = glm::mat4(new_mesh->getModelMatrix()*totalTransform) * max;
        glm::vec3 min3 = glm::vec3(min.x/min.w,min.y/min.w,min.z/min.w);
        glm::vec3 max3 = glm::vec3(max.x/max.w,max.y/max.w,max.z/max.w);
        model->pushBoundingBox(min3,max3);
    }

    // 读取面片信息
    for (unsigned int j = 0; j < mesh->mNumFaces; j++)
    {
        aiFace face = mesh->mFaces[j];
        new_mesh->faces.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
        new_mesh->normal_index.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
        new_mesh->color_index.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
        new_mesh->texture_index.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
        new_mesh->boneID_index.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
        new_mesh->weight_index.push_back(vec3i(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
    }
    std::string file_path = filename.substr(0, filename.find_last_of('/'));
    // 获取当前mesh的贴图
    std::vector<Texture> diffuseMaps = painter->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",file_path);
    new_mesh->textures_message.insert(new_mesh->textures_message.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = painter->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular",file_path);
    new_mesh->textures_message.insert(new_mesh->textures_message.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Texture> heightMaps = painter->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height",file_path);
    new_mesh->textures_message.insert(new_mesh->textures_message.end(), heightMaps.begin(), heightMaps.end());

    // 给mesh的两个骨骼vector预分配空间
    new_mesh->vertex_m_BoneID.resize(new_mesh->vertex_positions.size());
    new_mesh->vertex_m_Weight.resize(new_mesh->vertex_positions.size());
    for (int i = 0; i < new_mesh->vertex_positions.size(); ++i)
    {
        for(int j = 0; j < MAX_BONES_INFLUENCE; ++j)
        {
            new_mesh->vertex_m_BoneID[i][j] = -1;
            new_mesh->vertex_m_Weight[i][j] = 0.0;
        }
    }
    // 读取骨骼信息
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (!model->findBoneInfoMap(boneName))
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = model->getBoneCounter();
            auto offsetMatrix = mesh->mBones[boneIndex]->mOffsetMatrix;
            glm::mat4 offset = glm::mat4(1.0f);
            offset[0][0] = offsetMatrix.a1;
            offset[0][1] = offsetMatrix.b1;
            offset[0][2] = offsetMatrix.c1;
            offset[0][3] = offsetMatrix.d1;
            offset[1][0] = offsetMatrix.a2;
            offset[1][1] = offsetMatrix.b2;
            offset[1][2] = offsetMatrix.c2;
            offset[1][3] = offsetMatrix.d2;
            offset[2][0] = offsetMatrix.a3;
            offset[2][1] = offsetMatrix.b3;
            offset[2][2] = offsetMatrix.c3;
            offset[2][3] = offsetMatrix.d3;
            offset[3][0] = offsetMatrix.a4;
            offset[3][1] = offsetMatrix.b4;
            offset[3][2] = offsetMatrix.c4;
            offset[3][3] = offsetMatrix.d4;
            newBoneInfo.offset = offset;

            model->setBoneInfoMap(boneName, newBoneInfo);
            boneID = model->getBoneCounter();
            model->setBoneCounter(boneID + 1);
        }
        else
        {
            boneID = model->getBoneInfoMap()[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= new_mesh->vertex_positions.size());
            for (int i = 0; i < MAX_BONES_INFLUENCE; ++i)
            {
                if (new_mesh->vertex_m_BoneID[vertexId][i] <= 0)
                {
                    new_mesh->vertex_m_BoneID[vertexId][i] = boneID;
                    new_mesh->vertex_m_Weight[vertexId][i] = weight;
                    break;
                }
            }
        }
    }
    new_mesh->storeFacesPoints();
    return new_mesh;
}
std::vector<glm::vec3> TriMesh::getVertexPositions()
{
	return vertex_positions;
}

std::vector<glm::vec3> TriMesh::getVertexColors()
{
	return vertex_colors;
}

std::vector<glm::vec3> TriMesh::getVertexNormals()
{
	return vertex_normals;
}

std::vector<glm::vec2> TriMesh::getVertexTextures()
{
	return vertex_textures;
}
std::vector<glm::ivec4> TriMesh::getVertexBoneID()
{
    return vertex_m_BoneID;
}
std::vector<glm::vec4> TriMesh::getVertexWeight()
{
    return vertex_m_Weight;
}
std::vector<vec3i> TriMesh::getFaces()
{
	return faces;
}

std::vector<glm::vec3> TriMesh::getPoints()
{
	return points;
}

std::vector<glm::vec3> TriMesh::getColors()
{
	return colors;
}

std::vector<glm::vec3> TriMesh::getNormals()
{
	return normals;
}

std::vector<glm::vec2> TriMesh::getTextures()
{
	return textures;
}
std::vector<Texture> TriMesh::getTexturesMessage()
{
    return textures_message;
}

void TriMesh::computeTriangleNormals()
{
	face_normals.resize(faces.size());
	for (size_t i = 0; i < faces.size(); i++)
	{
		auto &face = faces[i];
		glm::vec3 v01 = vertex_positions[face.y] - vertex_positions[face.x];
		glm::vec3 v02 = vertex_positions[face.z] - vertex_positions[face.x];
		face_normals[i] = normalize(cross(v01, v02));
	}
}

void TriMesh::computeVertexNormals()
{
	// 计算面片的法向量
	if (face_normals.size() == 0 && faces.size() > 0)
	{
		computeTriangleNormals();
	}
	// 初始化法向量为0
	vertex_normals.resize(vertex_positions.size(), glm::vec3(0, 0, 0));
	for (size_t i = 0; i < faces.size(); i++)
	{
		auto &face = faces[i];
		vertex_normals[face.x] += face_normals[i];
		vertex_normals[face.y] += face_normals[i];
		vertex_normals[face.z] += face_normals[i];
	}
	for (size_t i = 0; i < vertex_normals.size(); i++)
	{
		vertex_normals[i] = normalize(vertex_normals[i]);
	}
	// 球心在原点的球法向量为坐标
	// for (int i = 0; i < vertex_positions.size(); i++)
	// 	vertex_normals.push_back(vertex_positions[i] - vec3(0.0, 0.0, 0.0));
}

glm::vec3 TriMesh::getTranslation()
{
	return translation;
}

glm::vec3 TriMesh::getRotation()
{
	return rotation;
}

glm::vec3 TriMesh::getScale()
{
	return scale;
}

glm::mat4 TriMesh::getModelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 trans = getTranslation();
	model = glm::translate(model, getTranslation());
	model = glm::rotate(model, glm::radians(getRotation()[2]), glm::vec3(0.0, 0.0, 1.0));
	model = glm::rotate(model, glm::radians(getRotation()[1]), glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(getRotation()[0]), glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, getScale());
	return model;
}

void TriMesh::setTranslation(glm::vec3 translation)
{
	this->translation = translation;
}

void TriMesh::setRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
}

void TriMesh::setScale(glm::vec3 scale)
{
	this->scale = scale;
}

glm::vec4 TriMesh::getAmbient() { return ambient; };
glm::vec4 TriMesh::getDiffuse() { return diffuse; };
glm::vec4 TriMesh::getSpecular() { return specular; };
float TriMesh::getShininess() { return shininess; };

void TriMesh::setAmbient(glm::vec4 _ambient) { ambient = _ambient; };
void TriMesh::setDiffuse(glm::vec4 _diffuse) { diffuse = _diffuse; };
void TriMesh::setSpecular(glm::vec4 _specular) { specular = _specular; };
void TriMesh::setShininess(float _shininess) { shininess = _shininess; };

void TriMesh::cleanData()
{
	vertex_positions.clear();
	vertex_colors.clear();
	vertex_normals.clear();
	vertex_textures.clear();

	faces.clear();
	normal_index.clear();
	color_index.clear();
	texture_index.clear();

	face_normals.clear();


	points.clear();
	colors.clear();
	normals.clear();
	textures.clear();
}

void TriMesh::storeFacesPoints()
{
	// 将读取的顶点根据三角面片上的顶点下标逐个加入
	// 要传递给GPU的points等容器内
    // 调整骨骼权重
    for(int i = 0; i < vertex_m_Weight.size(); i++)
    {
        float sum = vertex_m_Weight[i].x + vertex_m_Weight[i].y + vertex_m_Weight[i].z + vertex_m_Weight[i].w;
        vertex_m_Weight[i].x /= sum;
        vertex_m_Weight[i].y /= sum;
        vertex_m_Weight[i].z /= sum;
        vertex_m_Weight[i].w /= sum;
    }
	// 计算法向量
	if (vertex_normals.size() == 0)
		computeVertexNormals();
    // 预分配空间
    points.reserve(faces.size() * 3);
    colors.reserve(faces.size() * 3);
    normals.reserve(faces.size() * 3);
	// @TODO Task2 根据每个三角面片的顶点下标存储要传入GPU的数据
	for (int i = 0; i < faces.size(); i++)
	{
		// 坐标
        points.push_back(vertex_positions[faces[i].x]);
        points.push_back(vertex_positions[faces[i].y]);
        points.push_back(vertex_positions[faces[i].z]);
		// 颜色
        if( vertex_colors.empty() ) {
            colors.push_back(glm::vec3(1.0, 1.0, 1.0));
            colors.push_back(glm::vec3(1.0, 1.0, 1.0));
            colors.push_back(glm::vec3(1.0, 1.0, 1.0));
        } else {
            colors.push_back(vertex_colors[color_index[i].x]);
            colors.push_back(vertex_colors[color_index[i].y]);
            colors.push_back(vertex_colors[color_index[i].z]);
        }
		// 法向量
        normals.push_back(vertex_normals[normal_index[i].x]);
        normals.push_back(vertex_normals[normal_index[i].y]);
        normals.push_back(vertex_normals[normal_index[i].z]);
        if( vertex_textures.empty() ) continue;
		// 纹理
        textures.push_back(vertex_textures[texture_index[i].x]);
        textures.push_back(vertex_textures[texture_index[i].y]);
        textures.push_back(vertex_textures[texture_index[i].z]);
        if( vertex_m_BoneID.empty() ) continue;
        //骨骼ID
        m_BoneID.push_back(vertex_m_BoneID[boneID_index[i].x]);
        m_BoneID.push_back(vertex_m_BoneID[boneID_index[i].y]);
        m_BoneID.push_back(vertex_m_BoneID[boneID_index[i].z]);
        //骨骼权重
        m_Weight.push_back(vertex_m_Weight[weight_index[i].x]);
        m_Weight.push_back(vertex_m_Weight[weight_index[i].y]);
        m_Weight.push_back(vertex_m_Weight[weight_index[i].z]);
	}
}

// 立方体生成12个三角形的顶点索引
void TriMesh::generateCube()
{
	// 创建顶点前要先把那些vector清空
    // 统计时间
    auto start = std::chrono::steady_clock::now();
	cleanData();

	for (int i = 0; i < 8; i++)
	{
		vertex_positions.push_back(cube_vertices[i]);
		vertex_colors.push_back(basic_colors[i]);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 3));
	faces.push_back(vec3i(0, 3, 2));
	faces.push_back(vec3i(1, 4, 5));
	faces.push_back(vec3i(1, 0, 4));
	faces.push_back(vec3i(4, 0, 2));
	faces.push_back(vec3i(4, 2, 6));
	faces.push_back(vec3i(5, 6, 4));
	faces.push_back(vec3i(5, 7, 6));
	faces.push_back(vec3i(2, 6, 7));
	faces.push_back(vec3i(2, 7, 3));
	faces.push_back(vec3i(1, 5, 7));
	faces.push_back(vec3i(1, 7, 3));

	// 顶点纹理坐标，这里是每个面都用一个正方形图片的情况
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(0, 1));
	vertex_textures.push_back(glm::vec2(1, 1));
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(0, 1));
	vertex_textures.push_back(glm::vec2(1, 1));

	normal_index = faces;
	color_index = faces;
	texture_index = faces;
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "generateCube() time cost: " << elapsed_seconds.count() << "s\n";
	storeFacesPoints();
}

void TriMesh::generateTriangle(glm::vec3 color)
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 3; i++)
	{
		vertex_positions.push_back(triangle_vertices[i]);
		vertex_colors.push_back(color);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 2));

	// 顶点纹理坐标
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(0.5, 1));

	normal_index = faces;
	color_index = faces;
	texture_index = faces;

	storeFacesPoints();
}

void TriMesh::generateSquare(glm::vec3 color)
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 4; i++)
	{
		vertex_positions.push_back(square_vertices[i]);
		vertex_colors.push_back(color);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 2));
	faces.push_back(vec3i(0, 2, 3));

	// 顶点纹理坐标
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(1, 1));
	vertex_textures.push_back(glm::vec2(0, 1));

	normal_index = faces;
	color_index = faces;
	texture_index = faces;

	storeFacesPoints();
}

void TriMesh::generateCylinder(int num_division, float radius, float height)
{

	cleanData();

	int num_samples = num_division;
	float step = 2 * M_PI / num_samples; // 每个切片的弧度

	// 按cos和sin生成x，y坐标，z为负，即得到下表面顶点坐标
	// 顶点， 纹理
	float z = -height;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);
		// 添加顶点坐标
		vertex_positions.push_back(glm::vec3(x, y, z));
		vertex_normals.push_back( normalize(glm::vec3(x, y, 0)));
		// 这里颜色和法向量一样
		vertex_colors.push_back( normalize(glm::vec3(x, y, 0)));
	}

	// 按cos和sin生成x，y坐标，z为正，即得到上表面顶点坐标
	z = height;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);
		vertex_positions.push_back(glm::vec3(x, y, z));
		vertex_normals.push_back( normalize(glm::vec3(x, y, 0)));
		vertex_colors.push_back( normalize(glm::vec3(x, y, 0)));
	}

	// 面片生成三角面片，每个矩形由两个三角形面片构成
	for (int i = 0; i < num_samples; i++)
	{
		// 面片1
		faces.push_back(vec3i(i, (i + 1) % num_samples, (i) + num_samples));
		// 面片2
		faces.push_back(vec3i((i) + num_samples, (i + 1) % num_samples, (i + num_samples + 1) % (num_samples) + num_samples));

		// 面片1对应的顶点的纹理坐标
		vertex_textures.push_back(glm::vec2(1.0 * i / num_samples, 0.0));
		vertex_textures.push_back(glm::vec2(1.0 * (i+1) / num_samples, 0.0));
		vertex_textures.push_back(glm::vec2(1.0 * i / num_samples, 1.0));
		// 对应的三角面片的纹理坐标的下标
		texture_index.push_back( vec3i( 6*i, 6*i+1, 6*i+2 ) );

		// 面片2对应的顶点的纹理坐标
		vertex_textures.push_back(glm::vec2(1.0 * i / num_samples, 1.0));
		vertex_textures.push_back(glm::vec2(1.0 * (i+1) / num_samples, 0.0));
		vertex_textures.push_back(glm::vec2(1.0 * (i+1) / num_samples, 1.0));
		// 对应的三角面片的纹理坐标的下标
		texture_index.push_back( vec3i( 6*i+3, 6*i+4, 6*i+5 ) );
	}

	// 三角面片的每个顶点的法向量的下标，这里和顶点坐标的下标 faces是一致的，所以我们用faces就行
	normal_index = faces;
	// 三角面片的每个顶点的颜色的下标
	color_index = faces;

	storeFacesPoints();
}

void TriMesh::generateDisk(int num_division, float radius)
{
	cleanData();

	int num_samples = num_division;
	float step = 2 * M_PI / num_samples; // 每个切片的弧度

	// 按cos和sin生成x，y坐标，z为负，即得到下表面顶点坐标
	float z = 0;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);
		vertex_positions.push_back(glm::vec3(x, y, z));
		vertex_normals.push_back(glm::vec3(0, 0, 1));
		// 这里采用法线来生成颜色，可以自定义自己的颜色生成方式
		vertex_colors.push_back(glm::vec3(0, 0, 1));
	}
	// 中心点
	vertex_positions.push_back(glm::vec3(0, 0, 0));
	vertex_normals.push_back(glm::vec3(0, 0, 1));
	vertex_colors.push_back(glm::vec3(0, 0, 1));

	// 生成三角面片，每个矩形由两个三角形面片构成
	for (int i = 0; i < num_samples; i++)
	{
		// 面片1
		faces.push_back(vec3i(i, (i + 1) % num_samples, num_samples));

		// 将0-360度映射到UV坐标的0-1
		for (int j = 0; j < 2; j++)
		{
			float r_r_r = (i + j) * step;
			float x = cos(r_r_r) / 2.0 + 0.5;
			float y = sin(r_r_r) / 2.0 + 0.5;
			// 纹理坐标
			vertex_textures.push_back(glm::vec2(x, y));
		}
		// 纹理坐标 中心
		vertex_textures.push_back(glm::vec2(0.5, 0.5));

		// 对应的三角面片的每个顶点的纹理坐标的下标
		texture_index.push_back(vec3i(3 * i, 3 * i + 1, 3 * i + 2));
	}

	// 三角面片的每个顶点的法向量的下标，这里和顶点坐标的下标 faces是一致的，所以我们用faces就行
	normal_index = faces;
	// 三角面片的每个顶点的颜色的下标
	color_index = faces;


	storeFacesPoints();
}

void TriMesh::generateCone(int num_division, float radius, float height)
{
	cleanData();

	int num_samples = num_division;

	float step = 2 * M_PI / num_samples;

	float z = 0;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);

		vertex_positions.push_back(glm::vec3(x, y, z));
		vertex_normals.push_back( normalize(glm::vec3(x, y, 0)) );
		vertex_colors.push_back( normalize(glm::vec3(x, y, 0)) );
	}

	vertex_positions.push_back(glm::vec3(0, 0, height));
	vertex_normals.push_back(glm::vec3(0, 0, 1));
	vertex_colors.push_back(glm::vec3(0, 0, 1));

	for (int i = 0; i < (num_samples); i++)
	{
		// 三角面片
		faces.push_back( vec3i( num_samples, (i) % (num_samples), (i + 1) % (num_samples) ) );
		// 该面片每个顶点的纹理坐标
		vertex_textures.push_back(glm::vec2(0.5, 1-0));
		vertex_textures.push_back(glm::vec2( 1.0 * (i) / num_samples , 1-1));
		vertex_textures.push_back(glm::vec2( 1.0 * (i + 1) / num_samples , 1-1));
		// 该三角面片的每个顶点的纹理坐标的下标
		texture_index.push_back(vec3i( 3*i, 3*i+1, 3*i+2 ));
	}


	// 三角面片的每个顶点的法向量的下标，这里和顶点坐标的下标 faces是一致的，所以我们用faces就行
	normal_index = faces;
	// 三角面片的每个顶点的颜色的下标
	color_index = faces;

	storeFacesPoints();
}
std::vector<glm::ivec4> TriMesh::getBoneID() {
    return m_BoneID;
}

std::vector<glm::vec4> TriMesh::getWeight() {
    return m_Weight;
}

TriMesh::TriMesh() {

}


// Light
glm::mat4 Light::getShadowProjectionMatrix()
{
	// 这里只实现了Y=0平面上的阴影投影矩阵，其他情况自己补充
	float lx, ly, lz;

	glm::mat4 modelMatrix = this->getModelMatrix();
	glm::vec4 light_position = modelMatrix * glm::vec4(this->translation, 1.0);

	lx = light_position[0];
	ly = light_position[1];
	lz = light_position[2];

	return glm::mat4(
		-ly, 0.0, 0.0, 0.0,
		lx, 0.0, lz, 1.0,
		0.0, 0.0, -ly, 0.0,
		0.0, 0.0, 0.0, -ly);
}

// 设置衰减系数的参数
void Light::setAttenuation(float _constant, float _linear, float _quadratic)
{
	constant = _constant;
	linear = _linear;
	quadratic = _quadratic;
}

float Light::getConstant() { return constant; };
float Light::getLinear() { return linear; };
float Light::getQuadratic() { return quadratic; };

