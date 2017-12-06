#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0; //will store the number of MyOctants instantiated
uint MyOctant::m_uMaxLevel = 3;//will store the maximum level an MyOctant can go to
uint MyOctant::m_uIdealEntityCount = 5; //will tell how many ideal Entities this object will contain

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uOctantCount = 0;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> MinMaxBound;

	uint nObject = m_pEntityMngr->GetEntityCount();
	for (uint i = 0;i < nObject;i++)
	{
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		MinMaxBound.push_back(pRigidBody->GetMinGlobal());
		MinMaxBound.push_back(pRigidBody->GetMaxGlobal());
	}
	MyRigidBody* pRigidBody = new MyRigidBody(MinMaxBound);

	vector3 halfWidth = pRigidBody->GetHalfWidth();
	float fMax = halfWidth.x;
	for (int i = 1;i < 3;i++)
	{
		if (fMax < halfWidth[i])
		{
			fMax = halfWidth[i];
		}
	}

	vector3 v3Center = pRigidBody->GetCenterLocal();
	MinMaxBound.clear();
	SafeDelete(pRigidBody);

	m_fSize = fMax *2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - (vector3(fMax));
	m_v3Min = m_v3Center + (vector3(fMax));

	m_uOctantCount++;

	ConstructTree(m_uMaxLevel);
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

MyOctant::MyOctant(MyOctant const & other)
{
	m_uID = other.m_uID; 
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pParent = other.m_pParent;

	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8;i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}


}

MyOctant & MyOctant::operator=(MyOctant const & other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

MyOctant::~MyOctant(void)
{
	Release();
}

void MyOctant::Swap(MyOctant & other)
{
	std::swap(m_uID , other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren , other.m_uChildren); 

	std::swap(m_fSize , other.m_fSize);

	std::swap(m_v3Center , other.m_v3Center);
	std::swap(m_v3Min ,other.m_v3Min);
	std::swap(m_v3Max , other.m_v3Max);
	std::swap(m_pParent , other.m_pParent);

	std::swap(m_pRoot , other.m_pRoot); 
	std::swap(m_lChild , other.m_lChild);
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8;i++)
	{
		std::swap(m_pChild[i] , other.m_pChild[i]);
	}
}

float MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool MyOctant::IsColliding(uint a_uRBIndex)
{
	uint nObjectCount = m_pEntityMngr->GetEntityCount();
	if (a_uRBIndex >= nObjectCount)
	{
		return false;
	}
	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	vector3 v3Min = pRigidBody->GetMinGlobal();
	vector3 v3Max = pRigidBody->GetMaxGlobal();

	// check x
	if (m_v3Max.x < v3Min.x)
	{
		return false;
	}
	if (m_v3Min.x > v3Max.x)
	{
		return false;
	}

	// check y
	if (m_v3Max.y < v3Min.y)
	{
		return false;
	}
	if (m_v3Min.y > v3Max.y)
	{
		return false;
	}

	// check z
	if (m_v3Max.z < v3Min.z)
	{
		return false;
	}
	if (m_v3Min.z > v3Max.z)
	{
		return false;
	}

	return true;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);

		return;
	}
	for (uint i = 0;i < m_uChildren;i++)
	{
		m_pChild[i]->Display(a_nIndex);
	}
}

void MyOctant::Display(vector3 a_v3Color)
{
	for (uint i = 0;i < m_uChildren;i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	uint leafs = m_lChild.size();
	for (uint i = 0;i < leafs;i++)
	{
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void MyOctant::ClearEntityList(void)
{
	for (uint i = 0;i < m_uChildren;i++)
	{
		m_pChild[i]->ClearEntityList();
	}
	m_EntityList.clear();
}

void MyOctant::Subdivide(void)
{
	if (m_uLevel >= m_uMaxLevel)
	{
		return;
	}
	if (m_uChildren != 0)
	{
		return;
	}
	m_uChildren = 8;

	float fSize = m_fSize / 4.0f;
	float fSize0 = fSize *2.0f;
	vector3 v3Center;

	v3Center = m_v3Center;
	v3Center.x -= fSize;
	v3Center.y -= fSize;
	v3Center.z -= fSize;
	//0- bottom left back
	m_pChild[0] = new MyOctant(v3Center, fSize0);
	//1- b r b
	v3Center.x += fSize0;
	m_pChild[1] = new MyOctant(v3Center, fSize0);
	//2- b r f
	v3Center.z += fSize0;
	m_pChild[2] = new MyOctant(v3Center, fSize0);

	// 3 - b l f
	v3Center.x -= fSize0;
	m_pChild[3] = new MyOctant(v3Center, fSize0);

	//4 - t l f
	v3Center.y += fSize0;
	m_pChild[4] = new MyOctant(v3Center, fSize0);

	// 5 - t l b
	v3Center.z -= fSize0;
	m_pChild[5] = new MyOctant(v3Center, fSize0);

	// 6- t r b
	v3Center.x += fSize0;
	m_pChild[6] = new MyOctant(v3Center, fSize0);

	// 7 - t r f
	v3Center.z += fSize0;
	m_pChild[7] = new MyOctant(v3Center, fSize0);

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel+1;
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[i]->Subdivide();
		}
	}


}

MyOctant * MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild > 7)
	{
		return nullptr;
	}
	return m_pChild[a_nChild];
}

MyOctant * MyOctant::GetParent(void)
{
	return m_pParent;
}

bool MyOctant::IsLeaf(void)
{
	return m_uChildren == 0;
}

bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	uint count = 0;
	uint nObjectCount = m_pEntityMngr->GetEntityCount();
	for (uint i = 0;i < nObjectCount;i++)
	{
		if (IsColliding(i))
		{
			count++;
		}
		if (count > a_nEntities)
		{
			return true;
		}
	}
	return false;
}

void MyOctant::KillBranches(void)
{
	for (uint i = 0;i < m_uChildren;i++)
	{
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}
	m_uChildren = 0;
}

void MyOctant::ConstructTree(uint a_nMaxLevel)
{
	if (m_uLevel != 0)
	{
		return;
	}

	m_uMaxLevel = a_nMaxLevel;

	m_uOctantCount = 1;

	m_EntityList.clear();

	KillBranches();
	m_lChild.clear();

	if (ContainsMoreThan(m_uIdealEntityCount))
	{
		Subdivide();
	}
	AssignIDtoEntity();
	ConstructList();
}

void MyOctant::AssignIDtoEntity(void)
{
	for (uint i = 0; i < m_uChildren;i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}
	if (m_uChildren == 0)
	{
		uint nEntities = m_pEntityMngr->GetEntityCount();
		for (int i = 0;i < nEntities;i++)
		{
			if (IsColliding(i))
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}

uint MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void MyOctant::Release(void)
{
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0;
	m_EntityList.clear();
	m_lChild.clear();
}

void MyOctant::Init(void)
{
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_uID = m_uOctantCount;

	m_uLevel = 0;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint n = 0;n < 8;n++)
	{
		m_pChild[n] = nullptr;
	}
}

void MyOctant::ConstructList(void)
{
	for (uint i = 0;i < m_uChildren;i++)
	{
		m_pChild[i]->ConstructList();
	}
	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
