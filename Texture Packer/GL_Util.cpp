#include <glad/glad.h>

#include <glm\gtc\type_ptr.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "GL_Util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
typedef GLuint Shader;

bool test_strings_start_equals_string_internal(std::string start_test, std::string string)
{
	if (start_test.length() > string.length())
		return false;
	for (int i = 0; i < start_test.length(); i++)
		if (start_test[i] != string[i])
			return false;
	return true;
}

std::string convert_core_to_es_internal(std::string input)
{
	std::string to_delete = "#version 300 core";
	std::string to_replace = "#version 300 es";
	int len = (int)to_delete.length() + 2;
	std::string result = to_replace + input.substr(17, std::string::npos);
	return result;
}

std::string convert_char_core_to_es_internal(const char* input)
{
	std::string temp = std::string(input);
	return convert_core_to_es_internal(temp);
}

Shader shader_compile_loaded_program(const char* vertexDataTemp, const char* fragmentDataTemp, const char* geometryData = nullptr)
{
	printf("NARROWED BUG DOWN TO THIS SEGMENT OF CODE!");
#ifdef EMSCRIPTEN
	std::string vertexDatastr = convert_char_core_to_es_internal(vertexDataTemp);
	std::string fragmentDatastr = convert_char_core_to_es_internal(fragmentDataTemp);
	const char* vertexData = vertexDatastr.c_str();
	const char* fragmentData = fragmentDatastr.c_str();
#else
	const char* vertexData = vertexDataTemp;
	const char* fragmentData = fragmentDataTemp;
#endif 
	//std::cout << vertexData << std::endl;
	//std::cout << fragmentData << std::endl;

	//create the shaders.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::cout << "glShaderSource VERT CREATE" << glGetError() << std::endl; // returns 0 (no error)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::cout << "glShaderSource FRAG CREATE" << glGetError() << std::endl; // returns 0 (no error)

	//compile the vertex shader.
	glShaderSource(vertexShader, 1, &vertexData, NULL);
	std::cout << "glShaderSource VERT LOAD" << glGetError() << std::endl; // returns 0 (no error)
	glCompileShader(vertexShader);
	std::cout << "glShaderSource VERT COMPILE" << glGetError() << std::endl; // returns 0 (no error)

	GLint success;
	char infoLog[1024];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	std::cout << "glGetShaderiv VERT" << glGetError() << std::endl; // returns 0 (no error)
	if (!success) {
		glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		std::cout << infoLog << std::endl;
		abort();
	}
	else
	{
		std::cout << "successfully attached vertex shader" << std::endl;
	}


	glShaderSource(fragmentShader, 1, &fragmentData, NULL);
	std::cout << "glShaderSource FRAG LOAD" << glGetError() << std::endl; // returns 0 (no error)
	glCompileShader(fragmentShader);
	std::cout << "glShaderSource FRAG COMPILE" << glGetError() << std::endl; // returns 0 (no error)

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		std::cout << infoLog << std::endl;
		abort();
	}
	else
	{
		std::cout << "successfully attached fragment shader" << std::endl;
	}

	Shader result = glCreateProgram();
	std::cout << "glCreateProgram: " << glGetError() << std::endl; // returns 0 (no error)
	glAttachShader(result, vertexShader);
	std::cout << "glAttachShader VERT: " << glGetError() << std::endl; // returns 0 (no error)
	glAttachShader(result, fragmentShader);
	std::cout << "glAttachShader FRAG: " << glGetError() << std::endl; // returns 0 (no error)
	glLinkProgram(result);
	std::cout << "LINK: " << glGetError() << std::endl; // returns 0 (no error)
	glGetProgramiv(result, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(result, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
		std::cout << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return result;
}

Shader shader_compile_program(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	std::string vertexString = resource_load_text_file(vertexPath);
	const char* vertex = vertexString.c_str();
	std::string fragmentString = resource_load_text_file(fragmentPath);
	const char* frag = fragmentString.c_str();
	const char* geo = nullptr;
	if (geometryPath != nullptr)
		geo = resource_load_text_file(geometryPath).c_str();
	//std::cout << vertexString << std::endl;
	//std::cout << fragmentString << std::endl;
	return shader_compile_loaded_program(vertex, frag, geo);

}
void shader_use(Shader shader)
{
	glUseProgram(shader);
}
void shader_set_uniform_int(Shader shader, const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(shader, name.c_str()), value);
}
void shader_set_uniform_float(Shader shader, const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(shader, name.c_str()), value);
}
void shader_set_uniform_vec2(Shader shader, const std::string& name, glm::vec2 value)
{
	glUniform2f(glGetUniformLocation(shader, name.c_str()), value.x, value.y);
}
void shader_set_uniform_vec3(Shader shader, const std::string& name, glm::vec3 value)
{
	glUniform3f(glGetUniformLocation(shader, name.c_str()), value.x, value.y, value.z);
}
void shader_set_uniform_vec4(Shader shader, const std::string& name, glm::vec4 value)
{
	glUniform4f(glGetUniformLocation(shader, name.c_str()), value.x, value.y, value.z, value.w);
}
void shader_set_uniform_mat2(Shader shader, const std::string& name, glm::mat2 value)
{
	glUniformMatrix2fv(glGetUniformLocation(shader, name.c_str()), 1, false, glm::value_ptr(value));
}
void shader_set_uniform_mat3(Shader shader, const std::string& name, glm::mat3 value)
{
	glUniformMatrix3fv(glGetUniformLocation(shader, name.c_str()), 1, false, glm::value_ptr(value));
}
void shader_set_uniform_mat4(Shader shader, const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(shader, name.c_str()), 1, false, glm::value_ptr(value));
}

