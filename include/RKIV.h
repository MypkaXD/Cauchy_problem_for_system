#include <vector>
#include <iostream>
#include <tuple>
#include <math.h>

#pragma once

enum class ACTIONS_WITH_X {
    NOTHING,
    GET_LAST,
    STOP
};

enum class ACTIONS_WITH_H {
    NOTHING, // ничего не делаем
    MULTIPLY_BY_2, // необходимо умножить на два
    DIVIDE_BY_2_AND_RECALCULATE, // делим на два и пересчитываем
};

enum class Task {
    TEST_FUNC = 0,
    FIRST_TASK = 1,
    SECOND_TASK = 2
};

class RK {
private:

    double m_h_start = 0.1; // численный шаг интегрирования (начальный)
    double m_E_check_right = 0.5 * 10e-6; // проверка для "выхода" справа
    double m_E_check_up = 0.5 * 10e-4; // проверка для "вверха"
    double m_E_check_down = ((m_E_check_up ) / 32); // проверка для "низа"

    double m_a = 0; // левая граница
    double m_b = 1; // правая граница

    int m_N_max = 100; // максимальное число шагов
    
    int p = 4; // порядок метода

    std::vector<std::tuple<double, double, double>> m_data; // вектор m_data для хранения точки (x,u,u')
    
    std::vector<std::pair<double, double>> m_analytical_solution_data; // вектор для хранения аналитического решения
    
    std::vector<double> m_vector_of_h; // веткор для хранения шагов
    std::vector<double> m_twice_half_h_u; // вектор для хранения точек, посчитанных с двойным шагом
    std::vector<double> difference_of_v; // вектор разности точки с обычным шагом и двойным
    std::vector<double> vector_S; // вектор ОЛП
    std::vector<double> difference_of_u; // вектор разности точки, полученной численно и аналитически
    std::vector<double> m_vecotor_u; // веткор для точек аналитического решения

    std::vector<int> C1; // вектор кол-ва деления шага
    std::vector<int> C2; // вектор кол-ва умножения шага


    double test_func(double x, double u) { // реализация функции du/dx для тестовой задачи
        // У нас 5-ый №команды => du/dx = (-1)^5*5/2*u => du/dx = -5/2*u

        return (-5/2*u);
    }

    double func_of_first_task(double x, double u) { // реализация фунцкии du/dx для задачи №1
        // У нас 5-ый №команды => du/dx = ln(x+1)/(x^2+1)

        return ((log(x+1)/(1+x*x))*u*u+u-u*u*u*sin(10*x));
    }

    double func_of_second_task_for_f(double x, double u, double u_, double a, double b) {
        return (-b*sin(u) - a*u_*u_);
    }

    double func_of_second_task_for_g(double x, double u, double u_) {
        return u_;
    }

    double test_func_analytical_solution(double x) {
        return (exp(((-5) / 2) * (x)));
    }

    double getNewX(double x, double h) { // получаем новый x
        return (x + h); // считаем новый x
    }

    double getNewU(std::vector<double> k, double u, double h) { // получаем новый u
        if (k.size() != 4) // выбрасываем ошибку, если размер вектора k-коэффициентов != 4. Такое впринципе невозможно, но путь будет
            throw ("ERROR: The vector of K-koef isn't contain 4 elems");
        else {
            return (u + h / 6 * (k[0] + 2 * k[1] + 2 * k[2] + k[3])); // считаем новый u
        }
    }

    double getS(double u_with_h, double u_with_twice_half_h) {
        return ((u_with_twice_half_h - u_with_h) / ((2 << (p - 1)) - 1)); // считаем S по формуле S = (v_h/2(n+1) - v_h(n+1)) / (2^p - 1)
    }

