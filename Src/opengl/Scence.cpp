#include "Scence.h"

Scence* Scence::CreateCornellbox()
{
    Scence* cornellboxScence = new Scence();
    auto& models = cornellboxScence->models;
    auto& vertexBuffer = cornellboxScence->vertexBuffer;

    glm::vec3 white(0.725f, 0.71f, 0.68f);
    glm::vec3 red(0.63f, 0.065f, 0.05f);
    glm::vec3 green(0.14f, 0.45f, 0.091f);
    glm::vec3 light_ = 8.0f * glm::vec3(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * glm::vec3(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * glm::vec3(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);

    Model floor(vertexBuffer, "Assets/cornellbox/floor.obj");
    floor.SetColor(white);
    models.push_back(floor);

    Model shortbox(vertexBuffer, "Assets/cornellbox/shortbox.obj");
    shortbox.SetColor(white);
    models.push_back(shortbox);

    Model tallbox(vertexBuffer, "Assets/cornellbox/tallbox.obj");
    tallbox.SetColor(white);
    models.push_back(tallbox);

    Model left(vertexBuffer, "Assets/cornellbox/left.obj");
    left.SetColor(red);
    models.push_back(left);

    Model right(vertexBuffer, "Assets/cornellbox/right.obj");
    right.SetColor(green);
    models.push_back(right);

    Model light(vertexBuffer, "Assets/cornellbox/light.obj");
    light.SetColor(glm::vec3(0.65));
    light.SetEmission(light_);
    models.push_back(light);


    std::vector<Hitable*> hitables;
    for (int i = 0; i < models.size(); i++) {
        for (auto& tri : models[i].fBuffer) {
            hitables.push_back(&tri);
        }
    }
    cornellboxScence->bvhRoot = new BVH_Node(hitables);
    return cornellboxScence;
}
