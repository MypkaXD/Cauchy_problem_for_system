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
            
            static int item_current_idx = 0;
            static double x0 = 0;
            static double u0 = 1;
            static double u_0 = 1;
            static int param_a = 1;
            static int param_b = 2;
            static ImGuiWindowFlags flagsForWindows = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
            static bool isConstH = false;
            static bool isPressed = false;

            MakeWindowForInput(flagsForWindows); // �������� ������� ��� �������� ���� ��� ����� ���� � �������-������
            MakeWindowForTasks(item_current_idx, flagsForWindows, isPressed); // �������� ������� �� �������� ���� � ����������� ������� �����
            MakeWindowForInputInitial�onditions(x0,u0, flagsForWindows); // �������� ������� �� �������� ���� ��� ����� ��������� �������
            makeWindowForInputBorder(flagsForWindows);
            makeWindowForCheckBoxAboutE(flagsForWindows, isConstH, isPressed);
            createButton(x0, u0, flagsForWindows, item_current_idx, isConstH, isPressed, u_0, param_a, param_b);
            makeWindowForInfo(flagsForWindows, isPressed, item_current_idx, isConstH);
            if (item_current_idx == 2)
                makeWindowForSecondTaskInput(flagsForWindows, isPressed, param_a, param_b, u_0);


            if (isPressed) { // ���� ������ ���� ������
                createGraph(flagsForWindows, item_current_idx); // ������ ������
                createTable(flagsForWindows, item_current_idx, isConstH); // ������� �������
            }
            else
                clearTableAndGraph(flagsForWindows);

            m_window.clear();
            render();
        }
    }

    void makeWindowForSecondTaskInput(ImGuiWindowFlags& flagsForWindows, bool& isPressed, int& param_a, int& param_b, double& u_0) {
        ImGui::SetNextWindowPos({ 1013,0 }); // ������������� ������� ��� �������� ����
        ImGui::SetNextWindowSize({ 317,220 });

        ImGui::Begin("Input for 2-nd task", 0, flagsForWindows);

        createInputParamA(flagsForWindows, param_a);
        createInputParamB(flagsForWindows, param_b);
        createInputStartU_0(flagsForWindows, u_0);

        ImGui::End();
    }

    void createInputStartU_0(ImGuiWindowFlags flagsForWindows, double& u_0) {

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ��������� ��� ����, ����� �� ���� ������� ����

        ImGui::BeginChild("Input X", { 300,60 }, true, flagsForWindows); // ������� ������� ���� � �������� 300*60 � �����������-flags

        ImGui::SeparatorText("Input U'0"); // �������������� ����� � ����

        ImGui::InputDouble(" ", &u_0, 0.01f, 1.0f, "%.8f"); // ���� ��� ����� double x

        ImGui::EndChild(); // ������� �������� ����
    }

    void createInputParamA(ImGuiWindowFlags flagsForWindows, int& param_a) {

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ��������� ��� ����, ����� �� ���� ������� ����

        ImGui::BeginChild("Input param A", { 300,60 }, true, flagsForWindows); // ������� ������� ���� � �������� 300*60 � �����������-flags

        ImGui::SeparatorText("Input param A"); // �������������� ����� � ����

        ImGui::InputInt(" ", &param_a, 1, 100); // ���� ��� ����� double x

        ImGui::EndChild(); // ������� �������� ����
    }

    void createInputParamB(ImGuiWindowFlags flagsForWindows, int& param_b) {

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ��������� ��� ����, ����� �� ���� ������� ����

        ImGui::BeginChild("Input param B", { 300,60 }, true, flagsForWindows); // ������� ������� ���� � �������� 300*60 � �����������-flags

        ImGui::SeparatorText("Input param B"); // �������������� ����� � ����

        ImGui::InputInt(" ", &param_b, 1, 100); // ���� ��� ����� double x

        ImGui::EndChild(); // ������� �������� ����
    }

    void makeWindowForInfo(ImGuiWindowFlags& flagsForWindows, bool& isPressed, int& item_current_idx, bool& isConstH) {

        if (item_current_idx == 2) {
            ImGui::SetNextWindowPos({ 1331,0 }); // ������������� ������� ��� �������� ����
            ImGui::SetNextWindowSize({ 607,220 });
        }
        else {
            ImGui::SetNextWindowPos({ 1013,0 }); // ������������� ������� ��� �������� ����
            ImGui::SetNextWindowSize({ 907,220 });
        }

        ImGui::Begin("Window for info",0,flagsForWindows);
        if (isPressed) {
            ImGui::Text("Count of N: %d", rk.getCoords().size() - 1);
            if (!isConstH) {
                std::vector<double> S = rk.getVectorOfS();
                double maxS = *max_element(S.begin(), S.end());
                ImGui::Text("MAX OLP: %.24lf", maxS);

                std::vector<double> H = rk.getH();
                double maxH = *max_element(H.begin()+1, H.end());
                double minH = *min_element(H.begin()+1, H.end());
                ImGui::Text("MIN H: %.24lf", minH);
                ImGui::Text("MAX H: %.24lf", maxH);

                if (item_current_idx == 0) {
                    std::vector<double> diff = rk.getVectorOfDifferenceU();
                    double maxDiff = *max_element(diff.begin(), diff.end());
                    ImGui::Text("MAX |Ui-Vi|: %.24lf", maxDiff);
                }

                //std::vector<int> C1 = rk.getC1();
                //int countOfDiv;
                //for (size_t count = 0; count < C1.size(); ++count)
                //    countOfDiv += C1[count];
                //ImGui::Text("Count of divisions: %d", countOfDiv);
                //
                //std::vector<int> C2 = rk.getC2();
                //int countOfMult;
                //for (size_t count = 0; count < C2.size(); ++count)
                //    countOfMult += C2[count];
                //ImGui::Text("Count of doublings: %d", countOfMult);
            }
        }

        ImGui::End();
    }

    void makeWindowForCheckBoxAboutE(ImGuiWindowFlags& flagsForWindows, bool& isConstH, bool& isPressed) {

        ImGui::SetNextWindowPos({ 682,141 }); // ������������� ������� ��� �������� ����
        ImGui::SetNextWindowSize({ 330,79 }); 

        ImGui::Begin("Checkbox", 0, flagsForWindows); // ������� ���� � ���������� 

        if (ImGui::Checkbox("CONST H", &isConstH))
            isPressed = false;

        int N = rk.getN();

        if (ImGui::InputInt("Count of N", &N, 1, 10, 0))
            rk.setN(N);

        ImGui::End();

    }

    void makeWindowForInputBorder(ImGuiWindowFlags& flagsForWindows) {

        ImGui::SetNextWindowPos({ 682,0 }); // ������������� ������� ��� �������� ����
        ImGui::SetNextWindowSize({ 330,140 }); // ������������ ������ ��� �������� ���� = 330*140

        ImGui::Begin("Input Border", 0, flagsForWindows); // ������� ���� � ���������� �����������

        createInputBorderA(flagsForWindows);
        createInputBorderB(flagsForWindows);

        ImGui::End(); // ������� ����
    }

    void createInputBorderA(ImGuiWindowFlags flagsForWindows) {

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ��������� ��� ����, ����� �� ���� ������� ����

        ImGui::BeginChild("Input A", { 300,60 }, true, flagsForWindows); // ������� ������� ���� � �������� 300*60 � �����������-flags

        ImGui::SeparatorText("Input A"); // �������������� ����� � ����

        double a = rk.getBorder().first;
        double max_for_a = rk.getBorder().second - 0.0001;
        static double min_for_a = -INFINITY;

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &a, 0.0005f, &min_for_a, &max_for_a, "%.10f", ImGuiSliderFlags_AlwaysClamp)) 
            rk.setBorderA(a);

        ImGui::EndChild();
    }

    void createInputBorderB(ImGuiWindowFlags flagsForWindows) {
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ��������� ��� ����, ����� �� ���� ������� ����

        ImGui::BeginChild("Input B", { 300,60 }, true, flagsForWindows); // ������� ������� ���� � �������� 300*60 � �����������-flags

        ImGui::SeparatorText("Input B"); // �������������� ����� � ����

        double b = rk.getBorder().second;
        static double max_for_b = +INFINITY;
        double min_for_b = rk.getBorder().first + 0.0001;

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &b, 0.0005f, &min_for_b, &max_for_b, "%.10f", ImGuiSliderFlags_AlwaysClamp))
            rk.setBorderB(b);

        ImGui::EndChild(); // ������� �������� ����
    }

    void MakeWindowForInputInitial�onditions(double& x0, double& u0, ImGuiWindowFlags& flagsForWindows) { // ������� ���� ��� ����� ��������� �������
        
        ImGui::SetNextWindowPos({ 0,41 }); // ������������� ������� ����
        ImGui::SetNextWindowSize({ 350,140 }); // ������������� ������ ����

        ImGui::Begin("Iput start", 0, flagsForWindows); // �������� ����

        createInputX(x0, flagsForWindows); // �������� ������� �� �������� ��������� ���� ��� ����� ���������� X
        createInputU(u0, flagsForWindows); // �������� ������� �� �������� ��������� ���� ��� ����� ���������� U

        ImGui::End(); // ������� ����
    }

    void createInputX(double& x0, ImGuiWindowFlags flagsForWindows) { // ������� ���� ��� ����� �
        
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ��������� ��� ����, ����� �� ���� ������� ����

        ImGui::BeginChild("Input X", { 300,60 }, true, flagsForWindows); // ������� ������� ���� � �������� 300*60 � �����������-flags

        ImGui::SeparatorText("Input X0"); // �������������� ����� � ����

        ImGui::InputDouble(" ", &x0, 0.01f, 1.0f, "%.8f"); // ���� ��� ����� double x

        ImGui::EndChild(); // ������� �������� ����
    }

    void createInputU(double& u0, ImGuiWindowFlags flagsForWindows) { // ������� ���� ��� ����� U
        
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ��������� ��� ����, ����� �� ���� ������� ����

        ImGui::BeginChild("Input U", { 300,60 }, true, flagsForWindows); // ������� ������� ���� � �������� 300*60 � �����������-flags

        ImGui::SeparatorText("Input U0"); // �������������� ����� � ����

        ImGui::InputDouble(" ", &u0, 0.01f, 1.0f, "%.8f"); // ���� ��� ����� double u

        ImGui::EndChild();// ������� �������� ����
    }

    void MakeWindowForTasks(int& item_current_idx, ImGuiWindowFlags& flagsForWindows, bool& isPressed) { // ������� ���� ��� ������ ����������� ������. ��� item_current_idx - �������� ������ �� ���������� �� ��������� �����, ���������� �� ��������� ������, ���� 0 - ��������, 1 - ������ ������, 2 - ������ ������
        
        ImGui::SetNextWindowPos({ 0,0 }); // ������������� ������� � �����-������� ���� (0,0)
        ImGui::SetNextWindowSize({ 350,40 }); // ������������� ������� ��� ���� 350*40

        ImGui::Begin("Select Task", 0, flagsForWindows); // ������� ���� � ���������� ����������� flagsForWindows

        const char* items[] = { "Test", "First", "Second" }; // ������� ������ � ��������� ������
        const char* combo_preview_value = items[item_current_idx];  // ���������� ���������� ��������� ������
        
        if (ImGui::BeginCombo("Select the task", combo_preview_value)) // ������� ����������� ������ �� ������
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) // ������� ������ �� ������� �����
            {
                const bool is_selected = (item_current_idx == n); // ���� ������ � �������, ����������� � item_current_idx -> true
                if (ImGui::Selectable(items[n], is_selected)) { // ������������� ������ n ��� item_current_idx, ���� ������� ������ n
                    item_current_idx = n;
                    isPressed = false;
                }
                if (is_selected) // ���� is_selected
                    ImGui::SetItemDefaultFocus(); // ������������� ����� �� ��������� ������
            }
            ImGui::EndCombo(); // ������� ����������� ����
        }
         
        ImGui::End(); // ������� ����
    }

    void clearTableAndGraph(ImGuiWindowFlags& flagsForWindows) {
        ImGui::SetNextWindowSize({ 960,860 }); // ������������� ������ ��� ���� � ��������
        ImGui::SetNextWindowPos({ 960,220 }); // ������������� ������� ��� ���� � ��������
    
        ImGui::Begin("My Window", 0, flagsForWindows); // ������� ���� � ���������� �����������
        
        ImGui::End(); // ������� ����
    
        ImGui::SetNextWindowPos({ 0,220 }); // ������������� ������� ��� ������������ ���� ��� �������
        ImGui::SetNextWindowSize({ 960,860 }); // ������������� ������ ��� ������������ ���� ��� �������
        ImGui::Begin("Table", 0, flagsForWindows); // ������� ���� � ��������� �����������
    
        ImGui::End(); // ������� ����
    }

    void MakeWindowForInput(ImGuiWindowFlags& flagsForWindows) { // ������� ��� �������� ���� ��� ����� ���� � �������-������
        
        ImGui::SetNextWindowPos({ 351,0 }); // ������������� ������� ��� �������� ����
        ImGui::SetNextWindowSize({ 330,220}); // ������������ ������ ��� �������� ���� = 330*220
        
        ImGui::Begin("InPut", 0 , flagsForWindows); // ������� ���� � ���������� �����������
        
        createInputH(flagsForWindows); // �������� ������� ��� �������� ���� ��� ����� ����
        createInputE_local(flagsForWindows); // ������� ������� ��� �������� ���� ��� ����� �������-�������, �������������� u ������ � �����
        createInputE_right(flagsForWindows); // �������� ������� ��� �������� ���� ��� ����� �������-������, �������������� x ������
       
        ImGui::End(); // ������� ����
    }

    void createInputE_local(ImGuiWindowFlags flagsForWindows) { // ������� ��� �������� ��������� ���� ��� ����� e_local
        
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ���������, ����� �� ���� ������� ����

        ImGui::BeginChild("Input E local", { 300,60 }, true, flagsForWindows); // ������� �������� ����, � ��������� ��������� 300*60

        ImGui::SeparatorText("Input E local"); // �������������� ����� Input E local

        static double input_e_local = rk.get_E_local(); // ���������� input_e_local �� ������ RK
        static double max = 1.0; // max �������� ��� input_e_local
        static double min = 0.000000001; // min �������� ��� input_e_local
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_local, 0.0005f, &min, &max, "%.10f", ImGuiSliderFlags_AlwaysClamp)) // ������� ���� ��� ����� ������ � ���� ���� ������� �����-�� ������, ��
            rk.set_E_local(input_e_local); // ��������������� �������� ��� m_E_check_up � ������ RK

        ImGui::EndChild(); // �������� ���� ���������
    }

    void createInputE_right(ImGuiWindowFlags flagsForWindows) { // ������� ��� �������� ��������� ���� ��� ����� e_right - ������� ��� x ������

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ���������, ����� �� ���� ������� ����

        ImGui::BeginChild("Input E right", { 300,60 }, true, flagsForWindows); // ������� �������� ����, � ��������� ��������� 300*60

        ImGui::SeparatorText("Input E right"); // �������������� ����� Input E right

        static double input_e_right = rk.get_E_right(); // ���������� input_e_right �� ������ RK
        static double max = 1.0; // max �������� ��� input_e_right
        static double min = 0.000000001; // min �������� ��� input_e_right

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_right, 0.0005f, &min, &max, "%.10f", ImGuiSliderFlags_AlwaysClamp)) // ������� ���� ��� ����� ������ � ���� ���� ������� �����-�� ������, ��
            rk.set_E_right(input_e_right); // ��������������� �������� ��� m_E_check_right � ������ RK

        ImGui::EndChild(); // �������� ���� ���������
    }

    void createInputH(ImGuiWindowFlags flagsForWindows) { // ������� ��� �������� ��������� ���� ��� ����� h - ���

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ���������, ����� �� ���� ������� ����

        ImGui::BeginChild("Input H", { 300,60 }, true, flagsForWindows); // ������� �������� ����, � ��������� ��������� 300*60

        ImGui::SeparatorText("Input H"); // �������������� ����� Input H

        static double input_h = rk.get_Start_H(); // ���������� input_h �� ������ RK
        static double max = 1.0; // max �������� ��� input_h
        static double min = 0.0000001; // min �������� ��� input_h

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_h, 0.0005f, &min, &max, "%.10f", ImGuiSliderFlags_AlwaysClamp)) // ������� ���� ��� ����� ������ � ���� ���� ������� �����-�� ������, ��
            rk.set_Start_H(input_h); // ��������������� �������� ��� m_h � ������ RK

        ImGui::EndChild(); // �������� ���� ���������
    }

    void createButton(double& x0, double& u0, ImGuiWindowFlags& flagsForWindows, int& item_current_idx, bool& isConstH, bool& isPressed, double& u_0, int& param_a, int& param_b) { // ������� ��� �������� ������, ����� ������ ������ ������� ��-4

        ImGui::SetNextWindowPos({ 0,181 }); // ������������� ������� ��� ������������ ����
        ImGui::SetNextWindowSize({ 350,39 }); // ������������� ������ ��� ������������ ����

        ImGui::Begin("Button", 0, flagsForWindows); // ������� ���� � ��������� �����������

        //bool isPressed = false; // ������� ����������, ������������ ���� �� ������ ������

        if (ImGui::Button("START", { 100,20 })) { // ������� ������ � ��������� 100*20
            rk.clear_data(); // ������� �������� �������� � ������� ������
            if (item_current_idx == 2)
                rk.run_func(x0, u0, (Task)item_current_idx, isConstH, u_0, param_a,param_b);
            else
                rk.run_func(x0, u0, (Task)item_current_idx, isConstH); // ������� ������ ���� � ����� (x0,u0)
            isPressed = true; // ������������ ��� ���������� isPressed, ��� ��� ���� ������
        }

        ImGui::SameLine(); // � ��� �� ������
        ImGui::Text("Start Calc"); // ����� Start Calc

        ImGui::End(); // ������� ����
        
        //return isPressed; // ��������� �������� isPressed
    }

    void createGraph(ImGuiWindowFlags& flagsForWindows, int& item_current_idx) { // ������� ��� ��������� �������

        std::vector<std::pair<double, double>> data_for_analytical_solution = rk.getCoordsForAnalytical_Solution(); // ������� ������ ����� ��� �������������� �������, ���� �������������� ������� ���, �� ��� ����� ������ �������

        const size_t size_of_analytical_solution = data_for_analytical_solution.size(); // �������� ������ ������� ����� ��� �������������� �������
        double* x_of_analytical_solution = new double[size_of_analytical_solution]; // ������� ������������ ������ x-����� �������������� �������. ������ ����� ������� ����� ������� ������ ����� ��� �������������� �������
        double* y_of_analytical_solution = new double[size_of_analytical_solution]; // ������� ������������ ������ y-����� �������������� �������. ������ ����� ������� ����� ������� ������ ����� ��� �������������� �������

        for (size_t count = 0; count < size_of_analytical_solution; ++count) { // ���� ��� ����������� ������ � ������ �� ������� ����� ��� �������������� ������� ("������� ������� �� �����")
            x_of_analytical_solution[count] = (data_for_analytical_solution[count].first);
            y_of_analytical_solution[count] = (data_for_analytical_solution[count].second);
        }

        std::vector<std::tuple<double, double, double>> data_for_numerical_solution = rk.getCoords(); // ������� ������ ����� ��� ���������� ������� ������ ����

        const size_t size_of_numerical_solution = data_for_numerical_solution.size(); // �������� ������ ������� ����� ��� ���������� �������
        double* x_of_numerical_solution = new double[size_of_numerical_solution]; // ������� ������������ ������ x-����� ���������� �������. ������ ����� ������� ����� ������� ������ ����� ��� ���������� �������
        double* y_of_numerical_solution = new double[size_of_numerical_solution]; // ������� ������������ ������ y-����� ���������� �������. ������ ����� ������� ����� ������� ������ ����� ��� ���������� �������
        double* y__of_numerical_solution = new double[size_of_numerical_solution];


        for (int count = 0; count < size_of_numerical_solution; ++count) { // ���� ��� ����������� ������ � ������ �� ������� ����� ��� ���������� �������
            x_of_numerical_solution[count] = std::get<0>(data_for_numerical_solution[count]);
            y_of_numerical_solution[count] = std::get<1>(data_for_numerical_solution[count]);
            if (item_current_idx == 2)
                y__of_numerical_solution[count] = std::get<2>(data_for_numerical_solution[count]);
        }

        ImGui::SetNextWindowSize({ 960,860 }); // ������������� ������ ��� ���� � ��������
        ImGui::SetNextWindowPos({ 960,220 }); // ������������� ������� ��� ���� � ��������

        ImGui::Begin("Graph", 0, flagsForWindows); // ������� ���� � ���������� �����������

        if (ImPlot::BeginPlot("Solution schedule", "x", "u", { 940,780 })) { // ������������ ������
            if (item_current_idx == 0) // ���� � ��� �������� ������, �� ���� ���������� ��� ������������� �������
                ImPlot::PlotLine("Analytical solution graph", x_of_analytical_solution, y_of_analytical_solution, size_of_analytical_solution); // ������������ �����
            else if (item_current_idx == 2)
                ImPlot::PlotLine("Phase portrait", y_of_numerical_solution, y__of_numerical_solution, size_of_analytical_solution); // ������������ �����
            ImPlot::PlotLine("Numerical solution graph", x_of_numerical_solution, y_of_numerical_solution, size_of_numerical_solution); // ������������ �����
            ImPlot::EndPlot(); // ����������� ��������� �������
        }
        ImGui::End(); // ������� ����
    }

    void createTable(ImGuiWindowFlags& flagsForWindows, int& item_current_idx, bool& isConstH) {
        std::vector<double> vector_of_h = rk.getH(); // ������� ������ �������� �����
        std::vector<std::tuple<double, double, double>> data_for_numerical_solution = rk.getCoords(); // ������� ������ ���������, ���������� ��������� ��������
        std::vector<double> twice_half_h_u = rk.getVectorOfTwiceHalfHU(); // ������ ��������� v^i
        std::vector<double> difference_of_v = rk.getVectorOfDifferenceOfV(); // ������ vi-v^i
        std::vector<double> vector_S = rk.getVectorOfS(); // ������ ���

        std::vector<double> difference_of_u;
        std::vector<double> coordsOfU;

        int size = 9;

        if (isConstH)
            size = 3; // ������ 3 ��� �������

        if (item_current_idx == 0) {
            difference_of_u = rk.getVectorOfDifferenceU();
            coordsOfU = rk.getCoordsOfU();
            size += 2;
        }

        std::vector<int> C1 = rk.getC1(); // ������ ������� ����� 
        std::vector<int> C2 = rk.getC2(); // ������ ��������� �����

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg; // ��������� ��� �������

        ImGui::SetNextWindowPos({ 0,220 }); // ������������� ������� ��� ������������ ���� ��� �������
        ImGui::SetNextWindowSize({ 960,790 }); // ������������� ������ ��� ������������ ���� ��� �������
        ImGui::Begin("Table", 0 , flagsForWindows); // ������� ���� � ��������� �����������
        
        if (ImGui::BeginTable("table1", size, flags)) // ������� ������� � ��������� �����������
        {
            if (true)
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Xi");
                ImGui::TableSetupColumn("Vi");
                if (!isConstH) {
                    ImGui::TableSetupColumn("V^i");
                    ImGui::TableSetupColumn("Vi-V^i");
                    ImGui::TableSetupColumn("OLP");
                    ImGui::TableSetupColumn("Hi");
                    ImGui::TableSetupColumn("divisions", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("doublings", ImGuiTableColumnFlags_WidthFixed);
                }
                if (item_current_idx == 0) {
                    ImGui::TableSetupColumn("Ui");
                    ImGui::TableSetupColumn("|Ui-Vi|");
                }
                ImGui::TableHeadersRow();
            }

            for (int row = 0; row < data_for_numerical_solution.size(); row++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", row);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.12lf", std::get<0>(data_for_numerical_solution[row]));
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.12lf", std::get<1>(data_for_numerical_solution[row]));
                if (!isConstH) {
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%.12lf", twice_half_h_u[row]);
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%.12lf", difference_of_v[row]);
                    ImGui::TableSetColumnIndex(5);
                    ImGui::Text("%.16lf", vector_S[row]);
                    ImGui::TableSetColumnIndex(6);
                    ImGui::Text("%.12lf", vector_of_h[row]);
                    ImGui::TableSetColumnIndex(7);
                    ImGui::Text("%d", C1[row]);
                    ImGui::TableSetColumnIndex(8);
                    ImGui::Text("%d", C2[row]);
                }
                if (item_current_idx == 0 && !isConstH) {
                    ImGui::TableSetColumnIndex(9);
                    ImGui::Text("%.12lf", coordsOfU[row]);
                    ImGui::TableSetColumnIndex(10);
                    ImGui::Text("%.12lf", difference_of_u[row]);
                }
                else if (item_current_idx == 0 && isConstH) {
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%.12lf", coordsOfU[row]);
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%.12lf", difference_of_u[row]);
                }
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