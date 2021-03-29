#include "Billboard.h"
#include "nicoFunctions.h"

nico::Billboard::Billboard(Texture* texture2d, Camera* cam) : VAO(), VBO()
{
    image = texture2d;
    this->cam = cam;

    VAO.bind();
    glGenBuffers(1, VBO.getIdPointer());
    VAO.unbind();
}

void nico::Billboard::setLocations(std::vector<float>* vertices)
{
    VAO.bind();
    VBO.bind();
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    VAO.unbind();
}

void nico::Billboard::draw(Shader* shader)
{
    shader->use();
    shader->set("VP", cam->getProjection() * cam->getView());
    shader->set("up", glm::vec3(cam->getView()[0][1], cam->getView()[1][1], cam->getView()[2][1]));
    shader->set("viewPos", cam->getPosition());

    this->image->bind();
    
    VAO.bind();
    glDrawArrays(GL_POINTS, 0, locs.size() / 3);

    VAO.unbind();
}

void nico::Billboard::assignTexture(Texture* texture2d)
{
    this->image = texture2d;
}
