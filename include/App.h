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
    ImGuiIO io;
    RK rk;
public:
    App() 
    {
    }
    ~App() {
        ImGui::SFML::Shutdown();
    }

	void init() {
        m_window.create(sf::VideoMode(1920, 1080, 32), "Cauchy problem");
        m_window.setFramerateLimit(60);
        ImGui::SFML::Init(m_window);
        io = ImGui::GetIO();
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
            static double param_a = 1;
            static double param_b = 2;
            static ImGuiWindowFlags flagsForWindows = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
            static bool isConstH = false;
            static bool isPressed = false;
            static bool isNotice = false;
            const char* names[3] = { "X", "U", "U'" };
            static bool opened[3] = { true , true, false };

            MakeWindowForInput(flagsForWindows); // вызываем функцию для создания окна для ввода шага и епсилон-границ
            MakeWindowForTasks(item_current_idx, flagsForWindows, isPressed); // вызываем функцию по созданию окна с всплывающим списком задач
            MakeWindowForInputInitialСonditions(x0,u0, flagsForWindows); // вызываем функцию по созданию окна для ввода начальных условий
            makeWindowForInputBorder(flagsForWindows);
            makeWindowForCheckBoxAboutE(flagsForWindows, isConstH, isPressed);
            createButton(x0, u0, flagsForWindows, item_current_idx, isConstH, isPressed, isNotice, u_0, param_a, param_b);
            makeWindowForInfo(flagsForWindows, isPressed, item_current_idx, isConstH);
            if (item_current_idx == 2) {
                makeWindowForSelectAxles(flagsForWindows, names, opened);
                makeWindowForSecondTaskInput(flagsForWindows, isPressed, param_a, param_b, u_0);
            }


            if (isPressed) { // если кнопка была нажата
                createGraph(flagsForWindows, item_current_idx, names, opened); // рисуем график
                createTable(flagsForWindows, item_current_idx, isConstH); // создаем таблицу
            }
            else
                clearTableAndGraph(flagsForWindows);

            if (isNotice)
                makeWindowNoticeAboutCalculating(isNotice);

            m_window.clear();
            render();
        }
    }

    void makeWindowForSelectAxles(ImGuiWindowFlags& flagsForWindows, const char* names[3], bool (&opened)[3]) {

        ImGui::SetNextWindowPos({1331,160});
        ImGui::SetNextWindowSize({ 607,60 });

        ImGui::Begin("ASDASD",0, flagsForWindows);

        ImGui::Text("Set axles");

        int counter = 0;

        for (size_t count = 0; count < IM_ARRAYSIZE(opened); ++count) {
            if (opened[count])
                counter += 1;
            else counter -= 1;
        }

        for (size_t count = 0; count < IM_ARRAYSIZE(opened); ++count)
        {
            if (count > 0)
                ImGui::SameLine();
            if (ImGui::Checkbox(names[count], &opened[count])) {
                ++counter;
                if (counter > 1)
                    opened[count] = false;
            }
        }

        ImGui::End();
    }

    void makeWindowForSecondTaskInput(ImGuiWindowFlags& flagsForWindows, bool& isPressed, double& param_a, double& param_b, double& u_0) {
        ImGui::SetNextWindowPos({ 1013,0 }); // устанавливаем позицию для будущего окна
        ImGui::SetNextWindowSize({ 317,220 });

        ImGui::Begin("Input for 2-nd task", 0, flagsForWindows);

        createInputParamA(flagsForWindows, param_a);
        createInputParamB(flagsForWindows, param_b);
        createInputStartU_0(flagsForWindows, u_0);

        ImGui::End();
    }

    void createInputStartU_0(ImGuiWindowFlags flagsForWindows, double& u_0) {

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка для окна, чтобы не было заднего фона

        ImGui::BeginChild("Input X", { 300,60 }, true, flagsForWindows); // Создаем дочернеё окно с размером 300*60 и настройками-flags

        ImGui::SeparatorText("Input U'0"); // дополнительный текст в окне

        ImGui::InputDouble(" ", &u_0, 0.01f, 1.0f, "%.8f"); // поле для ввода double x

        ImGui::EndChild(); // удаляем дочернее окно
    }

    void createInputParamA(ImGuiWindowFlags flagsForWindows, double& param_a) {

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка для окна, чтобы не было заднего фона

        ImGui::BeginChild("Input param A", { 300,60 }, true, flagsForWindows); // Создаем дочернеё окно с размером 300*60 и настройками-flags

        ImGui::SeparatorText("Input param A"); // дополнительный текст в окне

        ImGui::InputDouble(" ", &param_a, 0.01f, 1.0f, "%.8f"); // поле для ввода double x


        ImGui::EndChild(); // удаляем дочернее окно
    }

    void createInputParamB(ImGuiWindowFlags flagsForWindows, double& param_b) {

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка для окна, чтобы не было заднего фона

        ImGui::BeginChild("Input param B", { 300,60 }, true, flagsForWindows); // Создаем дочернеё окно с размером 300*60 и настройками-flags

        ImGui::SeparatorText("Input param B"); // дополнительный текст в окне

        ImGui::InputDouble(" ", &param_b, 0.01f, 1.0f, "%.8f"); // поле для ввода double x

        //ImGui::InputDouble(" ", &x0, 0.01f, 1.0f, "%.8f");

        ImGui::EndChild(); // удаляем дочернее окно
    }

    void makeWindowForInfo(ImGuiWindowFlags& flagsForWindows, bool& isPressed, int& item_current_idx, bool& isConstH) {

        if (item_current_idx == 2) {
            ImGui::SetNextWindowPos({ 1331,0 }); // устанавливаем позицию для будущего окна
            ImGui::SetNextWindowSize({ 607,160 });
        }
        else {
            ImGui::SetNextWindowPos({ 1013,0 }); // устанавливаем позицию для будущего окна
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

                double diff_of_b_and_x_last = rk.getBorder().second - std::get<0>(rk.getCoords().back());
                ImGui::Text("b - x_n: %.24lf", diff_of_b_and_x_last);

                if (item_current_idx == 0) {
                    std::vector<double> diff = rk.getVectorOfDifferenceU();
                    double maxDiff = *max_element(diff.begin(), diff.end());
                    ImGui::Text("MAX |Ui-Vi|: %.24lf", maxDiff);
                }

                int countOfDiv = rk.getCountOfDivisions();
                int countOfMult = rk.getCountOfDoublings();

                ImGui::Text("Count of divisions: %d", countOfDiv);
                ImGui::Text("Count of doublings: %d", countOfMult);
            }
        }

        ImGui::End();
    }

    void makeWindowForCheckBoxAboutE(ImGuiWindowFlags& flagsForWindows, bool& isConstH, bool& isPressed) {

        ImGui::SetNextWindowPos({ 682,141 }); // устанавливаем позицию для будущего окна
        ImGui::SetNextWindowSize({ 330,79 }); 

        ImGui::Begin("Checkbox", 0, flagsForWindows); // создаем окно с выбранными 

        if (ImGui::Checkbox("CONST H", &isConstH))
            isPressed = false;

        int N = rk.getN();

        if (ImGui::InputInt("Count of N", &N, 1, 10, 0))
            rk.setN(N);

        ImGui::End();

    }

    void makeWindowForInputBorder(ImGuiWindowFlags& flagsForWindows) {

        ImGui::SetNextWindowPos({ 682,0 }); // устанавливаем позицию для будущего окна
        ImGui::SetNextWindowSize({ 330,140 }); // устанвливаем размер для будущего окна = 330*140

        ImGui::Begin("Input Border", 0, flagsForWindows); // создаем окно с выбранными настройками

        createInputBorderA(flagsForWindows);
        createInputBorderB(flagsForWindows);

        ImGui::End(); // удаляем окно
    }

    void createInputBorderA(ImGuiWindowFlags flagsForWindows) {

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка для окна, чтобы не было заднего фона

        ImGui::BeginChild("Input A", { 300,60 }, true, flagsForWindows); // Создаем дочернеё окно с размером 300*60 и настройками-flags

        ImGui::SeparatorText("Input A"); // дополнительный текст в окне

        double a = rk.getBorder().first;
        double max_for_a = rk.getBorder().second - 0.0001;
        static double min_for_a = -INFINITY;

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &a, 0.0005f, &min_for_a, &max_for_a, "%.10f", ImGuiSliderFlags_AlwaysClamp)) 
            rk.setBorderA(a);

        ImGui::EndChild();
    }

    void createInputBorderB(ImGuiWindowFlags flagsForWindows) {
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка для окна, чтобы не было заднего фона

        ImGui::BeginChild("Input B", { 300,60 }, true, flagsForWindows); // Создаем дочернеё окно с размером 300*60 и настройками-flags

        ImGui::SeparatorText("Input B"); // дополнительный текст в окне

        double b = rk.getBorder().second;
        static double max_for_b = +INFINITY;
        double min_for_b = rk.getBorder().first + 0.0001;

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &b, 0.0005f, &min_for_b, &max_for_b, "%.10f", ImGuiSliderFlags_AlwaysClamp))
            rk.setBorderB(b);

        ImGui::EndChild(); // удаляем дочернее окно
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

        ImGui::SeparatorText("Input X0"); // дополнительный текст в окне

        ImGui::InputDouble(" ", &x0, 0.01f, 1.0f, "%.8f"); // поле для ввода double x

        ImGui::EndChild(); // удаляем дочернее окно
    }

    void createInputU(double& u0, ImGuiWindowFlags flagsForWindows) { // Создаем окно для ввода U
        
        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка для окна, чтобы не было заднего фона

        ImGui::BeginChild("Input U", { 300,60 }, true, flagsForWindows); // Создаем дочернеё окно с размером 300*60 и настройками-flags

        ImGui::SeparatorText("Input U0"); // дополнительный текст в окне

        ImGui::InputDouble(" ", &u0, 0.01f, 1.0f, "%.8f"); // поле для ввода double u

        ImGui::EndChild();// удаляем дочернее окно
    }

    void MakeWindowForTasks(int& item_current_idx, ImGuiWindowFlags& flagsForWindows, bool& isPressed) { // создаем окно для выбора необходимой задачи. Тут item_current_idx - получает ссылку на переменную из основного цикла, отвечающую за выбранную задачу, если 0 - тестовая, 1 - первая задача, 2 - вторая задача
        
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
                if (ImGui::Selectable(items[n], is_selected)) { // устанавливаем индекс n для item_current_idx, если выбрана задача n
                    item_current_idx = n;
                    isPressed = false;
                }
                if (is_selected) // если is_selected
                    ImGui::SetItemDefaultFocus(); // устанавливаем фокус на выбранную задачу
            }
            ImGui::EndCombo(); // удаляем выплывающее окно
        }
         
        ImGui::End(); // удаляем окно
    }

    void clearTableAndGraph(ImGuiWindowFlags& flagsForWindows) {
        ImGui::SetNextWindowSize({ 960,860 }); // устанавливаем размер для окна с графиком
        ImGui::SetNextWindowPos({ 960,220 }); // устанавливаем позицию для окна с графиком
    
        ImGui::Begin("My Window", 0, flagsForWindows); // создаем окно с выбранными настройками
        
        ImGui::End(); // Удаляем окно
    
        ImGui::SetNextWindowPos({ 0,220 }); // устанавливаем позицию для создаваемого окна для таблицы
        ImGui::SetNextWindowSize({ 960,860 }); // устанавливаем размер для создаваемого окна для таблицы
        ImGui::Begin("Table", 0, flagsForWindows); // создаем окно с заданными настройками
    
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
        static double max = 1.0; // max значение для input_e_local
        static double min = 0.000000001; // min значение для input_e_local
        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_local, 0.0005f, &min, &max, "%.10f", ImGuiSliderFlags_AlwaysClamp)) // создает поле для ввода данных и если были введены какие-то данные, то
            rk.set_E_local(input_e_local); // устанавливается значение для m_E_check_up в классе RK

        ImGui::EndChild(); // дочернее окно удаляется
    }

    void createInputE_right(ImGuiWindowFlags flagsForWindows) { // функция для создания дочернего окна для ввода e_right - граница для x справа

        flagsForWindows |= ImGuiWindowFlags_NoBackground; // дополнительная настройка, чтобы не было заднего фона

        ImGui::BeginChild("Input E right", { 300,60 }, true, flagsForWindows); // создаем дочернее окно, с заданными размерами 300*60

        ImGui::SeparatorText("Input E right"); // дополнительный текст Input E right

        static double input_e_right = rk.get_E_right(); // переменная input_e_right из класса RK
        static double max = 1.0; // max значение для input_e_right
        static double min = 0.000000001; // min значение для input_e_right

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_e_right, 0.0005f, &min, &max, "%.10f", ImGuiSliderFlags_AlwaysClamp)) // создает поле для ввода данных и если были введены какие-то данные, то
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

        if (ImGui::DragScalar(" ", ImGuiDataType_Double, &input_h, 0.0005f, &min, &max, "%.10f", ImGuiSliderFlags_AlwaysClamp)) // создает поле для ввода данных и если были введены какие-то данные, то
            rk.set_Start_H(input_h); // устанавливается значение для m_h в классе RK

        ImGui::EndChild(); // дочернее окно удаляется
    }

    void makeWindowNoticeAboutCalculating(bool& isNotice) {

        ImGuiWindowFlags flagsForWindows = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse ;

        ImGui::SetNextWindowSize({ 300,50 });
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 150, io.DisplaySize.y * 0.5f - 25));

        if (isNotice)
        {
            ImGui::OpenPopup("Notice");

            if (ImGui::BeginPopupModal("Notice", &isNotice, flagsForWindows)) {
                ImGui::Text("Calculating was done!");
                ImGui::EndPopup();
            }
        }
    }

    void createButton(double& x0, double& u0, ImGuiWindowFlags& flagsForWindows, int& item_current_idx, bool& isConstH, bool& isPressed, bool& isNotice, double& u_0, double& param_a, double& param_b) { // функция для создания кнопки, чтобы начать работу методом РК-4

        ImGui::SetNextWindowPos({ 0,181 }); // устанавливаем позицию для создаваемого окна
        ImGui::SetNextWindowSize({ 350,39 }); // устанавливаем размер для создаваемого окна

        ImGui::Begin("Button", 0, flagsForWindows); // Создаем окно с заданными параметрами

        //bool isPressed = false; // создаем переменную, показывающую была ли нажата кнопка

        if (ImGui::Button("START", { 100,20 })) { // создаем кнопку с размерами 100*20
            rk.clear_data(); // очищаем значения векторов с прошлой задачи
            if (item_current_idx == 2)
                rk.run_func(x0, u0, (Task)item_current_idx, isConstH, u_0, param_a,param_b);
            else
                rk.run_func(x0, u0, (Task)item_current_idx, isConstH); // считаем задачу коши в точке (x0,u0)
            isPressed = true; // устанвливаем для переменной isPressed, что она была нажата
            isNotice = true;
        }

        ImGui::SameLine(); // в той же строке
        ImGui::Text("Start Calc"); // пишем Start Calc

        ImGui::End(); // удаляем окно
        
        //return isPressed; // возврщаем значение isPressed
    }

    void createGraph(ImGuiWindowFlags& flagsForWindows, int& item_current_idx, const char* names[3], bool(&opened)[3]) { // функция для отрисовки графика

        std::vector<std::pair<double, double>> data_for_analytical_solution = rk.getCoordsForAnalytical_Solution(); // создаем вектор точек для аналитического решения, если аналитического решения нет, то тут будет пустой ветктор

        const size_t size_of_analytical_solution = data_for_analytical_solution.size(); // получаем размер вектора точек для аналитического решения
        double* x_of_analytical_solution = new double[size_of_analytical_solution]; // создаем динамический массив x-точек аналитического решения. Размер этого массива равен размеру вектор точек для аналитического решения
        double* y_of_analytical_solution = new double[size_of_analytical_solution]; // создаем динамический массив y-точек аналитического решения. Размер этого массива равен размеру вектор точек для аналитического решения

        for (size_t count = 0; count < size_of_analytical_solution; ++count) { // цикл для копирования данных в массив из вектора точек для аналитического решения ("Мирного решения не будет")
            x_of_analytical_solution[count] = (data_for_analytical_solution[count].first);
            y_of_analytical_solution[count] = (data_for_analytical_solution[count].second);
        }

        std::vector<std::tuple<double, double, double>> data_for_numerical_solution = rk.getCoords(); // создаем вектор точек для численного решения задачи Коши

        const size_t size_of_numerical_solution = data_for_numerical_solution.size(); // получаем размер вектора точек для численного решения
        double* x_of_numerical_solution = new double[size_of_numerical_solution]; // создаем динамический массив x-точек численного решения. Размер этого массива равен размеру вектор точек для численного решения
        double* y_of_numerical_solution = new double[size_of_numerical_solution]; // создаем динамический массив y-точек численного решения. Размер этого массива равен размеру вектор точек для численного решения
        double* y__of_numerical_solution = new double[size_of_numerical_solution];


        for (int count = 0; count < size_of_numerical_solution; ++count) { // цикл для копирования данных в массив из вектора точек для численного решения
            x_of_numerical_solution[count] = std::get<0>(data_for_numerical_solution[count]);
            y_of_numerical_solution[count] = std::get<1>(data_for_numerical_solution[count]);
            if (item_current_idx == 2)
                y__of_numerical_solution[count] = std::get<2>(data_for_numerical_solution[count]);
        }

        ImGui::SetNextWindowSize({ 960,860 }); // устанавливаем размер для окна с графиком
        ImGui::SetNextWindowPos({ 960,220 }); // устанавливаем позицию для окна с графиком

        ImGui::Begin("Graph", 0, flagsForWindows); // создаем окно с выбранными настройками

        if (item_current_idx != 2) {
            if (ImPlot::BeginPlot("Solution schedule", "x", "u", { 940,780 })) { // отрисовываем график
                if (item_current_idx == 0) // если у нас тестовая задача, то надо нарисовать еще аналитическое решение
                    ImPlot::PlotLine("Analytical solution graph", x_of_analytical_solution, y_of_analytical_solution, size_of_analytical_solution); // отрисовываем линию
                ImPlot::PlotLine("Numerical solution graph", x_of_numerical_solution, y_of_numerical_solution, size_of_numerical_solution); // отрисовываем линию
                ImPlot::EndPlot(); // заканчиваем отрисовку графика
            }
        }
        else {
            if (opened[0] && opened[1]){
                if (ImPlot::BeginPlot("Solution schedule", "x", "u", { 940,780 }))
                    ImPlot::PlotLine("Numerical solution graph", x_of_numerical_solution, y_of_numerical_solution, size_of_numerical_solution); // отрисовываем линию
                ImPlot::EndPlot(); // заканчиваем отрисовку графика
            }
            else if (opened[1] && opened[2]) {
                if (ImPlot::BeginPlot("Solution schedule", "u", "u'", { 940,780 }))
                    ImPlot::PlotLine("Phase portrait", y_of_numerical_solution, y__of_numerical_solution, size_of_numerical_solution); // отрисовываем линию
                ImPlot::EndPlot(); // заканчиваем отрисовку графика
            }
            else if (opened[0] && opened[2]) {
                if (ImPlot::BeginPlot("Solution schedule", "x", "u'", { 940,780 }))
                    ImPlot::PlotLine("Numerical solution graph", x_of_numerical_solution, y__of_numerical_solution, size_of_numerical_solution); // отрисовываем линию
                ImPlot::EndPlot(); // заканчиваем отрисовку графика
            }
        }

        ImGui::End(); // удаляем окно
    }

    void createTable(ImGuiWindowFlags& flagsForWindows, int& item_current_idx, bool& isConstH) {
        std::vector<double> vector_of_h = rk.getH(); // создаем вектор значений шагов
        std::vector<std::tuple<double, double, double>> data_for_numerical_solution = rk.getCoords(); // создаем вектор координат, полученных чсиленным решением
        std::vector<double> twice_half_h_u = rk.getVectorOfTwiceHalfHU(); // вектор координат v^i
        std::vector<double> difference_of_v = rk.getVectorOfDifferenceOfV(); // вектор vi-v^i
        std::vector<double> vector_S = rk.getVectorOfS(); // вектор ОЛП

        std::vector<double> difference_of_u;
        std::vector<double> coordsOfU;

        int size = 9;

        if (isConstH)
            size = 7; // размер 3 для таблицы

        if (item_current_idx == 0) {
            difference_of_u = rk.getVectorOfDifferenceU();
            coordsOfU = rk.getCoordsOfU();
            size += 2;
        }

        std::vector<int> C1 = rk.getC1(); // вектор делений шагов 
        std::vector<int> C2 = rk.getC2(); // вектор умножений шагов

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg; // настройки для таблицы

        ImGui::SetNextWindowPos({ 0,220 }); // устанавливаем позицию для создаваемого окна для таблицы
        ImGui::SetNextWindowSize({ 960,790 }); // устанавливаем размер для создаваемого окна для таблицы
        ImGui::Begin("Table", 0 , flagsForWindows); // создаем окно с заданными настройками
        
        if (ImGui::BeginTable("table1", size, flags)) // создаем таблицу с заданными настройками
        {
            if (true)
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Xi");
                ImGui::TableSetupColumn("Vi");
                ImGui::TableSetupColumn("OLP");
                ImGui::TableSetupColumn("V^i");
                ImGui::TableSetupColumn("Vi-V^i");
                ImGui::TableSetupColumn("Hi");
                if (!isConstH) {
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
                ImGui::Text("%.24lf", std::get<0>(data_for_numerical_solution[row]));
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.24lf", std::get<1>(data_for_numerical_solution[row]));
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.24lf", vector_S[row]);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.24lf", twice_half_h_u[row]);
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%.24lf", difference_of_v[row]);
                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%.24lf", vector_of_h[row]);
                if (!isConstH) {
                    ImGui::TableSetColumnIndex(7);
                    ImGui::Text("%d", C1[row]);
                    ImGui::TableSetColumnIndex(8);
                    ImGui::Text("%d", C2[row]);
                }
                if (item_current_idx == 0 && !isConstH) {
                    ImGui::TableSetColumnIndex(9);
                    ImGui::Text("%.24lf", coordsOfU[row]);
                    ImGui::TableSetColumnIndex(10);
                    ImGui::Text("%.24lf", difference_of_u[row]);
                }
                else if (item_current_idx == 0 && isConstH) {
                    ImGui::TableSetColumnIndex(7);
                    ImGui::Text("%.24lf", coordsOfU[row]);
                    ImGui::TableSetColumnIndex(8);
                    ImGui::Text("%.24lf", difference_of_u[row]);
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