#version 460 core

out vec4 FragColor;

uniform int u_fiter;

// Camera Uniforms
uniform float u_vpDist;
uniform float u_camPitch;
uniform float u_camYaw;
uniform vec3 u_camPos;

// Textures
uniform sampler2D u_skyboxTexture;
uniform sampler2D u_prevFrameTexture;
uniform int u_frameProgCount;


// Global Variables
int SAMPLES = 32;
float skyboxEmissionStrength = 5;
float exposure = 1;
#define BOUNCES 15
#define NUM_OF_OBJECTS 5
#define PI 3.1415926
#define antiAliasAmount 0.0015

vec2 rngState = gl_FragCoord.xy + u_fiter;

// --- Data Structures ----
struct Ray {
    vec3 origin;
    vec3 dir;
    vec3 tint;
    vec3 light;
    int lastHitObjInd;
};

struct Material {
    vec3 albedo;
    vec3 specularColor;
    float specularity;
    float ior;
    float roughness;
    float emmisivePower;
};

struct hit {
    bool didHit;
    vec3 location;
    vec3 normal;
    float dist;
    int objIndex;
    Material material;
};

struct PathInfo {
    vec3 resultingColor;
};

struct Sphere {
    vec3 position;
    float radius;
    Material material;
};
// -----------------------

Sphere sphereArray[NUM_OF_OBJECTS];


// Math Functions
vec3 rotateVector(vec3 v, vec3 axis, float angle) {
    float cosTheta = cos(angle);
    float sinTheta = sin(angle);
    float oneMinusCos = 1.0 - cosTheta;
    
    // Rotation matrix elements
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    
    // Apply rotation matrix
    vec3 rotatedVec = vec3(
        (cosTheta + oneMinusCos * x * x) * v.x + (oneMinusCos * x * y - sinTheta * z) * v.y + (oneMinusCos * x * z + sinTheta * y) * v.z,
        (oneMinusCos * x * y + sinTheta * z) * v.x + (cosTheta + oneMinusCos * y * y) * v.y + (oneMinusCos * y * z - sinTheta * x) * v.z,
        (oneMinusCos * x * z - sinTheta * y) * v.x + (oneMinusCos * y * z + sinTheta * x) * v.y + (cosTheta + oneMinusCos * z * z) * v.z
    );
    
    return rotatedVec;
}

float randomFloat(vec2 seed) {
    float modifiedSeed = float(u_fiter * PI) + seed.x * 10.0 + seed.y * 20.0; // Adjust the seed based on the frame count
    return fract(sin(dot(vec2(modifiedSeed, modifiedSeed + 1.0), vec2(12.9898, 78.233))) * 43758.5453) * 2.0 - 1.0;
}

vec3 randHemisphere(vec3 normal, vec2 seed) {
    vec3 randomCubeDir =  normalize(vec3(randomFloat(vec2(seed.x, seed.y)), randomFloat(vec2(seed.y, seed.x)), randomFloat(vec2(seed.x, seed.y / seed.x))));
    randomCubeDir += normal;
    return normalize(randomCubeDir);
}


vec3 LessThan(vec3 f, float value) {
    return vec3(
        f.x < value ? 1.0 : 0.0,
        f.y < value ? 1.0 : 0.0,
        f.z < value ? 1.0 : 0.0
    );
}

vec3 ReinhardToneMap(vec3 hdr) {
    return hdr / (hdr + vec3(0.9));
}


vec3 SRGBtoLinear(vec3 rgb) {
    //rgb = clamp(rgb, 0.0f, 1.0f);
    bool xCond = rgb.x <= 0.04045;
    bool yCond = rgb.y <= 0.04045;
    bool zCond = rgb.z <= 0.04045;

    return vec3 (
        xCond ? rgb.x / 12.92 : pow((rgb.x + 0.55)/1.055, 2.4),
        yCond ? rgb.y / 12.92 : pow((rgb.y + 0.55)/1.055, 2.4),
        zCond ? rgb.z / 12.92 : pow((rgb.z + 0.55)/1.055, 2.4)
    );
}
vec3 LineartoSRGB(vec3 rgb) {
    //rgb = clamp(rgb, 0.0f, 1.0f);
    bool xCond = rgb.x <= 0.0031308;
    bool yCond = rgb.y <= 0.0031308;
    bool zCond = rgb.z <= 0.0031308;

    return vec3 (
        xCond ? rgb.x * 12.92 : 1.055 * pow(rgb.x, 1.0/2.4) - 0.55,
        yCond ? rgb.y * 12.92 : 1.055 * pow(rgb.y, 1.0/2.4) - 0.55,
        zCond ? rgb.z * 12.92 : 1.055 * pow(rgb.z, 1.0/2.4) - 0.55
    );
}

