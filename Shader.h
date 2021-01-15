#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	unsigned int ID;

	Shader() {
		const char* vertexShaderCode = "#version 410 core\n"
			"layout (location = 0) in vec3 aPos;"
			"layout (location = 1) in vec2 aTexCoord;"
			"layout (location = 2) in vec3 aNormal;"
			"out vec2 TexCoord;"
			"out vec3 FragPos;"
			"out vec3 Normal;"
			"uniform mat4 model;"
			"uniform mat4 view;"
			"uniform mat4 projection;"
			"void main(){"
			//clip V = projection M ¡¤ view M ¡¤ model M ¡¤ object V
			/*Remever that the order of matrix multiplication is reversed(we need to read matrix multiplication from right to left. ¡ç)*/
			"	gl_Position = projection * view * model * vec4(aPos, 1.0);"
			"	TexCoord = vec2(aTexCoord.x, aTexCoord.y);"
			"   FragPos = vec3(model * vec4(aPos, 1.0));"
			"	Normal = mat3(transpose(inverse(model))) * aNormal;"
			//inverse() is a costly operation for shader(since they have to be done on each vertex of the scene),
			//so try to avoid doing inverse in shader.
			//For an efficient application, we must calculate the normal matrix on the CPU and
			//send it to the shaders via uniform.(like mofel matrix)
			"}\0";
		const char* fragmentShaderCode = "#version 410 core\n"
			"struct Material {"
			"   vec3 ambient;"
			"   vec3 diffuse;"
			"   vec3 specular;"
			"   float shininess;"
			"};"
			"struct Light {"
			"   vec3 position;" 
			"   vec3 direction;"
			"   float cutOff;"
			"   float outerCutOff;"
			"   vec3 ambient;"
			"   vec3 diffuse;"
			"   vec3 specular;"
			"   float constant;"
			"   float linear;"
			"   float quadratic;"
			"};"
			"out vec4 FragColor;"
			"in vec2 TexCoord;"
			"in vec3 FragPos;"
			"in vec3 Normal;"
			"uniform sampler2D texture1;"
			"uniform sampler2D texture2;"
			"uniform sampler2D emission;"
			"uniform Material material;"
			"uniform Light light;"
			"uniform vec3 lightPos;"
			"uniform vec3 viewPos;"
			"void main() {"
			//check if lighting is inside the soptlight cone
			"	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);"
			"   vec3 show = step(vec3(1.0), vec3(1.0) - texture(texture2, TexCoord).rgb);"
			"   vec3 emission = texture(emission, TexCoord).rgb * show;"
			"   FragColor +=  vec4(emission, 1.0);"
			// ambient
			"   vec3 ambient = light.ambient * material.ambient;"
			// diffuse
			"   vec3 normal = normalize(Normal);"
			"	vec3 lightDir = normalize(lightPos - FragPos);"	
			"	float diff = max(dot(normal, lightDir), 0.0);"
			"   vec3 diffuse = light.diffuse * (diff * material.diffuse);"
			// specular
			"   vec3 viewDir = normalize(viewPos - FragPos);"
			"   vec3 reflectDir = reflect(-lightDir, normal);"
			/*-lightDir : we reverse its direction to get the correct reflect vector
			* The reflect function expects the first vector to point from the light
			* towards the fragment's position.(This depends on the order of subtraction earlier on
			* when we calculated the lightDir vector)*/
			"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);"
			"   vec3 specular = light.specular * spec * vec3(texture(texture2, TexCoord));"
			// spotlight (soft edges)
			"   float theta = dot(lightDir, normalize(-light.direction));"
			"   float epsilon = (light.cutOff - light.outerCutOff);"// cosine difference between the inner and the outer cone
			"   float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);"
			"   diffuse *= intensity;"
			"   specular *= intensity;"
			// attenuation
			"   float distance = length(light.position - FragPos);"
			"   float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));"
			"   ambient *= attenuation;"
			"   diffuse *= attenuation;"
			"   specular *= attenuation;"	
			"   FragColor *= vec4(ambient + diffuse + specular, 1.0);"
			"}\0";
		//¡Ù2. compile shaders
		unsigned int vertexShader, fragmentShader;

		//vertex Shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
		glCompileShader(vertexShader);
		checkCompileError(vertexShader, "VERTEX");
		//fragment Shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
		glCompileShader(fragmentShader);
		checkCompileError(fragmentShader, "FRAGMENT");

		//shader Program 
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	//use/activate the shader
	void use()//activates the shader program
	{
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void setVec3(const std::string& name, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void setVec4(const std::string& name, const glm::vec4& value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const std::string& name, const glm::mat3& mat) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const std::string& name, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}


private://check shader compile/link errors
	void checkCompileError(GLuint shader, std::string type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			//print compile errors if any
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "¡ÚSHADER_COMPILATION_ERROR: " << type << "\n" << infoLog << "\n---------------------------------------------------" << std::endl;
			}
			else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "¡ÚPROGRAM_LINKING_ERROR: " << type << "\n" << infoLog << "\n---------------------------------------------------" << std::endl;
				}
			}
		}

	}
};
#endif // !SHADER_H