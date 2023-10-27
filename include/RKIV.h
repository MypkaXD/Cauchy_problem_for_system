#include <vector>
#include <iostream>
#include <tuple>
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
    TEST_FUNC = 0,
    FIRST_TASK = 1,
    SECOND_TASK = 2
};

class RK {
private:

    double m_h_start = 0.1; // ��������� ��� �������������� (���������)
    double m_E_check_right = 0.5 * 10e-6; // �������� ��� "������" ������
    double m_E_check_up = 0.5 * 10e-4; // �������� ��� "������"
    double m_E_check_down = ((m_E_check_up ) / 32); // �������� ��� "����"

    double m_a = 0; // ����� �������
    double m_b = 1; // ������ �������

    int m_N_max = 100; // ������������ ����� �����
    int p = 4; // ������� ������

    std::vector<double> m_vector_of_h; // ������ ��� �������� �����
    std::vector<std::tuple<double, double, double>> m_data; // ������ m_data ��� �������� ����� x � u
    std::vector<std::pair<double, double>> m_analytical_solution_data; // ������ ��� �������� �������������� �������
    std::vector<double> m_twice_half_h_u; // ������ ��� �������� �����, ����������� � ������� �����
    std::vector<double> difference_of_v; // ������ �������� ����� � ������� ����� � �������
    std::vector<double> vector_S; // ������ ���
    std::vector<double> difference_of_u; // ������ �������� �����, ���������� �������� � ������������
    std::vector<double> m_vecotor_u; // ������ ��� ����� �������������� �������

    std::vector<int> C1; // ������ ���-�� ������� ����
    std::vector<int> C2; // ������ ���-�� ��������� ����


    double test_func(double x, double u) { // ���������� ������� du/dx ��� �������� ������
        // � ��� 5-�� �������� => du/dx = (-1)^5*5/2*u => du/dx = -5/2*u

        return (-5/2*u);
    }

    double func_of_first_task(double x, double u) { // ���������� ������� du/dx ��� ������ �1
        // � ��� 5-�� �������� => du/dx = ln(x+1)/(x^2+1)

        return ((log(x + 1) / (x * x + 1)) * u * u + u - u * u * u * sin(10 * x));
    }

    double func_of_second_task_for_f(double x, double u, double u_, double a, double b) {
        return ((1+x*x)*(1+x*x)*u_-2*x*u);
        
        //return (-1)*(u_ * u_ * a + b * sin(u));
    }

    double func_of_second_task_for_g(double x, double u, double u_) {
        return u_;
    }