vec3 prevPixelColor = SRGBtoLinear(texture(u_prevFrameTexture, gl_FragCoord.xy / vec2(1400, 1000)).rgb);

vec3 getSkybox(vec3 dir) {
    vec2 st;
    st.x = 0.5 + atan(dir.z, dir.x) / (2.0 * PI);
    st.y = 0.5 - asin(dir.y) / PI;
    // Sample the cube map using the texture coordinates
    vec3 col = texture(u_skyboxTexture, st).rgb;

    //col = clamp(col, 0.0f, 9.0f);

    return SRGBtoLinear(ReinhardToneMap(col *  skyboxEmissionStrength));
}
// Cleanup functions
Ray calcRayInfo () {
    vec3 TEMPrightVec = vec3(1, 0, 0);

    vec3 OGpixelPos = vec3((gl_FragCoord.xy - vec2(500, 450)), 0);
    vec3 focalPoint = vec3(0, 0, u_vpDist);
    
    vec3 modPixelPos = (OGpixelPos + u_camPos) + vec3(0, 0, u_vpDist);

    vec3 rayDir = normalize(modPixelPos - u_camPos);

    vec3 rotatedRayDir1 = rotateVector(rayDir, vec3(0, 1, 0), u_camYaw);
    vec3 rotatedRightVector = rotateVector(TEMPrightVec, vec3(0, 1, 0), u_camYaw);

    vec3 rotatedRayDir2 = rotateVector(rotatedRayDir1, rotatedRightVector, u_camPitch);

    Ray resultRay;
    resultRay.origin = u_camPos;
    resultRay.dir = rotatedRayDir2;
    resultRay.lastHitObjInd = -1;
    return resultRay;
}

// Raytracing functionality
hit TraceRay (const Ray originalRay, const int indToExclude) { // Given origin and dir trace through all spheres in scene and get closest intersection
    // Set Hit Info For No Hit/Hit Sky
    hit hit;
    hit.didHit = false;
    hit.dist = 3.402823e38;
    hit.normal = vec3(0, 0, 0);
    hit.location = vec3(0, 0, 0);
    hit.objIndex = -1;
    Material skyMat_t;
    skyMat_t.albedo = vec3(0.0, 0.0, 0.0);
    skyMat_t.roughness = 0;
    skyMat_t.specularity = 0;
    skyMat_t.emmisivePower = 0;
    hit.material = skyMat_t;



    for(int i = 0; i < NUM_OF_OBJECTS; i++) {
        if (i != indToExclude); // Dont trace previously hit sphere

        Ray ray = originalRay; // In order to not modify source ray

        Sphere sphereCurr = sphereArray[i];
       
        // Trace Single Ray For Current Object
        ray.origin -= sphereCurr.position;
        float a = dot(ray.dir, ray.dir);
        float b = 2 * dot(ray.origin, ray.dir);
        float c = dot(ray.origin, ray.origin) - sphereCurr.radius * sphereCurr.radius;
        float discriminant = (b * b) - 4 * a * c;
        if (discriminant > 0) {
            
            float t = (-b - sqrt(discriminant)) / (2 * a);
            if (t < hit.dist && t > 0) {
                hit.didHit = true;
                hit.dist = t;
                vec3 hitLoc_p = (ray.origin + (ray.dir * t));
                hit.location = (hitLoc_p) + sphereCurr.position;
                hit.normal = normalize(hitLoc_p);
                hit.material = sphereCurr.material;
                hit.objIndex = i;
            }
        }

    }
    // Return the last closet hit we got
    return hit;
}



// TODO: Cosine weighted bounces
// TODO: refraction
// TODO: Depth of Field

float fresnelFunction (float ior1, float ior2, vec3 normal, vec3 incident);


