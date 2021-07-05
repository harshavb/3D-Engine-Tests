#include "TestThree.h"

// GLSL shader that simply passes location data to output
static const char* vertexShaderSource =
	"#version 410 core\n"
	"layout(location = 0) in vec3 aPos;\n"
	"layout(location = 1) in vec3 vertexColor;"
	"layout(location = 2) in float garbage_position_duplicate;\n"

	"out vec3 ourColor;\n"

	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos, 1.0f);\n"
	"	ourColor = vertexColor;\n"
	"}\0";

// GLSL shader that outputs a FragColor
static const char* fragmentShaderSource =
	"#version 410 core\n"
	"out vec4 FragColor;\n"
	""
	"uniform vec4 ourColorUniform;\n"  // A uniform is a global variable that can be changed anywhere outside the shader
	""
	"void main()\n"
	"{\n"
	"	FragColor = ourColorUniform;\n"
	"}\0";

// Another GLSL shader that outputs a different FragColor
static const char* fragmentShaderSourceTwo =
	"#version 410 core\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;\n"

	"void main()\n"
	"{\n"
	"	FragColor = vec4(ourColor, 1.0f);\n"
	"}\0";

void TestThree::runTest()
{
	glfwInit();  // Begins GLFW
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // For Mac support
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // Defines GL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);  // Defines GL version - 4.1 because Mac doesn't support 4.2 to 4.6
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Sets whether or not to use post 3.0 GL features, pre 3.0 GL features, or both

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGLTests", NULL, NULL);  // Creates the GL window
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);  // Sets window to run GL
	glfwSetFramebufferSizeCallback(window, UsefulMethods::framebuffer_size_callback);  // Defines what function to run whenever window is resized

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  // Allows GLAD to read functions from driver (OS specific without GLAD)
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	// Creates vertex shader object and supply shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Creates fragment shader object and supply shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Links shader program after attaching shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Retreives global variable address of ourColor and stores it
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColorUniform");

	// Reuses fragment shader variable but with a different source
	glShaderSource(fragmentShader, 1, &fragmentShaderSourceTwo, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgramTwo = glCreateProgram();
	glAttachShader(shaderProgramTwo, vertexShader);
	glAttachShader(shaderProgramTwo, fragmentShader);
	glLinkProgram(shaderProgramTwo);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float exerciseTwoVerticesOne[] = {
		-0.5f, -0.25f, 0.0f, 5.0f,
		0.0f, -0.25f, 0.0f, 5.0f,
		-0.25f,  0.25f, 0.0f, 5.0f
	};

	float exerciseTwoVerticesTwo[] = {
		0.5f, -0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f,
		0.0f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 5.0f,
		0.25f,  0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 5.0f
	};

	// Defines indices of vertices of triangles to be drawn
	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	// Generates a VBO, EBO, and a VAO for storing vertices and how to deal with those vertices
	// GLuint VBO, EBO, VAO;
	GLuint VBOs[2], VAOs[2];

	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);

	// First binds VAO(s), then binds and sets VBO(s)/EBO(s), then configures vertex attribute(s)
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(exerciseTwoVerticesOne), exerciseTwoVerticesOne, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	// Also allowed: glVertexAttribPointer(glGetAttribLocation("aPos"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(3 * sizeof(float)));

	glBindVertexArray(VAOs[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(exerciseTwoVerticesTwo), exerciseTwoVerticesTwo, GL_STATIC_DRAW);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(6 * sizeof(float)));

	// Unbinds the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Wireframe Mode
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		// Input
		UsefulMethods::processInput(window);

		// Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		// Draws based on the first VAO
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Updates color based on time and passes it to global variable
		float greenValue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		glUseProgram(shaderProgramTwo);

		// Draws based on the second VAO
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swaps the buffers then checks and calls events (?)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// De-allocates resources
	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);

	glDeleteProgram(shaderProgram);

	glfwTerminate();
}
