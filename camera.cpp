#include "camera.h"
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

using namespace std;

Camera::Camera() :
    speed(.05f),
    theta(.0f),
    phi(.0f),
    circleX(0),
    circleY (0)
{}

Camera::Camera(GLfloat x, GLfloat y, GLfloat z) {
    position = glm::vec3(x, y, z);
    theta = 0.0f;
    phi = 0.0f;
    booster = false;
    enabledMoves = false;
    this->endurance = 300;
    circleX = 0;
    circleY = 0;
}


Camera::~Camera() {}

void Camera::clear() {}

void Camera::bind(glm::mat4* context) {
	this->context = context;
}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::vec3 Camera::getForward() const {
    return forward;
}

void Camera::vectorsFromAngles() {
    static const glm::vec3 up(0,1,0);

    // On limite phi : pas de loopings
    if (phi > 89)
        phi = 89;
    else if (phi < -89)
        phi = -89;

    // coordonnées sphériques -> cartésiennes
    double r_temp = cos(phi  * M_PI/180);
    forward.y = -0.5f + sin(phi * M_PI/180) / 2;
    forward.x = r_temp * cos(theta * M_PI/180);
    forward.z = r_temp * sin(theta * M_PI/180);
    left = glm::cross(up, forward);
    glm::normalize(left);

    // on calcule ce que regarde la caméra (la cible)
    target = position + forward;
}

void Camera::toForward() {
    double v = getRealSpeed();
    position.x += forward.x * v;
    position.z += forward.z * v;
}

glm::vec3 Camera::forwardPosition() const {
    double v = getRealSpeed();
    return glm::vec3(position.x +  forward.x * v, position.y, position.z +  forward.z * v);
}

void Camera::toBackward() {
    double v = getRealSpeed();
    position.x -= forward.x * v;
    position.z -= forward.z * v;
}

glm::vec3 Camera::backwardPosition() const {
    double v = getRealSpeed();
    return glm::vec3(position.x - forward.x * v, position.y, position.z - forward.z * v);
}

void Camera::toLeft() {
    double v = getRealSpeed();
    position.x -= left.x * v;
    position.z -= left.z * v;
}


glm::vec3 Camera::leftPosition() const {
    double v = getRealSpeed();
    return glm::vec3(position.x - left.x * v, position.y, position.z - left.z * v);
}

GLfloat Camera::getRealSpeed() const {
    return(booster ? boost * speed : speed);
}

void Camera::toRight() {
    double v = getRealSpeed();
    position.x += left.x * v;
    position.z += left.z * v;
}

glm::vec3 Camera::rightPosition() const {
    double v = getRealSpeed();
    return glm::vec3(position.x + left.x * v, position.y, position.z + left.z * v);
}

void Camera::toUp() {
    //position.y += getRealSpeed();
    position.y += 3.0f;
}


glm::vec3 Camera::upPosition() const {
    //double v = getRealSpeed();
    double v = 2.0f;
    return glm::vec3(position.x, position.y + v, position.z);
}

void Camera::toDown() {
    //position.y -= getRealSpeed();

    position.y -= 2.0f;
}

void Camera::toDown(double offset) {
    //position.y -= getRealSpeed();

    position.y -= offset;
}

glm::vec3 Camera::downPosition() const {
    //double v = getRealSpeed();
    double v = 2.0f;
    return glm::vec3(position.x, position.y - v, position.z);
}

// FIXIT : transformer en coordonée sphérique et stocker dans phi et theta
void Camera::see(GLfloat x, GLfloat y, GLfloat z) {
    forward = glm::vec3(x, y, z) - position;
}

GLfloat Camera::getX() const {
    return position.x;
}

GLfloat Camera::getY() const {
    return position.y;
}

GLfloat Camera::getZ() const {
    return position.z;
}

void Camera::setBooster(bool enable) {
    booster = enable;
}

void Camera::setTheta(GLfloat theta) {
    this->theta = theta;
}

void Camera::setPhi(GLfloat phi) {
    this->phi = phi;
}

void Camera::setWidth(int w) {
    this->width = w;
}

void Camera::setHeight(int h) {
    this->height = h;
}

void Camera::setSpeed(GLfloat speed) {
    this->speed = speed;
}

void Camera::setBoost(GLfloat boost) {
    this->boost = boost;
}

void Camera::setSensivity(GLfloat sensivity) {
    this->sensivity = sensivity;
}

void Camera::mouseMove(int x, int y) {
    if ((x != 0) && (y != 0)) {
        setTheta(x * sensivity);
        setPhi(y * sensivity);   
    }
}

// Gère les évenements SDL
void Camera::update(SDL_Event& event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		    case SDLK_LSHIFT:
		        if (this->endurance >= 100) {
		    		setBooster(true);	
		        }
		    break;
		}
	} else if (event.type == SDL_KEYUP) {
		switch (event.key.keysym.sym) {
		    case SDLK_LSHIFT :
		    	setBooster(false);
		    break;
		}
	} else if (event.type == SDL_MOUSEBUTTONUP) {
		enabledMoves = false;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		enabledMoves = true;
	} else if (event.type == SDL_MOUSEMOTION) {
        //cout << "A " << event.button.x << " " << event.button.y << endl;
        //cout << "B " << event.motion.xrel << " " << event.motion.yrel << endl;
   
        // Si on est à 0, on prend les valeurs courantes
       
        circleX += event.motion.xrel;
        circleY += event.motion.yrel;

        //cout << circleX << " " << circleY << endl;
        mouseMove(circleX,  circleY);
        //mouseMove(event.motion.xrel,  event.motion.yrel);
        //mouseMove(event.button.x,  event.button.y);
        
    
		if (enabledMoves) {
			//mouseMove(event.button.x, event.button.y);
		}
	} else {
        cout << "il se passe rien" << endl;
    }

}


void Camera::use() {
   if (booster) {
		if (this->endurance <= 5) {
			setBooster(false);
		} else {
			this->endurance -= 5;	
		} 
	} else {
		if (this->endurance < 300) {
			this->endurance += 1;	
	    }
	}
    // Recalcul des coordonées position - vue
    vectorsFromAngles();
    target = position + forward;

    *context = glm::lookAt(
        glm::vec3(position.x, position.y, position.z),
        glm::vec3(target.x, target.y, target.z),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}