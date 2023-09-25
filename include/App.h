#include "imgui.h"
#include "imgui-SFML.h"

#include "implot.h"
#include "implot_internal.h"

#include "imconfig.h"
#include "imconfig-SFML.h"

#include "SFML/Graphics.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#pragma once

class App {
private:
    sf::RenderWindow m_window;
public:
	App() {
	}
    ~App() {
        ImGui::SFML::Shutdown();
    }

	void init() {
        m_window.create(sf::VideoMode(640, 480, 32), "ImGui + SFML = <3");
        m_window.setFramerateLimit(60);
        ImGui::SFML::Init(m_window);
        ImPlot::CreateContext();
	}

    void run() {

        sf::Clock deltaClock;
        while (m_window.isOpen()) {
            ImPlot::SetCurrentContext(ImPlot::GetCurrentContext());
            sf::Event event;
            while (m_window.pollEvent(event)) {
                ImGui::SFML::ProcessEvent(m_window, event);

                if (event.type == sf::Event::Closed) {
                    m_window.close();
                }
            }
            ImGui::SFML::Update(m_window, deltaClock.restart());

            float x_data[1000] = { 0,0.0001,0.0003,0.0007,0.0015,0.0031,0.0063,0.0127,0.0255,0.0511, 0.1023,
                0.2047 , 0.4095 ,0.8191 ,1.6383 };
            float y_data[1000] = { 0,2.08333e-13,7.20833e-12,9.65417e-11,9.68542e-10,
                8.62188e-09 ,7.26539e-08 ,5.96323e-07 ,4.83172e-06 ,3.88998e-05 ,
                0.000312187 ,0.00250167 ,0.0200573 ,0.164231 ,2.10427 };

            ImGui::Begin("My Window");
            
            if (ImPlot::BeginPlot("My Plot")) {
                ImPlot::PlotLine("My Line Plot", x_data, y_data, 1000);
                ImPlot::EndPlot();
            }
            ImGui::End();

            m_window.clear();
            render();
        }
    }

    void render() {
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
};
