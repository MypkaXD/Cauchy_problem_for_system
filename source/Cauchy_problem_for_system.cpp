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
    double b = 0.2;

public:

    RK_IV() {
        m_data.push_back({ 0,0 });
    }

    double getNewX(int count) {
        return ( a + m_h * count);
    }

    double func(double x, double y) {
        return (2 * x - y + x * x);
    }

    void run() {
        int count = 0;
        while (m_data[count].first <= b) {
            double c0 = func(m_data[count].first, m_data[count].second);
            double c1 = func(m_data[count].first + m_h / 2, m_data[count].second + m_h * c0 / 2);
            double c2 = func(m_data[count].first + m_h / 2, m_data[count].second + m_h * c1 / 2);
            double c3 = func(m_data[count].first + m_h / 2, m_data[count].second + m_h * c2);
            m_data.push_back({ getNewX(count), m_data[count].second + m_h / 6 * (c0 + 2 * c1 + 2 * c2 + c3) });
            ++count;
        }
    }

    void print() {
        int count = 0;
        while (!m_data.empty()) {
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