#include <vector>
#include <iostream>

#include <math.h>

#pragma once

enum class Actions_with_H { // enum class для установки статуса шага
    MULTIPLY_BY_2, // необходимо умножить на два
    NOTHING, // ничего не делаем
    DIVIDE_BY_2_AND_RECALCULATE, // делим на два и пересчитываем
    GET_LAST, // получаем последнюю точку
    STOP // останавливаемся
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

    std::vector<double> m_vector_of_h; // веткор для хранения шагов
    std::vector<std::pair<double, double>> m_data; // вектор m_data для хранения точек x и u
    std::vector<std::pair<double, double>> m_analytical_solution_data; // вектор для хранения аналитического решения
    std::vector<double> m_twice_half_h_u; // вектор для хранения точек, посчитанных с двойным шагом
    std::vector<double> difference_of_u; // вектор разности точки с обычным шагом и двойным
    std::vector<double> vector_S; // вектор ОЛП

    std::vector<int> C1; // вектор кол-ва деления шага
    std::vector<int> C2; // вектор кол-ва умножения шага


    double test_func(double x, double u) { // реализация функции du/dx для тестовой задачи
        // У нас 5-ый №команды => du/dx = (-1)^5*5/2*u => du/dx = -5/2*u

        return (-5/2*u);
    }

