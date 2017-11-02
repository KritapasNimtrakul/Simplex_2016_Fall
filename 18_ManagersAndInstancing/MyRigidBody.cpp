#include "MyRigidBody.h"

Simplex::MyrigidBody::MyrigidBody(MyMesh* a_pObject)
{
	if (a_pObject == nullptr)
	{
		return;
	}
	
	std::vector<vector3> pointList = a_pObject->GetVertexList();

	if (pointList.size() < 1)
	{
		return;
	}

	v3Min = pointList[0];
	v3Max = pointList[0];
	for (uint i = 1; i < pointList.size();++i)
	{
		if (pointList[i].x < v3Min.x)
		{
			v3Min.x = pointList[i].x;
		}
		else if (pointList[i].x > v3Max.x)
		{
			v3Max.x = pointList[i].x;
		}
		if (pointList[i].y < v3Min.y)
		{
			v3Min.y = pointList[i].y;
		}
		else if (pointList[i].y > v3Max.y)
		{
			v3Max.y = pointList[i].y;
		}
		if (pointList[i].z < v3Min.z)
		{
			v3Min.z = pointList[i].z;
		}
		else if (pointList[i].z > v3Max.z)
		{
			v3Max.z = pointList[i].z;
		}
	}

	v3Center = v3Min + v3Max;
	v3Center = v3Center / 2.0f;

	float fRadius = glm::distance(v3Center, v3Max);

	m_BoundingSphere = new MyMesh();
	m_BoundingSphere->GenerateSphere(fRadius,5,C_BLUE);

	m_BoundingBox = new MyMesh();
	m_BoundingBox->GenerateCube(1.0f, C_WHITE);
}

void Simplex::MyrigidBody::Render(MyCamera* a_pCamera, matrix4 a_m4Model)
{
	
	float* matrix = new float[16];
	memcpy(matrix, glm::value_ptr(a_m4Model), 16 * sizeof(float));
	vector3 v3Size = v3Max-v3Min;
	matrix4 resize = a_m4Model * glm::scale(IDENTITY_M4, v3Size);
	//m_BoundingSphere->Render(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), resize,1);
	m_BoundingSphere->Render(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), a_m4Model);

	m_BoundingBox->Render(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), a_m4Model);
	SafeDelete(matrix);
}
