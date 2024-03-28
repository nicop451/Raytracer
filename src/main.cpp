#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <DefaultSettings.h>
//#include <InputUtil.h>
#include <string>
#include <cmath>
#include <Scene.h>
#include <ErrorUtility.h>
#include <ShaderCreation.h>
#include <FileUtil.h>


int main(void) {
    Scene* scene = new Scene();

    // Program loop
    while (true) {
        scene->update();
    }
    return 0;
}