    std::tuple<double, double, double> RKIV(double x, double u, double h, Task task, double u_ = 0, double a = 1, double b = 2) { // реализация метода

        double k1, k2, k3, k4;

        if (task == Task::TEST_FUNC) {
            k1 = test_func(x, u); // считаем k1
            k2 = test_func(x + h / 2, u + h / 2 * k1); // считаем k2
            k3 = test_func(x + h / 2, u + h / 2 * k2); // считаем k3
            k4 = test_func(x + h, u + h * k3); // считаем k4
        }
        else if (task == Task::FIRST_TASK) {
            k1 = func_of_first_task(x, u); // считаем k1
            k2 = func_of_first_task(x + h / 2, u + h / 2 * k1); // считаем k2
            k3 = func_of_first_task(x + h / 2, u + h / 2 * k2); // считаем k3
            k4 = func_of_first_task(x + h, u + h * k3); // считаем k4
        }
        else if (task == Task::SECOND_TASK) {
            k1 = func_of_second_task_for_g(x, u, u_);
            double l1 = func_of_second_task_for_f(x, u, u_,a,b);

            k2 = func_of_second_task_for_g(x + h / 2, u + h / 2 * k1, u_ + h / 2 * l1);
            double l2 = func_of_second_task_for_f(x + h / 2, u + h / 2 * k1, u_ + h / 2 * l1, a, b);

            k3 = func_of_second_task_for_g(x + h / 2, u + h / 2 * k2, u_ + h / 2 * l2);
            double l3 = func_of_second_task_for_f(x + h / 2, u + h / 2 * k2, u_ + h / 2 * l2, a, b);

            k4 = func_of_second_task_for_g(x + h / 2, u + h  * k3, u_ + h * l3);
            double l4 = func_of_second_task_for_f(x + h / 2, u + h * k3, u_ + h * l3, a, b);

            u_ += h / 6 * (l1 + 2 * l2 + 2 * l3 + l4);
        }

        return  std::make_tuple(getNewX(x, h), getNewU({ k1,k2,k3,k4 }, u, h), u_); // создаем коллекию (x,u,u')
    }

    ACTIONS_WITH_H checkUpDown(std::tuple<double, double, double> coords_with_h, std::tuple<double, double, double> coords_with_twice_half_h) {
        
        if (std::isnan(std::get<1>(coords_with_h))|| std::isnan(std::get<1>(coords_with_twice_half_h)))
            return ACTIONS_WITH_H::DIVIDE_BY_2_AND_RECALCULATE;

        double S1 = abs(getS(std::get<1>(coords_with_h), std::get<1>(coords_with_twice_half_h))); // получаем S от u и сразу берем от неё модуль
        double S2 = abs(getS(std::get<2>(coords_with_h), std::get<2>(coords_with_twice_half_h))); // получаем S от u' и сразу берем от неё модуль

        double S = std::max(S1, S2); // вычисляем max между S1 и S2

        vector_S.push_back(S); // сохраняем максимальное значение
        // Даже если у нас обычная задача #1 и тестовая задача будет, то S1 считается для u, а S2 считается для u' = 0, так как u' = 0; u'^ = 0

        if (S >= m_E_check_down && S <= m_E_check_up) { // если S находится в границе "локальной погрешности" то возвращаем статус NOTHING
            return ACTIONS_WITH_H::NOTHING;
        }
        else if (S < m_E_check_down) { // если S находится ниже нижней границы, то возвращаем статус MULTIPLY_BY_2
            return ACTIONS_WITH_H::MULTIPLY_BY_2;
        }
        else if (S > m_E_check_up) { // если S находится выше верхней границы, то возвращаем статус DIVIDE_BY_2_AND_RECALCULATE
            return ACTIONS_WITH_H::DIVIDE_BY_2_AND_RECALCULATE;
        }
    }

    ACTIONS_WITH_X checkRight(double x) {
        if (x > m_b)
            return ACTIONS_WITH_X::GET_LAST; // если мы попали за правую границу, то нам необходимо вернуться и получить последний элемент
        else if (x == m_b || (x > m_b - m_E_check_right && x < m_b))
            return ACTIONS_WITH_X::STOP; // если мы попали ровно на правую границу, то мы получаем последний элемент и заканчиваем
        else
            return ACTIONS_WITH_X::NOTHING; // если мы попали левее эпсилон - правая-граница, то ничего не делаем и продолжаем работу
    }

