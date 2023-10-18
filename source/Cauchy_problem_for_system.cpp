﻿#include "imgui.h"
#include "imgui-SFML.h"

#include "imconfig.h"
#include "imconfig-SFML.h"

#include "SFML/Graphics.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <vector>
#include <iostream>

#include "RKIV.h"
#include "App.h"

int main() {
    
    system("chcp 1251");

    App app(0,1);
    app.init();
    app.run();
    
    return 0;
}