void main() {
    

    Material m1;
    m1.albedo = vec3(0.3 , 0.7, 0.98);
    m1.roughness = 0.1;
    m1.specularity = 0.2;
    m1.specularColor = vec3(0.98, 0.98, 0.98);
    m1.emmisivePower = 0;
    m1.ior = 1.7;

    sphereArray[0].radius = 200;
    sphereArray[0].position = vec3((2 * 450) - 700, -500, 0);
    sphereArray[0].material = m1;
    
    m1.albedo = vec3(0.9, 0.5, 0.7);
    sphereArray[1].radius = 200;
    sphereArray[1].position = vec3((1 * 450) - 700, -500, 0);
    sphereArray[1].material = m1;


    m1.albedo = vec3(0.7,0.7,0.7);
    m1.roughness = 0.05;
    m1.specularity = 0.99;
    m1.specularColor = m1.albedo;
    sphereArray[2].radius = 200;
    sphereArray[2].position = vec3((0 * 450) - 700, -500, 0);
    sphereArray[2].material = m1;

    m1.albedo = vec3(0.4, 0.8, 0.5);
    m1.emmisivePower = 5;
    m1.roughness = 0.0;
    m1.specularity = 0.0;
    sphereArray[3].radius = 200;
    sphereArray[3].position = vec3((3 * 450) - 700, -500, 0);
    sphereArray[3].material = m1;
    
    // Floor
    Material m;
    m.emmisivePower = 0;
    m.roughness = 1.0;
    m.specularity = 0.0;
    m.specularColor = vec3(0.9, 0.9, 0.9);


    m.albedo = vec3(0.6, 0.6, 0.6);
    sphereArray[4].radius = 90000;
    sphereArray[4].position = vec3(0, -90700, 0);
    sphereArray[4].material = m;

    /*m.roughness = 0.4;
    m.specularity = 0.4;
    sphereArray[6].radius = 90000;
    sphereArray[6].position = vec3(0, 0, 90700);
    sphereArray[6].material = m;*/

    //-------------------------------------------

    // Ceiling Light
    m.emmisivePower = 10;
    m.roughness = 1.0;
    m.specularity = 1.0;
    m.albedo = vec3(1.0, 1.0, 1.0);
    
    /*sphereArray[7].radius = 500;
    sphereArray[7].position = vec3(0, 1200, -100);
    sphereArray[7].material = m;*/

    // Main Stuff
    Ray sourceRay = calcRayInfo();
    
    // Variable that ends up being average of all samples
    vec3 cumulationColors;


    for (int s = 0; s < SAMPLES; s++) { // For Each Sample
        
        // Initialize Ray From Camera
        Ray ray = sourceRay;
        ray.tint = vec3(1, 1, 1);
        ray.light = vec3(0, 0, 0);

        // Anti Aliasing !!Could be done per ray in order to do depth of field
        ray.dir += vec3(randomFloat(vec2(gl_FragCoord.xy/s)) * antiAliasAmount, randomFloat(vec2(gl_FragCoord.yx/s)) * antiAliasAmount, 0);

        // Trace Ray and keep on tracing
        for (int b = 0; b < BOUNCES; b++) { // Trace Ray through scene
            
            // Trace single Ray
            hit hit = TraceRay(ray, ray.lastHitObjInd); 

            // Just Sample Skybox if there is no hit
            if(!hit.didHit) {
                ray.light += getSkybox(ray.dir);
                break; 
            }

            ray.origin = hit.location;
            ray.lastHitObjInd = hit.objIndex;

            ray.light += hit.material.emmisivePower;
            

            // Calc if ray should be a specular ray
            bool isSpecularRay =  hit.material.specularity + fresnelFunction(1, hit.material.ior, hit.normal, ray.dir) * 0.6 > abs(randomFloat(vec2(hit.location.yx / s)));
               
            // Calc outgoing ray direction
            vec3 diffDir = randHemisphere(hit.normal, (hit.location.xy / s)); // Fix Randomization

            if (isSpecularRay) { // Ray is Specular
                vec3 specDir = reflect(ray.dir, hit.normal);
                ray.tint *= hit.material.specularColor;//  * dot(-ray.dir, hit.normal) * 2;
                ray.dir = mix(specDir, diffDir, hit.material.roughness * hit.material.roughness);
            }
            else { // Ray is diffuse
                ray.tint *= hit.material.albedo;// * dot(-ray.dir, hit.normal) * 2;
                ray.dir = diffDir;
            }

        }
        cumulationColors += (ray.tint * ray.light); // Add Path Color To Sampling Array
    }
    // Average Out Accumulated Data
    vec3 finalColor = cumulationColors / SAMPLES;

    bool isFirstFrame = u_fiter > 1;

    vec3 prevCombinedWithCurr = ((prevPixelColor * (u_fiter)) + finalColor)/(u_fiter+1);//mix(prevFramePixelColor, finalColor, 1 / (u_fiter + 1));

    FragColor = vec4(LineartoSRGB(isFirstFrame ? prevCombinedWithCurr : finalColor), 1.0);
}

float fresnelFunction (float ior1, float ior2, vec3 normal, vec3 incident) {
    float ro = (ior1 - ior2)/(ior1 + ior2);

    float cosTheta = -dot(incident, normal);

    return ro + (1 - ro) * pow((1 - cosTheta), 5);
}
