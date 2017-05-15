#include <camera.hpp>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <math.h>

using namespace std;

Camera::Camera() : position(), target(), forward(), left(),
context(NULL), width(), height(),
circleX(0), circleY (0),
speed(.05f), sensivity(),
theta(.0f), phi(.0f),
booster(), enabledMoves(), boost(), endurance(){}

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
glm::vec3 Camera::getTarget() const{
	return target;
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
    forward.y = -0.5f + -1.8 * sin(phi * M_PI/180);
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


    /*
    double nextX = forward.x * v;
    if ((nextX >= 0.01) && (nextX <= 0.5)) { nextX = 0.5; }
    double nextZ = forward.z * v;
    if ((nextZ >= 0.01) && (nextZ <= 0.5)) { nextZ = 0.5; }
    position.x += nextX;
    position.z += nextZ;
    */
/*
        int xInt = position.x;
    if (position.x - xInt < 0.1) {
        position.x = xInt + 0.1;
    } else if (position.x - xInt > 0.9) {
        position.x = xInt + 0.9;
    }

    int yInt = position.z;
    if (position.z - yInt < 0.1) {
        position.z = yInt + 0.1;
    } else if (position.z - yInt > 0.9) {
        position.z = yInt + 0.9;
    }
    */
}

glm::vec3 Camera::forwardPosition() const {
    double v = getRealSpeed();
    glm::vec3 nextPos = glm::vec3(position.x +  forward.x * v, position.y, position.z +  forward.z * v);
  /*
    int xInt = nextPos.x;
    if (nextPos.x - xInt < 0.1) {
        nextPos.x = xInt + 0.1;
    } else if (nextPos.x - xInt > 0.9) {
        nextPos.x = xInt + 0.9;
    }

    int yInt = nextPos.z;
    if (nextPos.z - yInt < 0.1) {
        nextPos.z = yInt + 0.1;
    } else if (nextPos.z - yInt > 0.9) {
        nextPos.z = yInt + 0.9;
    }

*/
    return nextPos;
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
    //if ((x != 0) && (y != 0)) {
        setTheta(x * sensivity);
        setPhi(y * sensivity);
    //}
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
       // std::cout << circleX << " " << circleY << std::endl;
        circleX += event.motion.xrel;
        circleY += event.motion.yrel;

        if (circleY > height) circleY = height;
        if (circleY < -height) circleY = -height;
        //if (circleX > width) circleX = width;
        //if (circleX < -width) circleX = -width;

        mouseMove(circleX,  circleY);
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
    /*
    int xInt = position.x;
    if (position.x - xInt < 0.05) {
        position.x = xInt + 0.15;
    } else if (position.x - xInt > 0.95) {
        position.x = xInt + 1.2;
    }

    int yInt = position.z;
    if (position.z - yInt < 0.05) {
        position.z = yInt + 0.15;
    } else if (position.z - yInt > 0.95) {
        position.z = yInt + 1.2;
    }
    */
    *context = glm::lookAt(
        glm::vec3(position.x, position.y, position.z),
        glm::vec3(target.x, target.y, target.z),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}