    void calculate_with_e(double x0, double u0, Task task, double u_0 = 0, double a = 0, double b = 0) { // функция для рассчета численного решения с переменным шагом

        m_data.push_back({ x0,u0, u_0 }); // пушим в вектор координат начальные условия (x0, u0, u'0)
        m_vector_of_h.push_back(0); // пушим в вектор шагов начальный шаг
        m_twice_half_h_u.push_back(u0); // пушим в вектор v^ (u0)
        difference_of_v.push_back(0); // пушим в вектор разности v - v^ (0)
        vector_S.push_back(0); // пушим в вектор ОЛП (0)
        C2.push_back(0); // пушим в вектор умножений (0)
        C1.push_back(0); // пушим в вектор делений (0)

        if (task == Task::TEST_FUNC) { // если у нас тестовая задача
            m_vecotor_u.push_back(test_func_analytical_solution(x0)); // пушим в вектор аналитическое решение в точке (x0)
            difference_of_u.push_back(0); // пушим в вектор разность между численным и аналитическим решением в точке (x0)
        }

        std::tuple<double, double, double> current_coords; // создаем коллекцию current_coords текущих координат
        std::tuple<double, double, double> coords_with_h; // создаем коллекцию coords_with_h для хранения точек (x, u, u') с шагом h
        std::tuple<double, double, double>  coords_with_half_h; // создаем коллекцию coords_with_half_h для хранения точек (x, u, u') с шагом h/2
        std::tuple<double, double, double>  coords_with_twice_half_h; // создаем коллекцию coords_with_twice_half_h для хранения точек (x, u, u') с двойным шагом h/2

        int C1count = 0; // счетчик делений
        int C2count = 0; // счетчик умножений

        double h = m_h_start; // текущий шаг
        double next_x; // следующая точка x

        bool wasDivideStep = false; // булева переменная для отслеживания был ли поделен шаг пополам
        
        int N = m_N_max; // счетчик для отслеживания, чтобы не выйти за максимальное число шагов

        while (N > 0) { // считаем пока не выйдем за максимально допустимое число шагов
            
            current_coords = m_data.back(); // получаем текущие координаты

            next_x = getNewX(std::get<0>(current_coords), h); // получаем x с шагом h

            ACTIONS_WITH_X act_for_x = checkRight(next_x); // проверям выход x за границу

            if (act_for_x == ACTIONS_WITH_X::NOTHING || act_for_x == ACTIONS_WITH_X::STOP) { // если x не вышел за границу или находится в епсилон окрестности правой границы

                if (wasDivideStep) // был ли поделен шаг в предыдущем пополам
                    coords_with_h = coords_with_twice_half_h; // если да, то координаты с шагом h' = h/2 не надо просчитывать, так как мы их уже имеем (ОПТИМИЗАЦИЯ)
                else // если шаг не был поделен
                    coords_with_h = RKIV(std::get<0>(current_coords), std::get<1>(current_coords), h, task, std::get<2>(current_coords), a, b); // получаем координаты из текущей точки с шагом h
                coords_with_half_h = RKIV(std::get<0>(current_coords), std::get<1>(current_coords), h / 2, task, std::get<2>(current_coords), a, b); // получаем координаты из текущей точки с шагом h/2
                coords_with_twice_half_h = RKIV(std::get<0>(coords_with_half_h), std::get<1>(coords_with_half_h), h / 2, task, std::get<2>(coords_with_half_h), a, b); // получаем координаты из точки coords_with_half_h с шагом h/2

                ACTIONS_WITH_H act_for_h = checkUpDown(coords_with_h, coords_with_twice_half_h); // проверям на локальную погрешность

                if (act_for_h == ACTIONS_WITH_H::NOTHING) { // если всё "НОРМ"
                    m_twice_half_h_u.push_back(std::get<1>(coords_with_twice_half_h)); // сохраняем в вектор точку, полученную двойным шагом
                    difference_of_v.push_back(std::get<1>(coords_with_h) - std::get<1>(coords_with_twice_half_h)); // разность между точкой полученный с помощью целого шага и двойного половинного
                    m_data.push_back(coords_with_h); // сохраняем полученные координаты
                    m_vector_of_h.push_back(h); // сохраняем в вектор текущий шаг
                    C1.push_back(C1count); // сохраняем число делений шага
                    C2.push_back(C2count); // сохраняем число умножений шага
                    
                    if (task == Task::TEST_FUNC) { // если у нас тестовая задача
                        m_vecotor_u.push_back(test_func_analytical_solution(std::get<0>(coords_with_h))); // сохраняем точку для аналитического решения
                        difference_of_u.push_back(abs(m_vecotor_u.back() - std::get<1>(coords_with_h))); // сохраняем разность между аналитическим и численным решением
                    }

                    --N; // уменьшаем N на 1 = шаг был сделан
                    C1count = 0; // зануляем число удвоений шага
                    C2count = 0; // зануляем число умножений шага
                    wasDivideStep = false; // устанавливаем false, чтобы не было непредвиденных обстоятельств
                }
                else if (act_for_h == ACTIONS_WITH_H::MULTIPLY_BY_2) { // если нам надо умножить шаг на два
                    m_twice_half_h_u.push_back(std::get<1>(coords_with_twice_half_h)); // сохраняем в вектор точку, полученную двойным шагом
                    difference_of_v.push_back(std::get<1>(coords_with_h) - std::get<1>(coords_with_twice_half_h)); // разность между точкой полученный с помощью целого шага и двойного половинного
                    m_data.push_back(coords_with_h); // сохраняем полученные координаты
                    m_vector_of_h.push_back(h); // сохраняем в вектор текущий шаг
                    C1.push_back(C1count); // сохраняем число делений шага
                    C2.push_back(++C2count); // сохраняем число умножений шага + 1, так как мы умножили наш шаг (не забываем)
                    
                    if (task == Task::TEST_FUNC) { // если у нас тестовая задача
                        m_vecotor_u.push_back(test_func_analytical_solution(std::get<0>(coords_with_h))); // сохраняем точку для аналитического решения
                        difference_of_u.push_back(abs(m_vecotor_u.back() - std::get<1>(coords_with_h))); // сохраняем разность между аналитическим и численным решением
                    }

                    --N; // уменьшаем N на 1 = шаг был сделан
                    h *= 2; // умножаем шаг на два
                    C1count = 0; // зануляем число удвоений шага
                    C2count = 0; // зануляем число умножений шага
                    wasDivideStep = false; // устанавливаем false, чтобы не было непредвиденных обстоятельств
                }
                else if (act_for_h == ACTIONS_WITH_H::DIVIDE_BY_2_AND_RECALCULATE) { // если нам надо поделить шаг пополам и пересчитать точку
                    h /= 2; // делим шаг на два
                    C1count += 1; // число делений увеличиваем на 1
                    wasDivideStep = true; // шаг был поделен, поэтому true (ОПТИМИЗАЦИЯ)
                }

                if (act_for_x == ACTIONS_WITH_X::STOP) // если x попал в епсилон область правой границы
                    break; // то заканчиваем наши вычисления
            }
            else if (act_for_x == ACTIONS_WITH_X::GET_LAST) { // если нам нужно получить последную точку
                wasDivideStep = false; // устанавливаем false, чтобы не было непредвиденных обстоятельств
                h = m_b - std::get<0>(current_coords); // шаг = правая граница - текущая координата (он 100% будет <= шага который был до этого, так как в противном случаем мы бы не пересели правую границу)
            }
        }
    }

