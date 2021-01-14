#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"
#include "Shader.h"
#include "LightShader.h"
#include "Camera.h"
#include <iostream>
#include <cmath>

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

//camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//lighting
glm::vec3 lightPosition(1.0f, 0.7f, 2.0f);
//represents the light(¡Ösun) location in world-space coordinates

void user_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
}

void window_size_change(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//to calculate the pitch and yaw values from mouse-movement events
void mouse_callback(GLFWwindow* window, /*mouse position*/double xPos, double yPos) {
	/*To check if this is the first time we receive mouse input.
	(whenever the window first receives focus of the mouse cursor, happen large sudden jump
	This is often a position that is significantly far away from the center of the screen,
	resulting in large offsets and thus a large movement jump.)*/
	if (firstMouse) {
		lastX = (float)xPos;
		lastY = (float)yPos;
		firstMouse = false;
	}

	//calculate the distance of movement between the last and current frame.
	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos; //reverse (¡ñ y-coordinates : ¡è)
	lastX = (float)xPos;
	lastY = (float)yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(yOffset);
}




int main() {
	//initialize and configue
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfw window creation
	GLFWwindow* window = glfwCreateWindow(1600, 1200, "Xion's OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "¡ÚFailed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_size_change);

	//register this function with GLFW each time the mouse moves
	//glfwSetCursorPosCallback(window, mouse_callback);

	//to zoon in, use mouse's scroll wheel
	glfwSetScrollCallback(window, scroll_callback);

	//to calculate the yaw and pitch values from mouse movement.
	//When the application has focus, the mouse cursor should be hiden and stays within the window
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//glad : load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//configure global OpenGL state
	glEnable(GL_DEPTH_TEST);

	//shader program
	Shader myShader = Shader();
	LightShader lampShader = LightShader();

	//set up vertex data(and buffer) and configure vertex attributes
	float vertices[] = {
		// positions          // texture	//normal
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,	0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,	0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,	0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,	0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f,  1.0f,  0.0f
	};





	//¡ÚConfigure the ALPHA cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * (sizeof(float)), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//¡ÚConfigure the light's VAO
	//(VBO stays the same: the vertices are the same for the light object which is also a 3D cube)
	unsigned lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	//The only need to do is binding to the VBO, (to link it with glVertexAttribPointer)
	//the container's VBO's data already contains the exact data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//set the vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/*It may look inefficient using the cube's vertex data that is not completely used by the lamp shader,
	but the vertex data is already stored in the GPU's memory from the cube object
	so we don't have to store new data into the GPU's memory.
	This actually makes it more efficient compared to allocating a new VBO for the lamp.*/







	//load and create textures
	unsigned int texture1, texture2, emission;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps

	stbi_set_flip_vertically_on_load(true); // tell stb_image.h flip loaded texture's on the y-axis.

	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with my own image path.	
	unsigned char* data = stbi_load("container2.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("steel.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "¡ÚFailed to load texture2" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &emission);
	glBindTexture(GL_TEXTURE_2D, emission);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("matrix.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "¡ÚFailed to load emission" << std::endl;
	}
	stbi_image_free(data);

	//activate shader & set the shader's uniform attributes
	myShader.use();
	myShader.setInt("texture1", 0);
	myShader.setInt("texture2", 1);
	myShader.setInt("emission", 2);
	myShader.setVec3("lightPos", lightPosition);




	//------------------------------------------------------
	//render loop
	while (!glfwWindowShouldClose(window))
	{
		//per-frame time logic
		deltaTime = (float)glfwGetTime() - lastFrame;
		lastFrame = (float)glfwGetTime();

		//input
		user_input(window);

		//render
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//¦¦also clear the depth buffer(otherwise the depth infomation of the precious frame stays in the buffer)

		// bind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emission);

		//¡Úactivate shader & setting unifroms
		myShader.use();
		myShader.setVec3("viewPos", camera.Position);

		//¡Úlight properties ---> (all light colors are set at full intensity)
		myShader.setVec3("light.ambient", .5f, .5f, .5f);
		myShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
		myShader.setVec3("light.specular", 0.0f, 1.0f, 1.0f);

		//¡Úmaterial properties
		myShader.setVec3("material.ambient", .3f, .3f, .3f);
		myShader.setVec3("material.diffuse", .8f, .8f, .8f);
		myShader.setVec3("material.specular", 0.0f, 1.0f, 1.0f);
		myShader.setFloat("material.shininess", 64.0f);

		//¡Úview/projection transformations
		//pass projection matrix to shader(in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)4 / (float)3, .1f, 100.0f);
		myShader.setMat4("projection", projection);

		// create a frustum that defines the visible space
		/*frustum : in clip space, viewing range will end up on the user's screen. .
					anything out side the frustum will not end up and will become clipped*/
					//projection = glm::perspective(glm::radians(40.0f), 6.0f / 8.0f, 0.1f, 100.0f); 
					/*parametaer
					1: fov(field of view) value == how large the viewspace is.
					2: aspect ratio == viewport's width / viewport's height
					3,4: distance between near plane and far plane (usually set near distance: 0.1, far distance: 100.0)
						 All the vertices between the near and far plane and inside the frustum  will be rendered.
					* The projection matrix not only maps a given frustum range to clip space,
					* but also manipulates the w value of each vertex coordinate.
					* Each component of the vertex coordinate is divided by its w component.
					* The further away a vertex from the viewer, will receive smaller vertex coordinates.
					*/

					//camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		myShader.setMat4("view", view);

		//¡Úworld transformation
		glm::mat4 model = glm::mat4(1.0f);
		float angle = (float)glfwGetTime() * 80.0f + 5.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.8f, 0.6f));

		myShader.setMat4("model", model);
		//¢Õ
		//unsigned int modelLocation = glGetUniformLocation(myShader.ID, "model");
		//glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		//¡Úrender the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);





		//¡Údraw the lamp object

		//activate Lamp's Shader
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		//translate the lamp cube to the light's position and scale it down
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.5f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);



		//glfw : swap buffer and poll event (key pressed/release, mouse moved etc..)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	//optional - de-allocate all resource once they've outlived their purpose
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	//glfw: terminate, clearing all previously allocatedd GLFW resources
	glfwTerminate();
	return 0;
}