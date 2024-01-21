#include "MeshPainter.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint openGLObject::program = 0xffffffff;
GLuint openGLObject::program2 = 0xffffffff;
MeshPainter::MeshPainter(Model *model){this->model = model;};
MeshPainter::MeshPainter(){};
MeshPainter::~MeshPainter(){};

std::vector<std::string> MeshPainter::getMeshNames(){ return mesh_names;};
std::vector<TriMesh *> MeshPainter::getMeshes(){ return meshes;};
std::vector<openGLObject> MeshPainter::getOpenGLObj(){ return opengl_objects;};

void MeshPainter::bindObjectAndData(TriMesh *mesh, openGLObject &object, const std::string &vshader,const std::string &fshader) {
    // 初始化各种对象
    std::vector<glm::vec3> points = mesh->getPoints();
    std::vector<glm::vec3> normals = mesh->getNormals();
    std::vector<glm::vec3> colors = mesh->getColors();
    std::vector<glm::vec2> textures = mesh->getTextures();
    std::vector<glm::ivec4> bone_id = mesh->getBoneID();
    std::vector<glm::vec4> bone_weight = mesh->getWeight();
	// 创建顶点数组对象
	glGenVertexArrays(1, &object.vao);  	// 分配1个顶点数组对象
	glBindVertexArray(object.vao);  	// 绑定顶点数组对象

	// 创建并初始化顶点缓存对象
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 points.size() * sizeof(glm::vec3) +
                     normals.size() * sizeof(glm::vec3) +
                     colors.size() * sizeof(glm::vec3) +
                     textures.size() * sizeof(glm::vec2) +
                     bone_id.size() * sizeof(glm::ivec4) +
                     bone_weight.size() * sizeof(glm::vec4),
                 NULL, GL_STATIC_DRAW);

    // 绑定顶点数据
    glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), points.data());
    // 绑定颜色数据
    glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), colors.data());
    // 绑定法向量数据
    glBufferSubData(GL_ARRAY_BUFFER, (points.size() + colors.size()) * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), normals.data());
    // 绑定纹理数据
    glBufferSubData(GL_ARRAY_BUFFER, (points.size() + normals.size() + colors.size()) * sizeof(glm::vec3), textures.size()
                    * sizeof(glm::vec2), textures.data());
    // 绑定骨骼id数据
    glBufferSubData(GL_ARRAY_BUFFER, (points.size() + normals.size() + colors.size())* sizeof(glm::vec3)
                    + textures.size() * sizeof(glm::vec2), bone_id.size()
                    * sizeof(glm::ivec4), bone_id.data());
    // 绑定骨骼权重数据
    glBufferSubData(GL_ARRAY_BUFFER, (points.size() + normals.size() + colors.size())* sizeof(glm::vec3)
                    + textures.size() * sizeof(glm::vec2) + bone_id.size()
                    * sizeof(glm::ivec4), bone_weight.size()
                    * sizeof(glm::vec4), bone_weight.data());
	object.vshader = vshader;
	object.fshader = fshader;
	if(openGLObject::program == 0xffffffff)
        openGLObject::program = InitShader(object.vshader.c_str(), object.fshader.c_str());
    // 将顶点传入着色器
	object.pLocation = glGetAttribLocation(object.program, "vPosition");
	glEnableVertexAttribArray(object.pLocation);
	glVertexAttribPointer(object.pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // 将颜色传入着色器
	object.cLocation = glGetAttribLocation(object.program, "vColor");
	glEnableVertexAttribArray(object.cLocation);
	glVertexAttribPointer(object.cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(glm::vec3)));

    // 将法向量传入着色器
	object.nLocation = glGetAttribLocation(object.program, "vNormal");
	glEnableVertexAttribArray(object.nLocation);
	glVertexAttribPointer(object.nLocation, 3, 
		GL_FLOAT, GL_FALSE, 0, 
		BUFFER_OFFSET( (points.size() + colors.size())  * sizeof(glm::vec3)));
    // 将纹理坐标传入着色器
	object.tLocation = glGetAttribLocation(object.program, "vTexture");
	glEnableVertexAttribArray(object.tLocation);
	glVertexAttribPointer(object.tLocation, 2, 
		GL_FLOAT, GL_FALSE, 0, 
		BUFFER_OFFSET( ( points.size() + colors.size() + normals.size())  * sizeof(glm::vec3)));
    // 将骨骼id传入着色器
    object.boneIDLocation = glGetAttribLocation(object.program, "boneIds");
    glEnableVertexAttribArray(object.boneIDLocation);
    glVertexAttribIPointer(object.boneIDLocation, MAX_BONES_INFLUENCE,
        GL_INT, 0,
        BUFFER_OFFSET( ( points.size() + colors.size() + normals.size())  * sizeof(glm::vec3) + textures.size() * sizeof(glm::vec2)));
    // 将骨骼权重传入着色器
    object.boneWeightLocation = glGetAttribLocation(object.program, "weights");
    glEnableVertexAttribArray(object.boneWeightLocation);
    glVertexAttribPointer(object.boneWeightLocation, MAX_BONES_INFLUENCE,
        GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET( ( points.size() + colors.size() + normals.size())  * sizeof(glm::vec3) + textures.size() * sizeof(glm::vec2)
        + bone_id.size() * sizeof(glm::ivec4)));

	// 获得矩阵位置
    object.shadowMatrixLocation = glGetUniformLocation(object.program, "ShadowMatrix");
	object.modelLocation = glGetUniformLocation(object.program, "model");
	object.viewLocation = glGetUniformLocation(object.program, "view");
	object.projectionLocation = glGetUniformLocation(object.program, "projection");
    object.finalBoneMatricesLocation = glGetUniformLocation(object.program, "finalBonesMatrices[0]");
	object.shadowLocation = glGetUniformLocation(object.program, "isShadow");
    object.textureLocation = glGetUniformLocation(object.program, "isTexture");
    
    // Clean up
    glUseProgram(0);
    glBindVertexArray(0);
};


