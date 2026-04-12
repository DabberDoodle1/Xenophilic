#include "core/handles/entity.hpp"
#include <glad/glad.h>

Entity::Entity(): pos_x(0.0), pos_y(0.0), vel_x(0.0), vel_y(0.0), VAO(0) {}

void Entity::initialize()
{
    float vertices[] = {
        0.5f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f,
        -0.5, 0.5f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    unsigned int BO[2];

    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, BO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, BO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO[1]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDeleteBuffers(2, BO);
}

Entity::~Entity()
{
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

void Entity::bind() const
{
    glBindVertexArray(VAO);
}
