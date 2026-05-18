#pragma once

#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <random>

const double EPS = 1e-8;

struct Point2D {
    double x, y;

    bool operator==(const Point2D& other) const {
        return std::abs(x - other.x) < EPS && std::abs(y - other.y) < EPS;
    }

    bool operator!=(const Point2D& other) const {
        return !(*this == other);
    }
};

struct LineSegment {
    Point2D start;
    Point2D end;
};

struct Circle {
    Point2D center;
    double radius;

    bool isValid() const {
        return radius >= 0;
    }

    bool contains(const Point2D& point) const {
        double dx = point.x - center.x;
        double dy = point.y - center.y;
        return dx * dx + dy * dy <= radius * radius + EPS;
    }

    bool contains(const LineSegment& segment) const {
        return contains(segment.start) && contains(segment.end);
    }

    bool containsEntireSegment(const LineSegment& segment) const {
        if (!contains(segment.start) || !contains(segment.end)) return false;
        
        Point2D mid = {(segment.start.x + segment.end.x) / 2.0,
                       (segment.start.y + segment.end.y) / 2.0};
        
        for (double t = 0.1; t < 1.0; t += 0.2) {
            Point2D point = {segment.start.x + t * (segment.end.x - segment.start.x),
                             segment.start.y + t * (segment.end.y - segment.start.y)};
            if (!contains(point)) return false;
        }
        
        return contains(mid);
    }
};

// Объявления функций (реализация в .cpp)
Circle MinimumEnclosingCircleForSegments(const std::vector<LineSegment>& segments);
double distanceSquared(const Point2D& p1, const Point2D& p2);
double distance(const Point2D& p1, const Point2D& p2);
Circle circleFromDiameter(const Point2D& p1, const Point2D& p2);
Circle circleFrom3Points(const Point2D& p1, const Point2D& p2, const Point2D& p3);
Circle minCircleHelper(std::vector<Point2D>& points, std::vector<Point2D> boundary, size_t n);
Circle minCircle(std::vector<Point2D> points);
std::vector<Point2D> extractPointsFromSegments(const std::vector<LineSegment>& segments);
Point2D findMidPoint(const LineSegment& segment);
