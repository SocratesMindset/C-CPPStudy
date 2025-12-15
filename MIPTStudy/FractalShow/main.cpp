// Draw a Sierpinski triangle fractal (step-by-step) WITHOUT vec2d

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>

#include "Graph_lib/Graph.h"
#include "Graph_lib/Simple_window.h"

using namespace Graph_lib;

// Точка в double-координатах (нам нужно делить на 2 и т.п.)
struct DPoint {
  double x;
  double y;
};

// Треугольник в double-координатах
struct TriangleD {
  DPoint a;
  DPoint b;
  DPoint c;
};

// середина двух точек
DPoint mid(const DPoint& p, const DPoint& q) {
  return DPoint{ (p.x + q.x) * 0.5, (p.y + q.y) * 0.5 };
}

// длина отрезка
double dist(const DPoint& p, const DPoint& q) {
  const double dx = p.x - q.x;
  const double dy = p.y - q.y;
  return std::sqrt(dx*dx + dy*dy);
}

// самая длинная сторона у одного треугольника
double max_edge_length(const TriangleD& t) {
  const double ab = dist(t.a, t.b);
  const double bc = dist(t.b, t.c);
  const double ca = dist(t.c, t.a);
  return std::max({ab, bc, ca});
}

// самая длинная сторона среди всех треугольников
double max_edge_length(const std::vector<TriangleD>& tris) {
  double m = 0.0;
  for (const auto& t : tris) {
    m = std::max(m, max_edge_length(t));
  }
  return m;
}

// Один шаг фрактала Серпинского: каждый треугольник → 3 треугольника
void sierpinski_step(std::vector<TriangleD>& tris) {
  std::vector<TriangleD> next;
  next.reserve(tris.size() * 3);

  for (const auto& t : tris) {
    const DPoint ab = mid(t.a, t.b);
    const DPoint bc = mid(t.b, t.c);
    const DPoint ca = mid(t.c, t.a);

    next.push_back(TriangleD{t.a, ab, ca});
    next.push_back(TriangleD{ab, t.b, bc});
    next.push_back(TriangleD{ca, bc, t.c});
  }

  tris.swap(next);
}

// перевести DPoint -> Graph_lib::Point (int)
Point to_point(const DPoint& p) {
  return Point{ int(std::lround(p.x)), int(std::lround(p.y)) };
}

// TriangleD -> Closed_polyline (его можно attach к окну)
std::unique_ptr<Closed_polyline> make_polyline(const TriangleD& t) {
  auto p = std::make_unique<Closed_polyline>();
  p->add(to_point(t.a));
  p->add(to_point(t.b));
  p->add(to_point(t.c));
  return p;
}

void draw_sierpinski(int w) {
  Simple_window win{Point{100, 100}, w, w, "Sierpinski triangle"};
  const int max_steps = 12;

  // стартовый большой треугольник
  const DPoint A{ w / 2.0, 0.06 * w };
  const DPoint B{ 0.08 * w, 0.92 * w };
  const DPoint C{ 0.92 * w, 0.92 * w };

  std::vector<TriangleD> tris;
  tris.push_back(TriangleD{A, B, C});

  Text step_text{Point{10, 20}, "step: 0"};
  step_text.set_color(Color::blue);
  win.attach(step_text);

  int step = 0;

  for (bool done = false; !done; ) {
    // рисуем текущий набор треугольников
    std::vector<std::unique_ptr<Closed_polyline>> shapes;
    shapes.reserve(tris.size());

    for (const auto& t : tris) {
      auto shape = make_polyline(t);
      shape->set_color(Color::blue);
      win.attach(*shape);
      shapes.push_back(std::move(shape));
    }

    win.wait_for_button();

    // убираем старые фигуры
    for (auto& s : shapes) {
      win.detach(*s);
    }

    // следующий шаг
    sierpinski_step(tris);
    ++step;

    // считаем шаги; держим минимум max_steps итераций, дальше стоп
    done = step >= max_steps;

    step_text.set_label("step: " + std::to_string(step) + "/" + std::to_string(max_steps));
    if (done) step_text.set_color(Color::red);
  }

  // финальный кадр — рисуем красным и ждём кнопку
  std::vector<std::unique_ptr<Closed_polyline>> final_shapes;
  final_shapes.reserve(tris.size());
  for (const auto& t : tris) {
    auto shape = make_polyline(t);
    shape->set_color(Color::red);
    win.attach(*shape);
    final_shapes.push_back(std::move(shape));
  }
  win.wait_for_button();
}

void help(const char prog[]) {
  std::cerr
    << "Generate Sierpinski triangle fractal (step-by-step)\n\n"
    << "Usage: " << prog << " [-h|--help] [window width]\n"
    << "Example: " << prog << " 600\n\n";
}

int main(int argc, char* argv[])
try {
  int w = 600;

  if (argc > 1) w = std::stoi(argv[1]);
  if (argc > 2) { help(argv[0]); return 2; }

  draw_sierpinski(w);
}
catch (std::invalid_argument&) {
  for (int i = 1; i < argc; ++i) {
    std::string arg{argv[i]};
    if (arg == "-h" || arg == "--help") { help(argv[0]); return 0; }
  }
  std::cerr << "not an integer\n";
  return 2;
}
catch (std::exception& e) {
  std::cerr << e.what() << '\n';
  return 1;
}
catch (...) {
  std::cerr << "Oops, something went wrong...\n";
  return 2;
}
