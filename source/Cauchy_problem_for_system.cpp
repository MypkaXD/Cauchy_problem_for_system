#include "imgui.h"
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
    
    App app;
    app.init();
    app.run();
    //
    //double m_E_check_up = 0.5 * 10e-4; // проверка для "вверха"
    //double m_E_check_down = m_E_check_up / 15; // проверка для "низа"
    //std::cout << m_E_check_up << std::endl;
    //std::cout << m_E_check_down << std::endl;
    return 0;
}