    void calculate_with_constH(double x0, double u0, Task task, double u_0 = 0, double a = 0, double b = 0) {

        m_data.push_back({ x0,u0, u_0 }); // пушим в вектор начальные условия
        m_vector_of_h.push_back(0);

        if (task == Task::TEST_FUNC) {
            m_vecotor_u.push_back(test_func_analytical_solution(x0)); // сохраняем точку, полученную через аналитическое решение в точке x0
            difference_of_u.push_back(0); // пушим в вектор 0 - разность числового и аналитического решения
        }

        std::tuple<double, double, double> current_coord; // создаем коллекцию current_coord для хранения текущих координат
        std::tuple<double, double, double> coords_with_h; // создаем коллекцию coords_with_h для хранения точек (x,u,u') с шагом h

        double h = (m_b - m_a) / m_N_max; // рассчитываем шаг, как разность правой границы и левой границы, поделенную на максимальное число шагов
        double x_next;
        int N = m_N_max; // счетчик для отслеживания, чтобы не выйти за максимальное число шагов

        while (N > 0) { // считаем пока не выйдем за максимально допустимое число шагов
            
            current_coord = m_data.back(); // получаем текущие координаты (x_n, u_n, u'_n)

            x_next = getNewX(std::get<0>(current_coord), h);
            
            ACTIONS_WITH_X act_for_x = checkRight(x_next); // проверка за выход за правую границу

            if (act_for_x == ACTIONS_WITH_X::NOTHING) {

                coords_with_h = RKIV(std::get<0>(current_coord), std::get<1>(current_coord), h, task, std::get<2>(current_coord), a, b); // получаем координаты из текущей точки с шагом h

                m_data.push_back(coords_with_h); // сохраняем точку
                m_vector_of_h.push_back(h); // сохраняем шаг

                --N;
                if (task == Task::TEST_FUNC) {
                    m_vecotor_u.push_back(test_func_analytical_solution(std::get<0>(coords_with_h)));
                    difference_of_u.push_back(abs(m_vecotor_u.back() - std::get<1>(coords_with_h)));
                }
            }
            else if (act_for_x == ACTIONS_WITH_X::STOP || act_for_x == ACTIONS_WITH_X::GET_LAST) {
                
                coords_with_h = RKIV(std::get<0>(current_coord), std::get<1>(current_coord), h, task, std::get<2>(current_coord), a, b); // получаем координаты из текущей точки с шагом h

                m_data.push_back(coords_with_h); // сохраняем точку
                m_vector_of_h.push_back(h); // сохраняем шаг

                --N;
                if (task == Task::TEST_FUNC) {
                    m_vecotor_u.push_back(test_func_analytical_solution(std::get<0>(coords_with_h)));
                    difference_of_u.push_back(abs(m_vecotor_u.back() - std::get<1>(coords_with_h)));
                }
                break;
            }
        }
    }