void MeshPainter::bindLightAndMaterial( TriMesh* mesh, openGLObject &object, Light* light, Camera* camera ) {
    // 传递材质、光源等数据给着色器
    
    // 传递相机的位置
    glUniform3fv(glGetUniformLocation(object.program, "eye_position"), 1, &camera->eye[0]);

    // 传递物体的材质
    glm::vec4 meshAmbient = mesh->getAmbient();
    glm::vec4 meshDiffuse = mesh->getDiffuse();
    glm::vec4 meshSpecular = mesh->getSpecular();
    float meshShininess = mesh->getShininess();

    glUniform4fv(glGetUniformLocation(object.program, "material.ambient"), 1, &meshAmbient[0]);
    glUniform4fv(glGetUniformLocation(object.program, "material.diffuse"), 1, &meshDiffuse[0]);
    glUniform4fv(glGetUniformLocation(object.program, "material.specular"), 1, &meshSpecular[0]);
    glUniform1f(glGetUniformLocation(object.program, "material.shininess"), meshShininess);

    // 传递光源信息
    glm::vec4 lightAmbient = light->getAmbient();
    glm::vec4 lightDiffuse = light->getDiffuse();
    glm::vec4 lightSpecular = light->getSpecular();
    glm::vec3 lightPosition = light->getTranslation();
    glUniform4fv(glGetUniformLocation(object.program, "light.ambient"), 1, &lightAmbient[0]);
    glUniform4fv(glGetUniformLocation(object.program, "light.diffuse"), 1, &lightDiffuse[0]);
    glUniform4fv(glGetUniformLocation(object.program, "light.specular"), 1, &lightSpecular[0]);
    glUniform3fv(glGetUniformLocation(object.program, "light.position"), 1, &lightPosition[0]);

    glUniform1f(glGetUniformLocation(object.program, "light.constant"), light->getConstant());
    glUniform1f(glGetUniformLocation(object.program, "light.linear"), light->getLinear());
    glUniform1f(glGetUniformLocation(object.program, "light.quadratic"), light->getQuadratic());
}


void MeshPainter::addMesh( TriMesh* mesh, const std::string &name, const std::string &vshader, const std::string &fshader ){
	mesh_names.push_back(name);
    meshes.push_back(mesh);

    openGLObject object;
    // 绑定openGL对象，并传递顶点属性的数据
    bindObjectAndData(mesh, object, vshader, fshader);

    opengl_objects.push_back(object);
};


