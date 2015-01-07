#include "AIEAiProject.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

AIEAiProject::AIEAiProject()
{

}

AIEAiProject::~AIEAiProject()
{

}

bool AIEAiProject::onCreate(int a_argc, char* a_argv[])
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Load model
	m_sponza = new FBXFile();
	m_sponza->load("./resources/models/SponzaSimple.fbx", FBXFile::UNITS_CENTIMETER);
	createOpenGLBuffers(m_sponza);
	
	// Load Navmesh for model
	m_navMesh = new FBXFile();
	m_navMesh->load("./resources/models/SponzaSimpleNavMesh.fbx", FBXFile::UNITS_CENTIMETER);

	buildNavMesh(m_navMesh->getMeshByIndex(0), m_nodeGraph);

	// Load Shaders
	unsigned int vShader = Utility::loadShader("./resources/shaders/sponza.vert", GL_VERTEX_SHADER);
	unsigned int fShader = Utility::loadShader("./resources/shaders/sponza.frag", GL_FRAGMENT_SHADER);
	m_prog = Utility::createProgram(vShader, 0, 0, 0, fShader);
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return true;
}

void AIEAiProject::onUpdate(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	Gizmos::addTransform(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

	// add a 20x20 grid on the XZ-plane
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));

		Gizmos::addLine(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));
	}

	for (auto node : m_nodeGraph)
	{
		Gizmos::addAABBFilled(node->pos, glm::vec3(0.5f), glm::vec4(1, 0, 0, 1));

		if (node->edgeTarget[0] != nullptr)
		{
			Gizmos::addLine(node->pos + glm::vec3(0, 0.05f, 0),
				node->edgeTarget[0]->pos + glm::vec3(0, 0.05f, 0),
				glm::vec4(1, 1, 0, 1));
		}

		if (node->edgeTarget[1] != nullptr)
		{
			Gizmos::addLine(node->pos + glm::vec3(0, 0.05f, 0),
				node->edgeTarget[1]->pos + glm::vec3(0, 0.05f, 0),
				glm::vec4(1, 1, 0, 1));
		}

		if (node->edgeTarget[2] != nullptr)
		{
			Gizmos::addLine(node->pos + glm::vec3(0, 0.05f, 0),
				node->edgeTarget[2]->pos + glm::vec3(0, 0.05f, 0),
				glm::vec4(1, 1, 0, 1));
		}
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void AIEAiProject::onDraw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

	glUseProgram(m_prog);

	int location = glGetUniformLocation(m_prog, "projectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix * viewMatrix));

	unsigned int count = m_sponza->getMeshCount();

	for (unsigned int i = 0; i < count; i++)
	{
		FBXMeshNode* mesh = m_sponza->getMeshByIndex(i);
		GLData* data = (GLData*)mesh->m_userData;

		location = glGetUniformLocation(m_prog, "model");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mesh->m_globalTransform));

		location = glGetUniformLocation(m_prog, "invTransformModel");
		glUniformMatrix4fv(location, 1, GL_TRUE, glm::value_ptr(glm::inverse(mesh->m_globalTransform)));

		glBindVertexArray(data->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);

	}

	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));
}

void AIEAiProject::onDestroy()
{
	cleanupOpenGLBuffers(m_sponza);
	//cleanupOpenGLBuffers(m_navMesh);

	delete m_navMesh;
	delete m_sponza;

	glDeleteProgram(m_prog);

	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new AIEAiProject();

	if (app->create("AIE - AIEAiProject", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, argc, argv) == true)
		app->run();

	// explicitly control the destruction of our application
	delete app;

	return 0;
}

