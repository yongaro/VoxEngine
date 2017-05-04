#ifndef CAMERA_H
#define CAMERA_H

#include "VoxMap.hpp"


class Camera {
    protected :
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 forward;
        glm::vec3 left;

        glm::mat4* context;
        int width;
        int height;

        int circleX;
        int circleY;

        GLfloat speed;
        GLfloat sensivity;

        GLfloat theta;
        GLfloat phi;

        bool booster;
        bool enabledMoves;
        float boost;
        int endurance; 


        void vectorsFromAngles();
        void setTheta(GLfloat);
        void setPhi(GLfloat);
     public :
        Camera();
        Camera(GLfloat, GLfloat, GLfloat);
        ~Camera();

        void see(GLfloat, GLfloat, GLfloat);

        // Gestion de la caméra
        void toForward();
        void toBackward();
        void toLeft();
        void toRight();
        void toUp();
        void toDown();
        void toDown(double offset);

        glm::vec3 forwardPosition () const;
        glm::vec3 backwardPosition () const;
        glm::vec3 leftPosition () const;
        glm::vec3 rightPosition () const;
        glm::vec3 upPosition () const;
        glm::vec3 downPosition () const;

        void bind(glm::mat4*);

        GLfloat getRealSpeed() const;

        glm::vec3 getPosition() const;
        glm::vec3 getForward() const;
		glm::vec3 getTarget() const;
        GLfloat getX() const;
        GLfloat getY() const;
        GLfloat getZ() const;

        void mouseMove(int, int);

        // Settings
        void setBooster(bool);
        void setHeight(int);
        void setWidth(int);
        void setSpeed(GLfloat);
        void setSensivity(GLfloat);
        void setBoost(GLfloat);

        void use();
        void update(SDL_Event&);
        void clear();
        //void update(SDL_Event&);
};

#endif // CAMERA_H
