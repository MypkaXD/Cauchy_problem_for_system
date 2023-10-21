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
    App() 
    {}
    ~App() {
        ImGui::SFML::Shutdown();
    }

	void init() {
        m_window.create(sf::VideoMode(1920, 1080, 32), "Cauchy problem");
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
            
            static int item_current_idx = 0; // Here we store our selection data as an index.
            static double x0 = 0;
            static double u0 = 1;


            MakeWindowForInput();
            MakeWindowForTasks(item_current_idx);
            MakeWindowForInputInitial—onditions(x0,u0);

            std::cout << "You selected: " << item_current_idx << std::endl;
            std::cout << "X: " << x0 << std::endl;
            std::cout << "U: " << u0 << std::endl;

            static int check;
            
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

    void MakeWindowForInputInitial—onditions(double& x0, double& u0) {
        ImGui::SetNextWindowPos({ 0,41 });
        ImGui::SetNextWindowSize({ 350,140 });

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Iput start", 0, flags);
        createInputX(x0, flags);
        createInputU(u0, flags);
        ImGui::End();
    }

    void createInputX(double& x0, ImGuiWindowFlags flags) {
        flags |= ImGuiWindowFlags_NoBackground;

        ImGui::BeginChild("Input X", { 300,60 }, true, flags);
        ImGui::SeparatorText("Input X");
        ImGui::InputDouble(" ", &x0, 0.01f, 1.0f, "%.8f");
        ImGui::EndChild();
    }

    void createInputU(double& u0, ImGuiWindowFlags flags) {
        flags |= ImGuiWindowFlags_NoBackground;

        ImGui::BeginChild("Input U", { 300,60 }, true, flags);
        ImGui::SeparatorText("Input U");
        ImGui::InputDouble(" ", &u0, 0.01f, 1.0f, "%.8f");
        ImGui::EndChild();
    }

    void MakeWindowForTasks(int& item_current_idx) {
        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::SetNextWindowSize({ 350,40 });
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Select Task", 0, flags);

        const char* items[] = { "Test", "First", "Second" };
        const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
        if (ImGui::BeginCombo("Select the task", combo_preview_value))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::End();
    }

    void MakeWindowForInput() {
        ImGui::SetNextWindowPos({ 351,0 });
        ImGui::SetNextWindowSize({ 330,220});
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("InPut", 0 ,flags);
        createInputH(flags);
        createInputE_local(flags);
        createInputE_right(flags);
        ImGui::End();
    }

    void createInputE_local(ImGuiWindowFlags flags) {
        flags |= ImGuiWindowFlags_NoBackground;
        ImGui::BeginChild("Input E local", { 300,60 }, true, flags);
        ImGui::SeparatorText("Input E local");
        static double input_e_local = rk.get_E_local();
        static double max2 = 1.0;
        static double min2 = 0.000000001;
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_local, 0.0005f, &min2, &max2, "%.10f"))
            rk.set_E_local(input_e_local);
        ImGui::EndChild();
    }

    void createInputE_right(ImGuiWindowFlags flags) {
        flags |= ImGuiWindowFlags_NoBackground;
        ImGui::BeginChild("Input E right", { 300,60 }, true, flags);
        ImGui::SeparatorText("Input E right");
        static double input_e_right = rk.get_E_right();
        static double max1 = 1.0;
        static double min1 = 0.000000001;
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_right, 0.0005f, &min1, &max1, "%.10f"))
            rk.set_E_right(input_e_right);
        ImGui::EndChild();
    }

    void createInputH(ImGuiWindowFlags flags) {
        flags |= ImGuiWindowFlags_NoBackground;
        ImGui::BeginChild("Input H", { 300,60 }, true, flags);
        ImGui::SeparatorText("Input H");
        static double input_h = rk.get_Start_H();
        static double max = 1.0;
        static double min = 0.0000001;
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_h, 0.0005f, &min, &max, "%.10f")) {
            rk.set_Start_H(input_h);
            std::cout << input_h << std::endl;
        }

        ImGui::EndChild();
    }

    bool createButton() {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

        ImGui::SetNextWindowPos({ 0,181 });
        ImGui::SetNextWindowSize({ 350,39 });

        ImGui::Begin("Button", 0, flags);

        bool isPressed = false;

        if (ImGui::Button("START", { 100,20 })) {
            rk.clear();
            rk.test_func(0, 1, Task::FIRST_TASK);
            isPressed = true;
        }
        ImGui::SameLine();
        ImGui::Text("Start Calc");

        ImGui::End();
        
        return isPressed;
    }

    void createGraph() {
        std::vector<std::pair<double, double>> data = rk.getCoordsForAnalytical_Solution();

        const int size = data.size();
        double* x = new double[size];
        double* y = new double[size];

        for (int count = 0; count < data.size(); ++count)
            x[count] = (data[count].first);
        for (int count = 0; count < data.size(); ++count)
            y[count] = (data[count].second);


        std::vector<std::pair<double, double>> data1 = rk.getCoords();

        const int size1 = data1.size();
        double* x1 = new double[size1];
        double* y1 = new double[size1];

        for (int count = 0; count < data1.size(); ++count)
            x1[count] = (data1[count].first);
        for (int count = 0; count < data1.size(); ++count)
            y1[count] = (data1[count].second);


        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

        ImGui::SetNextWindowSize({ 1000,1000 });
        ImGui::SetNextWindowPos({ 1000,500 });
        ImGui::Begin("My Window", 0, flags);

        if (ImPlot::BeginPlot("My Plot")) {
            ImPlot::PlotLine("My Line Plot", x, y, size);
            ImPlot::PlotLine("My Line Plot1", x1, y1, size1);
            ImPlot::EndPlot();
        }
        ImGui::End();
    }

    void createTable() {
        std::vector<double> h = rk.getH();
        std::vector<std::pair<double, double>> data = rk.getCoords();
        std::vector<double> twice_half_h_u = rk.getVectorOfTwiceHalfHU();
        std::vector<double> difference_of_u = rk.getVectorOfDifferenceOfU();
        std::vector<double> vector_S = rk.getVectorOfS();;

        std::vector<int> C1 = rk.getC1();
        std::vector<int> C2 = rk.getC2();

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        
        ImGui::SetNextWindowPos({ 0,220 });
        ImGui::SetNextWindowSize({ 960,860 });
        ImGui::Begin("Table");
        if (ImGui::BeginTable("table1", 9, flags))
        {
            if (true)
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("X");
                ImGui::TableSetupColumn("Ui");
                ImGui::TableSetupColumn("U2i");
                ImGui::TableSetupColumn("Ui-U2i");
                ImGui::TableSetupColumn("OLP");
                ImGui::TableSetupColumn("H");
                ImGui::TableSetupColumn("C1");
                ImGui::TableSetupColumn("C2");
                ImGui::TableHeadersRow();
            }

            for (int row = 0; row < h.size(); row++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", row);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lf", data[row].first);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%lf", data[row].second);
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%lf", twice_half_h_u[row]);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%lf", difference_of_u[row]);
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%.12lf", vector_S[row]);
                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%.12lf", h[row]);
                ImGui::TableSetColumnIndex(7);
                ImGui::Text("%d", C1[row]);
                ImGui::TableSetColumnIndex(8);
                ImGui::Text("%d", C2[row]);
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }

    void render() {
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
};


/*

—ƒÂÎ‡Ú¸ ËÌËˆ ÙÎ‡„Ó‚ Ó‰ËÌ ‡Á Ë ÔÂÂ‰‡‚‡Ú¸ ‚ ÙÛÌÍˆËË

*/