void AIEAiProject::createOpenGLBuffers(FBXFile* a_fbx)
{
	// create the GL VAO/VBO/IBO data for meshes
	for (unsigned int i = 0; i < a_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		// storage for the opengl data in 3 unsigned int
		GLData* glData = new GLData();

		glGenVertexArrays(1, &glData->VAO);
		glBindVertexArray(glData->VAO);

		glGenBuffers(1, &glData->VBO);
		glGenBuffers(1, &glData->IBO);

		glBindBuffer(GL_ARRAY_BUFFER, glData->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->IBO);

		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void AIEAiProject::cleanupOpenGLBuffers(FBXFile* a_fbx)
{
	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < a_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		GLData* glData = (GLData*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData->VAO);
		glDeleteBuffers(1, &glData->VBO);
		glDeleteBuffers(1, &glData->IBO);

		delete[] glData;
	}
}

void AIEAiProject::buildNavMesh(FBXMeshNode * a_mesh,
	std::vector < NavNodeTri*>& a_nodeGraph)
{
	unsigned int triCount = a_mesh->m_indices.size() / 3;

	for (unsigned int tri = 0; tri < triCount; ++tri)
	{
		NavNodeTri*node = new NavNodeTri();

		// edge[ABC]

		//[AB] = 0
		//[BC] = 1
		//[CA] = 2
		node->edgeTarget[0] = nullptr;
		node->edgeTarget[1] = nullptr;
		node->edgeTarget[2] = nullptr;

		node->verts[0] = a_mesh->m_vertices[a_mesh->m_indices[tri * 3 + 0]].position.xyz;
		node->verts[1] = a_mesh->m_vertices[a_mesh->m_indices[tri * 3 + 1]].position.xyz;
		node->verts[2] = a_mesh->m_vertices[a_mesh->m_indices[tri * 3 + 2]].position.xyz;

		node->pos = (node->verts[0] + node->verts[1] + node->verts[2]) / 3.0f;

		a_nodeGraph.push_back(node);

	}

	for (auto start : a_nodeGraph)
	{
		for (auto end : a_nodeGraph)
		{
			if (start == end)
			{
				continue;
			}

			// ABC XYZ

			// AB == XY || AB == YZ || AB == ZX
			// AB == YX || AB == ZY || AB == XZ
			if ((start->verts[0] == end->verts[0] && start->verts[1] == end->verts[1]) ||
				(start->verts[0] == end->verts[1] && start->verts[1] == end->verts[2]) ||
				(start->verts[0] == end->verts[2] && start->verts[1] == end->verts[0]) ||
				(start->verts[0] == end->verts[1] && start->verts[1] == end->verts[0]) ||
				(start->verts[0] == end->verts[2] && start->verts[1] == end->verts[1]) ||
				(start->verts[0] == end->verts[0] && start->verts[1] == end->verts[2]))
			{
				start->edgeTarget[0] = end;
			}

			// BC == XY || BC == YZ || BC == ZX
			// BC == YX || BC == ZY || BC == XZ
			if ((start->verts[1] == end->verts[0] && start->verts[2] == end->verts[1]) ||
				(start->verts[1] == end->verts[1] && start->verts[2] == end->verts[2]) ||
				(start->verts[1] == end->verts[2] && start->verts[2] == end->verts[0]) ||
				(start->verts[1] == end->verts[1] && start->verts[2] == end->verts[0]) ||
				(start->verts[1] == end->verts[2] && start->verts[2] == end->verts[1]) ||
				(start->verts[1] == end->verts[0] && start->verts[2] == end->verts[2]))
			{
				start->edgeTarget[1] = end;
			}

			// CA == XY || CA == YZ || CA == ZX
			// CA == YX || CA == ZY || CA == XZ
			if ((start->verts[2] == end->verts[0] && start->verts[0] == end->verts[1]) ||
				(start->verts[2] == end->verts[1] && start->verts[0] == end->verts[2]) ||
				(start->verts[2] == end->verts[2] && start->verts[0] == end->verts[0]) ||
				(start->verts[2] == end->verts[1] && start->verts[0] == end->verts[0]) ||
				(start->verts[2] == end->verts[2] && start->verts[0] == end->verts[1]) ||
				(start->verts[2] == end->verts[0] && start->verts[0] == end->verts[2]))
			{
				start->edgeTarget[2] = end;
			}
		}
	}
}