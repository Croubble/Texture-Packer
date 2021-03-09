
int main()
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
	window = SDL_CreateWindow("Castle Elsewhere", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_STARTING_WIDTH, SCREEN_STARTING_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	mainContext = SDL_GL_CreateContext(window);
	if (mainContext == NULL)
	{
		printf("we have failed to initialise the gl context");
	}
	auto rdr = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
}