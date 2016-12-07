#include "Timer.hpp"
#include "camera.h"
//#include "VoxMap.hpp"
#include <string>
using namespace std;




std::string windowName = "VoxEngine -- ";
// How many frames time values to keep
// The higher the value the smoother the result is...
// Don't make it 0 or less :)
#define FRAME_VALUES 10
// An array to store frame times:
Uint32 frametimes[FRAME_VALUES];
// Last calculated SDL_GetTicks
Uint32 frametimelast;
// total frames rendered
Uint32 framecount;
// the value you want
float framespersecond;
// This function gets called once on startup.

// On régule le nombre de frame par seconde
const int FRAMES_PER_SECOND = 1000000;
double step = 0.2f;

bool intersection2DEdge (glm::vec3 A, glm::vec3 B, glm::vec3 I, glm::vec3 P) {

	
	glm::vec3 D,E;

   D.x = B.x - A.x;

   D.y = B.y - A.y;

   E.x = P.x - I.x;

   E.y = P.y - I.y;
   double t, u;

   double denom = D.x*E.y - D.y*E.x;

   if (denom==0)

       return -1;   // erreur, cas limite

   t = - (A.x*E.y-I.x*E.y-E.x*A.y+E.x*I.y) / denom;

   if (t<0 || t>=1)

      return 0;

   u = - (-D.x*A.y+D.x*I.y+D.y*A.x-D.y*I.x) / denom;

   if (u<0 || u>=1)

      return 0;

   return 1;
  
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Camera cam;

/**
*	contexte qui contient les constantes globales (caméras, lumières, UBO)
*/
glContext* context;
VoxMap* testVox;

void initCamera(glContext* context) {
	cam = Camera(0.0f, 133.0f, 0.0f);
    cam.bind(&context->globalUBO.view);


    cam.see(2.0f, 0.0f, 0.0f);
    cam.setSpeed(step);
    cam.setBoost(10.0f);
    cam.setSensivity(0.3f);
}

glm::vec3 getVoxMapIndicesByOpenGlPosition(glm::vec3& position) {
	return glm::vec3(
		ceil((position.x) / testVox->voxelSize[0]),
		position.y / testVox->voxelSize[1],
		ceil((position.z) / testVox->voxelSize[2])
	);
}

bool isInsideMap(glm::vec3& position) {
return (
	(position.x < testVox->map.width()) && 
	(position.y < testVox->map.height()) && 
	(position.z < testVox->map.depth()) &&
	(position.x >= 0) && 
	(position.y >= 0) && 
	(position.z >= 0) 
	);
}

bool isFluidCube (glm::vec3& indicesVoxMap) {
	int type = testVox->map(indicesVoxMap.x, indicesVoxMap.y, indicesVoxMap.z, MapChannels::BLOC);
	//std::cout << "Prochain cube de type :" << type << std::endl; 
	return ((type == CubeTypes::AIR) || (type == CubeTypes::WATER));
}

// inutilisé
bool testCollisionSegment(glm::vec3 nextPosition) {
	double semiEpaisseur = 0.5;
	glm::vec4 cameraCollider (cam.getX() - semiEpaisseur, cam.getZ() - semiEpaisseur, 2 * semiEpaisseur, 2 * semiEpaisseur);

	double size = testVox->voxelSize[0];
	// On transforme les coordonée 3D en coordonnées images	
	glm::vec3 currentCamPosition = cam.getPosition();
	glm::vec3 currentIndices = getVoxMapIndicesByOpenGlPosition(currentCamPosition);
	glm::vec3 nextIndices = getVoxMapIndicesByOpenGlPosition(nextPosition);
	glm::vec3 directionGL = nextPosition - currentCamPosition;
	glm::vec3 destination = currentCamPosition + directionGL;

	//std::cout << "directionGL :" << directionGL.x << " " << directionGL.y << " " << directionGL.z << std::endl;
	std::vector<glm::vec3> edgeNeighborhood;
	if ((directionGL.x > 0) && (directionGL.z > 0)) {
		// AB - Edge

		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 1) * size));
		// BC - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 1) * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x + 1) * size, 0, (currentIndices.z + 1) * size));
		// BD - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 1) * size));
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 2) * size));
		// BE - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 1) * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));

