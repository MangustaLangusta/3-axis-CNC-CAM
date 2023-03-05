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
struct Line3DCanonicalEquasionMembers;
struct Line3DCrossingPlanesEquasionMembers;

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
	double GetX() const;
	double GetY() const;
	double GetZ() const;
};

struct Line3D{
	Point3D a;
	Point3D b;
	Line3D();
	Line3D(const Point3D &new_a, const Point3D &new_b);
	MathVector3D GetDirectingVector() const;
	Line3DCanonicalEquasionMembers GetCanonicalEquasionMembers() const;
	Line3DCrossingPlanesEquasionMembers GetCrossingPlanesEquasionMembers() const;
};

	//canonical equasion: (x-x1) / a_x = (y-y1) / a_y = (z-z1) / a_z
struct Line3DCanonicalEquasionMembers{
	double x1;
	double a_x;
	double y1;
	double a_y;
	double z1;
	double a_z;	
};

	//crossing planes equasion: / A1*x + B1*y + C1 * z + D1 = 0
	//													\ A2*x + B2*y + C2 * z + D2 = 0
struct Line3DCrossingPlanesEquasionMembers{
	double A1;
	double A2;
	double B1;
	double B2;
	double C1;
	double C2;
	double D1;
	double D2;
	Line3DCrossingPlanesEquasionMembers();
	~Line3DCrossingPlanesEquasionMembers();
	std::vector<std::vector<double>> VectorForm() const;
	void Print();
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

class Matrix{
	private:
		std::pair<int, int> size;
		std::vector<std::vector<double>> elements; //cols->rows
		std::pair<int, int> Size() const;
		int Rank() const;
	public:
		Matrix(const std::vector<std::vector<double>> &new_elements);
		~Matrix();
		std::vector<std::vector<double>> GetMinor(const int &target_col, const int &target_row) const;
		double Determinant() const;
		bool Gauss(Point3D* intersection_point);
		bool IsSquare() const;
		int ColsAmount() const;
		int RowsAmount() const;
		void Print() const;
};

namespace MathOperations{
	MathVector3D VectorMultiplication(const MathVector3D &vec_a, const MathVector3D &vec_b);
	double Dot(const MathVector3D &vec_a, const MathVector3D &vec_b);
	std::vector<Plane3D> CreateZPlanesArray(std::pair<double, double> boundaries, double spacing);	
	double Interpolate(const double x1, const double x2, const double ratio);
	bool IntersectionOfLineAndZPlane(const Line3D line, const double z_plane, Point3D* intersection_point);
	bool LimitedIntersectionOfTwoLines(const Line3D &line_a, const Line3D &line_b, Point3D* intersection_point);
	bool UnlimitedIntersectionOfTwoLines(const Line3D &line_a, const Line3D &line_b, Point3D* intersection_point);
	
	double AngleBetweenVectors(const MathVector3D &vec_a, const MathVector3D &vec_b);
};

enum Validity {NOT_VALID, VALID};

bool operator== (const Point3D &a, const Point3D &b);
bool operator< (const Point3D &a, const Point3D &b);


#endif