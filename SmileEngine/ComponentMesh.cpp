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
	type = MESH; 
	meshType = Mesh_Type::PRIMITIVE; 

	// Generate mesh buffers from par_shapes
	GenerateModelMeshFromParShapes(mesh); 

	// Assign the parent transform once the mesh center has been computed 
	if(parent)
		parent->SetupTransformAtMeshCenter();
}

ComponentMesh::ComponentMesh(ModelMeshData* mesh, std::string name) : model_mesh(mesh)
{
	SetName(name);
	type = MESH;
	meshType = Mesh_Type::MODEL;

	// Generate mesh buffers
    GenerateBuffers();

	// Assign the parent transform once the mesh center has been computed 
	if (parent)
		parent->SetupTransformAtMeshCenter();
}

ComponentMesh::~ComponentMesh()
{
	
}

void ComponentMesh::Draw()
{
	// Draw the OpenGL mesh 
	if (model_mesh != nullptr)  
	{
		// Transformation  
		glPushMatrix(); 
		glMultMatrixf(dynamic_cast<ComponentTransform*>(parent->GetParent()->GetComponent(TRANSFORM))->GetGlobalMatrix().Transposed().ptr()); 

		// Cient states
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		

		// Material
		ComponentMaterial* mat = dynamic_cast<ComponentMaterial*>(parent->GetComponent(MATERIAL));
		if (mat != nullptr)
		{
			// UVs buffer
			if (model_mesh->UVs != nullptr)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_UVs);
				glTexCoordPointer(2, GL_FLOAT, 0, NULL);

				// texture buffer
				glBindTexture(GL_TEXTURE_2D, mat->textureInfo->id_texture);
			}

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


		// Disanle Cient states && clear data
		glColor3f(1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

	    // Debug on top
		DebugDraw();

		// Transformation
		glPopMatrix(); 
	}
}

void ComponentMesh::DebugDraw()
{
	
		if (model_mesh->normals != nullptr)
		{
			// draw vertex normals
			if (debugData.vertexNormals)
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

				glColor3f(1.0f, 1.0f, 1.0f);
			}

			// draw face normals
			if (debugData.faceNormals)
			{
				glColor3f(0.f, 1.0f, 0.f);

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

				glColor3f(1.0f, 1.0f, 1.0f);
			}
		}


		// stencil
		if (debugData.outilineMesh || debugData.outlineParent)
		{
			// Init 
			if (!glIsEnabled(GL_STENCIL_TEST))
				glEnable(GL_STENCIL_TEST);

			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);

			// Transformation  
			glPushMatrix();
			glMultMatrixf(dynamic_cast<ComponentTransform*>(parent->GetParent()->GetComponent(TRANSFORM))->GetGlobalMatrix().Transposed().ptr());

			// Cient states
			glEnableClientState(GL_VERTEX_ARRAY);

			// Set drawing data: todo = colors diff on mesh vs obj
			glColor3f(0, 1, 0);
			glLineWidth(3);

			// The vertex and index data should be provided inside 
			// vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_vertex);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			// index buffer 
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_mesh->id_index);
			glDrawElements(GL_TRIANGLES, model_mesh->num_index * 3, (meshType == MODEL) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, NULL);

			// Disable Cient states && clear data
			glColor3f(1.0f, 1.0f, 1.0f);
			glLineWidth(1);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisable(GL_STENCIL_TEST);

			// Transformation
			glPopMatrix();
		}


}

// -----------------------------------------------------------------
void ComponentMesh::Update()
{
	Draw(); 
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
		model_mesh->ComputeMeshSpatialData();
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

// Executed only once 
void ModelMeshData::ComputeMeshSpatialData()
{
	if (computedData)
		return; 

	// 1) find the min-max coords
	for (uint i = 0; i < num_vertex; i += 3)
	{
		// first, initialize the min-max coords to the first vertex, 
		// in order to compare the following ones with it
		if (i == 0)
		{
			minmaxCoords[minMaxCoords::MIN_X] = vertex[i];
			minmaxCoords[minMaxCoords::MAX_X] = vertex[i];
			minmaxCoords[minMaxCoords::MIN_Y] = vertex[i + 1];
			minmaxCoords[minMaxCoords::MAX_Y] = vertex[i + 1];
			minmaxCoords[minMaxCoords::MIN_Z] = vertex[i + 2];
			minmaxCoords[minMaxCoords::MAX_Z] = vertex[i + 2];
			continue;
		}

		// find min-max X coord
		if (vertex[i] < minmaxCoords[minMaxCoords::MIN_X])
			minmaxCoords[minMaxCoords::MIN_X] = vertex[i];
		else if (vertex[i] > minmaxCoords[minMaxCoords::MAX_X])
			minmaxCoords[minMaxCoords::MAX_X] = vertex[i];

		// find min-max Y coord
		if (vertex[i + 1] < minmaxCoords[minMaxCoords::MIN_Y])
			minmaxCoords[minMaxCoords::MIN_Y] = vertex[i + 1];
		else if (vertex[i + 1] > minmaxCoords[minMaxCoords::MAX_Y])
			minmaxCoords[minMaxCoords::MAX_Y] = vertex[i + 1];

		// find min-max Z coord
		if (vertex[i + 2] < minmaxCoords[minMaxCoords::MIN_Z])
			minmaxCoords[minMaxCoords::MIN_Z] = vertex[i + 2];
		else if (vertex[i + 2] > minmaxCoords[minMaxCoords::MAX_Z])
			minmaxCoords[minMaxCoords::MAX_Z] = vertex[i + 2];

	}

	// 2) find the center 
	float c_X = (minmaxCoords[minMaxCoords::MIN_X] + minmaxCoords[minMaxCoords::MAX_X]) / 2;
	float c_Y = (minmaxCoords[minMaxCoords::MIN_Y] + minmaxCoords[minMaxCoords::MAX_Y]) / 2;
	float c_Z = (minmaxCoords[minMaxCoords::MIN_Z] + minmaxCoords[minMaxCoords::MAX_Z]) / 2;
	meshCenter = vec3(c_X, c_Y, c_Z);

	// 3) find the bounding sphere radius
	vec3 min_Vec(minmaxCoords[minMaxCoords::MIN_X], minmaxCoords[minMaxCoords::MIN_Y], minmaxCoords[minMaxCoords::MIN_Z]);
	vec3 max_Vec(minmaxCoords[minMaxCoords::MAX_X], minmaxCoords[minMaxCoords::MAX_Y], minmaxCoords[minMaxCoords::MAX_Z]);
	vec3 rad_Vec = (max_Vec - min_Vec) / 2;
	meshBoundingSphereRadius = (double)sqrt(rad_Vec.x * rad_Vec.x + rad_Vec.y * rad_Vec.y + rad_Vec.y * rad_Vec.y);

		
	computedData = true; 
}

void ComponentMesh::OnSelect(bool select)
{
	debugData.outilineMesh = !debugData.outilineMesh; 
}