/*
		// AB - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 1) * size));
		// AF - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x + 1) * size, 0, currentIndices.z * size));
		// AH - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		// AG - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z - 1)* size));

		// AH - Edge

		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		// HI - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z - 1) * size));
		// HJ - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 2) * size, 0, (currentIndices.z) * size));
		// HE - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));

		// BE - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 1) * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));
		// EL - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));	
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 2) * size));
		// Ek - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));	
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 2) * size, 0, (currentIndices.z + 1) * size));
		// HE - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));	
*/
	} /*else if ((directionGL.x > 0) && (directionGL.z < 0)) {
		// AB - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 1) * size));
		// AF - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x + 1) * size, 0, currentIndices.z * size));
		// AH - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		// AG - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z - 1)* size));
	} else if ((directionGL.x > 0) && (directionGL.z > 0)) {
		// AH - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		// HI - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z - 1) * size));
		// HJ - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 2) * size, 0, (currentIndices.z) * size));
		// HE - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));
	} else if ((directionGL.x < 0) && (directionGL.z > 0)) {
		// BE - Edge
		edgeNeighborhood.push_back(glm::vec3(currentIndices.x * size, 0, (currentIndices.z + 1) * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));
		// EL - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));	
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 2) * size));
		// Ek - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));	
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 2) * size, 0, (currentIndices.z + 1) * size));
		// HE - Edge
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, currentIndices.z * size));
		edgeNeighborhood.push_back(glm::vec3((currentIndices.x - 1) * size, 0, (currentIndices.z + 1) * size));	
	}*/
	int nbEdge = edgeNeighborhood.size();
	if (nbEdge == 0) {
		//std::cout << "aucune aretes" << std::endl;
		return true;
	} else {

		//std::cout << "on a des aretes" << std::endl;
		for (int k = 0; k < nbEdge; k += 2) {
			int nextX = currentIndices.x;
				
			int nextZ = currentIndices.z;
	
			switch (k) {
				case 0:
				nextX++;
				break;
				case 2:
				nextZ++;
				break;
			}			
				//std::cout << "Cube actuel :" << currentIndices.x << " " << currentIndices.y << " " <<  currentIndices.z << " " << std::endl;
				glm::vec3 cubeIntersected = glm::vec3(nextX, currentIndices.y, nextZ);

				std::cout << "Cube Intersecté :" << cubeIntersected.x << " " << cubeIntersected.y << " " <<  cubeIntersected.z << " " << std::endl;
			if (intersection2DEdge(currentCamPosition, destination, edgeNeighborhood[k], edgeNeighborhood[k + 1])) {
				//std::cout << "intersection----------------------------------------------------------------------------------------------" <<std::endl;
				if (!isFluidCube(cubeIntersected)) {
				//	std::cout << "Edge collision dectection !" << std::endl;
					return false;
				}
			}
		}
	}
	return true;
}

// true -> movement accepted
bool isOverTextureHeight(glm::vec3 nextPosition) {
	double size = testVox->voxelSize[0];
	// On transforme les coordonée 3D en coordonnées images	
	glm::vec3 currentCamPosition = cam.getPosition();
	glm::vec3 currentForwad = cam.getForward();

	//currentCamPosition += currentForwad;

	glm::vec3 currentIndices = getVoxMapIndicesByOpenGlPosition(currentCamPosition);
	glm::vec3 nextIndices = getVoxMapIndicesByOpenGlPosition(nextPosition);
	glm::vec3 directionGL = nextPosition - currentCamPosition;
	glm::vec3 destination = currentCamPosition + directionGL;
/*
	std::cout << "indice courant :" << currentIndices.x << " " << currentIndices.y << " " << currentIndices.z << std::endl;
	std::cout << "indice prochain :" << nextIndices.x << " " << nextIndices.y << " " << nextIndices.z << std::endl;
	std::cout << "position courante :" << currentCamPosition.x << " " << currentCamPosition.y << " " << currentCamPosition.z << std::endl;
	std::cout << "position prochaine :" << destination.x << " " << destination.y << " " << destination.z << std::endl;

*/	if (!(isInsideMap(nextIndices) && isFluidCube(nextIndices))) {
		//return testCollisionSegment(nextPosition);// && testCollisionSegmentExtended(nextPosition));
		return false;
	}
	nextPosition += glm::vec3(0.2,0.0,0.0);
	nextIndices = getVoxMapIndicesByOpenGlPosition(nextPosition);
	if (!(isInsideMap(nextIndices) && isFluidCube(nextIndices))) {
		//return testCollisionSegment(nextPosition);// && testCollisionSegmentExtended(nextPosition));
		return false;
	}
	nextPosition += glm::vec3(-0.6,0.0,0.0);
	nextIndices = getVoxMapIndicesByOpenGlPosition(nextPosition);
	if (!(isInsideMap(nextIndices) && isFluidCube(nextIndices))) {
		//return testCollisionSegment(nextPosition);// && testCollisionSegmentExtended(nextPosition));
		return false;
	}
	nextPosition += glm::vec3(0.4,0.0, 0.5);
	nextIndices = getVoxMapIndicesByOpenGlPosition(nextPosition);
	if (!(isInsideMap(nextIndices) && isFluidCube(nextIndices))) {
		//return testCollisionSegment(nextPosition);// && testCollisionSegmentExtended(nextPosition));
		return false;
	}
	nextPosition += glm::vec3(0.0,0.0,-1.5);
	nextIndices = getVoxMapIndicesByOpenGlPosition(nextPosition);
	if (!(isInsideMap(nextIndices) && isFluidCube(nextIndices))) {
		//return testCollisionSegment(nextPosition);// && testCollisionSegmentExtended(nextPosition));
		return false;
	} /*else {
		return false;
	}*/
	return true;
}

