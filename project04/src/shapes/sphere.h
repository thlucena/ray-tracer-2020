#ifndef _SPHERE_
#define _SPHERE_

#include <iostream>
#include <cmath>
#include "../core/point3D.h"
#include "../core/primitive.h"
#include "../core/shape.h"
#include "../parser/scene_xml_params.h"

using namespace std;

class Sphere: public Shape {
    public:
        float radius;
        Point3D center;

        Sphere(float mRadius, Point3D mCenter) {
            this->radius = mRadius;
            this->center = mCenter;
        };
        
        bool intersect( Ray r, float * t_hit, Surfel *sf ) const override {
            return intersect_p(r);
        }

		bool intersect_p( Ray r ) const override { 
            Point3D origin_center_vec = r.getOrigin() - center;
            // B = 2 (o - c) dot d
            float B = dot( Vec3(2 * origin_center_vec.getX(), 2 * origin_center_vec.getY(), 2 * origin_center_vec.getZ()), r.getDirection() );
            // A = d dot d
            float A = dot( r.getDirection(), r.getDirection() );
            // C = (o - c) dot (o - c) - r²
            float C = dot( Vec3(origin_center_vec.getX(), origin_center_vec.getY(), origin_center_vec.getZ()), Vec3(origin_center_vec.getX(), origin_center_vec.getY(), origin_center_vec.getZ()) );
            C = C - pow(radius, 2.0);

            // DELTA = B² - 4AC
            return (pow(B, 2.0) - 4 * A * C) >= 0;
        };
};

#endif