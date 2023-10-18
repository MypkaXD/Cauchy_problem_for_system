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
    RK rk;
public:
	App(double a, double b) :
        rk(a,b)
    {    
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

            static int check;
            
            MakeWindowForInput();

            if (createButton()) {
                ++check;
            }

            if (check & 1) {
                createGraph();
                createTable();
            }

            m_window.clear();
            render();
        }
    }

    void MakeWindowForInput() {
        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::SetNextWindowSize({ 400,150 });
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
        ImGui::Begin("InPut", 0 ,flags);
        ImGui::SeparatorText("Input H");
        createInputH();
        ImGui::SeparatorText("Input E local");
        createInputE_local();
        ImGui::SeparatorText("Input E right");
        createInputE_right();
        ImGui::End();
    }

    void createInputE_local() {
        static double input_e_local = rk.get_E_local();
        static double max2 = 1.0;
        static double min2 = 0.000000001;
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_local, 0.0005f, &min2, &max2, "%.10f"))
            rk.set_E_local(input_e_local);
    }

    void createInputE_right() {
        static double input_e_right = rk.get_E_right();
        static double max1 = 1.0;
        static double min1 = 0.000000001;
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_right, 0.0005f, &min1, &max1, "%.10f"))
            rk.set_E_right(input_e_right);
    }

    void createInputH() {
        static double input_h = rk.get_Start_H();
        static double max = 1.0;
        static double min = 0.0000001;
        if (ImGui::DragScalar(" " , ImGuiDataType_Double, &input_h, 0.0005f, &min, &max, "%.10f"))
            rk.set_Start_H(input_h);
    }

    bool createButton() {
        if (ImGui::Button("START", { 100,20 })) {
            rk.run(0, 1);
            return true;
        }
        else return false;
    }

    void createGraph() {
        std::vector<std::pair<double, double>> data = rk.getCoords();

        const int size = data.size();
        double* x = new double[size];
        double* y = new double[size];

        for (int count = 0; count < data.size(); ++count)
            x[count] = (data[count].first);
        for (int count = 0; count < data.size(); ++count)
            y[count] = (data[count].second);


        ImGui::Begin("My Window");

        if (ImPlot::BeginPlot("My Plot")) {
            ImPlot::PlotLine("My Line Plot", x, y, size);
            ImPlot::EndPlot();
        }
        ImGui::End();
    }

    void createTable() {
        std::vector<double> h = rk.getH();
        std::vector<std::pair<double, double>> data = rk.getCoords();

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        if (ImGui::BeginTable("table1", 3, flags))
        {
            if (true)
            {
                ImGui::TableSetupColumn("X");
                ImGui::TableSetupColumn("U");
                ImGui::TableSetupColumn("H");
                ImGui::TableHeadersRow();
            }

            for (int row = 0; row < h.size(); row++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%lf", data[row].first);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lf", data[row].second);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%lf", h[row]);
            }
            ImGui::EndTable();
        }
    }

    void render() {
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
};
