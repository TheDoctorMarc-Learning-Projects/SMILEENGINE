#include "ComponentMesh.h"

#include "Glew/include/GL/glew.h" 
#include "DevIL/include/IL/ilu.h"
#include "SmileApp.h"
#include "SmileFBX.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"

ComponentMesh::ComponentMesh(par_shapes_mesh* mesh, std::string name)
{
	SetName(name); 
	FillComponentBuffers(); 
	std::get<GameObject*>(parent)->AddComponentToMesh(DBG_NEW ComponentTransform(), this); // TODO: what should the transform matrix have? 
	type = MESH; 
	meshType = Mesh_Type::PRIMITIVE; 

	GenerateModelMeshFromParShapes(mesh); 
}

ComponentMesh::ComponentMesh(ModelMeshData* mesh, std::string name) : model_mesh(mesh)
{
	SetName(name);
	FillComponentBuffers();
	std::get<GameObject*>(parent)->AddComponentToMesh(DBG_NEW ComponentTransform(), this);
	type = MESH;
	meshType = Mesh_Type::MODEL;
}

void ComponentMesh::FillComponentBuffers() // needed in order to have either a Component or a vector of Components in each slot
{
	components[TRANSFORM] = (Component*)NULL; // one 
	components[MESH] = (Component*)NULL; // placeholder
	components[MATERIAL] = (Component*)NULL; // one 
	components[LIGHT] = (Component*)NULL;  // placeholder
}

ComponentMesh::~ComponentMesh()
{
	
}

void ComponentMesh::Draw()
{
	// Draw the OpenGL mesh 
	if (model_mesh != nullptr)  
	{
		// Transformation -> for the mom let's try to use the parent gameobject transform matrix 
		glPushMatrix(); 
		glMultMatrixf(dynamic_cast<ComponentTransform*>(std::get<Component*>(std::get<GameObject*>(parent)->GetComponent(TRANSFORM)))->GetGlobalMatrix().Transposed().ptr()); 

		// Cient states
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//glEnableClientState(GL_TEXTURE_BUFFER);
		//glEnableClientState(GL_COLOR_ARRAY); 

		// UV buffer
		if (model_mesh->UVs != nullptr)
		{
			glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_UVs);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		}

		// color buffer 
		/*if (mesh->color != nullptr) {
			glBindBuffer(GL_COLOR_ARRAY, mesh->id_color);
			glColorPointer(3, GL_FLOAT, 8 * sizeof(GLfloat), 0);
		}*/

		// texture buffer
		ComponentMaterial* mat = dynamic_cast<ComponentMaterial*>(std::get<Component*>(GetComponent(MATERIAL)));
		if (mat != nullptr)
		{
			glBindTexture(GL_TEXTURE_2D, mat->textureInfo->id_texture);

		}
			
		// normal buffer
		if (model_mesh->normals != nullptr)
		{
			glBindBuffer(GL_NORMAL_ARRAY, model_mesh->id_normals);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}

		// vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		// index buffer 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_mesh->id_index);
		glDrawElements(GL_TRIANGLES, model_mesh->num_index * 3, (meshType == MODEL) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, NULL);


		// Cient states
		//glDisableClientState(GL_COLOR_ARRAY);
		//glDisableClientState(GL_TEXTURE_BUFFER);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);


		if(App->object_manager->debug)
			DebugDraw();


		// Transformation
		glPopMatrix(); 
	}
}

