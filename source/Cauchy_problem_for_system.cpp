#include "imgui.h"
#include "imgui-SFML.h"

#include "imconfig.h"
#include "imconfig-SFML.h"

#include "SFML/Graphics.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <vector>
#include <iostream>


class RK_IV {
private:
    std::vector<std::pair<double, double>> m_data; // хранит точки (x,y)
    double m_h = 10e-5; // численный шаг интегрирования
    int m_N_max = 10000; // 
    double m_E_check_right = 0.5*10e-6;
    double m_E_check_up = 0.5 * 10e-4;
    double m_E_check_down = m_E_check_up / (2 >> 4);

    double a = 0;
    double b = 100.0;

public:

    RK_IV() {
        m_data.push_back({ 0,0 });
    }

    double getNewX(double x, double h) {
        return ( x + h );
    }

    double func(double x, double y) {
        return (2 * x - y + x * x);
    }

    std::vector<double> findPoint(double x ,double y, double h) {
        std::vector<double> result(4);

        result[0] = func(x, y);
        result[1] = func(x + h / 2, y + h * result[0] / 2);
        result[2] = func(x + h / 2, y + h * result[1] / 2);
        result[3] = func(x + h / 2, y + h * result[2]);

        return result;
    }

    std::vector<double> calc(double x, double y) {
        std::vector<double> points(3);

        std::vector<double> result = findPoint(x, y, m_h);
        double newXwithH = getNewX(x, m_h);
        double newYwithH = y + m_h / 6 * (result[0] + 2 * result[1] + 2 * result[2] + result[3]);

        double XwithHalfH = getNewX(x, m_h / 2);
        std::vector<double> result_sec = findPoint(x, y, m_h / 2);
        double newYHalfH = y + m_h / 6 * (result_sec[0] + 2 * result_sec[1] + 2 * result_sec[2] + result_sec[3]);

        double NewXWithTwiceHalfH = getNewX(XwithHalfH, m_h / 2);
        std::vector<double> result_third = findPoint(XwithHalfH, newYHalfH, m_h / 2);
        double NewYWithTwiceHalfH = newYHalfH + m_h / 6 * (result_third[0] + 2 * result_third[1] + 2 * result_third[2] + result_third[3]);

        points[0] = newXwithH;
        points[1] = newYwithH;
        points[2] = NewYWithTwiceHalfH;

        return points;
    }

    bool check(int count) {
        std::vector<double> points = calc(m_data[count].first, m_data[count].second);


        double S = (points[2] - points[1]) / (2 << 3 + 1);


        if (abs(S) >= m_E_check_down && abs(S) <= m_E_check_up) {
            m_data.push_back({ points[0] , points[1] });
            std::cout << "H" << std::endl;
            return true;
        }
        else if (abs(S) < m_E_check_down) {
            m_data.push_back({ points[0] , points[1] });
            m_h = m_h * 2;
            std::cout << "H*2" << std::endl;
            return true;
        }
        else {
            m_h = m_h / 2;
            std::cout << "H/2" << std::endl;
            return false;
        }
    }

    void run() {
        int count = 0;
        while (m_data[count].first <= b) {
            while (!check(count)) {
            }
            ++count;
        }
    }

    void print() {
        int count = 0;
        while (m_data.size() > count) {
            std::cout << "x" << count << " = " << m_data[count].first << "\t|\ty" << count << " = " << m_data[count].second << std::endl;
            ++count; 
        }
    }
};

int main() {

    RK_IV rkiv;
    rkiv.run();
    rkiv.print();

    return 0;
}