    void analytical_solution() {
        double h = (m_b - m_a) / m_N_max;
        double x = m_a;

        for (int count = 0; count <= m_N_max; ++count) {
            m_analytical_solution_data.push_back({ x, test_func_analytical_solution(x)});
            x += h;
        }
    }

public:

    RK(double a, double b) {
        m_a = a;
        m_b = b;
    }
    RK() {
    }

    void setBorder(double a, double b) {
        m_a = a;
        m_b = b;
    }

    void setBorderA(double a) {
        if (a <= m_b)
            m_a = a;
    }

    void setBorderB(double b) {
        if (b >= m_a)
            m_b = b;
    }

    void setN(int N) {
        if (N > 0)
            m_N_max = N;
    }

    void clear_data() {
        m_vector_of_h.clear(); // веткор для хранения шагов
        m_data.clear(); // вектор m_data для хранения точек x и u
        m_analytical_solution_data.clear(); // вектор для хранения аналитического решения
        m_twice_half_h_u.clear(); // вектор для хранения точек, посчитанных с двойным шагом
        difference_of_v.clear(); // вектор разности точки с обычным шагом и двойным
        vector_S.clear(); // вектор ОЛП
        C1.clear(); // вектор кол-во деления шага
        C2.clear(); // вектор кол-во умножения шага
        m_vecotor_u.clear();
        difference_of_u.clear();
    }

    int getN() {
        return m_N_max;
    }

    int getCountOfDoublings() {
        int counter = 0;
        
        for (size_t count = 0; count < C2.size(); ++count)
            counter += C2[count];

        return counter;
    }

    int getCountOfDivisions() {
        int counter = 0;

        for (size_t count = 0; count < C1.size(); ++count)
            counter += C1[count];

        return counter;
    }

    std::pair<double, double> getBorder() {
        return { m_a,m_b };
    }

    std::vector<std::tuple<double, double, double>> getCoords() {
        return m_data;
    }

    std::vector<std::pair<double, double>> getCoordsForAnalytical_Solution() {
        return m_analytical_solution_data;
    }

    std::vector<double> getVectorOfTwiceHalfHU() {
        return m_twice_half_h_u;
    }

    std::vector<double> getVectorOfDifferenceOfV() {
        return difference_of_v;
    }

    std::vector<double> getCoordsOfU() {
        return m_vecotor_u;
    }

    std::vector<double> getVectorOfDifferenceU() {
        return difference_of_u;
    }

    std::vector<double> getVectorOfS() {
        return vector_S;
    }

    std::vector<int> getC1() {
        return C1;
    }
    
    std::vector<int> getC2() {
        return C2;
    }

    std::vector<double> getH() {
        return m_vector_of_h;
    }

    void set_E_right(double e) {
        m_E_check_right = e;
    }

    void set_E_local(double e) {
        m_E_check_up = e;
    }

    void set_Start_H(double h) {
        m_h_start = h;
    }

    double get_E_right() {
        return m_E_check_right;
    }

    double get_E_local() {
        return m_E_check_up;
    }

    double get_Start_H() {
        return m_h_start;
    }

    void run_func(double x0, double u0, Task task, bool isConstH, double u_0 = 0, double a = 0, double b = 0) {
        if (task == Task::TEST_FUNC)
            analytical_solution();
        if (!isConstH)
            calculate_with_e(x0, u0, task, u_0, a, b);
        else
            calculate_with_constH(x0,u0, task, u_0, a,b);
    }
};