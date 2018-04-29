#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include "glm/glm/glm.hpp"
//#include "shaderload.h"
#include <iostream>
#include <vector>
#include "stb_image.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "camera.h"
#include "map.h"
#include "const.h"
#include "player.h"
#include "user.h"
#include <stdlib.h>




//
//	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Prototypes  @@@@@@@@
//
void init();			// init glfw
void keyBoard(GLFWwindow * window);
void mouseCall(GLFWwindow * window, double mX, double mY);
void getDeltaTime();

// collision stuff
bool collision(glm::vec3 &p, glm::vec3 &other); // AABB - AABB collision
void collideWithBricks(Player & p);
void collideWithEverything(Player & p);

//	Object loader
bool loadOBJ(
	const char * path,
	std::vector <glm::vec3> & out_vertices,
	std::vector <glm::vec2> & out_uvs,
	std::vector <glm::vec3> & out_normals);


// deltatime
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera cam(glm::vec3(-20.0f, -20.0f ,40.3f));

// mouse stuff
bool firstMouse = true;
float lastMouseX = WIN_WIDTH / 2.0f;
float lastMouseY = WIN_HEIGHT / 2.0f;


// player stuff			
User * user;
Player player(2.0f, glm::vec3(-15.0f, -15.0f, 0.0f));
//	player model
std::vector< glm::vec3 > playerVertices;
std::vector< glm::vec2 > playerUvs;
std::vector< glm::vec3 > playerNormals;


// lists
std::vector<Player> ghosts;
std::vector<glm::vec3> posMap;
std::vector<glm::vec3> posFood;


//
//	SHADER
//
const char * vertexShaderSrc = "#version 330 core						\n"
"layout (location = 0) in vec3 aPos;									\n"
"layout (location = 1) in vec3 aNormal;									\n"
"layout (location = 2) in vec2 aTexCord;								\n"

"out vec2 TexCord;														\n"
"out vec3 Normal;														\n"
"out vec3 FragPos;														\n"

"uniform mat4 model;													\n"
"uniform mat4 view;													\n"
"uniform mat4 projection;												\n"

"void main(){															\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0f); \n"
"	TexCord = vec2(aTexCord.x, aTexCord.y);	\n"
"	FragPos = vec3(model * vec4(aPos, 1.0)); \n"
"	Normal = aNormal;						\n"
"}	 \n \0";

const char * fragmentShaderSrc = "#version 330 core \n"
"out vec4 FragColor; \n"
"in vec2 TexCord; \n"
"in vec3 Normal;  \n"
"in vec3 FragPos; \n"
"uniform vec4 ourColor; \n"
"uniform vec4 lightColor; \n"
"uniform vec3 lightPosition; \n"
"uniform sampler2D ourTexture; \n"
"uniform vec3 viewPosition;\n"
"void main(){ \n"
"	float ambientStrength = 0.1; \n"
"	vec4 ambient = ambientStrength * lightColor; \n"

"	vec3 norm = normalize(Normal);\n"
"   vec3 lightDir = normalize(lightPosition - FragPos);\n"
"   float diff = max(dot(norm, lightDir), 0.0);\n"
"   vec4 diffuse = diff * lightColor;\n"

"   float specularStrength = 0.5;\n"
"   vec3 viewDir = normalize(viewPosition - FragPos);\n"
"   vec3 reflectDir = reflect(-lightDir, norm);\n"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);\n"
"   vec4 specular = specularStrength * spec * lightColor;\n"

"	FragColor = (ambient + diffuse + specular) * texture(ourTexture, TexCord) * ourColor; \n"
"} \n \0";