void forwardCam() {
    if (isOverTextureHeight(cam.forwardPosition())) {     
        cam.toForward();
    }
}

void rearwardCam() {
    if (isOverTextureHeight(cam.backwardPosition())) {
        cam.toBackward();
    }
}

void towardRightCam() {
    if (isOverTextureHeight(cam.rightPosition())) {
        cam.toRight();
    }
}

void towardLeftCam() {
    if (isOverTextureHeight(cam.leftPosition())) {
        cam.toLeft();
    }
}

void upCam() {
    if (isOverTextureHeight(cam.upPosition())) {
        cam.toUp();
    }
}

void downCam() {
    if (isOverTextureHeight(cam.downPosition())) {
        cam.toDown();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fpsinit() {
        // Set all frame times to 0ms.
        memset(frametimes, 0, sizeof(frametimes));
        framecount = 0;
        framespersecond = 0;
        frametimelast = SDL_GetTicks();
}

void fpsthink() {
        Uint32 frametimesindex;
        Uint32 getticks;
        Uint32 count;
        Uint32 i;

        // frametimesindex is the position in the array. It ranges from 0 to FRAME_VALUES.
        // This value rotates back to 0 after it hits FRAME_VALUES.
        frametimesindex = framecount % FRAME_VALUES;
        // store the current time
        getticks = SDL_GetTicks();
        // save the frame time value
        frametimes[frametimesindex] = getticks - frametimelast;
        // save the last frame time for the next fpsthink
        frametimelast = getticks;
        // increment the frame count
        framecount++;

        // Work out the current framerate
        // The code below could be moved into another function if you don't need the value every frame.
        // I've included a test to see if the whole array has been written to or not. This will stop
        // strange values on the first few (FRAME_VALUES) frames.
        if( framecount < FRAME_VALUES ){ count = framecount; }
        else{ count = FRAME_VALUES; }

        // add up all the values and divide to get the average frame time.
        framespersecond = 0;
        for( i = 0; i < count; i++ ){ framespersecond += frametimes[i]; }
        framespersecond /= count;
        // now to make it an actual frames per second value...
        framespersecond = 1000.f / framespersecond;
}


vector<string> ASSETS_PATHS;
vector<string> MODELS_NAMES;
vector<glMesh*> meshes;
glPipeline forwardPipeline;
glPipeline simpleShadowPipeline;
glPipeline instancedForwardPipeline;
glPipeline instancedDeferredGeoPassPipeline;
glPipeline deferredLightPassPipeline;
glPipeline deferred_SSAO_Pipeline;
glPipeline SSAOBlur_Pipeline;

glDeferredRenderer deferredRenderer(width,height);

/**
*	shadow map structs
*/
GLuint depthMapFBO = 0;
GLuint depthMap = 0;
GLuint lightMatUBO = 0;

static bool quitting = false;
static SDL_Window *window = NULL;
static SDL_GLContext gl_context;

// Il manque une classe scene
void updateMVP(){ for( glMesh* mesh : meshes ){ mesh->updateMVP(context->globalUBO.proj, context->globalUBO.view); } }
void listen_glDebugMessage();

void init(){
	GLint infoValue = 0;
	cout << "\e[1;33m---- Global infos ----\e[0m" << endl; 
	cout << glGetString(GL_RENDERER) << endl;
	cout << glGetString(GL_VENDOR) << endl;
	cout << glGetString(GL_VERSION) << endl;
	cout << "GLSL "<<glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	cout << "\e[1;31m---- Limitations ----\e[0m" << endl;
	cout << "\e[1;33m---- Uniform Buffer Objects ----\e[0m" << endl;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &infoValue);
	cout << "GL_MAX_UNIFORM_BUFFER_BINDINGS -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &infoValue);
	cout << "GL_MAX_UNIFORM_BLOCK_SIZE -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &infoValue);
	cout << "GL_MAX_VERTEX_UNIFORM_BLOCKS -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &infoValue);
	cout << "GL_MAX_FRAGMENT_UNIFORM_BLOCKS -- " << infoValue << endl;

	cout << "\e[1;33m---- Texture Units ----\e[0m" << endl;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &infoValue);
	cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS -- " << infoValue << endl;
	
	cout << "\e[1;33m---- Shader Storage Buffer Objects ----\e[0m" << endl;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &infoValue);
	cout << "GL_MAX_SHADER_STORAGE_BLOCK_SIZE -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &infoValue);
	cout << "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &infoValue);
	cout << "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS -- " << infoValue << endl;
	
	cout << "\e[1;33m---- Frame Buffer Objects ----\e[0m" << endl;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &infoValue);
	cout << "GL_MAX_COLOR_ATTACHMENTS -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &infoValue);
	cout << "GL_MAX_DRAW_BUFFERS -- " << infoValue << endl;
	
	// Map des voxels (wrapper Cimg)
	testVox = new VoxMap();
	std::string mapFile = "./testMap.hdr";
	std::string cubePath = "./assets/cubes/";
	std::string cubeName = "cube.obj";
	testVox->loadVoxel(cubePath,cubeName); // charge la forme du voxel
	testVox->newMap(128,128,256);
	testVox->testMap(); // remplissage test

	std::cout << "DONE" << std::endl;
	
	// Remplissage des Pipe-line (ensemble de shaders)
	string forward_vertex = "./shaders/forward.vert";
	string forward_fragment = "./shaders/forward.frag";
	string shadowPT_vertex = "./shaders/shadowTex.vert";
	string shadowPT_fragment = "./shaders/shadowTex.frag";
	string instancedForward_vertex = "./shaders/instancedForward.vert";
	string instancedForward_fragment = "./shaders/instancedForward.frag";

	string instancedDeferredGeoPass_vertex = "./shaders/instancedDeferredGeoPass.vert";
	string instancedDeferredGeoPass_fragment = "./shaders/instancedDeferredGeoPass.frag";

	string deferredLightPass_vertex = "./shaders/deferredLightPass.vert";
	string deferredLightPass_fragment = "./shaders/deferredLightPass.frag";

	string deferredSSAO_vertex = "./shaders/deferredSSAO.vert";
	string deferredSSAO_fragment = "./shaders/deferredSSAO.frag";

	string SSAOBlur_vertex = "./shaders/SSAOBlur.vert";
	string SSAOBlur_fragment = "./shaders/SSAOBlur.frag";

	std::cout << "\e[1;33mCompilation \e[1;36mforward rendering pipeline\e[0m" << std::endl;
	forwardPipeline.generateShaders(forward_vertex.c_str(), forward_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;
	std::cout << "\e[1;33mCompilation \e[1;36mshadow map pipeline\e[0m" << std::endl;
	simpleShadowPipeline.generateShaders(shadowPT_vertex.c_str(),shadowPT_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;
	std::cout << "\e[1;33mCompilation \e[1;36minstanced forward rendering pipeline\e[0m" << std::endl;
	instancedForwardPipeline.generateShaders(instancedForward_vertex.c_str(),instancedForward_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;
	std::cout << "\e[1;33mCompilation \e[1;36minstanced deferred rendering(Geometry pass) pipeline\e[0m" << std::endl;
	instancedDeferredGeoPassPipeline.generateShaders(instancedDeferredGeoPass_vertex.c_str(),
	                                                 instancedDeferredGeoPass_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;
	std::cout << "\e[1;33mCompilation \e[1;36mdeferred rendering(Light pass) pipeline\e[0m" << std::endl;
	deferredLightPassPipeline.generateShaders(deferredLightPass_vertex.c_str(), deferredLightPass_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;
	std::cout << "\e[1;33mCompilation \e[1;36mdeferred SSAO pipeline\e[0m" << std::endl;
	deferred_SSAO_Pipeline.generateShaders(deferredSSAO_vertex.c_str(), deferredSSAO_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;
	std::cout << "\e[1;33mCompilation \e[1;36mdeferred SSAO Blur pipeline\e[0m" << std::endl;
	SSAOBlur_Pipeline.generateShaders(SSAOBlur_vertex.c_str(), SSAOBlur_fragment.c_str(), NULL);
	std::cout << "\e[1;32mDONE\e[0m" << std::endl;
	 
	deferredRenderer.init(&instancedDeferredGeoPassPipeline, &deferredLightPassPipeline, &deferred_SSAO_Pipeline, &SSAOBlur_Pipeline);
	
	context = new glContext();

	
	// Gestion caméra
	context->camera.pos = glm::vec3(-20.0f, 30.0f,-20.0f);

	context->camera.backupPos = context->camera.pos;
	context->camera.target = glm::vec3(32.0f,32.0f,64.0f);
	context->globalUBO.update( context->camera );
	context->lights.pos[0] = glm::vec4( -0.5f, 1.0f, -0.5f, 0.0f );
	context->lights.pos[1] = glm::vec4( 50.0f, 100.0f, 15.5f, 1.0f );
	//context->lights.pos[2] = glm::vec4( 150.0f, 300.0f, 152.5f, 1.0f );
	context->lights.attenuation[2] = glm::vec4( 0.0f, 00.0f, 0.0f, 0.0f );
	//context->lights.diffuse[0] = glm::vec4( 0.6f, 0.8f, 0.4f, 1.0f );
	//context->lights.specular[0] = glm::vec4( 0.2f, 0.2f, 0.2f, 1.0f );
	context->globalUBO.proj = glm::perspective(glm::radians(80.0f),
	                                           width / (float)height,
	                                           0.001f, 500.0f);
	

	updateMVP();
	context->updateGlobalUniformBuffer();
	context->updateLightsUniformBuffer();



	//shadow map WIP
	//shadow map framebuffer
	
	glGenFramebuffers(1, &depthMapFBO);

	//shadow map texture
	const GLuint SHADOW_WIDTH = width, SHADOW_HEIGHT = width;
	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
	             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

	
	//binding texture to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//building light space infos
	GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(context->camera.pos), 
	                                  glm::vec3(context->camera.target), 
	                                  glm::vec3( 0.0f, 1.0f,  0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	//copying light space infos to uniform buffer object
	glGenBuffers(1, &lightMatUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightMatUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), &lightSpaceMatrix, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	
	glEnable(GL_DEPTH_TEST);
	glDepthRange(0,1);
	
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	//glEnable(GL_PROGRAM_POINT_SIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	initCamera(context);
}




void render(){
	SDL_GL_MakeCurrent(window, gl_context);


	//render to depth map
	//glCullFace(GL_FRONT);
	/*
	simpleShadowPipeline.bind();
	context->bindUBO();
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, lightMatUBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	for( glMesh* mesh : meshes ){ mesh->render(); }
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	//glCullFace(GL_BACK);
	*/
	//phong render
	//glClearColor(0.200f, 0.200f, 0.200f, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glActiveTexture(GL_TEXTURE0 + 6);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//instancedForwardPipeline.bind();
	deferredRenderer.bindGeometryPipeline();
	context->bindUBO();
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, lightMatUBO);
	testVox->render();

	deferredRenderer.ssaoPass();
	
	deferredRenderer.bindLightPipeline();
	deferredRenderer.basicLightPass();
	/*
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	phongPipeline.bind();
	context->bindUBO();
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, lightMatUBO);
	for( glMesh* mesh : meshes ){ mesh->render(); }
	*/
	
	glFlush();
	SDL_GL_SwapWindow(window);
}


bool deplacementOk = false;

int SDLCALL watch(void *userdata, SDL_Event* event) {
	if( event->type == SDL_APP_WILLENTERBACKGROUND ){ quitting = true; }
	return 1;
}

void useGravity(int fps) {;
	double gravity = 4.0; 
	glm::vec3 position = cam.getPosition();
	glm::vec3 indices = getVoxMapIndicesByOpenGlPosition(position);
	if (testVox->map(indices.x, indices.y - 1, indices.z, MapChannels::BLOC) == CubeTypes::AIR) {
		cam.toDown(gravity / double(fps+1));

		//cam.toDown(0.1);
		cam.use();

			
	}
	context->updateGlobalUniformBuffer();
	updateMVP();

}

int main(int argc, char *argv[]) {
	SDL_Event event;
	
	if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0 ){
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return 1;
	}
	fpsinit();
	window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED,
	                          width, height, SDL_WINDOW_OPENGL);

	gl_context = SDL_GL_CreateContext(window);
	SDL_AddEventWatch(watch, NULL);
	glewInit();
	listen_glDebugMessage();
	init();
	SDL_GL_SetSwapInterval(0); //disable vSync for 60 fps cap on desktop monitors

	// Code pour réguler les FPS
	//Keep track of the current frame
	int frame = 0;
	//Whether or not to cap the frame rate
	bool cap = true; 
	//The frame rate regulator
	Timer fps;

	while(!quitting) {
		fps.start();
		while( SDL_PollEvent(&event) ) {
			cam.update(event);
			
			if(event.type == SDL_KEYDOWN){
				switch (event.key.keysym.sym) {
				case SDLK_z :
					forwardCam();
					break;
					
				case SDLK_s :
					rearwardCam();
					break;
					
				case SDLK_d :
					towardLeftCam();
					break;
					
				case SDLK_q :
					towardRightCam();
	            break;
	            
				case SDLK_UP :
					upCam();
					break;
				
				case SDLK_SPACE :
					upCam();
					break;
					
				case SDLK_DOWN :
					downCam();
	            break;
				}
			}
			
			if(event.type == SDL_QUIT) { quitting = true; }
			
			if( event.type == SDL_MOUSEWHEEL ){
				if( event.wheel.y < 0 ){
					context->camera.pos.z += -0.5f; 
				}
				else{
					context->camera.pos.z += 0.5f;
				}

				//context->globalUBO.view = glm::lookAt(context->camera.pos, context->camera.target, context->camera.up);
				cam.use();
				context->updateGlobalUniformBuffer();
				updateMVP();
			}

			cam.use();
			context->updateGlobalUniformBuffer();
			updateMVP();
		}


		if (frame % FRAMES_PER_SECOND == 0) {
		
			render();

			fpsthink();
			windowName = "VoxEngine -- ";
			windowName += std::to_string((size_t)framespersecond);
			//windowName += " - ";
			//windowName += std::to_string(fps.get_ticks());
			windowName += " fps";
			SDL_SetWindowTitle(window, windowName.c_str());
		}
		if( frame % FRAMES_PER_SECOND == 1 )
		{
		//blit there
		}

		//Update the screen
		//if( SDL_Flip( screen ) == -1 ) { return 1; }
		//Increment the frame counter
		//frame++;
		
		//If we want to cap the frame rate
		if( ( cap == true ) && ( fps.get_ticks() < 1000 / FRAMES_PER_SECOND ) ) { 
		//Sleep the remaining frame time 
			SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
		}
		useGravity(framespersecond);
		//printf("%f\n", framespersecond);
	}

	SDL_DelEventWatch(watch, NULL);
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	
	exit(0);
	
}



/**
 * Function for openGL 4.5 debug callback
 * currently not used for openGL 4.3 compatibility
 */
void openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
	cout << "\e[1;32m" << "---------------------opengl-callback-start------------" << endl;
	cout << "message: "<< message << endl;
	cout << "type: ";
	switch( type ){
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << endl;
 
	cout << "id: " << id << endl;
	cout << "severity: ";
	switch (severity){
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	}
	cout << endl;
	cout << "---------------------opengl-callback-end--------------" << endl;
}

 void listen_glDebugMessage(){
	if( glDebugMessageCallback ){
        cout << "Register OpenGL debug callback " << endl;
        //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        //glDebugMessageCallback(openglCallbackFunction, nullptr);
        //GLuint unusedIds = 0;
        //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
    }
	else{ cout << "glDebugMessageCallback not available" << endl; }
}



