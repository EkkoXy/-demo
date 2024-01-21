#version 330 core

// 顶点着色器
in vec3 vPosition;
in vec3 vColor;
in vec3 vNormal;
in vec2 vTexture;
in ivec4 boneIds;
in vec4 weights;
// 传给片元着色器的变量
out vec3 position;
out vec3 normal;
out vec2 texCoord;
out vec3 color;
out float discardFlag;
uniform int isShadow;
// 模型变换矩阵、相机观察矩阵、投影矩阵
uniform mat4 ShadowMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 finalBonesMatrices[100];

const int MAX_BONES = 100;
void main() 
{
	discardFlag = 0;
	vec4 totalPosition = vec4(vPosition,1.0f);
	mat4 finalBoneMatrix = mat4(0.0f);
	normal = vNormal;
	for(int i = 0; i < 4; i++)
	{
		if(boneIds[i] == -1)
			continue;
		if(boneIds[i] >= 100)
		{
			finalBoneMatrix = mat4(0.0f);
			break;
		}
		finalBoneMatrix += finalBonesMatrices[boneIds[i]] * weights[i];
	}
	if(finalBoneMatrix == mat4(0.0f))
	{
		finalBoneMatrix = mat4(1.0f);
	}
	totalPosition = finalBoneMatrix * totalPosition;
	normal = mat3(finalBoneMatrix) * normal;
	// 归一化
	totalPosition /= totalPosition.w;
	normal = normalize(normal);
	vec4 v1 = model * totalPosition;
	if(v1.y < 0.0 && isShadow == 1)
	{
		discardFlag = 1;
	}
	v1 = ShadowMatrix * v1;
	// 由于model矩阵有可能为阴影矩阵，为了得到正确位置，我们需要做一次透视除法
	vec4 v2 = vec4(v1.xyz / v1.w, 1.0);
	// 考虑相机和投影
	vec4 v3 = projection* view * v2;
	
	gl_Position = v3;

    position = vec3(v2.xyz);
    normal = vec3( (model *  vec4(normal, 0.0)).xyz );
	texCoord = vTexture;
	color = vColor;
}

