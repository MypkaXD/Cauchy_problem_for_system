#include "imgui.h"
#include "imgui-SFML.h"
#include "implot.h"

#include "imconfig.h"
#include "imconfig-SFML.h"

#include "SFML/Graphics.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <vector>
#include <iostream>

#include "RKIV.h"
#include "App.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>

int main() {
    system("chcp 1251");

    App app;
    app.init();
    app.run();

    return 0;
}