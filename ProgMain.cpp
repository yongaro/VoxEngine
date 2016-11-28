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
const int FRAMES_PER_SECOND = 6000;
double step = 1.0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Camera cam;


void initCamera(glContext* context) {
	cam = Camera(0.0f, 50.0f, 10.0f);
    cam.bind(&context->globalUBO.view);


    cam.see(1.0f, 2.0f, 1.0f);
    cam.setSpeed(step);
    cam.setBoost(10.0f);
    cam.setSensivity(0.3f);
}

bool isOverTextureHeight(GLfloat x, GLfloat y, GLfloat z) {
	/*
   // On transforme les coordonée 3D en coordonnées images
    unsigned int xp = (x / m_step);
    unsigned int yp = (z / m_step);

    // Lecture du pixel
    if((xp < m_img_w) && (yp < m_img_h)) {
        //cout << y  << " : " << groundHeight[xp][yp];
        return (y > groundHeight[xp][yp]);
    } else {
        return true;
    }
    */
    return true;
}

void forwardCam() {
    if (isOverTextureHeight(cam.getX(), cam.getY(), cam.getZ() + step)) {        
        cam.toForward();
    }
}

void rearwardCam() {
    if (isOverTextureHeight(cam.getX(), cam.getY(), cam.getZ() - step)) {
        cam.toBackward();
    }
}

void towardRightCam() {
    if (isOverTextureHeight(cam.getX() + step, cam.getY(), cam.getZ())) {
        cam.toRight();
    }
}

void towardLeftCam() {
    if (isOverTextureHeight(cam.getX() - step, cam.getY(), cam.getZ())) {
        cam.toLeft();
    }
}

void upCam() {
    if (isOverTextureHeight(cam.getX(), cam.getY() + step, cam.getZ())) {
        cam.toUp();
    }
}

void downCam() {
    if (isOverTextureHeight(cam.getX(), cam.getY() - step, cam.getZ())) {
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
glPipeline phongPipeline;
glPipeline simpleShadowPipeline;
glPipeline instancedPhongPipeline;

/**
*	contexte qui contient les constantes globales (caméras, lumières, UBO)
*/
glContext* context;
VoxMap* testVox;

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
	//testVox->save(mapFile);
	//testVox->fillVisibleCubes(5,5,5);

	std::cout << "DONE" << std::endl;
	
	// Remplissage des Pipe-line (ensemble de shaders)
	string phong_vertex = "./shaders/phongVert.vert";
	string phong_fragment = "./shaders/phongFrag.frag";
	string shadowPT_vertex = "./shaders/shadowTex.vert";
	string shadowPT_fragment = "./shaders/shadowTex.frag";
	string instancedPhong_vertex = "./shaders/instancedPhong.vert";
	string instancedPhong_fragment = "./shaders/instancedPhong.frag";
	phongPipeline.generateShaders(phong_vertex.c_str(), phong_fragment.c_str(), NULL);
	simpleShadowPipeline.generateShaders(shadowPT_vertex.c_str(),shadowPT_fragment.c_str(), NULL);
	instancedPhongPipeline.generateShaders(instancedPhong_vertex.c_str(),instancedPhong_fragment.c_str(), NULL);
	context = new glContext();

	
	// Chargement des assets
	//ASSETS_PATHS.push_back("./assets/wall/");
	//MODELS_NAMES.push_back("Wall.obj");
	
	//ASSETS_PATHS.push_back("./assets/WII_U/classic_sonic/");
	//MODELS_NAMES.push_back("classic_sonic.dae");
	
	ASSETS_PATHS.push_back("./assets/arbre/");
	MODELS_NAMES.push_back("wintertree.ply");
	
	for( uint32_t i = 0; i < ASSETS_PATHS.size(); ++i ){
		meshes.push_back( new glMesh() );
		meshes.back()->loadMesh( ASSETS_PATHS.at(i), MODELS_NAMES.at(i) );
	}

	
	// Gestion caméra
	context->camera.pos = glm::vec3(-20.0f, 30.0f,-20.0f);//meshes[0]->getCamPos();

	context->camera.backupPos = context->camera.pos;
	context->camera.target = glm::vec3(32.0f,32.0f,64.0f);
	context->globalUBO.update( context->camera );
	context->lights.pos[0] = glm::vec4( context->camera.pos, 0.0f );
	context->globalUBO.proj = glm::perspective(glm::radians(50.0f),
	                                           width / (float)height,
	                                           0.1f, 500.0f);
	
	
	/*
	if( meshes.size() > 1 ){
		for(size_t i = 1; i < MODELS_NAMES.size(); ++i){
			meshes.at(i)->matrices.model = glm::translate(glm::mat4(1.0f),glm::vec3(context->globalUBO.camPos.x * i, 0.0f, 0.0f));
		}
	}
	*/

	// mise à l'échelle

	
	meshes.at(0)->matrices.model = glm::scale( meshes.at(0)->matrices.model, glm::vec3(10.0f));
	
	meshes.at(0)->matrices.model = glm::rotate(meshes.at(0)->matrices.model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
	meshes.at(0)->matrices.model = glm::translate(meshes.at(0)->matrices.model, glm::vec3(3.0f, -4.0f, 2.0f));

	int k = 1;
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			meshes.push_back( new glMesh() );
			meshes.back()->loadMesh( ASSETS_PATHS.at(0), MODELS_NAMES.at(0) );

			meshes.at(k)->matrices.model = glm::scale( meshes.at(k)->matrices.model, glm::vec3(10.0f));
			meshes.at(k)->matrices.model = glm::rotate(meshes.at(k)->matrices.model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			meshes.at(k)->matrices.model = glm::translate(meshes.at(k)->matrices.model, glm::vec3(6.0f + i * 2, -4.0f - j * 2, 2.5f));
			++k;
		}	
	}

	//meshes.at(1)->matrices.model = glm::scale( meshes.at(1)->matrices.model, glm::vec3(1.5f));
	
	

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
	//

	
	glEnable(GL_DEPTH_TEST);
	glDepthRange(0,1);
	
	//glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_PROGRAM_POINT_SIZE);

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
	//glClearColor(0.529f, 0.808f, 0.922f, 0.0);
	glClearColor(0.200f, 0.200f, 0.200f, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glActiveTexture(GL_TEXTURE0 + 6);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	instancedPhongPipeline.bind();
	context->bindUBO();
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, lightMatUBO);
	testVox->render();

	/*
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	phongPipeline.bind();
	context->bindUBO();
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, lightMatUBO);
	for( glMesh* mesh : meshes ){ mesh->render(); }
	*/
	
	
	//context->bindUBO();
	//glBindBufferBase(GL_UNIFORM_BUFFER, 5, lightMatUBO);
	
	glFlush();
	SDL_GL_SwapWindow(window);
}


bool deplacementOk = false;

int SDLCALL watch(void *userdata, SDL_Event* event) {
	if( event->type == SDL_APP_WILLENTERBACKGROUND ){ quitting = true; }
	return 1;
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

		//printf("%f\n", framespersecond);
	}

	SDL_DelEventWatch(watch, NULL);
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	
	exit(0);
	
}

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



