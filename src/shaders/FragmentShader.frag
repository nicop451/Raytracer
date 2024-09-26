#version 460 core

out vec4 FragColor;

uniform int u_fiter;
uniform float elapsedTime;

// To add spheres change object count

// Camera Uniforms
uniform float u_vpDist;
uniform float u_camPitch;
uniform float u_camYaw;
uniform vec3 u_camPos;

uniform float u_WIN_WIDTH;
uniform float u_WIN_HEIGHT;

// Textures
uniform sampler2D u_skyboxTexture;
uniform sampler2D u_prevFrameTexture;
uniform int u_frameProgCount;


// Global Variables
int SAMPLES = 24;
float skyboxEmissionStrength = 1.5;
float exposure = 1.0;
float DOF_AMOUNT = 5;
float DOF_DIST = 400;
#define BOUNCES 15
#define NUM_OF_OBJECTS 6
#define PI 3.1415926
#define antiAliasAmount 0.0005

uint rngState = uint(uint(gl_FragCoord.x) * uint(1973) + uint(gl_FragCoord.y) * uint(9277) + uint(u_fiter - 1000) * uint(26699)) | uint(1);

// --- Data Structures ----
struct Ray {
    vec3 origin; vec3 dir;
    vec3 tint; vec3 light;
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


// -------------- Math --------------
// Better Random 
uint wang_hash(inout uint seed);
float RandomFloat01(inout uint state);
vec3 RandomUnitVector(inout uint state);
// ACES tone mapping curve fit to go from HDR to LDR
//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x);
vec3 rotateVector(vec3 v, vec3 axis, float angle);
vec3 randHemisphere(vec3 normal, vec2 seed);
vec3 ReinhardToneMap(vec3 hdr, float bright);
vec3 SRGBtoLinear(vec3 rgb);
vec3 LineartoSRGB(vec3 rgb);
vec3 getSkybox(vec3 dir);
float fresnelFunction (float ior1, float ior2, vec3 normal, vec3 incident, float f0, float f90);
// ----------------------------------

vec3 prevPixelColor = SRGBtoLinear(texture(u_prevFrameTexture, gl_FragCoord.xy / vec2(u_WIN_WIDTH, u_WIN_HEIGHT)).rgb);

// Cleanup functions
Ray calcRayInfo () {
    vec3 TEMPrightVec = vec3(1, 0, 0);

    vec3 pixelPos = vec3((gl_FragCoord.xy - vec2(u_WIN_WIDTH/2, u_WIN_HEIGHT/2)), u_vpDist);
    float ratioToShrink = 1/pixelPos.z;
    pixelPos *= ratioToShrink * DOF_DIST;

    vec3 focalPoint = normalize(vec3(RandomFloat01(rngState) * 2.0f - 1.0f, RandomFloat01(rngState) * 2.0f - 1.0f, 0)) * DOF_AMOUNT;

    vec3 rayDir = normalize(pixelPos - focalPoint);

    vec3 rotatedRayDir1 = rotateVector(rayDir, vec3(0, 1, 0), u_camYaw);
    vec3 rotatedRightVector = rotateVector(TEMPrightVec, vec3(0, 1, 0), u_camYaw);

    vec3 rotatedRayDir2 = rotateVector(rotatedRayDir1, rotatedRightVector, u_camPitch);

    Ray resultRay;
    resultRay.origin = focalPoint;
    resultRay.origin = rotateVector(resultRay.origin, vec3(0, 1, 0), u_camYaw);
    resultRay.origin = rotateVector(resultRay.origin, rotatedRightVector, u_camPitch);
    resultRay.origin += u_camPos;
    resultRay.dir = rotatedRayDir2;
    resultRay.lastHitObjInd = -1;
    return resultRay;   
}
Ray calcCameraFacingRay () {
    vec3 TEMPrightVec = vec3(1, 0, 0);

    vec3 rayDir = vec3(0, 0, 1);

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
    skyMat_t.albedo = vec3(1.0, 1.0, 1.0);
    skyMat_t.roughness = 0;
    skyMat_t.specularity = 0;
    skyMat_t.emmisivePower = 1;
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
void initializeSpheres();
// TODO: Cosine weighted bounces
// TODO: refraction

void main() {
    initializeSpheres();
    // Calculate Auto Focus distance
    Ray focusRay = calcCameraFacingRay();
    hit focusTrace = TraceRay(focusRay, -1);
    if(focusTrace.didHit) { DOF_DIST = focusTrace.dist * 1.1; }
    else { DOF_DIST = 1400; }

    // Main Path Tracing Loop ----------------------------
    Ray sourceRay = calcRayInfo();
    
    // Variable that ends up being average of all samples
    vec3 cumulationColors;

    for (int s = 0; s < SAMPLES; s++) {
        // Initialize Ray From Camera
        Ray ray = sourceRay;
        ray.tint = vec3(1, 1, 1);
        ray.light = vec3(0, 0, 0);

        // Anti Aliasing !!Could be done per ray in order to do depth of field
        ray.dir += vec3(RandomFloat01(rngState) * antiAliasAmount, RandomFloat01(rngState) * antiAliasAmount, 0);

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
            bool isSpecularRay =  hit.material.specularity + fresnelFunction(1, hit.material.ior, hit.normal, ray.dir, 0.98, 0.25) > RandomFloat01(rngState);
               
            // Calc outgoing ray direction
            vec3 diffDir = normalize(hit.normal + RandomUnitVector(rngState)); // Fix Randomization

            if (isSpecularRay) { // Ray is Specular
                vec3 specDir = reflect(ray.dir, hit.normal);
                ray.tint *= hit.material.specularColor;//  * dot(-ray.dir, hit.normal) * 2;
                ray.dir = mix(specDir, diffDir, hit.material.roughness * hit.material.roughness);
            }
            else { // Ray is diffuse
                ray.tint *= hit.material.albedo * mix(dot(-ray.dir, hit.normal) * 2, 1.0, 0.85);
                ray.dir = diffDir;
            }
        }
        cumulationColors += (ray.tint * ray.light);
    }
    // Average Previous Frame and Current
    vec3 currCalcColor = (cumulationColors / SAMPLES) * exposure;
    vec3 averagedColor = ((prevPixelColor * u_fiter) + currCalcColor)/(u_fiter+1);
    //averagedColor = averagedColor;
    vec3 finalColor = LineartoSRGB(u_fiter > 1 ? averagedColor : currCalcColor);

    FragColor = vec4(finalColor, 1.0);
}

// Math Functions
vec3 ReinhardToneMap(vec3 hdr, float bright) {
    return hdr / (hdr + vec3(bright));
}
float fresnelFunction (float ior1, float ior2, vec3 normal, vec3 incident, float f0, float f90) {
    float ro = (ior1 - ior2)/(ior1 + ior2);

    float cosTheta = -dot(incident, normal);

    float reflectivityCoeff = ro + (1 - ro) * pow((1 - cosTheta), 5);

    return mix(f90, f0, reflectivityCoeff);
}
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
vec3 getSkybox(vec3 dir) {
    vec2 st;
    st.x = 0.5 + atan(dir.z, dir.x) / (2.0 * PI);
    st.y = 0.5 - asin(dir.y) / PI;
    // Sample the cube map using the texture coordinates
    vec3 col = texture(u_skyboxTexture, st).rgb;
    col = clamp(col, 0.0, 5.0);
    col = ReinhardToneMap(col, 1.0);
    return SRGBtoLinear(col * skyboxEmissionStrength);
}
vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0f, 1.0f);
}

