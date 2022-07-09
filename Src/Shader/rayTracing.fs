#version 330 core

layout(location = 0) out vec4 FragColor;

uniform int frameCount;
uniform sampler2D lastFrameTex;
//index:vec2(type="BVH_Node":dataPos,miss),vec2(type="Triangle":-dataPos,miss)
//	DATA: 
//		BVH_Node:(vec4(minPoint,0),vec4(maxPoint,0))
//		Triangle:(vec4(pos,u),vec4(normal,v)),vec4(kd,0),vec4(e,0)
uniform sampler2D indexTex;
uniform sampler2D dataTex;

const float width=784.0;
const float height=784.0;

const float PI=3.1415926;
const float tMin=0;
const float FLT_MAX = 99999999999999999999999999999999999999.0;
//vec4(kd,brdf)
vec4 Stack[50];


struct Vertex{
    vec3 pos;
    vec3 normal;
    vec2 uv;
};

struct DiffuseMtl{
    vec3 kd;
    vec3 emission;
};

struct Ray{
    vec3 origin;
    vec3 dir;
    float tMax;
};

struct Intersection{
    bool hit;
    vec3 pos;
    vec3 normal;
    vec3 kd;
    vec3 emission;
};

//sample
uint s = uint(
    uint(gl_FragCoord.x)  * uint(1973) + 
    uint(gl_FragCoord.y) * uint(9277) + 
    uint(frameCount) * uint(26699)) | uint(1);

uint wang_hash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}
 
float rand() {
    return fract(float(wang_hash(s)) / 4294967296.0);
}


vec3 toNormalHemisphere(vec3 v, vec3 N) {
    vec3 helper = vec3(1, 0, 0);
    if(abs(N.x)>0.999) helper = vec3(0, 0, 1);
    vec3 tangent = normalize(cross(N, helper));
    vec3 bitangent = normalize(cross(N, tangent));
    return v.x * tangent + v.y * bitangent + v.z * N;
}

vec3 SampleHemisphere(float xi_1, float xi_2,vec3 N) {
    float z = xi_1;
    float r = max(0, sqrt(1.0 - z*z));
    float phi = 2.0 * PI * xi_2;
    return toNormalHemisphere(vec3(r * cos(phi), r * sin(phi), z),N);
}

vec3 SampleCosineHemisphere(float xi_1, float xi_2, vec3 N) {
    float r = sqrt(xi_1);
    float theta = xi_2 * 2.0 * PI;
    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(max(0.0 ,1.0 - x*x - y*y));

    vec3 L = toNormalHemisphere(vec3(x, y, z), N);
    return L;
}

Ray SampleLight(vec3 pos,vec3 normal){
    Ray ray;
    ray.origin=pos;
    vec3 L = SampleCosineHemisphere(rand(), rand(),normal);
    //vec3 L=SampleHemisphere(rand(),rand(),normal);
    ray.dir=L;
    ray.tMax=FLT_MAX;
    return ray;
}

//get scence data
void GetNode(float idx,out vec2 node){
    vec2 texCoords = vec2((idx+0.5)/textureSize(indexTex,0).x,0.5);
    node = texture2D(indexTex,texCoords).xy;
}

void GetData(float idx,out vec4 data){
    vec2 texCoords = vec2((idx+0.5)/textureSize(dataTex,0).x,0.5);
    data = texture2D(dataTex,texCoords);
}

void GetData(float idx,out vec3 data){
    vec2 texCoords = vec2((idx+0.5)/textureSize(dataTex,0).x,0.5);
    data = texture2D(dataTex,texCoords).xyz;
}

void GetVertex(float dataPos,out Vertex vertex){
    vec4 pos,normal;
    GetData(dataPos,pos);
    GetData(dataPos+1,normal);

    vertex.pos=pos.xyz;
    vertex.normal=normal.xyz;
    vertex.uv.x=pos.w;
    vertex.uv.y=normal.w;
}

void GetMtl(float dataPos,out DiffuseMtl mtl){
    GetData(dataPos+6,mtl.kd);
    GetData(dataPos+7,mtl.emission);
}




//intersction test
bool AABB_Test(Ray ray,float dataPos){
    vec3 minPoint,maxPoint;
    GetData(dataPos,minPoint);
    GetData(dataPos+1,maxPoint);

    vec3 invD=vec3(1.0)/ray.dir;
    vec3 near=(minPoint-ray.origin)*invD;
    vec3 far=(maxPoint-ray.origin)*invD;

    vec3 tmax=max(near,far);
    vec3 tmin=min(near,far);

    float tEnter=max(tmin.x,max(tmin.y,tmin.z));
    float tExit=min(tmax.x,max(tmax.y,tmax.z));

    return tExit>=0 && (tExit >= tEnter);
}

