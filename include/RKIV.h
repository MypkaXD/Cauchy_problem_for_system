#include <vector>
#include <iostream>

#include <math.h>

#pragma once

enum class Actions_with_H { // enum class ��� ��������� ������� ����
    MULTIPLY_BY_2, // ���������� �������� �� ���
    NOTHING, // ������ �� ������
    DIVIDE_BY_2_AND_RECALCULATE, // ����� �� ��� � �������������
    GET_LAST, // �������� ��������� �����
    STOP // ���������������
};

enum class Task {
    TEST_FUNC,
    FIRST_TASK,
    SECOND_TASK
};

class RK {
private:

    double m_h = 0.1; // ��������� ��� ��������������
    double m_E_check_right = 0.5 * 10e-6; // �������� ��� "������" ������
    double m_E_check_up = 0.5 * 10e-4; // �������� ��� "������"
    double m_E_check_down = ((m_E_check_up ) / 32); // �������� ��� "����"

    double m_a = 0; // ����� �������
    double m_b = 1; // ������ �������

    int m_N_max = 100; // ������������ ����� �����
    int p = 4; // ������� ������

    std::vector<double> m_vector_of_h; // ������ ��� �������� �����
    std::vector<std::pair<double, double>> m_data; // ������ m_data ��� �������� ����� x � u
    std::vector<std::pair<double, double>> m_analytical_solution_data; // ������ ��� �������� �������������� �������
    std::vector<double> m_twice_half_h_u; // ������ ��� �������� �����, ����������� � ������� �����
    std::vector<double> difference_of_u; // ������ �������� ����� � ������� ����� � �������
    std::vector<double> vector_S; // ������ ���

    std::vector<int> C1; // ������ ���-�� ������� ����
    std::vector<int> C2; // ������ ���-�� ��������� ����


    double test_func(double x, double u) { // ���������� ������� du/dx ��� �������� ������
        // � ��� 5-�� �������� => du/dx = (-1)^5*5/2*u => du/dx = -5/2*u

        return (-5/2*u);
    }

    double func_of_first_task(double x, double u) { // ���������� ������� du/dx ��� ������ �1
        // � ��� 5-�� �������� => du/dx = ln(x+1)/(x^2+1)

        return (log(x + 1) / (x * x + 1));
    }

    double getNewX(double x, double h) { // �������� ����� x
        return (x + h); // ������� ����� x
    }

    double getNewU(std::vector<double> k, double u, double h) { // �������� ����� u
        if (k.size() != 4) // ����������� ������, ���� ������ ������� k-������������� != 4. ����� ��������� ����������, �� ���� �����
            throw ("ERROR: The vector of K-koef isn't contain 4 elems");
        else {
            return (u + h / 6 * (k[0] + 2 * k[1] + 2 * k[2] + k[3])); // ������� ����� u
        }
    }

    double getS(double u_with_h, double u_with_twice_half_h) {
        return ((u_with_twice_half_h - u_with_h) / ((2 << (p - 1)) - 1)); // ������� S �� ������� S = (v_h/2(n+1) - v_h(n+1)) / (2^p - 1)
    }

    std::pair<double, double> RKIV(double x, double u, double h, Task task) { // ���������� ������

        // ���������� ���-�� ���������, ����� ���������� ������� � ����������� �� ���������
        // ���� ���� ��������� test_func, �� ���������� test_func, ���� ���� ��������� main_func_1/2, �� ���������� main_func_1/2

        double k1, k2, k3, k4;

        if (task == Task::TEST_FUNC) {
            k1 = test_func(x, u); // ������� k1
            k2 = test_func(x + h / 2, u + h / 2 * k1); // ������� k2
            k3 = test_func(x + h / 2, u + h / 2 * k2); // ������� k3
            k4 = test_func(x + h, u + h * k3); // ������� k4
        }
        else if (task == Task::FIRST_TASK) {
            k1 = func_of_first_task(x, u); // ������� k1
            k2 = func_of_first_task(x + h / 2, u + h / 2 * k1); // ������� k2
            k3 = func_of_first_task(x + h / 2, u + h / 2 * k2); // ������� k3
            k4 = func_of_first_task(x + h, u + h * k3); // ������� k4
        }

        return (std::make_pair( getNewX(x, h), getNewU({k1,k2,k3,k4}, u, h))); // ������� ���� (x,u) �� �����, ���������� ����� getNewX, getNewU
    }

    Actions_with_H checkUpDown(double u_with_h, double u_with_twice_half_h) {
        
        double S = abs(getS(u_with_h, u_with_twice_half_h)); // �������� S � ����� ����� �� �� ������

        vector_S.push_back(S);

        //std::cout << m_E_check_down << std::endl;
        //std::cout << S << std::endl;

        if (S >= m_E_check_down && S <= m_E_check_up) { // ���� S ��������� � ������� "��������� �����������" �� ���������� ������ NOTHING
            return Actions_with_H::NOTHING;
        }
        else if (S < m_E_check_down) { // ���� S ��������� ���� ������ �������, �� ���������� ������ MULTIPLY_BY_2
            return Actions_with_H::MULTIPLY_BY_2;
        }
        else if (S > m_E_check_up) { // ���� S ��������� ���� ������� �������, �� ���������� ������ DIVIDE_BY_2_AND_RECALCULATE
            return Actions_with_H::DIVIDE_BY_2_AND_RECALCULATE;
        }
    }

