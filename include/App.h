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
            static ImGuiWindowFlags flagsForWindows = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

            MakeWindowForInput(flagsForWindows); // �������� ������� ��� �������� ���� ��� ����� ���� � �������-������
            MakeWindowForTasks(item_current_idx, flagsForWindows); // �������� ������� �� �������� ���� � ����������� ������� �����
            MakeWindowForInputInitial�onditions(x0,u0, flagsForWindows); // �������� ������� �� �������� ���� ��� ����� ��������� �������

            std::cout << "You selected: " << item_current_idx << std::endl;
            std::cout << "X: " << x0 << std::endl;
            std::cout << "U: " << u0 << std::endl;

            static int check;
            
            if (createButton(x0,u0, flagsForWindows, item_current_idx)) {
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

        ImGui::SeparatorText("Input X"); // �������������� ����� � ����

        ImGui::InputDouble(" ", &x0, 0.01f, 1.0f, "%.8f"); // ���� ��� ����� double x

        ImGui::EndChild(); // ������� �������� ����
    }

    void createInputU(double& u0, ImGuiWindowFlags flagsForWindows) { // ������� ���� ��� ����� U
        
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ��������� ��� ����, ����� �� ���� ������� ����

        ImGui::BeginChild("Input U", { 300,60 }, true, flagsForWindows); // ������� ������� ���� � �������� 300*60 � �����������-flags

        ImGui::SeparatorText("Input U"); // �������������� ����� � ����

        ImGui::InputDouble(" ", &u0, 0.01f, 1.0f, "%.8f"); // ���� ��� ����� double u

        ImGui::EndChild();// ������� �������� ����
    }

    void MakeWindowForTasks(int& item_current_idx, ImGuiWindowFlags& flagsForWindows) { // ������� ���� ��� ������ ����������� ������. ��� item_current_idx - �������� ������ �� ���������� �� ��������� �����, ���������� �� ��������� ������, ���� 0 - ��������, 1 - ������ ������, 2 - ������ ������
        
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
                if (ImGui::Selectable(items[n], is_selected)) // ������������� ������ n ��� item_current_idx, ���� ������� ������ n
                    item_current_idx = n;

                if (is_selected) // ���� is_selected
                    ImGui::SetItemDefaultFocus(); // ������������� ����� �� ��������� ������
            }
            ImGui::EndCombo(); // ������� ����������� ����
        }
         
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
        static double max2 = 1.0; // max �������� ��� input_e_local
        static double min2 = 0.000000001; // min �������� ��� input_e_local
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_local, 0.0005f, &min2, &max2, "%.10f")) // ������� ���� ��� ����� ������ � ���� ���� ������� �����-�� ������, ��
            rk.set_E_local(input_e_local); // ��������������� �������� ��� m_E_check_up � ������ RK

        ImGui::EndChild(); // �������� ���� ���������
    }

    void createInputE_right(ImGuiWindowFlags flagsForWindows) { // ������� ��� �������� ��������� ���� ��� ����� e_right - ������� ��� x ������

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // �������������� ���������, ����� �� ���� ������� ����

        ImGui::BeginChild("Input E right", { 300,60 }, true, flagsForWindows); // ������� �������� ����, � ��������� ��������� 300*60

        ImGui::SeparatorText("Input E right"); // �������������� ����� Input E right

        static double input_e_right = rk.get_E_right(); // ���������� input_e_right �� ������ RK
        static double max1 = 1.0; // max �������� ��� input_e_right
        static double min1 = 0.000000001; // min �������� ��� input_e_right

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_right, 0.0005f, &min1, &max1, "%.10f")) // ������� ���� ��� ����� ������ � ���� ���� ������� �����-�� ������, ��
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

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_h, 0.0005f, &min, &max, "%.10f")) // ������� ���� ��� ����� ������ � ���� ���� ������� �����-�� ������, ��
            rk.set_Start_H(input_h); // ��������������� �������� ��� m_h � ������ RK

        ImGui::EndChild(); // �������� ���� ���������
    }

    bool createButton(double& x0, double& u0, ImGuiWindowFlags& flagsForWindows, int& item_current_idx) { // ������� ��� �������� ������, ����� ������ ������ ������� ��-4

        ImGui::SetNextWindowPos({ 0,181 }); // ������������� ������� ��� ������������ ����
        ImGui::SetNextWindowSize({ 350,39 }); // ������������� ������ ��� ������������ ����

        ImGui::Begin("Button", 0, flagsForWindows); // ������� ���� � ��������� �����������

        bool isPressed = false; // ������� ����������, ������������ ���� �� ������ ������

        /*
            ��������:
                �� ���������, ������ ��� ��� ��������. � ����� ��� ���������� �� ����������. ���� �� ����� ��� ����, �� �� ���������
                ���� ��������� ������� ������ ���, ����� �� ���������� �� �������
                ���� ������ ��������� ���
        */

        if (ImGui::Button("START", { 100,20 })) { // ������� ������ � ��������� 100*20
            rk.clear_data();
            rk.test_func(x0, u0, (Task)item_current_idx);
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
        ImGuiWindowFlags flagsW = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowPos({ 0,220 });
        ImGui::SetNextWindowSize({ 960,860 });
        ImGui::Begin("Table", 0 ,flagsW);
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

������� ���� ������ ���� ��� � ���������� � �������
������� ���� ��� ����� ������
������� ����-������ ���� � ����������� �� ������������ ����

*/