void ComponentMesh::DebugDraw()
{
	// draw normals
		if (model_mesh->normals != nullptr)
		{
			glColor3f(0.f, 1.0f, 0.f);
			static float normalFactor = 20.f;

			for (int i = 0; i < model_mesh->num_normals * 3; i += 3)
			{
				glBegin(GL_LINES);

				vec3 normalVec = normalize({ model_mesh->normals[i], model_mesh->normals[i + 1], model_mesh->normals[i + 2] });
				glVertex3f(model_mesh->vertex[i], model_mesh->vertex[i + 1], model_mesh->vertex[i + 2]);
				glVertex3f(model_mesh->vertex[i] + normalVec.x, model_mesh->vertex[i + 1] + normalVec.y, model_mesh->vertex[i + 2] + normalVec.z);

				glEnd();
			}
			// draw face normals

			float size = 5.f;
			for (int i = 0; i < model_mesh->num_normals; i += 3) {

				glBegin(GL_LINES);
				glColor3f(0, 1, 0);

				float vec1_x = model_mesh->vertex[model_mesh->index[i] * 3];
				float vec1_y = model_mesh->vertex[(model_mesh->index[i] * 3) + 1];
				float vec1_z = model_mesh->vertex[(model_mesh->index[i] * 3) + 2];


				float vec2_x = model_mesh->vertex[model_mesh->index[i + 1] * 3];
				float vec2_y = model_mesh->vertex[(model_mesh->index[i + 1] * 3) + 1];
				float vec2_z = model_mesh->vertex[(model_mesh->index[i + 1] * 3) + 2];

				float vec3_x = model_mesh->vertex[model_mesh->index[i + 2] * 3];
				float vec3_y = model_mesh->vertex[(model_mesh->index[i + 2] * 3) + 1];
				float vec3_z = model_mesh->vertex[(model_mesh->index[i + 2] * 3) + 2];

				float mid_x = (vec1_x + vec2_x + vec3_x) / 3;
				float mid_y = (vec1_y + vec2_y + vec3_y) / 3;
				float mid_z = (vec1_z + vec2_z + vec3_z) / 3;

				vec3 face_center = { mid_x, mid_y, mid_z };

				vec3 vec_v1_center = face_center - vec3(vec1_x, vec1_y, vec1_z);
				vec3 vec_v2_center = face_center - vec3(vec2_x, vec2_y, vec2_z);

				vec3 normal_vec = cross(vec_v1_center, vec_v2_center);
				vec3 normalized_normal_vec = normalize(normal_vec);

				glVertex3f(mid_x, mid_y, mid_z);

				glVertex3f(mid_x + normalized_normal_vec.x * size, mid_y + normalized_normal_vec.y * size, mid_z + normalized_normal_vec.z * size);

				glEnd();
			}

		}
}

void ComponentMesh::Enable()
{
	// The variant "components" holds either a Component* if index = 0 or a std list of Components* if index = 1
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			Component* c = std::get<Component*>(comp);
			if (c)
				c->Enable(); 
		}
		
		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				comp2->Enable();
		}

	}
	active = true;
}

// ----------------------------------------------------------------- 
void ComponentMesh::Disable()
{
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			Component* c = std::get<Component*>(comp);
			if (c)
				c->Disable();
		}

		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				comp2->Disable();
		}

	}
	active = false;
}

// -----------------------------------------------------------------
void ComponentMesh::Update()
{
	// First draw it
	Draw(); 

	// Then update components
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			Component* c = std::get<Component*>(comp);
			if (c)
				c->Update();
		}
		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				comp2->Update();
		}

	}

}

// -----------------------------------------------------------------
void ComponentMesh::CleanUp()
{
	// Free model mesh
	if (model_mesh != nullptr)
	{
		if (model_mesh->vertex != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->vertex);
			RELEASE_ARRAY(model_mesh->vertex);
		}

		if (model_mesh->index != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->index);
			RELEASE_ARRAY(model_mesh->index);
		}

		if (model_mesh->normals != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->normals);
			RELEASE_ARRAY(model_mesh->normals);
		}

		if (model_mesh->color != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->color);
			RELEASE_ARRAY(model_mesh->color);
		}

		if (model_mesh->UVs != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->UVs);
			RELEASE_ARRAY(model_mesh->UVs);
		}


		RELEASE(model_mesh); 
	}

	// Free Components
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			Component* c = std::get<Component*>(comp);
			if (c)
			{
				c->CleanUp();
				RELEASE(c);
			}
		
		}

		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
			{
				comp2->CleanUp();
				RELEASE(comp2);
			}
			vComp.clear();
		}
	}


 
	//delete[] components;


}



void ComponentMesh::GenerateModelMeshFromParShapes(par_shapes_mesh* mesh) 
{
	if (model_mesh == nullptr)
	{
		par_shapes_unweld(mesh, true);
		par_shapes_compute_normals(mesh);
		par_shapes_translate(mesh, 0.f, 0.f, 0.f); // TODO: do this with the gameobject transform

		model_mesh = DBG_NEW ModelMeshData();

		model_mesh->num_vertex = mesh->npoints;
		model_mesh->vertex = new float[model_mesh->num_vertex * 3];
		memcpy(model_mesh->vertex, mesh->points, sizeof(float) * model_mesh->num_vertex * 3);

		model_mesh->num_index = mesh->ntriangles * 3;
		model_mesh->index = new uint[model_mesh->num_index];
		memcpy(model_mesh->index, mesh->triangles, sizeof(uint) * model_mesh->num_index);

		if (mesh->normals != nullptr)
		{
			model_mesh->num_normals = model_mesh->num_vertex; 
			model_mesh->normals = new float[model_mesh->num_vertex * 3];
			memcpy(model_mesh->normals, mesh->normals, sizeof(float) * model_mesh->num_vertex * 3);
		}

		if (mesh->tcoords != nullptr) 
		{
			model_mesh->num_UVs = model_mesh->num_vertex;
			model_mesh->UVs = new float[model_mesh->num_vertex * 2];
			memcpy(model_mesh->UVs, mesh->tcoords, sizeof(float) * model_mesh->num_UVs);
		}
		 
		// Generate Mesh Buffers
		GenerateBuffers();

		// bounding box
		ComputeSpatialData();  // TODO: the par shapes already has a compute aabb function, pass it a float* AABB
	}

	par_shapes_free_mesh(mesh); 
}

