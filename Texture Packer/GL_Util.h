#pragma once
#include<glm/glm.hpp>
#include <iostream>
#include <glad/glad.h>
#include <string>


//SHADER UTIL STUFF.
typedef GLuint Shader;

Shader shader_compile_program(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
void shader_use(Shader shader);	//just a wrapping for gl_use for util.

void shader_set_uniform_int(Shader shader, const std::string& name, int value);
void shader_set_uniform_float(Shader shader, const std::string& name, float value);
void shader_set_uniform_vec2(Shader shader, const std::string& name, glm::vec2 value);
void shader_set_uniform_vec3(Shader shader, const std::string& name, glm::vec3 value);
void shader_set_uniform_vec4(Shader shader, const std::string& name, glm::vec4 value);
void shader_set_uniform_mat2(Shader shader, const std::string& name, glm::mat2 value);
void shader_set_uniform_mat3(Shader shader, const std::string& name, glm::mat3 value);
void shader_set_uniform_mat4(Shader shader, const std::string& name, glm::mat4 value);
void save_gpu_image_as_png_file(GLuint shader,int w, int h, const char* save_file_path);
//RESOURCE UTIL STUFF.

void resource_write_text_file(std::string text, std::string filePath);
std::string resource_load_text_file(std::string filePath);
unsigned int resource_load_image_from_file_onto_gpu(std::string file_path, int* width_in_pixels_OUT, int* height_in_pixels_OUT); //returns GLuint reference to gpu texture.


#define CHK do {gl_check_err(__FILE__,__LINE__);} while(0);

void gl_check_err(const char* filename, int line);

