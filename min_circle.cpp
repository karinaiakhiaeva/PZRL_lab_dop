#include "min_circle.h"
#include <random>
#include <algorithm>


double distanceSquared(const Point2D& p1, const Point2D& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return dx * dx + dy * dy;
}

double distance(const Point2D& p1, const Point2D& p2) {
    return std::sqrt(distanceSquared(p1, p2));
}

Point2D findMidPoint(const LineSegment& segment) {
    return {(segment.start.x + segment.end.x) / 2.0,
            (segment.start.y + segment.end.y) / 2.0};
}

Circle circleFromDiameter(const Point2D& p1, const Point2D& p2) {
    Point2D center = {(p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0};
    double radius = distance(p1, p2) / 2.0;
    return {center, radius};
}

Circle circleFrom3Points(const Point2D& p1, const Point2D& p2, const Point2D& p3) {
    double area = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    if (std::abs(area) < EPS) {
        double d12 = distanceSquared(p1, p2);
        double d13 = distanceSquared(p1, p3);
        double d23 = distanceSquared(p2, p3);
        if (d12 >= d13 && d12 >= d23) return circleFromDiameter(p1, p2);
        if (d13 >= d12 && d13 >= d23) return circleFromDiameter(p1, p3);
        return circleFromDiameter(p2, p3);
    }
    
    double d = 2 * (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
    
    double ux = ((p1.x * p1.x + p1.y * p1.y) * (p2.y - p3.y) +
                 (p2.x * p2.x + p2.y * p2.y) * (p3.y - p1.y) +
                 (p3.x * p3.x + p3.y * p3.y) * (p1.y - p2.y)) / d;
    
    double uy = ((p1.x * p1.x + p1.y * p1.y) * (p3.x - p2.x) +
                 (p2.x * p2.x + p2.y * p2.y) * (p1.x - p3.x) +
                 (p3.x * p3.x + p3.y * p3.y) * (p2.x - p1.x)) / d;
    
    Point2D center = {ux, uy};
    double radius = distance(center, p1);
    return {center, radius};
}

bool segmentsIntersect(const LineSegment& s1, const LineSegment& s2, Point2D& inter) {
    double x1 = s1.start.x, y1 = s1.start.y;
    double x2 = s1.end.x, y2 = s1.end.y;
    double x3 = s2.start.x, y3 = s2.start.y;
    double x4 = s2.end.x, y4 = s2.end.y;
    
    double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (std::abs(denom) < EPS) return false;
    
    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
    double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;
    
    if (t >= -EPS && t <= 1 + EPS && u >= -EPS && u <= 1 + EPS) {
        inter.x = x1 + t * (x2 - x1);
        inter.y = y1 + t * (y2 - y1);
        return true;
    }
    return false;
}

//Алгоритм Вельцля

Circle minCircleHelper(std::vector<Point2D>& points, std::vector<Point2D> boundary, size_t n) {
    if (n == 0 || boundary.size() == 3) {
        if (boundary.empty()) return {{0, 0}, -1.0};
        if (boundary.size() == 1) return {boundary[0], 0.0};
        if (boundary.size() == 2) return circleFromDiameter(boundary[0], boundary[1]);
        if (boundary.size() == 3) return circleFrom3Points(boundary[0], boundary[1], boundary[2]);
    }
    
    size_t idx = rand() % n;
    Point2D p = points[idx];
    std::swap(points[idx], points[n - 1]);
    
    Circle c = minCircleHelper(points, boundary, n - 1);
    
    if (c.isValid() && c.contains(p)) {
        return c;
    }
    
    boundary.push_back(p);
    return minCircleHelper(points, boundary, n - 1);
}

Circle minCircle(std::vector<Point2D> points) {
    if (points.empty()) return {{0, 0}, -1.0};
    
    std::shuffle(points.begin(), points.end(), std::mt19937(std::random_device()()));
    return minCircleHelper(points, {}, points.size());
}

//Извлеение точек

std::vector<Point2D> extractPointsFromSegments(const std::vector<LineSegment>& segments) {
    std::vector<Point2D> points;
    
    for (const auto& seg : segments) {
        points.push_back(seg.start);
        points.push_back(seg.end);
        points.push_back(findMidPoint(seg));
        
        for (const auto& other : segments) {
            if (&seg == &other) continue;
            Point2D inter;
            if (segmentsIntersect(seg, other, inter)) {
                points.push_back(inter);
            }
        }
    }
    
    // Удаление дубликатов
    std::sort(points.begin(), points.end(), [](const Point2D& a, const Point2D& b) {
        if (std::abs(a.x - b.x) > EPS) return a.x < b.x;
        return a.y < b.y;
    });
    
    points.erase(std::unique(points.begin(), points.end(),
        [](const Point2D& a, const Point2D& b) { return a == b; }), points.end());
    
    return points;
}

Circle MinimumEnclosingCircleForSegments(const std::vector<LineSegment>& segments) {
    if (segments.empty()) {
        return {{0, 0}, -1.0};
    }
    
    if (segments.size() == 1) {
        Point2D mid = findMidPoint(segments[0]);
        double rad = distance(segments[0].start, segments[0].end) / 2.0;
        return {mid, rad};
    }
    
    std::vector<Point2D> points = extractPointsFromSegments(segments);
    return minCircle(points);
}
