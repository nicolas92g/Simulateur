#include "window.h"


nico::Window::Window(int openglMajorVersion, int openglMinorVersion, bool debugMode, int antialiasingLevel, const char* title, glm::vec2 widthAndHeight)
{
	deltaTime = 0;
	time = 0;
	fps = 0;
	pauseIfUnfocused = true;

	if (!glfwInit()) {
		fprintf(stderr, "error glfw cant init\n");
		exit(-1);
	}

	glfwWindowHint(GLFW_SAMPLES, antialiasingLevel); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglMajorVersion); // On veut OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglMinorVersion);
#	ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#	endif // __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugMode);

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	
	GLFWmonitor* monitor = monitors[0];

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);


	glfwWin = glfwCreateWindow(widthAndHeight.x, widthAndHeight.y, title, nullptr, nullptr);
	if (glfwWin == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible\n");
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(glfwWin); 
	// Initialise GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	// Assure que l'on peut capturer la touche d'échappement enfoncée ci-dessous
	glfwSetInputMode(glfwWin, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(glfwWin, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glViewport(0, 0, widthAndHeight.x, widthAndHeight.y);
	glfwSetFramebufferSizeCallback(glfwWin, framebuffer_size_callback);
	
	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(nico::glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	std::cout << glGetString(GL_VERSION) << "\n";

	glfwSetCharCallback(this->glfwWin, Window::getChar);
}


nico::Window::~Window()
{
	glfwDestroyWindow(glfwWin);
	glfwTerminate();
}
bool nico::Window::Key(int GLFW_KEY)
{
	return (glfwGetKey(glfwWin, GLFW_KEY) == GLFW_PRESS);
}

bool nico::Window::Mouse(int GLFW_MOUSE)
{
	return glfwGetMouseButton(glfwWin, GLFW_MOUSE) == GLFW_PRESS;
}

bool nico::Window::shouldClose() const
{
	return glfwWindowShouldClose(glfwWin);
}
void nico::Window::requestAttention() const
{
	glfwRequestWindowAttention(glfwWin);
}
void nico::Window::setIcon(const char* imagePath)
{
	GLFWimage icons[1];
	icons[0].pixels = stbi_load(imagePath, &icons[0].width, &icons[0].height, nullptr, 0);
	glfwSetWindowIcon(glfwWin, 1, icons);
	stbi_image_free(icons[0].pixels);
}

void nico::Window::endFrame()
{
	//calculate fps and deltatime
	deltaTime = getTime() - time;
	time = getTime();
	//fps is not updated in all the frames because we can't read anything that is displayed for 1/60 seconds
	if (!(int(time * 10000) % 100)) {
		fps = (double)(1.0 / deltaTime);
	}
	//pause if the window is not focused
	if (this->pauseIfUnfocused) {
		//get the time before the pause
		double previousTime(glfwGetTime());

		while (!glfwGetWindowAttrib(glfwWin, GLFW_FOCUSED))
		{
			glfwSwapBuffers(glfwWin);
			glfwPollEvents();
		}
		//reset the glfw time function to the time it was before the pause
		glfwSetTime(previousTime);
	}

	//end frame
	glfwSwapBuffers(glfwWin);
	glfwPollEvents();
}

void nico::Window::maximise()
{
	glfwMaximizeWindow(glfwWin);
}

void nico::Window::hideCursor(bool hide)
{
	if (hide) {
		glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else {
		glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void nico::Window::setFullscreen(bool mode)
{
	if (mode) {
		GLFWmonitor* p = glfwGetPrimaryMonitor();
		int width = 0, height = 0, x = 0, y = 0;
		glfwGetMonitorWorkarea(p, &x, &y, &width, &height);

		glfwSetWindowMonitor(glfwWin, p, x, y, width, height, GLFW_DONT_CARE);
	}
	else {
		glfwSetWindowMonitor(glfwWin, nullptr, 100, 100, 800, 600, GLFW_DONT_CARE);
	}
}

GLFWwindow* nico::Window::glfw() const
{
	return glfwWin;
}

glm::vec2 nico::Window::getSize() const
{
	return glm::vec2(getWidth(),getHeight());
}

int nico::Window::getWidth() const
{
	int width;
	glfwGetWindowSize(glfwWin,&width,nullptr);
	return width;
}

int nico::Window::getHeight() const
{
	int height;
	glfwGetWindowSize(glfwWin, nullptr, &height);
	return height;
}

double nico::Window::getCursorX() const
{
	double X	;
	glfwGetCursorPos(glfwWin, &X, nullptr);
	return X;
}

double nico::Window::getCursorY() const
{
	double Y;
	glfwGetCursorPos(glfwWin,nullptr,&Y);
	return Y;
}

double nico::Window::getTime() const
{
	return glfwGetTime();
}

double nico::Window::getDeltaTime() const
{
	return deltaTime;
}

int nico::Window::getFps() const
{
	return fps;
}

void nico::Window::setSize(int width, int height)
{
	glfwSetWindowSize(glfwWin, width, height);
	glViewport(0, 0, width, height);
}

void nico::Window::setTitle(const char* newTitle)
{
	glfwSetWindowTitle(glfwWin,newTitle);
}

void nico::Window::setPos(int x, int y)
{
	glfwSetWindowPos(glfwWin, x, y);
}

void nico::Window::setCursorPos(double x, double y)
{
	glfwSetCursorPos(this->glfwWin, x, y);
}

void nico::Window::setShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(this->glfwWin, shouldClose);
}

void nico::Window::framebuffer_size_callback(GLFWwindow* glfw, int width, int height)
{
	glViewport(0,0,width,height);
}

void nico::Window::getChar(GLFWwindow* glfw, unsigned int Char)
{
	input = Char;
}

char nico::Window::input;