// Random Number Generator
uint wang_hash(inout uint seed)
{
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

float RandomFloat01(inout uint state)
{
    return float(wang_hash(state)) / 4294967296.0;
}
 
vec3 RandomUnitVector(inout uint state)
{
    float z = RandomFloat01(state) * 2.0f - 1.0f;
    float a = RandomFloat01(state) * 2 * PI;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}

// Cleanup Functions
void initializeSpheres() {
    Material m1;
    m1.albedo = vec3(0.3, 0.5, 0.98);
    m1.roughness = 0.01;
    m1.specularity = 1.25;
    m1.specularColor = vec3(0.98, 0.98, 0.98);
    m1.emmisivePower = 0;
    m1.ior = 1.7;

    sphereArray[0].radius = 200;
    sphereArray[0].position = vec3((0 * 400) - 700, -500, 0);
    sphereArray[0].material = m1;
    
    m1.specularity = 1.5;
    m1.roughness = 0.0;
    m1.albedo = vec3(0.3, 0.8, 0.9);
    sphereArray[1].radius = 200;
    sphereArray[1].position = vec3((1 * 400) - 700, -500, 0);
    sphereArray[1].material = m1;

    m1.albedo = vec3(0.7,0.7,0.7);
    m1.roughness = 0.01;
    m1.specularity = 0.99;
    m1.specularColor = m1.albedo;
    sphereArray[2].radius = 200;
    sphereArray[2].position = vec3((3 * 450) - 700, -500, 0);
    sphereArray[2].material = m1;

    m1.albedo = vec3(0.8, 0.5, 0.5);
    m1.emmisivePower = 5;
    m1.roughness = 0.0;
    m1.specularity = 0.0;
    sphereArray[3].radius = 200;
    sphereArray[3].position = vec3((2 * 450) - 700, -500, 0);
    sphereArray[3].material = m1;

    m1.emmisivePower = 0;
    m1.roughness = 0.03;
    m1.specularity = 0.99;
    m1.albedo = vec3(0.5, 0.76, 0.85);
    m1.specularColor = m1.albedo;
    sphereArray[5].radius = 1000;
    sphereArray[5].position = vec3((3 * 120) - 300, 300, 1200);
    sphereArray[5].material = m1;


    
    // Floor
    Material m;
    m.emmisivePower = 0;
    m.roughness = 1.0;
    m.specularity = 0.0;
    m.specularColor = vec3(0.9, 0.9, 0.9);


    m.albedo = vec3(0.98, 0.98, 0.98);
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
    m.emmisivePower = 3000;
    m.roughness = 1.0;
    m.specularity = 1.0;
    m.albedo = vec3(0.2, 1.0, 1.0);
    
    /*sphereArray[6].radius = 50;
    sphereArray[6].position = vec3(0, 1200, -100);
    sphereArray[6].material = m;*/
}