#include "test.h"
#include <fstream>
#include "glm/glm.hpp"
using namespace glm;

std::vector<float> indexBuffer;
std::vector<float> dataBuffer;

struct DiffuseMtl {
    vec4 kd;
    vec4 emission;
};

struct vsRay {
    vec3 origin;
    vec3 dir;
    float tMax;
};

struct Intersection {
    bool hit;
    vec3 pos;
    vec3 normal;
    vec4 kd;
    vec4 emission;
};

vec2 GetNode(int index) {
    return vec2(indexBuffer[index * 2], indexBuffer[index * 2+1]);
}

vec4 GetData(int dataPos) {
    return vec4(dataBuffer[dataPos * 4], dataBuffer[dataPos * 4 + 1], dataBuffer[dataPos * 4 + 2], dataBuffer[dataPos * 4 + 3]);
}

Vertex GetVertex(int dataPos) {
    vec4 pos, normal;
    pos=GetData(dataPos);
    normal=GetData(dataPos + 1);

    return Vertex{ vec3(pos.x,pos.y,pos.z),vec3(normal.x,normal.y,normal.x),vec2(pos.w,normal.w) };
}

bool AABB_Test(vsRay ray, float dataPos) {
    vec4 minPoint, maxPoint;
    minPoint=GetData(dataPos);
    maxPoint=GetData(dataPos + 1);

    vec3 origin = ray.origin;
    vec3 dir = ray.dir;
    float local_tMin = 0.001;
    float tMax = ray.tMax;
    for (int i = 0; i < 3; i++) {
        float invD = 1.0f / dir[i];
        float t0 = (minPoint[i] - origin[i]) * invD;
        float t1 = (maxPoint[i] - origin[i]) * invD;
        if (invD < 0.0f) {
            float tmp = t0;
            t0 = t1;
            t1 = tmp;
        }

        local_tMin = max(t0, local_tMin);
        tMax = min(t1, tMax);
        if (tMax < local_tMin)
            return false;
    }
    return true;
}

DiffuseMtl GetMtl(float dataPos) {
    DiffuseMtl mtl;
    mtl.kd=GetData(dataPos + 6);
    mtl.emission=GetData(dataPos + 7);
    return mtl;
}

Vertex VertexInterpolate(vec3 abg, struct Vertex v0, struct Vertex v1, struct Vertex v2) {
    Vertex vertex;
    vertex.uv = abg[0] * v0.uv + abg[1] * v1.uv + abg[2] * v2.uv;
    vertex.pos = abg[0] * v0.pos + abg[1] * v1.pos + abg[2] * v2.pos;
    vertex.normal = abg[0] * v0.normal + abg[1] * v1.normal + abg[2] * v2.normal;
    return vertex;
}

vec4 Intersect_RayTri(vec3 e, vec3 d, vec3 a, vec3 b, vec3 c) {
    mat3 equation_A = mat3(vec3(a - b), vec3(a - c), d);

    if (abs(determinant(equation_A)) < 0.00001)
        return vec4(0);

    vec3 equation_b = a - e;
    vec3 equation_X = inverse(equation_A) * equation_b;
    float alpha = 1 - equation_X[0] - equation_X[1];
    return vec4(alpha, equation_X);
}


bool TriangleTest(vsRay& ray, float dataPos, struct Intersection& intersect) {
    struct Vertex v0, v1, v2;
    v0=GetVertex(dataPos);
    v1=GetVertex(dataPos + 2);
    v2=GetVertex(dataPos + 4);

    vec3 p0 = v0.pos;
    vec3 e0 = v1.pos - p0;
    vec3 e1 = v2.pos - p0;
    vec3 pv = cross(ray.dir, e1);
    float det = dot(e0, pv);
    vec3 tv = ray.origin - p0;
    vec3 qv = cross(tv, e0);

    vec4 uvt;
    uvt.x = dot(tv, pv);
    uvt.y = dot(ray.dir, qv);
    uvt.z = dot(e1, qv);
    for (int i = 0; i < 3; i++) {
        uvt[i] /= det;
    }
    uvt.w = 1.0 - uvt.x - uvt.y;
    if (all(greaterThanEqual(uvt, vec4(0.0)))) {
        intersect.hit = true;
        struct Vertex v;
        v=VertexInterpolate(vec3(uvt.w, uvt.x, uvt.y), v0, v1, v2);

        intersect.pos = v.pos;
        intersect.normal = v.normal;

        struct DiffuseMtl mtl;
        mtl=GetMtl(dataPos);
        intersect.kd = mtl.kd;
        intersect.emission = mtl.emission;

        ray.tMax = uvt.z;
        return true;
    }
    intersect.hit = false;
    return false;
}