void ComponentMesh::ComputeSpatialData()
{
	if (model_mesh != nullptr)
	{
		// Get the minimum and maximum x,y,z to create a bounding box 
		for (uint i = 0; i < model_mesh->num_vertex; i += 3)
		{
			// first, initialize the min-max coords to the first vertex, 
			// in order to compare the following ones with it
			if (i == 0)
			{
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_X] = model_mesh->vertex[i];
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_X] = model_mesh->vertex[i];
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_Y] = model_mesh->vertex[i + 1];
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_Y] = model_mesh->vertex[i + 1];
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_Z] = model_mesh->vertex[i + 2];
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_Z] = model_mesh->vertex[i + 2];
				continue;
			}

			// find min-max X coord
			if (model_mesh->vertex[i] < model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_X])
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_X] = model_mesh->vertex[i];
			else if (model_mesh->vertex[i] > model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_X])
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_X] = model_mesh->vertex[i];

			// find min-max Y coord
			if (model_mesh->vertex[i + 1] < model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_Y])
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_Y] = model_mesh->vertex[i + 1];
			else if (model_mesh->vertex[i + 1] > model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_Y])
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_Y] = model_mesh->vertex[i + 1];

			// find min-max Z coord
			if (model_mesh->vertex[i + 2] < model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_Z])
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MIN_Z] = model_mesh->vertex[i + 2];
			else if (model_mesh->vertex[i + 2] > model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_Z])
				model_mesh->minmaxCoords[model_mesh->minMaxCoords::MAX_Z] = model_mesh->vertex[i + 2];

		}

		model_mesh->ComputeMeshSpatialData();
	}

}

void ComponentMesh::GenerateBuffers()
{
	// Normals Buffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_mesh->id_normals);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * model_mesh->num_normals * 3, model_mesh->normals, GL_STATIC_DRAW);

	// Uvs vBuffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_UVs));
	glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_UVs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model_mesh->num_UVs * 2, model_mesh->UVs, GL_STATIC_DRAW);

	// Color Buffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_color));
	glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model_mesh->num_color * 3, model_mesh->color, GL_STATIC_DRAW);

	// Vertex Buffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model_mesh->num_vertex * 3, model_mesh->vertex, GL_STATIC_DRAW);

	// Index Buffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_index));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_mesh->id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * model_mesh->num_index, model_mesh->index, GL_STATIC_DRAW);
}

void ComponentMesh::OnTransform() // similar to gameobject: transform myself, and child components
{
	// 1) Transform myself 
	dynamic_cast<ComponentTransform*>(std::get<Component*>(components[TRANSFORM]))->CalculateAllMatrixes();

    // 2) Then, transform components
	for (auto& comp : components)
	{
		if (comp.index() == 0)
			std::get<Component*>(comp)->OnTransform();

		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				comp2->OnTransform();

		}
	}
}

bool ComponentMesh::AddComponent(Component* comp)
{
	if (comp != nullptr)
	{
		// Check if the component can be added to a Mesh 
		if (std::get<1>(uniquenessMap.at(comp->type)) == false)
			return false;

		// Check if there cannot be more than one instance of that component
		if (std::get<2>(uniquenessMap.at(comp->type)) == false)
		{
			// Check if there already exists a component of that type
			if (std::get<Component*>(components[comp->type]) != nullptr)
			{
				std::get<Component*>(components[comp->type])->CleanUp();
				RELEASE(std::get<Component*>(components[comp->type]));
			}

			std::get<Component*>(components[comp->type]) = comp;

			goto Enable;
		}
		else // if there can me more than one, push it to that component type list 
			std::get<std::vector<Component*>>(components[comp->type]).push_back(comp);

	Enable:
		comp->parent = this;
		comp->Enable();

		return true;
	}

	return false;
}