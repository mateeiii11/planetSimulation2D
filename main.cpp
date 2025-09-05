#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include "shader.h"
#include <math.h>
#include <vector>
#include <random>
using namespace std;

class Object
{
	public:
		vector<float> position;
		vector<float> velocity;
		vector<float> color;
		float radius;
		float mass;
		Object(vector<float> position, vector<float> color, vector<float> velocity, float radius, float mass)
		{
			this->position = position;
			this->velocity = velocity;
			this->radius = radius;
			this->mass = mass;
			this->color = color;
		}

		void accelerate(float x, float y)
		{
			this->velocity[0] += x;
			this->velocity[1] += y;
		}

		void updatePos()
		{
			this->position[0] += this->velocity[0];
			this->position[1] += this->velocity[1];
		}
		void Friction()
		{
			this->velocity[0] *= 0.99f;
			this->velocity[1] *= 0.99f;
		}
		void DrawCircle(float centerX, float centerY, int segments, vector<float>& vert, vector<float> color)
		{
			vert.clear();

			vert.push_back(centerX);
			vert.push_back(centerY);
			vert.push_back(color[0]);
			vert.push_back(color[1]);
			vert.push_back(color[2]);
			for (int i = 0; i <= segments; i++)
			{
				float angle = 2 * 3.14 * i / segments;
				float x = centerX + radius * cos(angle);
				float y = centerY + radius * sin(angle);

				vert.push_back(x);
				vert.push_back(y);
				vert.push_back(color[0]);
				vert.push_back(color[1]);
				vert.push_back(color[2]);
			}
		}
};


double x, y;
float randomRange(float x, float y)
{
	static mt19937 rng(std::random_device{}());
	uniform_real_distribution<float> dist(x, y);
	return dist(rng);
}

vector<Object> Objects = {
		//Object(vector<float>{-0.9f, 0.0f}, vector<float>{0.05f, 0.0f}, 0.05f, 0.1f),
		//Object(vector<float>{0.9f, 0.0f}, vector<float>{0.0f, 0.01f}, 0.05f,  0.1f),
		//Object(vector<float>{0.1f, 0.4f}, vector<float>{0.05f, 0.0f}, 0.05f, 0.3f),
		//Object(vector<float>{0.2f, 0.5f}, vector<float>{0.0f, 0.01f}, 0.05f,  0.5f),
};

void resizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void click(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
			glfwGetCursorPos(window, &x, &y);
			float  velocityX, velocityY, mass;
			velocityX = randomRange(-0.05f, 0.05f); velocityY = randomRange(-0.05f, 0.05f);
			mass = randomRange(1.0f, 2.0f);
			float nX, nY;
			nX = (x / 800.0f) * 2.0f - 1.0f;
			nY = 1.0f - (y / 800.0f) * 2.0f;
			float X, Y, Z;
			X = randomRange(0.0f, 1.0f);
			Y = randomRange(0.0f, 1.0f);
			Z = randomRange(0.0f, 1.0f);
			Objects.push_back(Object(vector<float>{nX, nY}, vector<float>{X,Y,Z}, vector<float>{velocityX, velocityY}, 0.05f, mass));
		
	}
}
float deltaTime;
const float G = 0.001f;
const float P = 10.0f;
int main()
{
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		cout << ".";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resizeWindow);
	glfwSetMouseButtonCallback(window, click);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD\n";
        return -1;
    }

	

	Shader shaderProgram("default.vert", "default.frag");

	unsigned int VBO, VAO;
	
	

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 1000000 * sizeof(float),nullptr, GL_DYNAMIC_DRAW);

	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	vector<float> vertices;
	float lastTime = glfwGetTime();
	
	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		deltaTime = time - lastTime;
		lastTime = time;
		glClear(GL_COLOR_BUFFER_BIT);
		float Fx, Fy;
		///cout << Objects.size();
		for(int i = 0; i < Objects.size(); i++)
			for (int j = i + 1; j < Objects.size(); j++)
			{


				float dx = Objects[j].position[0] - Objects[i].position[0];
				float dy = Objects[j].position[1] - Objects[i].position[1];
				float distance = sqrt(dx * dx + dy * dy);
				float F = G * (Objects[i].mass * Objects[j].mass) / (distance * distance + 0.01f);

				Fx = F * (dx / distance); 
				Fy = F * (dy / distance);



				Objects[i].accelerate((Fx / Objects[i].mass ) * deltaTime , (Fy / Objects[i].mass)* deltaTime);
				Objects[j].accelerate(-(Fx / Objects[j].mass) * deltaTime, -(Fy / Objects[j].mass) * deltaTime);
			}
		for (int i = 0; i < Objects.size(); i++)
			Objects[i].Friction();
		float d = 1e9;
		for(int i = 0; i < Objects.size(); i++)
			for (int j = i + 1; j < Objects.size(); j++)
			{
				float dx = Objects[j].position[0] - Objects[i].position[0];
				float dy = Objects[j].position[1] - Objects[i].position[1];
				d = sqrt(dx * dx + dy * dy);
				if (d <= Objects[j].radius + Objects[i].radius && d > 0.0f)
				{
					float nx = dx / d;
					float ny = dy / d;

					// Fix overlap
					float overlap = (Objects[i].radius + Objects[j].radius) - d;
					Objects[i].position[0] -= nx * (overlap * (Objects[j].mass / (Objects[i].mass + Objects[j].mass)));
					Objects[i].position[1] -= ny * (overlap * (Objects[j].mass / (Objects[i].mass + Objects[j].mass)));
					Objects[j].position[0] += nx * (overlap * (Objects[i].mass / (Objects[i].mass + Objects[j].mass)));
					Objects[j].position[1] += ny * (overlap * (Objects[i].mass / (Objects[i].mass + Objects[j].mass)));

					


					float relativeVelocityX = Objects[j].velocity[0] - Objects[i].velocity[0];
					float relativeVelocityY = Objects[j].velocity[1] - Objects[i].velocity[1];

					float p = nx * relativeVelocityX + ny * relativeVelocityY;

					if (p < -0.001f)
					{
						Objects[i].accelerate(-((p * nx) / P), -((p * ny) / P));
						Objects[j].accelerate(((p * nx) / P), ((p * ny) / P));
					}
				}
			}
		
		vertices.clear();
		for (auto& e : Objects)
		{
			e.updatePos();
			if (e.position[0] - e.radius <= -1.0f) {
				e.position[0] = -1.0f + e.radius; 
				e.velocity[0] *= -0.9f;
			}
			if (e.position[0] + e.radius >= 1.0f) {
				e.position[0] = 1.0f - e.radius;
				e.velocity[0] *= -0.9f;
			}

			if (e.position[1] - e.radius <= -1.0f) {
				e.position[1] = -1.0f + e.radius;
				e.velocity[1] *= -0.9f;
			}
			if (e.position[1] + e.radius >= 1.0f) {
				e.position[1] = 1.0f - e.radius;
				e.velocity[1] *= -0.9f;
			}

		
			vector<float> circleVert;
			e.DrawCircle(e.position[0], e.position[1], 128, circleVert, e.color);
			vertices.insert(vertices.end(), circleVert.begin(), circleVert.end());

		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
		shaderProgram.use();
		glBindVertexArray(VAO);

		int offset = 0;
		int vertsPerCircle = 130;


		for (int i = 0; i < Objects.size(); i++)
		{
			glDrawArrays(GL_TRIANGLE_FAN, offset, vertsPerCircle);
			offset += vertsPerCircle;
		}

		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}
