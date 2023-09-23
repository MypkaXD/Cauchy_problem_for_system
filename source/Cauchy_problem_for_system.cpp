#include "imgui.h"
#include "imgui-SFML.h"

#include "imconfig.h"
#include "imconfig-SFML.h"

#include "SFML/Graphics.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <vector>
#include <iostream>

double getNewX(double x, double h,double a, int i) {

    x = a + h * i;

    return x;
}

double func(double x, double y) {
    return (2 * x - y + x * x);
}

void run(std::vector<std::pair<double, double>>& Table, double a, double b, double h) {
    int count = 0;
    Table.push_back({ getNewX(0, h, a, count), 0 });
    while (Table[count].first <= b) {
        std::cout << Table[count].first << "||||" << Table[count].second << std::endl;
        Table[count+1].first = getNewX(Table[count].first, h, a, count);
        double c0 = func(Table[count].first, Table[count].second);
        double c1 = func(Table[count].first + h / 2, Table[count].second + h * c0 / 2);
        double c2 = func(Table[count].first + h / 2, Table[count].second + h * c1 / 2);
        double c3 = func(Table[count].first + h / 2, Table[count].second + h * c2);
        Table[count+1].second = (Table[count].second + h / 6 * (c0 + 2 * c1 + 2 * c2 + c3));
        ++count;
    }
}

bool checkUp(double y) {

}


int main() {
    double a, b;



    std::vector<std::pair<double,double>> Table(100);
    double t;
    double h = 10e-5;
    int N = 10e3;
    double e_right = 0.5 * 10e-6;
    double e_up = 0.5*10e-4;
    double e_min = e_up / (2 << 4);
    std::cout << (2 << 4) << std::endl;
    std::cout << h << std::endl;
    std::cout << e_min << std::endl;

    int n;
    std::cin >> a;
    std::cin >> b;
    std::cin >> n;
    h = (a + b) / n;

    run(Table, a, b, h);


    return 0;
}