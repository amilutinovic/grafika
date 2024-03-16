//
// Created by matf-rg on 15.11.20..
//

#ifndef PROJECT_BASE_CAMERA_H
#define PROJECT_BASE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
public:
    float Zoom = 45.f;
    float MovementSpeed = 2.5f;
    float Yaw = -90.0f;
    float Pitch = 0.0f;
    float MouseSensitivity = 0.1f;

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 WorldUp;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 Front = glm::vec3(0, 0, -1);

    Camera() {
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Direction direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
       switch (direction) {
           case FORWARD: {
              Position += Front * velocity;
           }break;
           case BACKWARD: {
                Position -= Front * velocity;
           }break;
           case LEFT: {
                Position -= Right * velocity;
           }break;
           case RIGHT: {
                Position += Right * velocity;
           }break;
       }
    }

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;
        if (constrainPitch) {
            if (Pitch > 89.0f) {
                Pitch = 89.0f;
            }
            if (Pitch < -89.0f) {
                Pitch = -89.0f;
            }
        }

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset) {
        Zoom -= yoffset;
        if (Zoom < 1.0f) {
            Zoom = 1.0f;
        }
        if (Zoom > 45.0f) {
            Zoom = 45.0f;
        }
    }
};

#endif //PROJECT_BASE_CAMERA_H
