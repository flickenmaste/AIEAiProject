#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include <FBXFile.h>

class AIEAiProject : public Application
{
public:

	AIEAiProject();
	virtual ~AIEAiProject();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	// OpenGL
	struct GLData
	{
		unsigned int VAO, VBO, IBO;
	};

	// NavMesh

	struct NavNodeTri
	{
		glm::vec3 pos;
		glm::vec3 verts[3];

		NavNodeTri* edgeTarget[3];
	};

	std::vector<NavNodeTri*> m_nodeGraph;

	void buildNavMesh(FBXMeshNode * a_mesh,
		std::vector < NavNodeTri*>& a_nodeGraph);

	FBXFile * m_sponza;		// level
	FBXFile * m_navMesh;	// nav mesh

	void createOpenGLBuffers(FBXFile * a_fbx);
	void cleanupOpenGLBuffers(FBXFile * a_fbx);

	unsigned int m_prog;	// program for rendering
};