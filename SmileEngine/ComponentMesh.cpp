#include "ComponentMesh.h"

#include "Glew/include/GL/glew.h" 
#include "DevIL/include/IL/ilu.h"
#include "SmileApp.h"
#include "SmileFBX.h"

ComponentMesh::ComponentMesh(par_shapes_mesh* mesh)
{
	FillComponentBuffers(); 
	type = MESH; 

	// TODO: pass the mesh by ref to a function that fills the model mesh data pointer, and then immediately free the par_shapes_mesh
	GenerateModelMeshFromParShapes(mesh); 
}

ComponentMesh::ComponentMesh(ModelMeshData* mesh) : model_mesh(mesh)
{
	FillComponentBuffers();
	type = MESH;
}

void ComponentMesh::FillComponentBuffers() // needed in order to have either a Component or a vector of Components in each slot
{
	components[TRANSFORM] = DBG_NEW Component(); // one 
	components[MESH] = DBG_NEW Component(); // placeholder
	components[MATERIAL] = DBG_NEW Component(); // one 
	components[LIGHT] = DBG_NEW Component();  // placeholder
}

ComponentMesh::~ComponentMesh()
{
	
}

void ComponentMesh::Draw()
{
	// Draw the OpenGL mesh 
	if (model_mesh != nullptr)  
	{
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
		if (model_mesh->texture != nullptr)
			glBindTexture(GL_TEXTURE_2D, model_mesh->id_texture);
		else
			glColor3f(0.3f, 0.3f, 0.3f);

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
		glDrawElements(GL_TRIANGLES, model_mesh->num_index * 3, GL_UNSIGNED_INT, NULL);


		// Cient states
		//glDisableClientState(GL_COLOR_ARRAY);
		//glDisableClientState(GL_TEXTURE_BUFFER);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);


		/*// draw normals
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

		}*/
	}
}


void ComponentMesh::Enable()
{
	// The variant "components" holds either a Component* if index = 0 or a std list of Components* if index = 1
	for (auto& comp : components)
	{
		if (comp.index() == 0)
			std::get<Component*>(comp)->Enable();
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
			std::get<Component*>(comp)->Disable();
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
			std::get<Component*>(comp)->Update();
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
			delete[] model_mesh->vertex;
		}

		if (model_mesh->index != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->index);
			delete[] model_mesh->index;
		}

		if (model_mesh->normals != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->normals);
			delete[] model_mesh->normals;
		}

		if (model_mesh->color != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->color);
			delete[] model_mesh->color;
		}

		if (model_mesh->UVs != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)& model_mesh->UVs);
			delete[] model_mesh->UVs;
		}

		if (model_mesh->texture != nullptr)
		{
			glDeleteTextures(1, (GLuint*)& model_mesh->texture);
			//delete[] mesh->texture; 
		}

		RELEASE(model_mesh); 
	}

	// Free Components
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			std::get<Component*>(comp)->CleanUp();
			RELEASE(std::get<Component*>(comp));
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

// -----------------------------------------------------------------
void ComponentMesh::AssignTexture(const char* path)
{
	

	if (model_mesh != nullptr) 
	{
		// Check if mesh had an image already 
	/*if (mesh->texture != nullptr)
		glDeleteTextures(1, (GLuint*)& mesh->texture);*/ // TODO: re-work this 

		// Devil stuff
		ilGenImages(1, &(ILuint)model_mesh->id_texture);
		ilBindImage((ILuint)model_mesh->id_texture);

		ILboolean success = ilLoadImage(path);

		if ((bool)success)
		{
			iluFlipImage();
			ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		
			glGenTextures(1, (GLuint*)& model_mesh->id_texture);
			glBindTexture(GL_TEXTURE_2D, (GLuint)model_mesh->id_texture);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), (GLuint)ilGetInteger(IL_IMAGE_WIDTH),
				(GLuint)ilGetInteger(IL_IMAGE_HEIGHT),	0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, 
				ilGetData());

			model_mesh->texture = ilGetData(); 

			glGenerateMipmap(GL_TEXTURE_2D);

		}
		
		glBindTexture(GL_TEXTURE_2D, 0);
		ilDeleteImage((ILuint)model_mesh->id_texture);
	}
	
}


// -----------------------------------------------------------------
void ComponentMesh::AssignCheckersTexture()
{
#ifndef CHECKERS_SIZE
#define CHECKERS_SIZE 20
#endif 

	if (model_mesh != nullptr)  
	{
		// Devil stuff
		ilGenImages(1, &(ILuint)model_mesh->id_texture);
		ilBindImage((ILuint)model_mesh->id_texture);

		// Generated the checkered image
		GLubyte checkImage[CHECKERS_SIZE][CHECKERS_SIZE][4];
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
				int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
				checkImage[i][j][0] = (GLubyte)c;
				checkImage[i][j][1] = (GLubyte)c;
				checkImage[i][j][2] = (GLubyte)c;
				checkImage[i][j][3] = (GLubyte)255;
			}
		}

		iluFlipImage();
		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		glGenTextures(1, (GLuint*)& model_mesh->id_texture);
		glBindTexture(GL_TEXTURE_2D, (GLuint)model_mesh->id_texture);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_SIZE,
			CHECKERS_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			checkImage);

		model_mesh->texture = (ILubyte*)checkImage; 

		glGenerateMipmap(GL_TEXTURE_2D);


		glBindTexture(GL_TEXTURE_2D, 0);
		ilDeleteImage((ILuint)model_mesh->id_texture);
		
	}

}

void ComponentMesh::GenerateModelMeshFromParShapes(par_shapes_mesh* mesh) 
{
	// TODO: this is terribly wrong: the new mesh buffers point to the parshapes mesh buffers, 
	// so when the parshapes mesh is freed, they point to nowhere. 
	// Instead of the operator "=", memcpy the contents of the par shapes mesh to the new mesh. 

	if (model_mesh == nullptr)
	{
		par_shapes_unweld(mesh, true);
		par_shapes_compute_normals(mesh);
		par_shapes_translate(mesh, 0.0f, 0.0f, 0.0f); // TODO: do this with the gameobject transform
	
		model_mesh = DBG_NEW ModelMeshData();

		model_mesh->num_vertex = mesh->npoints;
		model_mesh->vertex = new float[model_mesh->num_vertex * 3];
		memcpy(model_mesh->vertex, mesh->points, sizeof(float) * model_mesh->num_vertex * 3);

		model_mesh->num_index = mesh->ntriangles * 3;
		model_mesh->index = new uint[model_mesh->num_index * 3];
		memcpy(model_mesh->index, mesh->triangles, sizeof(uint) * model_mesh->num_index);

		if (mesh->normals != nullptr)
		{
			model_mesh->num_normals = model_mesh->num_vertex; 
			model_mesh->normals = new float[model_mesh->num_vertex * 3];
			memcpy(model_mesh->normals, mesh->normals, sizeof(float) * model_mesh->num_vertex * 3);
		}

		if (mesh->tcoords != nullptr) 
		{
			model_mesh->num_UVs = model_mesh->num_vertex * 2;
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