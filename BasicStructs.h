#ifndef BASIC_STRUCTS_H
#define BASIC_STRUCTS_H

#include <vector>
#include <set>
#include <cmath>
#include <iostream>
#include <assert.h>
#include "Utility.h"

#define PI 3.1415926535

#define Z_AXIS MathVector3D({0,0,0},{0,0,1})

#define CALCULATIONS_PRECISION 0.00001

struct Point3D;
struct MathVector3D;
struct Line3D;
struct Triangle3D;
struct Plane3D;
struct Matrix;
struct Line3DCanonicalEquasionMembers;
struct Line3DCrossingPlanesEquasionMembers;

struct Point3D{
	double x;
	double y;
	double z;
	void Shift(const MathVector3D &shift_vector);
	friend bool operator< (const Point3D &a, const Point3D &b);
	friend bool operator== (const Point3D &a, const Point3D &b);
	friend bool operator!= (const Point3D &a, const Point3D &b);
};

struct MathVector3D{
	Point3D vector_top;	
	MathVector3D(){};
	MathVector3D(const Point3D &root, const Point3D &top);
	friend bool operator== (const MathVector3D &a, const MathVector3D &b);
	friend bool operator!= (const MathVector3D &a, const MathVector3D &b);
	void Normalize();
	void Rotate(const MathVector3D &axis, const double &angle_rad);
	void MultiplyByNumber(const double &number);
	double Module() const;
	double GetX() const;
	double GetY() const;
	double GetZ() const;
	Matrix MatrixForm() const;
};

struct Line3D{
	Point3D a;
	Point3D b;
	Line3D();
	Line3D(const Point3D &new_a, const Point3D &new_b);
	MathVector3D GetDirectingVector() const;
	Line3DCanonicalEquasionMembers GetCanonicalEquasionMembers() const;
	Line3DCrossingPlanesEquasionMembers GetCrossingPlanesEquasionMembers() const;
	double Length() const;
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
		std::vector<std::vector<double>> elements; //rows->cols
		std::pair<int, int> Size() const;
		bool FindNonZeroWrappingMinor(std::set<int> *row_set, std::set<int> *col_set) const;	//for rank determination
		
	public:
		Matrix(const std::vector<std::vector<double>> &new_elements);
		~Matrix();
		std::vector<std::vector<double>> GetElements() const;
		double Minor(const std::set<int> &rows, const std::set<int> &cols) const;
		double Determinant() const;
		int Rank() const;
		bool IsSquare() const;
		int ColsAmount() const;
		int RowsAmount() const;
		void Transpose();
		void Print() const;
};

namespace MathOperations{
		//vector operations
	MathVector3D VectorMultiplication(const MathVector3D &vec_a, const MathVector3D &vec_b);
	double Dot(const MathVector3D &vec_a, const MathVector3D &vec_b);
	double AngleBetweenVectors(const MathVector3D &vec_a, const MathVector3D &vec_b);
	//Planes
	std::vector<Plane3D> CreateZPlanesArray(std::pair<double, double> boundaries, double spacing);	
		//pure math
	double Interpolate(const double x1, const double x2, const double ratio);
		//intersections
	bool IntersectionOfLineAndZPlane(const Line3D line, const double z_plane, Point3D* intersection_point);
	bool LimitedIntersectionOfTwoLines(const Line3D &line_a, const Line3D &line_b, Point3D* intersection_point);
	bool UnlimitedIntersectionOfTwoLines(const Line3D &line_a, const Line3D &line_b, Point3D* intersection_point);
	bool Gauss(const Matrix input_matrix, Point3D &intersection);
		//Points
	double DistanceBetweenPoints(const Point3D &point_a, const Point3D &point_b);
		//Matrix operations
	Matrix MatrixMultiplication(const Matrix &left, const Matrix &right);
};

enum Validity {NOT_VALID, VALID};

bool operator== (const Point3D &a, const Point3D &b);
bool operator< (const Point3D &a, const Point3D &b);


#endif