void RayIn(vsRay ray, struct Intersection& intersect) {
    intersect.hit = false;
    int curNodeIndex = 0;
    vec2 node;
    while (curNodeIndex >= 0) {
        node=GetNode(curNodeIndex);

        if (node.x >= 0) {  //BVH_Node
            if (AABB_Test(ray, node.x)) {
                curNodeIndex++;
            }
            else {
                curNodeIndex = int(node.y);
            }
        }
        else {  //Triangle
            float dataPos = -node.x;
            TriangleTest(ray, dataPos, intersect);
            curNodeIndex++;
        }
    }
}



//vec3 RayTrace(struct vsRay curRay) {
//    Intersection curIntersect;
//    float pdf = 0.5 / 3.1415926;
//
//    vec3 traceInfo;
//    do {
//        RayIn(curRay, curIntersect);
//
//        if (curIntersect.hit) {
//            return;
//            Ray nextRay;
//            SampleLight(curIntersect.pos, curIntersect.normal, nextRay);
//
//            if (curIntersect.emission != vec3(0.0)) {
//                traceInfo = curIntersect.emission;
//                Stack[++stackTop] = traceInfo;
//                break;
//            }
//
//            traceInfo = curIntersect.kd / (PI * pdf * dot(nextRay.dir, curIntersect.normal));
//            Stack[++stackTop] = traceInfo;
//            curRay = nextRay;
//        }
//    } while (rand() < 0.8);
//
//    if (stackTop < 0) {
//        return;
//    }
//
//
//    res = Stack[stackTop--];
//    for (int i = stackTop - 1; i >= 0; i--) {
//        res *= Stack[i];
//    }
//}


void TestRayTracing(Scence* scen)
{
    scen->bvhRoot->PackData(indexBuffer, dataBuffer);
    glm::vec3 eyePos(278.0, 273.0, -800.0);
    
    
    std::vector<vec3> framebuffer;
    Intersection intersect;
    /*for (int i =0; i < 784; i++) {
        for (int j = 0; j < 784; j++) {
            float scale = tan(radians(40.0 * 0.5));
            float x = (2 * (j + 0.5) / (float)784 - 1) * scale;
            float y = (1 - 2 * (i + 0.5) / (float)784) * scale;
    
            vec3 dir = normalize(vec3(-x, y, 1));
            vsRay cameraRay{ eyePos,dir,FLT_MAX };
            intersect.hit = false;
            HitRecord hitRecord=scen->bvhRoot->RayIn(Ray(eyePos, dir));
            if (hitRecord.hit) {
                std::cout << i << ":" << j << "   " << hitRecord.hit << std::endl;
            }
            framebuffer.push_back(vec3(hitRecord.hitPoint.kd[0], intersect.kd[1], intersect.kd[2]));
            
            RayIn(cameraRay, intersect); 
            if (intersect.hit) {
                std::cout << i << ":" << j << "   " << intersect.hit << std::endl;
                std::cout << "    " << intersect.pos[0] << "," << intersect.pos[1] << "," << intersect.pos[2] << std::endl;
                std::cout << "    " << intersect.kd[0] << "," << intersect.kd[1] << "," << intersect.kd[2] << std::endl;
            }
            framebuffer.push_back(vec3(intersect.kd[0], intersect.kd[1], intersect.kd[2]));
        }
    }*/


    //std::ofstream f("test.ppm", std::ios::out);
    //// save framebuffer to file
    //f << "P6\n784 784\n255\n";
    //for (auto i = 0; i < 784*784; ++i) {
    //    int color[3];
    //    color[0] = (unsigned char)(255 * framebuffer[i].x);
    //    color[1] = (unsigned char)(255 * framebuffer[i].y);
    //    color[2] = (unsigned char)(255 * framebuffer[i].z);
    //    f << color[0] << " " << color[1] << " " << color[2] << '\n';
    //}
    //f.close();
}