void MeshPainter::drawShadow(TriMesh *mesh, openGLObject &object, Light *light, Camera *camera) {
    // 相机矩阵计算
    camera->viewMatrix = camera->getViewMatrix();
    camera->projMatrix = camera->getProjectionMatrix(false);
    // 物体的变换矩阵
    glm::mat4 modelMatrix = mesh->getModelMatrix();
    // 计算阴影矩阵
    glm::mat4 shadowMatrix = light->getShadowProjectionMatrix() ;
    glBindVertexArray(object.vao);
    glUseProgram(object.program);
    // 绘制阴影
    glUniform1i(object.shadowLocation, 1);
    glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(object.viewLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]);
    glUniformMatrix4fv(object.projectionLocation, 1, GL_FALSE, &camera->projMatrix[0][0]);
    glUniformMatrix4fv(object.shadowMatrixLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
    glBindVertexArray(0);

    glUseProgram(0);
}
void MeshPainter::drawMeshWithAnimator(TriMesh *mesh, Animator *animator, openGLObject &object,
                                       Light *light, Camera *camera) {

    // 相机矩阵计算
	//camera->updateCamera();
	camera->viewMatrix = camera->getViewMatrix();
	camera->projMatrix = camera->getProjectionMatrix(false);


	glBindVertexArray(object.vao);
	glUseProgram(object.program);

	// 物体的变换矩阵
    glm::mat4 modelMatrix = mesh->getModelMatrix();
    glm::mat4 shadowMatrix = glm::mat4 (1.0f);
	// 传递矩阵
    glUniformMatrix4fv(object.shadowMatrixLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
	glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(object.viewLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]);
	glUniformMatrix4fv(object.projectionLocation, 1, GL_FALSE, &camera->projMatrix[0][0]);
    // 获取object.finalBoneMatricesLocation中元素的详细信息
    GLsizei length;
    GLint size;
    GLenum type;
    GLchar name[256];
    glGetActiveUniform(object.program, object.finalBoneMatricesLocation, 256, &length, &size, &type, name);

    auto boneMatrices = animator->GetFinalBoneMatrices();
    for(int i = 0; i < boneMatrices.size(); i++) {

        glUniformMatrix4fv(object.finalBoneMatricesLocation + i, 1, GL_FALSE, &boneMatrices[i][0][0]);
    }
	// 将着色器 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
	glUniform1i(object.shadowLocation, 0);
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int heightNr = 1;
    if(mesh->getTexturesMessage().size() == 0) {
        glUniform1i(object.textureLocation, 0);
    }
    else{
        glUniform1i(object.textureLocation, 1);
        for (unsigned int i = 0; i < mesh->getTexturesMessage().size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
            // 获取纹理序号（N）
            std::string number;
            std::string name = mesh->getTexturesMessage()[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);
            glUniform1i(glGetUniformLocation(object.program, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, mesh->getTexturesMessage()[i].id);
        }
    }
    glActiveTexture(GL_TEXTURE0);
    
	// 将材质和光源数据传递给着色器
	bindLightAndMaterial(mesh, object, light, camera);
	// 绘制
	glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());
	glBindVertexArray(0);
	glUseProgram(0);
};
void MeshPainter::drawMesh(TriMesh *mesh, openGLObject &object, Light *light, Camera *camera) {
    // 相机矩阵计算
    //camera->updateCamera();
    camera->viewMatrix = camera->getViewMatrix();
    camera->projMatrix = camera->getProjectionMatrix(false);


    glBindVertexArray(object.vao);
    glUseProgram(object.program);

    // 物体的变换矩阵
    glm::mat4 modelMatrix = mesh->getModelMatrix();
    glm::mat4 shadowMatrix = glm::mat4 (1.0f);
    // 传递矩阵
    glUniformMatrix4fv(object.shadowMatrixLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
    glUniformMatrix4fv(object.modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(object.viewLocation, 1, GL_FALSE, &camera->viewMatrix[0][0]);
    glUniformMatrix4fv(object.projectionLocation, 1, GL_FALSE, &camera->projMatrix[0][0]);
    auto boneMatrices = model->getBoneInfoMap();
    for(auto BoneInfo : boneMatrices) {
        glm::mat4 offset = BoneInfo.second.offset;
        glm::mat4 globalTransform = model->getBoneTransformMap()[BoneInfo.first];
        glm::mat4 matrix = globalTransform * offset;
        std::string name = "finalBonesMatrices[" + std::to_string(BoneInfo.second.id) + "]";
        glUniformMatrix4fv(glGetUniformLocation(object.program, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
    }
    // 将着色器 isShadow 设置为0，表示正常绘制的颜色，如果是1着表示阴影
    glUniform1i(object.shadowLocation, 0);
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int heightNr = 1;
    if(mesh->getTexturesMessage().size() == 0) {
        glUniform1i(object.textureLocation, 0);
    }
    else{
        glUniform1i(object.textureLocation, 1);
        for (unsigned int i = 0; i < mesh->getTexturesMessage().size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
            // 获取纹理序号（N）
            std::string number;
            std::string name = mesh->getTexturesMessage()[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);
            glUniform1i(glGetUniformLocation(object.program, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, mesh->getTexturesMessage()[i].id);
        }
    }

    glActiveTexture(GL_TEXTURE0);

    // 将材质和光源数据传递给着色器
    bindLightAndMaterial(mesh, object, light, camera);

    // 绘制
    glDrawArrays(GL_TRIANGLES, 0, mesh->getPoints().size());

    glBindVertexArray(0);

    glUseProgram(0);
}

void MeshPainter::drawModelWithAnimator(Animator *animator, Light *light, Camera *camera) {
    // 更新mesh的model矩阵
    model->updateModelMatrix();
    // 绘制多个物体
    for (int i = 0; i < meshes.size(); i++)
    {
        drawMeshWithAnimator(meshes[i],animator,opengl_objects[i], light, camera);
        drawShadow(meshes[i],opengl_objects[i],light,camera);
    }
};

void MeshPainter::cleanMeshes(){
    // 将数据都清空释放
    mesh_names.clear();

    for (int i = 0; i < meshes.size(); i++)
    {
        glDeleteVertexArrays(1, &opengl_objects[i].vao);
        glDeleteBuffers(1, &opengl_objects[i].vbo);
        glDeleteProgram(opengl_objects[i].program);
    }

    meshes.clear();
    opengl_objects.clear();
};



std::vector<Texture> MeshPainter::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                          std::string typeName,std::string file_path)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), file_path);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}
GLuint MeshPainter::TextureFromFile(const char *path, const std::string &directory)
{
    std::string filename = std::string(path);
    // 截取filename，从第一个为字母的字符开始
    int index = filename.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    filename = filename.substr(index);
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height,
                                    &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }
        else
        {
            std::cout << "ERROR::TEXTURE::UNKNOWN_FORMAT" << std::endl;
            exit(0);
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width,
                     height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // 设置纹理环绕方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_REPEAT);
        // 设置纹理过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path
                  << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

void MeshPainter::drawModel(Light *light, Camera *camera) {
    // 更新mesh的model矩阵
    model->updateModelMatrix();
    // 绘制多个物体
    for (int i = 0; i < meshes.size(); i++)
    {
        drawMesh(meshes[i],opengl_objects[i], light, camera);
        drawShadow(meshes[i],opengl_objects[i],light,camera);
    }
}

unsigned int MeshPainter::CubeTexture(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    unsigned char *data;
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    // 设置纹理环绕方式
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

int MeshPainter::BindSkyBox(std::vector<std::string> faces) {
    openGLObject object;
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    for(int i = 0; i < 36; i++) {
        skyboxVertices[i * 3] *= 80;
        skyboxVertices[i * 3 + 1] *= 80;
        skyboxVertices[i * 3 + 2] *= 80;
    }
    glGenVertexArrays(1, &object.vao);
    glBindVertexArray(object.vao);
    glGenBuffers(1, &object.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    openGLObject::program2 = InitShader("shaders/cubevshader.glsl", "shaders/cubefshader.glsl");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),(void*)0);
    opengl_objects.push_back(object);
    GLuint texture = CubeTexture(faces);
    return texture;
}
void MeshPainter::drawSkyBox(Camera *camera, openGLObject &object, GLuint texture) {

    glUseProgram(object.program2);
    glUniform1i(glGetUniformLocation(object.program2, "skybox"), 0);
    glDepthFunc(GL_LEQUAL);
    glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix()));
    glm::mat4 projection = camera->getProjectionMatrix(false);
    glUniformMatrix4fv(glGetUniformLocation(object.program2, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(object.program2, "projection"), 1, GL_FALSE, &projection[0][0]);
    glBindVertexArray(object.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glUseProgram(0);
    glDepthFunc(GL_LESS);

}

void MeshPainter::drawModelWithoutShadow(Light *light, Camera *camera) {
    // 更新mesh的model矩阵
    model->updateModelMatrix();
    // 绘制多个物体
    for (int i = 0; i < meshes.size(); i++)
    {
        drawMesh(meshes[i],opengl_objects[i], light, camera);
    }
}





