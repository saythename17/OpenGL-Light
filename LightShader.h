#pragma once
#ifndef LIGHTSHADER_H
#define LIGHTSHADER_H

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class LightShader
{
public:
	unsigned int ID;

	LightShader() {
		const char* vertexShaderCode = "#version 410 core\n"
			"layout (location = 0) in vec3 aPos;"
			"uniform mat4 model;"
			"uniform mat4 view;"
			"uniform mat4 projection;"
			"void main(){"
			//clip V = projection M ¡¤ view M ¡¤ model M ¡¤ object V
			/*Remever that the order of matrix multiplication is reversed(we need to read matrix multiplication from right to left. ¡ç)*/
			"	gl_Position = projection * view * model * vec4(aPos, 1.0);"
			"}\0";
		const char* fragmentShaderCode = "#version 410 core\n"
			"out vec4 FragColor;"
			"void main() {"
			"	FragColor = vec4(1.0);" //set all 4 vector values to 1.0
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
				std::cout << "¡ÚSHADER_COMPILATION_ERROR: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
			else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "¡ÚPROGRAM_LINKING_ERROR: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
		}

	}
};
#endif // !LIGHTSHADER_H