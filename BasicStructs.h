#ifndef BASIC_STRUCTS_H
#define BASIC_STRUCTS_H

#include <vector>
#include <cmath>
#include <iostream>
#include <assert.h>


struct Point3D;
struct MathVector3D;
struct Line3D;
struct Triangle3D;
struct Plane3D;
struct Matrix3D;

struct Point3D{
	double x;
	double y;
	double z;
	friend bool operator< (const Point3D &a, const Point3D &b);
	friend bool operator== (const Point3D &a, const Point3D &b);
	friend bool operator!= (const Point3D &a, const Point3D &b);
};

struct MathVector3D{
	Point3D vector_top;	
	MathVector3D(){};
	MathVector3D(const Point3D &root, const Point3D &top);
	void Normalize();
	double Module() const;
	double GetZ() const;
};

struct Line3D{
	Point3D a;
	Point3D b;
	Line3D();
	Line3D(const Point3D &new_a, const Point3D &new_b);
};


struct Triangle3D{
	Point3D vertex[3];
	MathVector3D GetNormal() const;	
};

struct Plane3D{
	MathVector3D normal;
	Point3D plane_point;	
	Plane3D(MathVector3D new_normal, Point3D new_point);
};

struct Matrix3D{
	Point3D cols[3];
};


namespace MathOperations{
	MathVector3D VectorMultiplication(const MathVector3D &vec_a, const MathVector3D &vec_b);
	double Dot(const MathVector3D &vec_a, const MathVector3D &vec_b);
	std::vector<Plane3D> CreateZPlanesArray(std::pair<double, double> boundaries, double spacing);	
	double Interpolate(const double x1, const double x2, const double ratio);
	bool IntersectionOfLineAndZPlane(const Line3D line, const double z_plane, Point3D* intersection_point);
	int Gauss();
	double AngleBetweenVectors(const MathVector3D &vec_a, const MathVector3D &vec_b);
};

enum Validity {NOT_VALID, VALID};

bool operator== (const Point3D &a, const Point3D &b);
bool operator< (const Point3D &a, const Point3D &b);


#endif