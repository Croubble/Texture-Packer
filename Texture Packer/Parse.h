#pragma once
/*

				//func: extract out the title string into folder_name_str[current_num_textures]
				std::string path_name = path.string();
				const char* path_c_style_name = path_name.c_str();
				{
					const char* token = path_c_style_name;
					char string_build[128];
					int i = 0;
					while (token[0] != '\\' && token[1] != '\\')
					{
						token++;
					}
					token += 2;
					while (token[0])
					{
						string_build[i] = token[0];
						token++;
						i++;
					}
					string_build[i] = NULL;
					folder_name_str[number_folders] = std::string(string_build);
				}

				number_folders++;
*/
bool parse_string_folder(const char* to_parse)
{
	while (to_parse[0])
	{
		if (to_parse[0] == '.')
			return false;
		to_parse++;
	}
	return true;
}
char* ParseFileName(const char* to_parse)
{
	const char* front = to_parse;

	//eat as many "//" symbols as possible to get a new front.
	const char* test_front = front;
	while (test_front[0])
	{
		if (test_front[0] == '\\')
		{
			front = &test_front[1];
		}
		test_front++;
	}

	char result[1024];
	int z = 0;
	while (front[0] && front[0] != '.')
	{
		result[z] = front[0];
		front++;
		z++;
	}

	char* finale = (char*) malloc(z + 1);
	for (int i = 0; i < z; i++)
		finale[i] = result[i];
	finale[z] = '\0';
	return finale;

}

void write_char(char** front, char write)
{
	front[0][0] = write;
	front[0]++;
}

void write_string(char** front, const char* write)
{
	while (write[0])
	{
		front[0][0] = write[0];
		front[0]++;
		write++;
	}
}

void write_integer(char** front, int num)
{
	const char* write = std::to_string(num).c_str();
	write_string(front, write);
}

void write_float(char** front, float num)
{
	std::string float_val = std::to_string(num);
	const char* write = float_val.c_str();
	write_string(front, write);
}

void write_int(char** front, int num)
{
	std::string int_val = std::to_string(num);
	const char* write = int_val.c_str();
	write_string(front, write);
}


