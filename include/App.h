#include "imgui.h"
#include "imgui-SFML.h"

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
	}

    void run() {

        sf::Clock deltaClock;
        while (m_window.isOpen()) {
            sf::Event event;
            while (m_window.pollEvent(event)) {
                ImGui::SFML::ProcessEvent(m_window, event);

                if (event.type == sf::Event::Closed) {
                    m_window.close();
                }
            }
            ImGui::SFML::Update(m_window, deltaClock.restart());
            
            bool open = true;
            ImGui::SetNextWindowSize({ 100,100});
            ImGui::Begin("Name", &open, ImGuiWindowFlags_NoResize);
            
            static int count = 0;
            if (ImGui::Button("NAME")) {
                ++count;
            }
            ImGui::SameLine(); ImGui::Text("Count of pressing: %d", count);
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
