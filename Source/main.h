#pragma once
///include
#include "../Externals/Include/Include.h"
#include "CTexture.h"
#include "CVertexBufferObject.h"
#include "CSkybox.h"
#include <math.h>

///define
#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3
#define FOG 4
#define SHADOW 5
#define BLINNPHONG 6
#define SHADOW_MAP_SIZE 4096

///using namespace
using namespace glm;
using namespace std;


///struct
typedef struct Shape
{
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	int drawCount;
	int materialID;
} Shape;

typedef struct Material
{
	GLuint diffuse_tex;
	aiColor3D diffuse;
	aiColor3D specular;
	aiColor3D ambient = aiColor3D(0.0f,0.0f,0.0f);
	float shininess = 384.314;
} Material;

typedef struct Model {
	std::vector<Material> materials;
	std::vector<Shape> shapes;
	mat4 model_matrix;
	vec3 center;
} Model;

/*struct
{
	struct
	{
		GLint mv_matrix;
		GLint proj_matrix;
	} render;
	struct
	{
		GLint view_matrix;
		GLint eye_position;
	} skybox;
} uniforms;*/

typedef struct _TextureData
{
	_TextureData(void) :
		width(0),
		height(0),
		data(0)
	{
	}

	int width;
	int height;
	unsigned char* data;
} TextureData;

struct
{
	struct
	{
		GLint mvp;
		GLint light_pos;
	} light;
	struct
	{
		GLuint  shadow_tex;
		GLint   shadow_matrix;
	} view;
	struct
	{
		GLint um4v;
		GLint um4mv;
		GLint um4p;
		GLint um4m;
		GLint us2dtex;
		GLint diffuse;
		GLint specular;
		GLint ambient;
		GLint shininess;
	} blinnPhong;
	struct
	{
		GLint fogSwitch;
		GLint shadowSwitch;
		GLint blinnPhongSwitch;
	} parameter;
} uniforms;

struct
{
	GLuint fbo;
	GLuint depthMap;
} shadowBuffer;

struct
{
	int width;
	int height;
} viewportSize;

struct
{
	GLint vColor;
	GLint fAmbientIntensity;
	GLint vDirection;
	GLint projectionMatrix;
	GLint gSampler;
} skyboxUniform;

typedef struct
{
	vec3 position;
	float horizontalAngle;
	float verticalAngle;
	float currentFov = 0;
} Camera;

/// timer
GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

/// models
Model streets[16]; 
unsigned int streetCount = 0;
Model grass[225];
Material grassTexture;
unsigned int grassCount = 0;
Model coord[457];
unsigned int coordCount = 0;

/// shader
GLuint program;
GLuint depthProg;
GLuint skyBoxProgram;
mat4 view_matrix;
mat4 proj_matrix;

/// sky box
CSkybox skybox;
float side = 1000.0f;

/// camera setting
Camera actualCamera;
Camera cameras[2];
unsigned int cameraCount = 2;
unsigned int nowCamera = 0;
float initialFoV = 45.0f;
float viewportAspect;

/// bezier curve
int detailOfLevel = 100;
int index = 0;
float pastTime = 0;
vector<vec3> curve;
const float basis_matrix[4][4] = {
	{-1, 3, -3, 1},
	{3, -6, 3, 0},
	{-3, 3, 0, 0},
	{1, 0, 0, 0}
};
vec3 controlPoints[] = {
	vec3(20, -10 , 470), vec3(-10, -10, 390), vec3(95, -10, 300),
	vec3(170, -10, 310)
};

/// track ball setting
float deltaTime;
float speed = 0.3f;
float mouseSpeed = 0.005f;
vec3 direction;
vec3 rightDirection;
vec3 up;
vec2 start;

/// mouse info
int xpos, ypos;
bool mousePressOrNot = false;

///Special effects switch parameters(not location)
int fogSwitch = 0;
int shadowSwitch = 0;
int blinnPhongSwitch = 0;

/// others
bool printOrNot = true;


/// sky box shader
const char *skybox_fs_glsl[] =
{
	"#version 410 core                          \n"
	"                                           \n"
	"uniform samplerCube tex_cubemap;           \n"
	"                                           \n"
	"in VS_OUT                                  \n"
	"{                                          \n"
	"    vec3    tc;                            \n"
	"} fs_in;                                   \n"
	"                                           \n"
	"layout (location = 0) out vec4 color;      \n"
	"                                           \n"
	"void main(void)                            \n"
	"{                                          \n"
	"    color = texture(tex_cubemap, fs_in.tc);\n"
	"}                                          \n"
	"                                           \n"
};

const char *skybox_vs_glsl[] =
{
	"#version 410 core                                         \n"
	"                                                          \n"
	"out VS_OUT                                                \n"
	"{                                                         \n"
	"    vec3    tc;                                           \n"
	"} vs_out;                                                 \n"
	"                                                          \n"
	"uniform mat4 view_matrix;                                 \n"
	"                                                          \n"
	"void main(void)                                           \n"
	"{                                                         \n"
	"    vec3[4] vertices = vec3[4](vec3(-1.0, -1.0, 1.0),     \n"
	"                               vec3( 1.0, -1.0, 1.0),     \n"
	"                               vec3(-1.0,  1.0, 1.0),     \n"
	"                               vec3( 1.0,  1.0, 1.0));    \n"
	"                                                          \n"
	"    vs_out.tc = mat3(view_matrix) * vertices[gl_VertexID];\n"
	"                                                          \n"
	"    gl_Position = vec4(vertices[gl_VertexID], 1.0);       \n"
	"}                                                         \n"
	"                                                          \n"
};

///////////////////////////////////////////////////////////////
const char *depth_vs[] =
{
	"#version 410 core                         \n"
	"                                          \n"
	"uniform mat4 mvp;                         \n"
	"                                          \n"
	"layout (location = 0) in vec4 position;   \n"
	"                                          \n"
	"void main(void)                           \n"
	"{                                         \n"
	"    gl_Position = mvp * position;         \n"
	"}                                         \n"
};

const char *depth_fs[] =
{
	"#version 410 core                                \n"
	"                                                 \n"
	"out vec4 fragColor;                              \n"
	"                                                 \n"
	"void main()                                      \n"
	"{                                                \n"
	"    fragColor = vec4(vec3(gl_FragCoord.z), 1.0); \n"
	"}                                                \n"
};