void VertexInterpolate(vec3 abg,Vertex v0,Vertex v1,Vertex v2,out Vertex vertex){
    vertex.uv=abg[0]*v0.uv+abg[1]*v1.uv+abg[2]*v2.uv;
    vertex.pos=abg[0]*v0.pos+abg[1]*v1.pos+abg[2]*v2.pos;
    vertex.normal=abg[0]*v0.normal+abg[1]*v1.normal+abg[2]*v2.normal;
}


bool TriangleTest(inout Ray ray,float dataPos, out Intersection intersect){
    Vertex v0,v1,v2;
    GetVertex(dataPos,v0);
    GetVertex(dataPos+2,v1);
    GetVertex(dataPos+4,v2);

    vec3 p0 = v0.pos;
    vec3 e0 = v1.pos - p0;
    vec3 e1 = v2.pos - p0;
    vec3 pv = cross(ray.dir, e1);
    float det = dot(e0, pv);
    if(det<0.00001f){
        return false;
    }


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
   if(uvt==vec4(0) || any(lessThan(uvt,vec4(0))) || any(greaterThan(uvt,vec4(1,1,ray.tMax,1)))){
        return false;
   }
      
    Vertex v;
    VertexInterpolate(vec3(uvt.w, uvt.x, uvt.y), v0, v1, v2,v);
    
    DiffuseMtl mtl;
    GetMtl(dataPos,mtl);

    intersect.hit = true;
    intersect.pos = v.pos;
    intersect.normal = v.normal;
    intersect.kd = mtl.kd;
    intersect.emission = mtl.emission;

    ray.tMax = uvt.z;
    return true;
}


Intersection RayIn( Ray ray){
   Intersection intersect;
   intersect.hit=false;
   intersect.kd=vec3(0);
   intersect.emission=vec3(0);

    int curNodeIndex=0;
    vec2 node;
    int indexCount=textureSize(indexTex,0).x;
    while(curNodeIndex >=0 && curNodeIndex<indexCount){
        GetNode(curNodeIndex,node);
        
        if(node.x>=0.0){  //BVH_Node
            if(AABB_Test(ray,node.x)){
                curNodeIndex++;
            }else{
                curNodeIndex = int(node.y);
            }
        }else{  //Triangle
            float dataPos=-node.x;
            if(TriangleTest(ray,dataPos,intersect)){
                curNodeIndex++;
            }else{
                curNodeIndex=int(node.y);
            }
        }
    }

    return intersect;
}



vec3 RayTrace( Ray curRay){
    Intersection intersect=RayIn(curRay);
    int stackTop=-1;
    float fr=1.0/PI;
    
    vec3 Lo=vec3(0);
    for(int bounce=0;bounce<5 && intersect.hit;bounce++){
        if(intersect.emission!=vec3(0.0)){
            Lo=intersect.emission;
            //return vec3((4-bounce)/4.0);
            break;
        }
        Ray nextRay=SampleLight(intersect.pos,intersect.normal);
        
        float invPdf=PI/dot(nextRay.dir,intersect.normal);
        Stack[++stackTop]=vec4(intersect.kd,fr *dot(nextRay.dir,intersect.normal) * invPdf);

        intersect=RayIn(nextRay);
        
    }
        
    while(stackTop>=0){
        vec4 kd=Stack[stackTop--];
        Lo=Lo*kd.w*kd.xyz;
    }

    return Lo;
}



void main()
{
    int spp=2;

    vec3 color=vec3(0,0,0);
    vec3 eyePos=vec3(278.0, 273.0, -800.0);
    float scale =tan(radians(40.0*0.5));
    float x=(1-2*gl_FragCoord.x/width)*scale;
    float y=(2*gl_FragCoord.y/height-1)*scale;
    vec3 dir=normalize(vec3(x,y,1));
    
    for(int i=0;i<spp;i++){
        vec3 AA = vec3((rand()-0.5)/width, (rand()-0.5)/height,0);

         Ray cameraRay;
        cameraRay.origin=eyePos;
        cameraRay.tMax=FLT_MAX;
        cameraRay.dir=normalize(dir+AA);
        color+=RayTrace(cameraRay)/float(spp);
    }


    if(frameCount>0){
        vec3 lastColor=texture2D(lastFrameTex,gl_FragCoord.xy/textureSize(lastFrameTex,0)).rgb;
        color=mix(lastColor,color,1.0/float(frameCount+1));
    }


    FragColor=vec4(color,1.0);
}