std::string resource_load_text_file(std::string filePath)
{
	std::ifstream myFile(filePath);
	std::stringstream myStream;
	myStream << myFile.rdbuf();
	myFile.close();
	std::string result = myStream.str();
	return result;
}
void resource_write_text_file(std::string text, std::string filePath)
{
	std::ofstream myFile(filePath);
	myFile << text;
	myFile.close();
}
void save_gpu_image_as_png_file(GLuint shader,int w, int h, const char* save_file_path)
{
	//TODO: get all the data using glReadPixels.
	GLvoid* data = malloc(w * h * 4);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//TODO: save that data using stbi 
	stbi_flip_vertically_on_write(true);
	int result = stbi_write_png(save_file_path, w, h, 4, data, w * 4);
	if (result == 0)
		std::cout << "image save had problem" << std::endl;
}
unsigned int resource_load_image_from_file_onto_gpu(std::string file_path, int* width_in_pixels_OUT,int* height_in_pixels_OUT)
{
	std::string asset_file_path =  file_path;
	unsigned int result;
	glGenTextures(1, &result); CHK
	glEnable(GL_BLEND); CHK


	glBindTexture(GL_TEXTURE_2D, result); CHK

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); CHK
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); CHK

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); CHK
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CHK

	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

	int width;
	int height;
	int num_of_channels;
	unsigned char* data = stbi_load(asset_file_path.c_str(), &width, &height, &num_of_channels, 0);
	
	GLuint format = GL_R8;
	if (num_of_channels == 1)
	{
		format = GL_R8;
	}
	if (num_of_channels == 3)
	{
		format = GL_RGB;
	}
	if (num_of_channels == 4)
	{
		format = GL_RGBA;
	}
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); CHK
		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Texture load failed, name of file:" << file_path.c_str() << "looks like the programs gonna crash now" << std::endl;
	}
	stbi_image_free(data);
	
	*width_in_pixels_OUT = width;
	*height_in_pixels_OUT = height;
	return result;
}


void gl_check_err(const char* filename, int line)
{
	int error_check = glGetError();
	if (error_check != 0)
	{
		std::cout << "we have produced an openGL error on line" << line << " , in file " << filename << "better check it out." << error_check << std::endl;
		abort();
	}
}
