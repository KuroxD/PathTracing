#include "Model.h"
#include "../Basic/BVH_Node.h"

class Scence {
public:
    Scence():bvhRoot(nullptr),vertexBuffer(new std::vector<Vertex>()){}
    Scence(Scence&& sce) noexcept{
        vertexBuffer = sce.vertexBuffer;
        bvhRoot = sce.bvhRoot;
        models = sce.models;

        sce.vertexBuffer = nullptr;
        sce.bvhRoot = nullptr;
    }
    ~Scence() { 
        if (vertexBuffer != nullptr) {
            delete vertexBuffer;
        }

        if (bvhRoot != nullptr) {
            delete bvhRoot;
        }
     }

    BVH_Node* bvhRoot;
    std::vector<Vertex>* vertexBuffer;
    std::vector<Model> models;

    static Scence* CreateCornellbox();
};

