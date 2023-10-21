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

            MakeWindowForInput(flagsForWindows); // вызываем функцию для создания окна для ввода шага и епсилон-границ
            MakeWindowForTasks(item_current_idx, flagsForWindows); // вызываем функцию по созданию окна с всплывающим списком задач
            MakeWindowForInputInitialСonditions(x0,u0, flagsForWindows); // вызываем функцию по созданию окна для ввода начальных условий

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

    void MakeWindowForInputInitialСonditions(double& x0, double& u0, ImGuiWindowFlags& flagsForWindows) { // Создаем окно для ввода начальных условий
        
        ImGui::SetNextWindowPos({ 0,41 }); // устанавливаем позицию окна
        ImGui::SetNextWindowSize({ 350,140 }); // устанавливаем размер окна

        ImGui::Begin("Iput start", 0, flagsForWindows); // создание окна

        createInputX(x0, flagsForWindows); // вызываем функцию по созданию дочернего окна для ввода координаты X
        createInputU(u0, flagsForWindows); // вызываем функцию по созданию дочернего окна для ввода координаты U

        ImGui::End(); // удаляем окно
    }

    void createInputX(double& x0, ImGuiWindowFlags flagsForWindows) { // Создаем окно для ввода Х
        
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка для окна, чтобы не было заднего фона

        ImGui::BeginChild("Input X", { 300,60 }, true, flagsForWindows); // Создаем дочернеё окно с размером 300*60 и настройками-flags

        ImGui::SeparatorText("Input X"); // дополнительный текст в окне

        ImGui::InputDouble(" ", &x0, 0.01f, 1.0f, "%.8f"); // поле для ввода double x

        ImGui::EndChild(); // удаляем дочернее окно
    }

    void createInputU(double& u0, ImGuiWindowFlags flagsForWindows) { // Создаем окно для ввода U
        
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка для окна, чтобы не было заднего фона

        ImGui::BeginChild("Input U", { 300,60 }, true, flagsForWindows); // Создаем дочернеё окно с размером 300*60 и настройками-flags

        ImGui::SeparatorText("Input U"); // дополнительный текст в окне

        ImGui::InputDouble(" ", &u0, 0.01f, 1.0f, "%.8f"); // поле для ввода double u

        ImGui::EndChild();// удаляем дочернее окно
    }

    void MakeWindowForTasks(int& item_current_idx, ImGuiWindowFlags& flagsForWindows) { // создаем окно для выбора необходимой задачи. Тут item_current_idx - получает ссылку на переменную из основного цикла, отвечающую за выбранную задачу, если 0 - тестовая, 1 - первая задача, 2 - вторая задача
        
        ImGui::SetNextWindowPos({ 0,0 }); // устанавливаем позицию в левом-верхнем углу (0,0)
        ImGui::SetNextWindowSize({ 350,40 }); // устанавливаем размеры для окна 350*40

        ImGui::Begin("Select Task", 0, flagsForWindows); // создаем окно с выбранными настройками flagsForWindows

        const char* items[] = { "Test", "First", "Second" }; // создаем массив с названием задача
        const char* combo_preview_value = items[item_current_idx];  // отображает предыдущую выбранную задачу
        
        if (ImGui::BeginCombo("Select the task", combo_preview_value)) // создаем выплывающий список из задача
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) // проходи циклом по массиву задач
            {
                const bool is_selected = (item_current_idx == n); // если задача в массиве, совпадающая с item_current_idx -> true
                if (ImGui::Selectable(items[n], is_selected)) // устанавливаем индекс n для item_current_idx, если выбрана задача n
                    item_current_idx = n;

                if (is_selected) // если is_selected
                    ImGui::SetItemDefaultFocus(); // устанавливаем фокус на выбранную задачу
            }
            ImGui::EndCombo(); // удаляем выплывающее окно
        }
         
        ImGui::End(); // удаляем окно
    }

    void MakeWindowForInput(ImGuiWindowFlags& flagsForWindows) { // функцию для создания окна для ввода шага и епсилон-границ
        
        ImGui::SetNextWindowPos({ 351,0 }); // устанавливаем позицию для будущего окна
        ImGui::SetNextWindowSize({ 330,220}); // устанвливаем размер для будущего окна = 330*220
        
        ImGui::Begin("InPut", 0 , flagsForWindows); // создаем окно с выбранными настройками
        
        createInputH(flagsForWindows); // вызываем функцию для создания окна для ввода шага
        createInputE_local(flagsForWindows); // вызыаем функцию для создания окна для ввода епсилон-границы, ограничивающая u сверху и снизу
        createInputE_right(flagsForWindows); // вызываем функцию для создания онка для ввода епсилон-грницы, ограничивающая x справа
       
        ImGui::End(); // удаляем окно
    }

    void createInputE_local(ImGuiWindowFlags flagsForWindows) { // функция для создания дочернего окна для ввода e_local
        
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка, чтобы не было заднего фона

        ImGui::BeginChild("Input E local", { 300,60 }, true, flagsForWindows); // создаем дочернее окно, с заданными размерами 300*60

        ImGui::SeparatorText("Input E local"); // дополнительный текст Input E local

        static double input_e_local = rk.get_E_local(); // переменная input_e_local из класса RK
        static double max2 = 1.0; // max значение для input_e_local
        static double min2 = 0.000000001; // min значение для input_e_local
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_local, 0.0005f, &min2, &max2, "%.10f")) // создает поле для ввода данных и если были введены какие-то данные, то
            rk.set_E_local(input_e_local); // устанавливается значение для m_E_check_up в классе RK

        ImGui::EndChild(); // дочернее окно удаляется
    }

    void createInputE_right(ImGuiWindowFlags flagsForWindows) { // функция для создания дочернего окна для ввода e_right - граница для x справа

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка, чтобы не было заднего фона

        ImGui::BeginChild("Input E right", { 300,60 }, true, flagsForWindows); // создаем дочернее окно, с заданными размерами 300*60

        ImGui::SeparatorText("Input E right"); // дополнительный текст Input E right

        static double input_e_right = rk.get_E_right(); // переменная input_e_right из класса RK
        static double max1 = 1.0; // max значение для input_e_right
        static double min1 = 0.000000001; // min значение для input_e_right

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_right, 0.0005f, &min1, &max1, "%.10f")) // создает поле для ввода данных и если были введены какие-то данные, то
            rk.set_E_right(input_e_right); // устанавливается значение для m_E_check_right в классе RK

        ImGui::EndChild(); // дочернее окно удаляется
    }

    void createInputH(ImGuiWindowFlags flagsForWindows) { // функция для создания дочернего окна для ввода h - шаг

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка, чтобы не было заднего фона

        ImGui::BeginChild("Input H", { 300,60 }, true, flagsForWindows); // создаем дочернее окно, с заданными размерами 300*60

        ImGui::SeparatorText("Input H"); // дополнительный текст Input H

        static double input_h = rk.get_Start_H(); // переменная input_h из класса RK
        static double max = 1.0; // max значение для input_h
        static double min = 0.0000001; // min значение для input_h

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_h, 0.0005f, &min, &max, "%.10f")) // создает поле для ввода данных и если были введены какие-то данные, то
            rk.set_Start_H(input_h); // устанавливается значение для m_h в классе RK

        ImGui::EndChild(); // дочернее окно удаляется
    }

    bool createButton(double& x0, double& u0, ImGuiWindowFlags& flagsForWindows, int& item_current_idx) { // функция для создания кнопки, чтобы начать работу методом РК-4

        ImGui::SetNextWindowPos({ 0,181 }); // устанавливаем позицию для создаваемого окна
        ImGui::SetNextWindowSize({ 350,39 }); // устанавливаем размер для создаваемого окна

        ImGui::Begin("Button", 0, flagsForWindows); // Создаем окно с заданными параметрами

        bool isPressed = false; // создаем переменную, показывающую была ли нажата кнопка

        /*
            Проблема:
                По программе, каждый раз шаг меняется. В конце шаг отличается от начального. Если мы ввели шаг сами, то он поменялся
                Надо почистить вектора каждый раз, чтобы не захламлять их мусором
                Идея ввести начальный шаг
        */

        if (ImGui::Button("START", { 100,20 })) { // создаем кнопку с размерами 100*20
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

Сделать иниц флагов один раз и передавать в функции
Создать окно для ввода границ
Сделать авто-ресайз окон в зависимости от создаваемого окна

*/