    Actions_with_H checkRight(double x) {
        if (x > m_b)
            return Actions_with_H::GET_LAST; // ���� �� ������ �� ������ �������, �� ��� ���������� ��������� � �������� ��������� �������
        else if (x == m_b)
            return Actions_with_H::STOP; // ���� �� ������ ����� �� ������ �������, �� �� �������� ��������� ������� � �����������
        else if (x <= m_b - m_E_check_right)
            return Actions_with_H::NOTHING; // ���� �� ������ ����� �������-������-�������, �� ������ �� ������ � ���������� ������
    }

    void calculate(double x0, double u0, Task task) {

        m_data.push_back({ x0,u0 }); // ����� � ������ ��������� �������
        m_vector_of_h.push_back(0);
        m_twice_half_h_u.push_back(u0);

        std::pair<double, double> coords_with_h; // ������� ������ coords_with_h ��� �������� ����� x � u � ����� h

        int C1count = 0;
        int C2count = 0;


        while (true) {
            std::pair<double, double> current_coord = m_data.back(); // �������� ������� ���������� (x_n, u_n)
            std::pair<double, double> coords_with_half_h;
            std::pair<double, double> coords_with_twice_half_h;

            coords_with_h = RKIV(current_coord.first, current_coord.second, m_h, task); // �������� ����� (x_n+1, u_n+1) � ����� h �� ����� (x_n, u_n)
            coords_with_half_h = RKIV(current_coord.first, current_coord.second, m_h / 2, task); //  �������� ����� (x_n+1/2, u_n+1/2) �� ����� (x_n, u_n) � ����� h/2
            coords_with_twice_half_h = RKIV(coords_with_half_h.first, coords_with_half_h.second, m_h / 2, task); // �������� ����� (x_n+1, u_n+1) �� ����� (x_n+1/2, u_n+1/2) � ����� h / 2

            m_twice_half_h_u.push_back(coords_with_twice_half_h.second);
            difference_of_u.push_back(coords_with_twice_half_h.second - coords_with_h.second);

            Actions_with_H act1 = checkUpDown(coords_with_h.second, coords_with_twice_half_h.second); // �������� �� ����� �� ��������� �����������
            Actions_with_H act2 = checkRight(coords_with_h.first); // �������� �� ����� �� ������ �������

            if (act2 == Actions_with_H::GET_LAST) {
                m_h = m_b - m_data.back().first; // ������������� ���, ������� �������� ��� ����� � ������ ������ m_b
            }
            else if (act2 == Actions_with_H::STOP) {
                m_data.push_back(coords_with_h); // ��������� �����
                m_vector_of_h.push_back(m_h); // ��������� ���
                break;
            }
            else if (act2 == Actions_with_H::NOTHING) { // ���� �� �� ����� �� ������ ������� - �������� ������ NOTHING
                if (act1 == Actions_with_H::MULTIPLY_BY_2) { // ���� ��������� ��� �������� �� ��� ����� �������� �� ���. �����������
                    m_data.push_back(coords_with_h); // ��������� �����
                    m_vector_of_h.push_back(m_h); // ��������� ���
                    m_h *= 2; // �������� ��� �� 2
                    //++C2;
                }
                else if (act1 == Actions_with_H::NOTHING) // ���� ��� ������ �� ���� ������ � �����, ��������� ����� � ������� ���. �����������
                {
                    m_data.push_back(coords_with_h); // ��������� �����
                    m_vector_of_h.push_back(m_h); // ��������� ���
                }
                else if (act1 == Actions_with_H::DIVIDE_BY_2_AND_RECALCULATE) { // ���� ��� ���� �������� ��� �� ��� � ����������� �����
                    m_h /= 2; // ����� ��� �� ���
                    //++C1;
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

    void clear() {
        m_data.clear();
        m_vector_of_h.clear();
        m_analytical_solution_data.clear();
        m_twice_half_h_u.clear();
        difference_of_u.clear();
        vector_S.clear();

        C1.clear();
        C2.clear();

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
        m_h = h;
    }

    double get_E_right() {
        return m_E_check_right;
    }

    double get_E_local() {
        return m_E_check_up;
    }

    double get_Start_H() {
        return m_h;
    }

    void test_func(double x0, double u0, Task task) {
        /*
        ����:
            ������� ��� ������� ��� ���� �����. ������ ������� ����� ��������� run(x0,u0)
            ��� �������� ������ ���������� ������ �������� � ������������ =>
            ����� ��� ������� ��� �����:
                1) ������ � ��������� ��������
                2) ������ � ������������� ��������
            ���� ����� ���-�� �������� � run, ����� ������� ������������ � RKIV.
            ��������, ����� ������� enum_class � 3-��� ����������
        
        
        */
        if (task == Task::TEST_FUNC)
            analytical_solution();
        
        calculate(x0, u0, task);
    }
};