//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	Main @@@@@@@@@@@@@
//
int main()
{
	init();
	//	window
	GLFWwindow * window = glfwCreateWindow(WIN_HEIGHT, WIN_WIDTH, "Well then", NULL, NULL);
	if (window == NULL) { std::cout << "WINDOW NOT MADE\n"; return -1; }
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouseCall);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glew stuff
	glewExperimental = GL_TRUE;
	glewInit();


	// shady stuff
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glEnable(GL_DEPTH_TEST);
	// vertices
	float vertices[] = {

		// vertex pos		  // normal vector    // texcoord
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f
	};

	GLuint indices[] = {		// NOTE GLUiNT!!		
		0, 1, 3,				
		1, 2, 3					
	};							

	//	Load player model
	loadOBJ("../Assets/YellowSphere.obj", playerVertices, playerUvs, playerNormals);

	// VBO
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// unbind VBO?
	glBindVertexArray(0);		

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//	Texture
	int width, height, nrChannels;
	unsigned char * data = stbi_load("../Assets/container.jpg",&width, &height, &nrChannels, 0);

	GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLuint)width, (GLuint)height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	
	
	glUseProgram(shaderProgram);

	int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform4f(lightColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

	int lightPositionLocation = glGetUniformLocation(shaderProgram, "lightPosition");
	glUniform3f(lightPositionLocation, 20.0f, 20.0f, 0.0f);

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(60.0f), (float)WIN_HEIGHT / (float)WIN_WIDTH, 0.1f, 100.0f); // frustum
	unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);


	// loop
	while (!glfwWindowShouldClose(window)) {
		getDeltaTime();
		keyBoard(window);

		glClearColor(0.0f, 0.05f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, texture1);



		glm::mat4 view;
		view = cam.GetViewMatrix();
		
		//	Since camera location can change, view position is updated continuously
		int viewPositionLocation = glGetUniformLocation(shaderProgram, "viewPosition");
		glUniform3f(viewPositionLocation, cam.Position.x, cam.Position.y, cam.Position.z);
		
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");	// getting the index of uniform in that program

		glBindVertexArray(VAO);
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");

		// DRAW TILES (FINISHED)
		glUniform4f(vertexColorLocation, 0.4f, 0.4f, 1.2f, 1.0f);
		for (unsigned int i = 0; i < posMap.size(); i++) {
			glm::mat4 model;
			model = glm::translate(model, posMap[i]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// DRAW AND UPDATE FOOD
		collideWithEverything(player);
		glUniform4f(vertexColorLocation, 0.5f, 1.2f, 1.2f, 1.0f);
		for (unsigned int i = 0; i < posFood.size(); i++) {
			glm::mat4 model;
			model = glm::translate(model, posFood[i]);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), posFood[i]);	// rotate on x
			model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// UPDATE PLAYER
		player.update(deltaTime);
		glm::mat4 playerModel;
		playerModel = glm::translate(playerModel, player.getPlayerPos());
		playerModel = glm::scale(playerModel, glm::vec3(SIZE));
		user->display();
		
		// DRAW PLAYER
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &playerModel[0][0]);
		glUniform4f(vertexColorLocation, 0.0f, 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);



		// UPDATE GHOSTS (FINISHED)
		glUniform4f(vertexColorLocation, 1.2f, 0.5f, 1.1f, 1.0f);
		for (unsigned int i = 0; i < ghosts.size(); i++) {
			collideWithBricks(ghosts[i]);
			ghosts[i].update(deltaTime);
			glm::mat4 ghostModel;
			ghostModel = glm::translate(ghostModel, ghosts[i].getPlayerPos());
			ghostModel = glm::scale(ghostModel, glm::vec3(SIZE));

			// DRAW GHOST
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &ghostModel[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clean-up
	glfwTerminate();


	return 0;
}



//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	Func defs @@@@@@@@@@@@
//


//	init glfw
void init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	MapLoader gameMap("../Assets/level0.DTA");
	gameMap.getMap(posMap);
	gameMap.getFood(posFood);
	player.pos = gameMap.getTypePos(PLAYER_POS);
	std::cout << "player " << player.pos.x << '\n';

	user = new User(3, posFood.size());


	ghosts.push_back(Player(1, true, 2.0f, glm::vec3(-14.0f, -16.0f, 0.0f)));
	ghosts.push_back(Player(2, true, 2.0f, glm::vec3(-14.0f, -17.0f, 0.0f)));
	ghosts.push_back(Player(0, true, 2.0f, glm::vec3(-17.0f, -16.0f, 0.0f)));
	ghosts.push_back(Player(0, true, 2.0f, glm::vec3(-17.0f, -15.0f, 0.0f)));
}


void keyBoard(GLFWwindow * window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		player.direction = 0;

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		player.direction = 1;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		player.direction = 2;

	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		player.direction = 3;

	}
}

void getDeltaTime()
{
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// deltaTime = 0.01f;

}

void mouseCall(GLFWwindow * window, double mX, double mY)
{
	if (firstMouse)
	{
		lastMouseX = (float)mX;
		lastMouseY = (float)mY;
		firstMouse = false;
	}
	// mouse offsets
	float dx = mX - lastMouseX;
	float dy = lastMouseY - mY;
	lastMouseX = mX;
	lastMouseY = mY;

	cam.ProcessMouseMovement(dx, dy);


}




bool collision(glm::vec3 &p, glm::vec3 &other) // AABB - AABB collision
{
	// Collision x-axis?

	bool collisionX = p.x + SIZE >= other.x &&
		other.x + 1 >= p.x;
	// Collision y-axis?
	bool collisionY = p.y + SIZE >= other.y &&
		other.y + 1 >= p.y;
	// Collision only if on both axes
	return collisionX && collisionY;
}





void collideWithBricks(Player & p) {
	for (unsigned int i = 0; i < posMap.size(); i++) {
		if (collision(p.pos, posMap[i])) {
			int preventStuck = 0;
			while (collision(p.getPlayerPos(), posMap[i])) {
				p.setPos(-0.01f);
				preventStuck++;
				if (preventStuck > 100)
					p.pos = glm::vec3(-15.0f, -15.0f, 0.0f);
			}
			
			if(!p.isGhost)
				p.direction = -1;
			else {
				p.direction = rand() % 4;
			}
		}	
	}
}


void collideWithEverything(Player & p) {
	// collide with food
	for (unsigned int i = 0; i < posFood.size(); i++) {
		if (collision(p.pos, posFood[i])) {
			posFood.erase(posFood.begin() + i);
			user->updateScore();

		}
	}

	// collide with ghosts
	for (unsigned int i = 0; i < ghosts.size(); i++) {
		if (collision(p.pos, ghosts[i].pos)) {
			p.pos = glm::vec3(-15.0f, -15.0f, 0.0f);
			user->loseLive();
		}
	}

	// collide with wall and push back
	collideWithBricks(p);
}

//	loadOBJ function is from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

bool loadOBJ(
	const char * path,
	std::vector <glm::vec3> & out_vertices,
	std::vector <glm::vec2> & out_uvs,
	std::vector <glm::vec3> & out_normals)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++) 
	{
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		out_uvs.push_back(uv);
	}

	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex - 1];
		out_normals.push_back(normal);
	}
}