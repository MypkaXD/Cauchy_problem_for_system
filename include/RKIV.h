#include <vector>
#include <iostream>

#include <math.h>

#pragma once

enum class Actions_with_H {
    MULTIPLY_BY_2,
    NOTHING,
    DIVIDE_BY_2_AND_RECALCULATE,
    GET_LAST,
    STOP
};

class RK {
private:
    //10e-5
    double m_h = 0.1; // численный шаг интегрирования
    double m_E_check_right = 0.5 * 10e-6; // проверка для "выхода" справа
    double m_E_check_up = 0.5 * 10e-4; // проверка для "вверха"
    double m_E_check_down = ((m_E_check_up ) / 32); // проверка для "низа"

    double m_a; // левая граница
    double m_b; // правая граница

    int m_N_max = 100; // максимальное число шагов
    int p = 4; // порядок метода

    std::vector<double> vector_of_h;

public:

    RK(double a, double b) {
        m_a = a;
        m_b = b;
    }

    double test_func(double x, double u) { // реализация функции du/dx для тестовой задачи
        // У нас 5-ый №команды => du/dx = (-1)^5*5/2*u => du/dx = -5/2*u

        return (log(x+1)/(x*x+1));
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

    std::pair<double, double> RKIV(double x, double u, double h) { // реализация метода

        // необходимо как-то придумать, чтобы вызывалась функция в зависимости от требуемой
        // если надо посчитать test_func, то вызывается test_func, если надо посчитать main_func_1/2, то вызывается main_func_1/2

        double k1 = test_func(x, u); // считаем k1
        double k2 = test_func(x + h / 2, u + h / 2 * k1); // считаем k2
        double k3 = test_func(x + h / 2, u + h / 2 * k2); // считаем k3
        double k4 = test_func(x + h, u + h * k3); // считаем k4

        return (std::make_pair( getNewX(x, h), getNewU({k1,k2,k3,k4}, u, h))); // создаем пару (x,u) из чисел, полученных через getNewX, getNewU
    }

    Actions_with_H checkUpDown(double u_with_h, double u_with_twice_half_h) {
        
        double S = abs(getS(u_with_h, u_with_twice_half_h)); // получаем S и сразу берем от неё модуль

        std::cout << m_E_check_down << std::endl;
        std::cout << S << std::endl;

        if (S >= m_E_check_down && S <= m_E_check_up) {
            std::cout << "ALL GOOD" << std::endl;
            return Actions_with_H::NOTHING;
        }
        else if (S < m_E_check_down) {
            std::cout << "H * 2" << std::endl;
            return Actions_with_H::MULTIPLY_BY_2;
        }
        else if (S > m_E_check_up) {
            std::cout << "H / 2" << std::endl;
            return Actions_with_H::DIVIDE_BY_2_AND_RECALCULATE;
        }
    }

    Actions_with_H checkRight(double x) {
        if (x > m_b)
            return Actions_with_H::GET_LAST;
        else if (x == m_b)
            return Actions_with_H::STOP;
        else if (x <= m_b - m_E_check_right)
            return Actions_with_H::NOTHING;
    }

    std::vector<std::pair<double, double>> run(double x0, double u0) {
        std::vector<std::pair<double, double>> data;
        
        data.push_back({ x0,u0 });

        Actions_with_H act = Actions_with_H::NOTHING;
        std::pair<double, double> coords_with_h;

        int k = 0;

        while (true) {
            std::pair<double, double> current_coord = data.back();
            
            coords_with_h = RKIV(current_coord.first, current_coord.second, m_h);
            std::pair<double, double> coords_with_half_h = RKIV(current_coord.first, current_coord.second, m_h / 2);
            std::pair<double, double> coords_with_twice_half_h = RKIV(coords_with_half_h.first, coords_with_half_h.second, m_h / 2);


            Actions_with_H act1 = checkUpDown(coords_with_h.second, coords_with_twice_half_h.second);
            Actions_with_H act2 = checkRight(coords_with_h.first);

            if (act2 == Actions_with_H::GET_LAST) {
                m_h = m_b - data.back().first;
            }
            else if (act2 == Actions_with_H::STOP) {
                data.push_back(coords_with_h);
                vector_of_h.push_back(m_h);
                std::cout << "STOP" << std::endl;
                break;
            }
            else if (act2 == Actions_with_H::NOTHING) {
                if (act1 == Actions_with_H::MULTIPLY_BY_2) {
                    data.push_back(coords_with_h);
                    vector_of_h.push_back(m_h);
                    m_h *= 2;
                    std::cout << "MULTIPLY_BY_2" << std::endl;
                    --m_N_max;
                }
                else if (act1 == Actions_with_H::NOTHING)
                {
                    data.push_back(coords_with_h);
                    vector_of_h.push_back(m_h);
                    std::cout << "NOTHING" << std::endl;
                    --m_N_max;
                }
                else if (act1 == Actions_with_H::DIVIDE_BY_2_AND_RECALCULATE) {
                    m_h /= 2;
                    std::cout << "DIVIDE_BY_2_AND_RECALCULATE" << std::endl;
                }
            }

            std::cout << "+++++++++++++++++++" << std::endl;

        }
        
        for (int count = 0; count < data.size(); ++count)
            std::cout << data[count].first << "\t " << data[count].second << std::endl;

        std::cout << "+++++++++++++++++++" << std::endl;

        for (int count = 0; count < vector_of_h.size(); ++count)
            std::cout << vector_of_h[count] << std::endl;

        return data;
    }
};

/*

Не забудь починить хрень с выходом за правую границу, чтобы мы не ошивались у m_b пока N > 0

*/