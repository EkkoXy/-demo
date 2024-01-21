#version 330 core

// 给光源数据一个结构体
struct Light{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	vec3 position;

    // 光源衰减系数的三个参数
    float constant; // 常数项
    float linear;	// 一次项
    float quadratic;// 二次项

};

// 给物体材质数据一个结构体
struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec3 color;
in float discardFlag;
// 相机坐标
uniform vec3 eye_position;
// 光源
uniform Light light;
uniform Light sun;
// 物体材质
uniform Material material;

uniform int isShadow;
uniform int isTexture;
// 纹理数据
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_height1;
uniform sampler2D texture_height2;
uniform sampler2D texture_height3;

out vec4 fColor;

void main()
{
	if (isShadow == 1) {
		if(discardFlag >= 0.9) {
			discard;
		}
		fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
		// 计算4个归一化向量
		vec3 N = normalize(normal);
		vec3 L = normalize(light.position - position);
		vec3 V = normalize(eye_position - position);
		vec3 H = normalize(L + V);
		// 环境光分量
		vec4 I_a = light.ambient;
		vec4 texAmbient = texture(texture_diffuse1, texCoord);
		if(isTexture == 1){
			I_a *= texAmbient;
		}else{
			I_a *= material.ambient;
			I_a *= vec4(color, 1.0);
		}
		// 漫反射分量
		vec4 I_d = light.diffuse * max(0.0, dot(N, L));
		vec4 texDiffuse = texture(texture_diffuse1, texCoord);
		if(isTexture == 1){
			I_d *= texDiffuse;
		}else{
			I_d *= material.diffuse;
			I_d *= vec4(color, 1.0);
		}
		// 镜面反射分量
		vec4 I_s = light.specular * pow(max(0.0, dot(N, H)), material.shininess);
		vec4 texSpecular = texture(texture_specular1, texCoord);
		if(isTexture == 1){
			I_s *= texSpecular;
		}else{
			I_s *= material.specular;
			I_s *= vec4(color, 1.0);
		}
		// 注意如果光源在背面则去除高光
		if (dot(N, L) < 0.0) {
			I_s = vec4(0.0, 0.0, 0.0, 1.0);
		}
		// 衰减计算
		float distance = length(light.position - position);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
		fColor = (I_a + I_d + I_s) * attenuation;

		// 太阳光
		N = normalize(normal);
		L = normalize(sun.position - position);
		V = normalize(eye_position - position);
		H = normalize(L + V);
		// 环境光分量
		I_a = sun.ambient;
		if(isTexture == 1){
			I_a *= texAmbient;
		}else{
			I_a *= material.ambient;
			I_a *= vec4(color, 1.0);
		}
		// 漫反射分量
		I_d = sun.diffuse * max(0.0, dot(N, L));
		if(isTexture == 1){
			I_d *= texDiffuse;
		}else{
			I_d *= material.diffuse;
			I_d *= vec4(color, 1.0);
		}
		// 镜面反射分量
		I_s = sun.specular * pow(max(0.0, dot(N, H)), material.shininess);
		if(isTexture == 1){
			I_s *= texSpecular;
		}else{
			I_s *= material.specular;
			I_s *= vec4(color, 1.0);
		}
		// 注意如果光源在背面则去除高光
		if (dot(N, L) < 0.0) {
			I_s = vec4(0.0, 0.0, 0.0, 1.0);
		}
		// 衰减计算
		distance = length(sun.position - position);
		attenuation = 1.0 / (sun.constant + sun.linear * distance + sun.quadratic * distance * distance);
		fColor += (I_a + I_d + I_s) * attenuation;
		fColor.a = 1.0;
	}
}