    double test_func_analytical_solution(double x) {
        return (exp(((-5) / 2) * (x)));
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

    std::tuple<double, double, double> RKIV(double x, double u, double h, Task task, double u_ = 0, double a = 1, double b = 2) { // ���������� ������

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
        else if (task == Task::SECOND_TASK) {
            k1 = h * func_of_second_task_for_f(x, u, u_, a, b);
            double l1 = h * func_of_second_task_for_g(x, u, u_);

            k2 = h * func_of_second_task_for_f(x + h / 2, u + k1 / 2, u_ + l1 / 2, a, b);
            double l2 = h * func_of_second_task_for_g(x + h / 2, u + k1 / 2, u_ + l1 / 2);

            k3 = h * func_of_second_task_for_f(x + h / 2, u + k2 / 2, u_ + l2 / 2, a, b);
            double l3 = h * func_of_second_task_for_g(x + h / 2, u + k2 / 2, u_ + l2 / 2);

            k4 = h * func_of_second_task_for_f(x + h, u + k3, u_ + l3, a, b);
            double l4 = h * func_of_second_task_for_g(x + h, u + k3, u_ + l3);

            u_ += (l1 + 2 * l2 + 2 * l3 + l4) / 6;
        }

        return  std::make_tuple(getNewX(x, h), getNewU({k1,k2,k3,k4}, u, h) , u_); // ������� ���� (x,u) �� �����, ���������� ����� getNewX, getNewU
    }

    Actions_with_H checkUpDown(double u_with_h, double u_with_twice_half_h) {
        
        double S = abs(getS(u_with_h, u_with_twice_half_h)); // �������� S � ����� ����� �� �� ������

        vector_S.push_back(S);

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
        else if (x == m_b || (x > m_b - m_E_check_right && x < m_b))
            return Actions_with_H::STOP; // ���� �� ������ ����� �� ������ �������, �� �� �������� ��������� ������� � �����������
        else
            return Actions_with_H::NOTHING; // ���� �� ������ ����� ������� - ������-�������, �� ������ �� ������ � ���������� ������
    }

    void calculate_with_e(double x0, double u0, Task task, double u_0 = 0, double a = 0, double b = 0) {

        m_data.push_back({ x0,u0, u_0 }); // ����� � ������ ��������� �������
        m_vector_of_h.push_back(m_h_start);
        m_twice_half_h_u.push_back(u0);
        difference_of_v.push_back(0);
        vector_S.push_back(0);
        C2.push_back(0);
        C1.push_back(0);

        if (task == Task::TEST_FUNC) {
            m_vecotor_u.push_back(test_func_analytical_solution(x0));
            difference_of_u.push_back(0);
        }
        

        std::tuple<double, double, double> coords_with_h; // ������� ������ coords_with_h ��� �������� ����� x � u � ����� h

        int C1count = 0;
        int C2count = 0;

        double h = m_h_start;

        int N = m_N_max;

        while (N > 0) {
            std::tuple<double, double, double>  current_coord = m_data.back(); // �������� ������� ���������� (x_n, u_n)
            std::tuple<double, double, double>  coords_with_half_h;
            std::tuple<double, double, double>  coords_with_twice_half_h;

            coords_with_h = RKIV(std::get<0>(current_coord), std::get<1>(current_coord), h, task, std::get<2>(current_coord),a, b); // �������� ����� (x_n+1, u_n+1) � ����� h �� ����� (x_n, u_n)
            coords_with_half_h = RKIV(std::get<0>(current_coord), std::get<1>(current_coord), h / 2, task, std::get<2>(current_coord),a,b); //  �������� ����� (x_n+1/2, u_n+1/2) �� ����� (x_n, u_n) � ����� h/2
            coords_with_twice_half_h = RKIV(std::get<0>(coords_with_half_h), std::get<1>(coords_with_half_h), h / 2, task, std::get<2>(current_coord),a,b); // �������� ����� (x_n+1, u_n+1) �� ����� (x_n+1/2, u_n+1/2) � ����� h / 2


            m_twice_half_h_u.push_back(std::get<1>(coords_with_twice_half_h));
            difference_of_v.push_back(std::get<1>(coords_with_h) - std::get<1>(coords_with_twice_half_h));

            Actions_with_H act1 = checkUpDown(std::get<1>(coords_with_h), std::get<1>(coords_with_twice_half_h)); // �������� �� ����� �� ��������� �����������
            Actions_with_H act2 = checkRight(std::get<0>(coords_with_h)); // �������� �� ����� �� ������ �������

            if (act2 == Actions_with_H::GET_LAST) {
                h = m_b - std::get<0>(m_data.back()); // ������������� ���, ������� �������� ��� ����� � ������ ������ m_b
            }
            else if (act2 == Actions_with_H::NOTHING || act2 == Actions_with_H::STOP) { // ���� �� �� ����� �� ������ ������� - �������� ������ NOTHING ��� �� ������� ��������� �����
                if (act1 == Actions_with_H::MULTIPLY_BY_2) { // ���� ��������� ��� �������� �� ��� ����� �������� �� ���. �����������
                    m_data.push_back(coords_with_h); // ��������� �����
                    m_vector_of_h.push_back(h); // ��������� ���
                    if (task == Task::TEST_FUNC) {
                        m_vecotor_u.push_back(test_func_analytical_solution(std::get<0>(coords_with_h)));
                        difference_of_u.push_back(abs(m_vecotor_u.back() - std::get<1>(coords_with_h)));
                    }
                    --N;
                    h *= 2; // �������� ��� �� 2
                    ++C2count;
                    C2.push_back(C2count);
                    C1.push_back(C1count);
                    C2count = C1count = 0;

                    if (act2 == Actions_with_H::STOP)
                        break;
                }
                else if (act1 == Actions_with_H::NOTHING) // ���� ��� ������ �� ���� ������ � �����, ��������� ����� � ������� ���. �����������
                {
                    m_data.push_back(coords_with_h); // ��������� �����
                    m_vector_of_h.push_back(h); // ��������� ���
                    if (task == Task::TEST_FUNC) {
                        m_vecotor_u.push_back(test_func_analytical_solution(std::get<0>(coords_with_h)));
                        difference_of_u.push_back(abs(m_vecotor_u.back() - std::get<1>(coords_with_h)));
                    }
                    --N;
                    C2.push_back(C2count);
                    C1.push_back(C1count);
                    C2count = C1count = 0;

                    if (act2 == Actions_with_H::STOP)
                        break;
                }
                else if (act1 == Actions_with_H::DIVIDE_BY_2_AND_RECALCULATE) { // ���� ��� ���� �������� ��� �� ��� � ����������� �����
                    h /= 2; // ����� ��� �� ���
                    ++C1count;
                }
            }

        }
    }

    void calculate_with_constH(double x0, double u0, Task task, double u_0 = 0, double a = 0, double b = 0) {
        m_data.push_back({ x0,u0, u_0 }); // ����� � ������ ��������� �������

        if (task == Task::TEST_FUNC) {
            m_vecotor_u.push_back(test_func_analytical_solution(x0));
            difference_of_u.push_back(0);
        }

        std::tuple<double, double, double> coords_with_h; // ������� ������ coords_with_h ��� �������� ����� x � u � ����� h

        double h = (m_b - m_a) / m_N_max;
        int N = m_N_max;

        m_vector_of_h.push_back(h);

        while (N > 0) {
            std::tuple<double, double, double> current_coord = m_data.back(); // �������� ������� ���������� (x_n, u_n)

            coords_with_h = RKIV(std::get<0>(current_coord), std::get<1>(current_coord), h, task, std::get<2>(current_coord), a, b); // �������� ����� (x_n+1, u_n+1) � ����� h �� ����� (x_n, u_n)

            Actions_with_H act = checkRight(std::get<0>(coords_with_h)); // �������� �� ����� �� ������ �������

            if (act == Actions_with_H::NOTHING) { // ���� �� �� ����� �� ������ ������� - �������� ������ NOTHING ��� �� ������� ��������� �����
                m_data.push_back(coords_with_h); // ��������� �����
                m_vector_of_h.push_back(h); // ��������� ���
                --N;
                if (task == Task::TEST_FUNC) {
                    m_vecotor_u.push_back(test_func_analytical_solution(std::get<0>(coords_with_h)));
                    difference_of_u.push_back(abs(m_vecotor_u.back() - std::get<1>(coords_with_h)));
                }
            }
            else if (act == Actions_with_H::STOP || act == Actions_with_H::GET_LAST) {
                m_data.push_back(coords_with_h); // ��������� �����
                m_vector_of_h.push_back(h); // ��������� ���
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

        for (int count = 0; count < m_N_max; ++count) {
            x += h;
            m_analytical_solution_data.push_back({ x, test_func_analytical_solution(x)});
            //std::cout << "COORD " << count << ":\t(" << m_analytical_solution_data.back().first << "; " << m_analytical_solution_data.back().second << ")" << std::endl;
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
        m_vector_of_h.clear(); // ������ ��� �������� �����
        m_data.clear(); // ������ m_data ��� �������� ����� x � u
        m_analytical_solution_data.clear(); // ������ ��� �������� �������������� �������
        m_twice_half_h_u.clear(); // ������ ��� �������� �����, ����������� � ������� �����
        difference_of_v.clear(); // ������ �������� ����� � ������� ����� � �������
        vector_S.clear(); // ������ ���
        C1.clear(); // ������ ���-�� ������� ����
        C2.clear(); // ������ ���-�� ��������� ����
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