/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/05
----------------------------------------------*/
#ifndef __MYRIGIDBODYCLASS_H_
#define __MYRIGIDBODYCLASS_H_

#include "MyMesh.h"
#include "MyCamera.h"
#include "Definitions.h"

namespace Simplex 
{
class MyrigidBody
	{
	public:
		MyMesh* m_BoundingSphere = nullptr;
		MyMesh* m_BoundingBox = nullptr;
		vector3 v3Min;
		vector3 v3Max;
		vector3 v3Center;
		MyrigidBody(MyMesh* a_pObject);
		void Render(MyCamera* a_pCamera, matrix4 a_m4Model);
	};
}


#endif