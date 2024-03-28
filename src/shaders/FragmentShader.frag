#version 460 core

out vec4 FragColor;

uniform int u_fiter;
uniform vec3 u_camFrontVector;
uniform vec3 u_camRightVector;
uniform float u_camPitch;
uniform float u_camYaw;
uniform vec3 u_camPos;

// Global Variables
float viewPlaneDist = 60;
vec3 cameraPos = u_camPos;
vec3 lightDir = vec3(0, -1, -0.1);
vec3 spherePos = vec3(0, 0, 0);
float sphereRadius = 195;

// Camera Vars


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


float sqr(float num) {
    return num * num;
}

struct Ray {
    vec3 origin;
    vec3 dir;
    
    vec3 hitPos;

};

Ray calcRayInfo() {
    vec3 OGpixelPos = vec3(gl_FragCoord.xy - vec2(500, 450), 0);
    vec3 focalPoint = vec3(0, 0, -60);
    vec3 OGrayDir = normalize(OGpixelPos - focalPoint);
    
    //vec3 newRayDir = rotateVector(OGrayDir, vec3(1, 0, 0), u_camPitch);

    Ray resultRay;
    resultRay.origin = u_camPos;
    resultRay.dir = OGrayDir;
    return resultRay;
}


vec4 trace(vec3 dir, vec3 origin, vec3 spherePos, float radius) {
        
        vec3 rotLightDir = rotateVector(lightDir, vec3(0, 0, 1), u_fiter * 0.0005);
        // ray is in form a + bt. a is origin, b is dir and t is scalar along ray
        // these a b and c's are coefficients after subbing parametric equation into sphere equation
        float a = sqr(dir.x) + sqr(dir.y) + sqr(dir.z); // bx^2 + by^2 + bz^2
        float b = (2 * origin.x * dir.x) + (2 * origin.y * dir.y) + (2 * origin.z * dir.z);// 2axbx + 2ayby + 2azbz
        float c = sqr(origin.x) + sqr(origin.y) + sqr(origin.z) - sqr(radius);// ax^2 + ay^2 - radius^2

        // Solve discriminant to see if there are any solutions
        float discriminant = sqr(b) - 4 * a * c;// b^2 - 4ac

        if (discriminant >= 0) {
            
            // Solve quadratic formula
            float t = (-b - sqrt(discriminant)) / 2 * a; // quadratic formula for smallest solution

            // Sub into parametric equation
            vec3 intersectionPos = origin + (dir * t);
            
            // Get normal
            vec3 intersNormal = normalize(intersectionPos - spherePos);

            // Get normal light correlation
            float correl = 1 * dot(intersNormal, -rotLightDir);
            if (correl < 0) {
                correl = 0;
            }


            vec3 outputColor = vec3(0.9, 0.6, 0.2) * correl;
            outputColor *= 1;
            outputColor += vec3(0.9 * 0.08, 0.6 * 0.08, 0.2 * 0.08);
            return vec4(outputColor, 1.0);
        }
        else { 
            return vec4(0.0, 0.0, 0.0, 1.0);
        }

   }




void main() {

    // Calculate ray direction


    // Create ray
    Ray ray = calcRayInfo();

    // Trace Ray and get hit info
    vec4 color = trace(ray.dir, ray.origin, spherePos, sphereRadius);
    FragColor = color;
    
}