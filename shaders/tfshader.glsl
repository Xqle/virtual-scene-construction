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
in vec3 color;
in vec2 texCoord;


// 相机坐标
uniform vec3 eye_position;
// 光源
uniform Light light;
// 物体材质
uniform Material material;

uniform int isShadow;
// 纹理数据
uniform sampler2D texture;

out vec4 fColor;

void main()
{
	if (isShadow == 1) {
		fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
		// 将顶点坐标、光源坐标和法向量转换到相机坐标系
		vec3 pos = (vec4(position, 1.0)).xyz;
		vec3 l_pos = (vec4(light.position, 1.0)).xyz;
		vec3 norm = (vec4(normal, 0.0)).xyz;
		vec3 eye = (vec4(eye_position, 1.0)).xyz;

		// @TODO: Task2 计算四个归一化的向量 N,V,L,R(或半角向量H)

		vec3 N = normalize(norm);
		vec3 V = normalize(eye_position - pos);
		vec3 L = normalize(l_pos - pos);
		vec3 H = normalize(L + V);
		/*
		vec3 N = normalize(vNormal);
		vec3 V = normalize(eye_position - vPosition);
		vec3 L = normalize(light.position - vPosition);
		vec3 H = normalize(L + V);
		*/

		// 环境光分量I_a
		vec4 I_a = light.ambient * material.ambient;

		// @TODO: Task2 计算漫反射系数alpha和漫反射分量I_d
		float diffuse_dot = max(dot(L, N), 0.0);
		vec4 I_d = diffuse_dot * light.diffuse * material.diffuse;

		// @TODO: Task2 计算高光系数beta和镜面反射分量I_s
		float s_t = pow(dot(N, H), material.shininess);
		float specular_dot_pow = max(s_t, 0.0);
		vec4 I_s = specular_dot_pow * light.specular * material.specular;


		// @TODO: Task2 计算高光系数beta和镜面反射分量I_s
		// 注意如果光源在背面则去除高光
		// if( dot(L, N) < 0.0 ) {
		//     I_s = vec4(0.0, 0.0, 0.0, 1.0);
		// } 

		// 合并三个分量的颜色，修正透明度
		vec4 phone = I_a + I_d + I_s;
		fColor = texture2D(texture, texCoord) * phone;
		// fColor = vec4(color, 1.0);
		// fColor = vec4(normal, 1.0);
		// fColor = vec4(normalize(position) , 1.0);
		// fColor = vec4(1.0/position.z, position.z, position.z , 1.0);
	}
}
