#include "glGeometry.hpp"

using namespace std;

vector<string> ASSETS_PATHS;
vector<string> MODELS_NAMES;
vector<glMesh*> meshes;
glPipeline phongPipeline;
glContext* context;


static bool quitting = false;
static SDL_Window *window = NULL;
static SDL_GLContext gl_context;

//Il manque une classe scene
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
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &infoValue);
	cout << "GL_MAX_UNIFORM_BUFFER_BINDINGS -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &infoValue);
	cout << "GL_MAX_UNIFORM_BLOCK_SIZE -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &infoValue);
	cout << "GL_MAX_VERTEX_UNIFORM_BLOCKS -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &infoValue);
	cout << "GL_MAX_FRAGMENT_UNIFORM_BLOCKS -- " << infoValue << endl;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &infoValue);
	cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS -- " << infoValue << endl;

	
	//string vertex = "./shaders/phongVert.vert";
	//string fragment = "./shaders/phongFrag.frag";
	//string vertex = "./shaders/basicShader2.vs";
	//string fragment = "./shaders/basicShader2.fs";
	//phongPipeline = glPipeline(vertex,fragment);
	phongPipeline.generateShaders();

	context = new glContext();
	
	
	ASSETS_PATHS.push_back("./assets/wall/");
	MODELS_NAMES.push_back("Wall.obj");
	
	//ASSETS_PATHS.push_back("./assets/WII_U/classic_sonic/");
	//MODELS_NAMES.push_back("classic_sonic.dae");

	ASSETS_PATHS.push_back("./assets/Aku Aku/");
	MODELS_NAMES.push_back("aku_aku.obj");
	
	//ASSETS_PATHS.push_back("./assets/gems/");
	//MODELS_NAMES.push_back("crystal.obj");
	
	//ASSETS_PATHS.push_back("./assets/Chaos Emeralds/");
	//MODELS_NAMES.push_back("Green Chaos Emerald.obj");	

	for( uint32_t i = 0; i < ASSETS_PATHS.size(); ++i ){
		meshes.push_back( new glMesh() );
		meshes.back()->loadMesh( ASSETS_PATHS.at(i), MODELS_NAMES.at(i) );
	}

	context->camera.pos = meshes[0]->getCamPos();
	context->camera.pos.z += 3.0f;
	//context->camera.pos.x -= 4.0f;
	context->camera.backupPos = context->camera.pos;
	context->camera.target = glm::vec3(0.0f, context->camera.pos.y - 0.5f, 0.0f);
	context->globalUBO.update(context->camera);
	context->lights.pos[0] = glm::vec4(context->camera.pos, 0.0f);
	

	
	
	if( meshes.size() > 1 ){
		for(size_t i = 1; i < MODELS_NAMES.size(); ++i){
			meshes.at(i)->matrices.model = glm::translate(glm::mat4(1.0f),glm::vec3(context->globalUBO.camPos.x * i, 0.0f, 0.0f));
		}
	}
	meshes.at(0)->matrices.model = glm::scale( meshes.at(0)->matrices.model, glm::vec3(1.5f));
	meshes.at(1)->matrices.model = glm::scale( meshes.at(1)->matrices.model, glm::vec3(0.5f));
	//meshes.at(2)->matrices.model = glm::scale( meshes.at(2)->matrices.model, glm::vec3(0.3f));
	//meshes.at(3)->matrices.model = glm::scale( meshes.at(3)->matrices.model, glm::vec3(0.01f));
	//meshes.at(3)->matrices.model = glm::translate( meshes.at(3)->matrices.model, glm::vec3(0.0f, context->globalUBO.camPos.y / 0.02f, 0.0f));
	

	updateMVP();
	context->updateGlobalUniformBuffer();
	context->updateLightsUniformBuffer();

	glEnable(GL_DEPTH_TEST);
	glDepthRange(0,1);
	
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_PROGRAM_POINT_SIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
}




void render(){
	SDL_GL_MakeCurrent(window, gl_context);

	glClearColor(0.05,0.05,0.2,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	phongPipeline.bind();
	context->bindUBO();
	for( glMesh* mesh : meshes ){ mesh->render(); }
	
	
	glFlush();
	SDL_GL_SwapWindow(window);
}


int SDLCALL watch(void *userdata, SDL_Event* event) {
	if( event->type == SDL_APP_WILLENTERBACKGROUND ){ quitting = true; }
	return 1;
}

int main(int argc, char *argv[]) {
	SDL_Event event;
	bool movedPitch = false;
	bool movedYaw = false;

	
	if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0 ){
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Vulkan to OpenGL Drama", SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED,
	                          width, height, SDL_WINDOW_OPENGL);

	gl_context = SDL_GL_CreateContext(window);
	SDL_AddEventWatch(watch, NULL);
	glewInit();
	listen_glDebugMessage();
	init();
	while(!quitting) {
		while( SDL_PollEvent(&event) ) {
			if(event.type == SDL_QUIT) { quitting = true; }
			if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_w){
					context->camera.rotatePitch = glm::rotate(glm::mat4(1.0f), 0.25f, context->camera.right );     movedPitch = true;
				}
				if(event.key.keysym.sym == SDLK_a){
					context->camera.rotateYaw = glm::rotate(glm::mat4(1.0f), 0.25f, context->camera.up );          movedYaw = true;
				}
				if(event.key.keysym.sym == SDLK_s){
					context->camera.rotatePitch = glm::rotate(glm::mat4(1.0f), -0.25f, context->camera.right );    movedPitch = true;
				}
				if(event.key.keysym.sym == SDLK_d){
					context->camera.rotateYaw = glm::rotate(glm::mat4(1.0f), -0.25f, context->camera.up );         movedYaw = true;
				}
				if(event.key.keysym.sym == SDLK_r){
					context->camera.rotatePitch = glm::mat4(1.0f);
					context->camera.rotateYaw = glm::mat4(1.0f);
					context->camera.pos = glm::vec3(context->camera.backupPos);
					context->globalUBO.camPos = glm::vec3(context->camera.pos);
					context->globalUBO.view = glm::lookAt(context->camera.pos, context->camera.target, context->camera.up);
				}
				updateMVP();
			}
			if(event.type == SDL_MOUSEWHEEL){
				if (event.wheel.y < 0){
					
				}
				else{
				
				}
			}
		
		}

		render();
		SDL_Delay(2);
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
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
    }
	else{ cout << "glDebugMessageCallback not available" << endl; }
}
