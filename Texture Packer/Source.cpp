#include <SDL.h>
#include <iostream>
#include <glad/glad.h> 
#include <fstream>
#include <iostream>
#include <filesystem>
#include "GL_Util.h"
#include "Parse.h"
namespace fs = std::filesystem;

const int ATLAS_WIDTH = 1024;
const int ATLAS_HEIGHT = 1024;
const int SCREEN_STARTING_WIDTH = 1024;
const int SCREEN_STARTING_HEIGHT = 1024;
const int FONT_ATLAS_WIDTH = 1024;
const int FONT_ATLAS_HEIGHT = 1024;

struct Square
{
	float x, y, width, height;
};
Square make_square(float x, float y, float width, float height) { Square result; result.x = x; result.y = y; result.width = width; result.height = height; return result; }

int main(int argc, char* argv[])
{
	//setup SDL
	{

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_WINDOW_RESIZABLE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		//SDL_GL_SetSwapInterval(0);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
		//int numBuffers;
		//int numSamples;
		//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &numBuffers);
		//SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &numSamples);
		//std::cout << "Num buffers" << numBuffers << "Num Samples" << numSamples << std::endl;


		SDL_GLContext mainContext;
		auto window = SDL_CreateWindow("TexturePacker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_STARTING_WIDTH, SCREEN_STARTING_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		mainContext = SDL_GL_CreateContext(window);
		if (mainContext == NULL)
		{
			printf("we have failed to initialise the gl context");
		}
		auto rdr = SDL_CreateRenderer(
			window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			return -1;
		}
	}

	unsigned int vertices_VBO = 0;
	unsigned int vertices_EBO = 0;
	unsigned int VAO = 0;
	Shader shader;
	//setup the vertices VBO, EBO, and the shader we will be using to draw. 
	{
		float vertices[] = {
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			};
		unsigned int atlas[] = {
			0,
			1,
			2,
			3,
			4,
			5
		};
		unsigned int indices[] = {
			0,1,3,
			1,2,3
		};
		
		glEnable(GL_DEPTH_TEST); CHK
		glDepthFunc(GL_LESS); CHK
		glEnable(GL_BLEND); CHK
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); CHK

		glGenBuffers(1, &vertices_VBO); CHK
		glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO); CHK
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); CHK


		glGenBuffers(1, &vertices_EBO); CHK
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO); CHK
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW); CHK

		//generate the shader.
		shader = shader_compile_program("text.vs", "sprite.f");
		glUseProgram(shader);

		//generate the VAO we will be using to draw every texture to the renderbuffer
		glGenVertexArrays(1, &VAO); CHK
		glBindVertexArray(VAO); CHK

		glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO); CHK
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_EBO); CHK
		
		GLint position = 0;
		glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); CHK
		glEnableVertexAttribArray(position); CHK

		GLint texcoord = 1;
		glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); CHK
		glEnableVertexAttribArray(texcoord); CHK
	}


	//generate the render buffer we will be drawing to, and set up the VAO we will be using to draw.
	unsigned int fbo;
	unsigned int texture;
	{
		glGenFramebuffers(1, &fbo); CHK
		glBindFramebuffer(GL_FRAMEBUFFER, fbo); CHK

		//create and attach a texture that we will be drawing too.
		glGenTextures(1, &texture); CHK
		glBindTexture(GL_TEXTURE_2D, texture); CHK
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); CHK
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); CHK
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CHK
	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0); CHK
		glViewport(0, 0, ATLAS_WIDTH, ATLAS_HEIGHT); CHK
		if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "uh oh, frame buffer failed real bad :(" << std::endl;
			abort();
		}
		else
		{
			std::cout << "frame buffer is happy :)" << std::endl;
		}


	}


	fs::path folder_names[16];
	std::string folder_name_str[16];
	int num_images_in_folder[16];
	std::string image_name_str[16][1024];
	glm::vec4 image_position_str[16][1024];
	int number_folders = 0;
	//grab the folder names
	{
		for (auto& p : fs::directory_iterator("assets"))
		{
			auto path = p.path();
			if (std::filesystem::is_directory(path))
			{
				folder_names[number_folders] = path;
				std::string path_name = path.string();
				const char* path_c_style_name = path_name.c_str();
				char* name = ParseFileName(path_c_style_name);
				folder_name_str[number_folders] = std::string(name);
				number_folders++;
			}
		}
	}
	//perform the algorithm on each sub-folder we've found.
	
	for (int z = 0; z < number_folders; z++)
	{
		unsigned int texture_id[1024];
		int texture_width[1024];
		int texture_height[1024];
		int current_num_textures = 0;
		{
			for (auto& p : fs::directory_iterator(folder_names[z]))
			{
				auto path = p.path();
				{
					texture_id[current_num_textures] = resource_load_image_from_file_onto_gpu(
						path.string().c_str(),
						&texture_width[current_num_textures],
						&texture_height[current_num_textures]);
					char* name = ParseFileName(path.string().c_str());
					image_name_str[z][current_num_textures] = std::string(name);
					current_num_textures++;
				}
			}
			num_images_in_folder[z] = current_num_textures;
		}

		//draw all of our texture to our texture atlas render buffer, saving the positions they are drawn to for later usage. 
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); CHK
			glClear(GL_COLOR_BUFFER_BIT); CHK
		{
			int current_width_written = 0;
			int current_height_written = 0;
			int current_rows_height = texture_height[0];
			for (int i = 0; i < current_num_textures; i++)
			{
				//1 calculate where we want to write the character too.
				int write_width = texture_width[i] + 2;
				bool room_to_write = write_width + current_width_written < FONT_ATLAS_WIDTH;
				if (!room_to_write)
				{
					current_width_written = 0;
					current_height_written += current_rows_height + 2;
					current_rows_height = texture_height[i];
				}
				//2: store the position we will write the character too.
				image_position_str[z][i] = glm::vec4(current_width_written + 1, current_height_written + 1, texture_width[i], texture_height[i]);

				//3: calculate in openGL coordinates where we will be drawing the character. on the texture atlas.
				glm::vec4 normalized;
				normalized = image_position_str[z][i];
				normalized.x = image_position_str[z][i].x / ATLAS_WIDTH;
				normalized.y = image_position_str[z][i].y / ATLAS_HEIGHT;
				normalized.z = image_position_str[z][i].z / ATLAS_WIDTH;
				normalized.w = image_position_str[z][i].w / ATLAS_HEIGHT;

				//4: draw the character.
				{
					unsigned int to_draw = texture_id[i];
					glBindTexture(GL_TEXTURE_2D, to_draw); CHK
					glm::vec4 vec_draw = *(glm::vec4*) & normalized;
						//	glm::vec4 vec_draw = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
					shader_set_uniform_vec4(shader, "offset", vec_draw); CHK
					float to_add_w = 1.0 / texture_width[i];
					float to_add_h = 1.0 / texture_height[i];
					float vertices_next[] = { 1.0f + to_add_w, 1.0f + to_add_h, 0.0f, 1.0f + to_add_w, 1.0f + to_add_h,
						1.0f + to_add_w, 0.0f - to_add_h, 0.0f, 1.0f + to_add_w, 0.0f - to_add_h,
						0.0f - to_add_w,  0.0f - to_add_h, 0.0f, 0.0f - to_add_w, 0.0f - to_add_h,
						0.0f - to_add_w , 1.0f + to_add_h, 0.0f, 0.0f - to_add_w, 1.0f + to_add_h};
						glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_next), vertices_next);
						current_width_written += write_width;
						current_rows_height = current_rows_height > image_position_str[z][i].w ? current_rows_height : image_position_str[z][i].w;
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); CHK

					}


				}
		}

		//TODO: extract out the texture we have drawn too on the gpu to the cpu, and then write it out to file.
		save_gpu_image_as_png_file(texture, ATLAS_WIDTH, ATLAS_HEIGHT, (("result\\" + folder_name_str[z] + ".png").c_str()));

	}
	
	//convert all the positions from (x,y,w,h) in format 0 to 1024, to format (x,y,x2,y2), in format 0 to 1.
	for(int folder_num = 0; folder_num < number_folders; folder_num++)
	{
		for (int i = 0; i < num_images_in_folder[folder_num]; i++)
		{
			glm::vec4* ele = &image_position_str[folder_num][i];
			ele->z += ele->x;
			ele->w += ele->y;

			ele->x = ele->x / ATLAS_WIDTH;
			ele->y = ele->y / ATLAS_HEIGHT;
			ele->z = ele->z / ATLAS_WIDTH;
			ele->w = ele->w / ATLAS_HEIGHT;
		}
	}
	//write our finished folders out!
	{
		char* to_write = (char*)malloc(1000000); //allocate a large number.
		char* front = to_write;

		write_string(&front, "namespace textureAssets {\n");
		for (int folder_num = 0; folder_num < number_folders; folder_num++)
		{
			//write enum { val1,val2,...valn} 
			bool write = num_images_in_folder[folder_num] > 0;
			if (write)
			{
				const char* folder_name = folder_name_str[folder_num].c_str();
				char* caps_folder_name = (char*)malloc(folder_name_str[folder_num].length() + 1);
				{
					int i;
					for (i = 0; i < folder_name_str[i].length(); i++)
						caps_folder_name[i] = std::toupper(folder_name[i]);
					caps_folder_name[i] = '\0';
				}
				write_string(&front, "enum ");
				write_string(&front, folder_name);
				write_string(&front, "\n");
				write_string(&front, "{\n    ");
				write_string(&front, image_name_str[folder_num][0].c_str());
			}
			for (int z = 1; z < num_images_in_folder[folder_num]; z++)
			{
				write_string(&front, ",\n    ");
				write_string(&front, image_name_str[folder_num][z].c_str());
			}
			if (write)
				write_string(&front, "\n};\n\n");

			if (write)
			{
				write_string(&front, "\n");
				write_string(&front, "glm::vec4* ");
				write_string(&front, folder_name_str[folder_num].c_str());
				write_string(&front, "_positions() {\n");
				write_string(&front, "glm::vec4 result[] = { ");
				{
					write_string(&front, "\n    glm::vec4(");
					write_float(&front, image_position_str[folder_num][0].x);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][0].y);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][0].z);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][0].w);
					write_string(&front, ")");
				}
				for (int j = 1; j < num_images_in_folder[folder_num]; j++)
				{
					write_char(&front, ',');
					write_char(&front, '\n');
					write_string(&front, "    glm::vec4(");
					write_float(&front, image_position_str[folder_num][j].x);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][j].y);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][j].z);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][j].w);
					write_string(&front, ")");
				}
				write_string(&front, "\n};\n\n");
				write_string(&front, "return result;\n");
				write_string(&front, "}\n");
			}
			//write glm::vec4 FOLDER_NAME_POSITIONS[] = { image_pos1,image_pos2,...., image_pos_n}
		}
		write_string(&front, "\n}\n");
		write_char(&front, '\0');
		//resource_write_text_file(to_write, "result//textureAssets.h");
	}

	//write the .cpp file
	{
		char* to_write = (char*)malloc(1000000); //allocate a large number.
		char* front = to_write;
		write_string(&front, "#include \"Memory.h\"\n");
		write_string(&front, "#include \"textureAssets.h\"\n");
		for (int folder_num = 0; folder_num < number_folders; folder_num++)
		{
			//write enum { val1,val2,...valn} 
			bool write = num_images_in_folder[folder_num] > 0;
			if (write)
			{
				const char* folder_name = folder_name_str[folder_num].c_str();
				char* caps_folder_name = (char*)malloc(folder_name_str[folder_num].length() + 1);
				{
					int i;
					for (i = 0; i < folder_name_str[i].length(); i++)
						caps_folder_name[i] = std::toupper(folder_name[i]);
					caps_folder_name[i] = '\0';
				}
				write_string(&front, "\n");
				write_string(&front, "glm::vec4* textureAssets::");
				write_string(&front, folder_name_str[folder_num].c_str());
				write_string(&front, "_positions(Memory* memory) {\n");
				write_string(&front, "glm::vec4* result = (glm::vec4*) memory_alloc(memory,sizeof(glm::vec4) * ");
				write_int(&front,num_images_in_folder[folder_num]);
				write_string(&front, ");");
				{
					write_string(&front, "\n result[0] = ");
					write_string(&front, "glm::vec4(");
					write_float(&front, image_position_str[folder_num][0].x);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][0].y);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][0].z);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][0].w);
					write_string(&front, ");");
				}
				for (int j = 1; j < num_images_in_folder[folder_num]; j++)
				{
					write_string(&front, "\n result[");
					write_int(&front, j);
					write_string(&front, "] = glm::vec4(");
					write_float(&front, image_position_str[folder_num][j].x);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][j].y);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][j].z);
					write_char(&front, ',');
					write_float(&front, image_position_str[folder_num][j].w);
					write_string(&front, ");");
				}
				write_string(&front, "return result;\n");
				write_string(&front, "}\n");
			}
		}
		write_char(&front, '\0');
		resource_write_text_file(to_write, "result//textureAssets.cpp");
	}
	//write the .h file
	{
		char* to_write = (char*)malloc(1000000); //allocate a large number.
		char* front = to_write;
		write_string(&front, "#pragma once\n#include \"Math.h\"\n"); 
		write_string(&front, "namespace textureAssets {\n");
		for (int folder_num = 0; folder_num < number_folders; folder_num++)
		{
			bool write = num_images_in_folder[folder_num] > 0;
			if (write)
			{
				{
					const char* folder_name = folder_name_str[folder_num].c_str();
					char* caps_folder_name = (char*)malloc(folder_name_str[folder_num].length() + 1);
					{
						int i;
						for (i = 0; i < folder_name_str[i].length(); i++)
							caps_folder_name[i] = std::toupper(folder_name[i]);
						caps_folder_name[i] = '\0';
					}
					write_string(&front, "enum ");
					write_string(&front, folder_name);
					write_string(&front, "\n");
					write_string(&front, "{\n    ");
					write_string(&front, image_name_str[folder_num][0].c_str());
				}
				for (int z = 1; z < num_images_in_folder[folder_num]; z++)
				{
					write_string(&front, ",\n    ");
					write_string(&front, image_name_str[folder_num][z].c_str());
				}
				if (write)
					write_string(&front, "\n};\n\n");
				

				write_string(&front, "glm::vec4* ");
				write_string(&front, folder_name_str[folder_num].c_str());
				write_string(&front, "_positions(Memory* memory);\n");

			}
		}
		write_string(&front, "}");
		write_char(&front, '\0');
		resource_write_text_file(to_write, "result//textureAssets.h");
	}
	return 0;
}

//okay so what do we need to do at long last?
//1: run this dang piece of code.
//2: copy and paste the art assets (.png files) into the assets folder.
//3: copy and paste the c++ program file 