    double func_of_first_task(double x, double u) { // реализация фунцкии du/dx для задачи №1
        // У нас 5-ый №команды => du/dx = ln(x+1)/(x^2+1)

        return (log(x + 1) / (x * x + 1));
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

    std::pair<double, double> RKIV(double x, double u, double h, Task task) { // реализация метода

        // необходимо как-то придумать, чтобы вызывалась функция в зависимости от требуемой
        // если надо посчитать test_func, то вызывается test_func, если надо посчитать main_func_1/2, то вызывается main_func_1/2

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

        return (std::make_pair( getNewX(x, h), getNewU({k1,k2,k3,k4}, u, h))); // создаем пару (x,u) из чисел, полученных через getNewX, getNewU
    }

    Actions_with_H checkUpDown(double u_with_h, double u_with_twice_half_h) {
        
        double S = abs(getS(u_with_h, u_with_twice_half_h)); // получаем S и сразу берем от неё модуль

        vector_S.push_back(S);

        //std::cout << m_E_check_down << std::endl;
        //std::cout << S << std::endl;

        if (S >= m_E_check_down && S <= m_E_check_up) { // если S находится в границе "локальной погрешности" то возвращаем статус NOTHING
            return Actions_with_H::NOTHING;
        }
        else if (S < m_E_check_down) { // если S находится ниже нижней границы, то возвращаем статус MULTIPLY_BY_2
            return Actions_with_H::MULTIPLY_BY_2;
        }
        else if (S > m_E_check_up) { // если S находится выше верхней границы, то возвращаем статус DIVIDE_BY_2_AND_RECALCULATE
            return Actions_with_H::DIVIDE_BY_2_AND_RECALCULATE;
        }
    }

    Actions_with_H checkRight(double x) {
        if (x > m_b)
            return Actions_with_H::GET_LAST; // если мы попали за правую границу, то нам необходимо вернуться и получить последний элемент
        else if (x == m_b)
            return Actions_with_H::STOP; // если мы попали ровно на правую границу, то мы получаем последний элемент и заканчиваем
        else if (x <= m_b - m_E_check_right)
            return Actions_with_H::NOTHING; // если мы попали левее эпсилон - правая-граница, то ничего не делаем и продолжаем работу
    }

    void calculate(double x0, double u0, Task task) {

        m_data.push_back({ x0,u0 }); // пушим в вектор начальные условия
        m_vector_of_h.push_back(0);
        m_twice_half_h_u.push_back(u0);
        difference_of_u.push_back(0);
        vector_S.push_back(0);
        C2.push_back(0);
        C1.push_back(0);

        std::pair<double, double> coords_with_h; // создаем вектор coords_with_h для хранения точек x и u с шагом h

        int C1count = 0;
        int C2count = 0;

        double h = m_h_start;

        while (true) {
            std::pair<double, double> current_coord = m_data.back(); // получаем текущие координаты (x_n, u_n)
            std::pair<double, double> coords_with_half_h;
            std::pair<double, double> coords_with_twice_half_h;

            coords_with_h = RKIV(current_coord.first, current_coord.second, h, task); // получаем точку (x_n+1, u_n+1) с шагом h из точки (x_n, u_n)
            coords_with_half_h = RKIV(current_coord.first, current_coord.second, h / 2, task); //  получаем точку (x_n+1/2, u_n+1/2) из точки (x_n, u_n) с шагом h/2
            coords_with_twice_half_h = RKIV(coords_with_half_h.first, coords_with_half_h.second, h / 2, task); // получаем точку (x_n+1, u_n+1) из точки (x_n+1/2, u_n+1/2) с шагом h / 2

            m_twice_half_h_u.push_back(coords_with_twice_half_h.second);
            difference_of_u.push_back(coords_with_h.second - coords_with_twice_half_h.second);

            Actions_with_H act1 = checkUpDown(coords_with_h.second, coords_with_twice_half_h.second); // проверка на выход за локальную погрешность
            Actions_with_H act2 = checkRight(coords_with_h.first); // проверка за выход за правую границу

            if (act2 == Actions_with_H::GET_LAST) {
                h = m_b - m_data.back().first; // устанавливаем шаг, который привидет нас точно в правую грницу m_b
            }
            else if (act2 == Actions_with_H::NOTHING || act2 == Actions_with_H::STOP) { // если мы не вышли за правую границу - вернулся статус NOTHING или мы считаем последнюю точку
                if (act1 == Actions_with_H::MULTIPLY_BY_2) { // если небходимо шаг умножить на два после проверки на лок. погрешность
                    m_data.push_back(coords_with_h); // сохраняем точку
                    m_vector_of_h.push_back(h); // сохраняем шаг
                    h *= 2; // умножаем шаг на 2
                    ++C2count;
                    C2.push_back(C2count);
                    C1.push_back(C1count);
                    C2count = C1count = 0;

                    if (act2 == Actions_with_H::STOP)
                        break;
                }
                else if (act1 == Actions_with_H::NOTHING) // если нам ничего не надо делать с шагом, находимся ровно в границе лок. погрешности
                {
                    m_data.push_back(coords_with_h); // сохраняем точку
                    m_vector_of_h.push_back(h); // сохраняем шаг
                    C2.push_back(C2count);
                    C1.push_back(C1count);
                    C2count = C1count = 0;

                    if (act2 == Actions_with_H::STOP)
                        break;
                }
                else if (act1 == Actions_with_H::DIVIDE_BY_2_AND_RECALCULATE) { // если нам надо поделить шаг на два и пересчитать точку
                    h /= 2; // делим шаг на два
                    ++C1count;
                }
            }

        }
    }

    void analytical_solution() {
        double h = (m_b - m_a) / m_N_max;

        for (int count = 0; count < m_N_max; ++count)
            m_analytical_solution_data.push_back({ m_a + count * h, exp(((-5) / 2) * (m_a + count * h)) });
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

    void clear_data() {
        m_vector_of_h.clear(); // веткор для хранения шагов
        m_data.clear(); // вектор m_data для хранения точек x и u
        m_analytical_solution_data.clear(); // вектор для хранения аналитического решения
        m_twice_half_h_u.clear(); // вектор для хранения точек, посчитанных с двойным шагом
        difference_of_u.clear(); // вектор разности точки с обычным шагом и двойным
        vector_S.clear(); // вектор ОЛП
        C1.clear(); // вектор кол-во деления шага
        C2.clear(); // вектор кол-во умножения шага
    }

    std::pair<double, double> getBorder() {
        return { m_a,m_b };
    }

    std::vector<std::pair<double, double>> getCoords() {
        return m_data;
    }

    std::vector<std::pair<double, double>> getCoordsForAnalytical_Solution() {
        return m_analytical_solution_data;
    }

    std::vector<double> getVectorOfTwiceHalfHU() {
        return m_twice_half_h_u;
    }

    std::vector<double> getVectorOfDifferenceOfU() {
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

    void run_func(double x0, double u0, Task task) {
        /*
        Идея:
            Создать три функции для трех задач. Каждая функция будет запускать run(x0,u0)
            Для тестовой задачи необходимо решить численно и аналитически =>
            Будет два вектора пар точек:
                1) Вектор с численным решением
                2) Вектор с аналитическим решением
            Надо будет как-то передать в run, какую функцию использовать в RKIV.
            Например, можно сделать enum_class с 3-емя элементами
        
        
        */
        if (task == Task::TEST_FUNC)
            analytical_solution();
        
        calculate(x0, u0, task);
    }
};