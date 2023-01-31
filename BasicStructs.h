#ifndef BASIC_STRUCTS_H
#define BASIC_STRUCTS_H

struct Point3D;
struct MathVector3D;
struct Triangle3D;
struct Matrix3D;

namespace MathOperations{
	MathVector3D VectorMultiplication(const MathVector3D &vec_a, const MathVector3D &vec_b);
	
	
	
};

struct Point3D{
	double x;
	double y;
	double z;
	friend bool operator< (const Point3D &a, const Point3D &b);
	friend bool operator== (const Point3D &a, const Point3D &b);
};

struct MathVector3D{
	Point3D vector_top;	
	MathVector3D(){};
	MathVector3D(const Point3D &root, const Point3D &top);
};

struct Triangle3D{
	Point3D vertex[3];
	MathVector3D GetNormal() const;	
};

struct Matrix3D{
	Point3D cols[3];
};

bool operator== (const Point3D &a, const Point3D &b){	return ( (a.x == b.x) && (a.y == b.y) && (a.z == b.z) ); }

bool operator< (const Point3D &a, const Point3D &b){
	return ( 
	(a.x < b.x) || 
	((a.x == b.x) && (a.y < b.y)) ||
	((a.x == b.x) && (a.y == b.y) && (a.z < b.z))	
	);
}




MathVector3D::MathVector3D(const Point3D &root, const Point3D &top){
	vector_top = Point3D{ 
		top.x - root.x, 
		top.y - root.y, 
		top.z - root.z 
	};
}

MathVector3D Triangle3D::GetNormal() const{
	MathVector3D vec_a(vertex[0], vertex[1]);
	MathVector3D vec_b(vertex[0], vertex[2]);
	return MathOperations::VectorMultiplication(vec_a, vec_b);	
}

MathVector3D MathOperations::VectorMultiplication(const MathVector3D &vec_a, const MathVector3D &vec_b){
	Point3D a = vec_a.vector_top;
	Point3D b = vec_b.vector_top;
	MathVector3D result_vector;
	result_vector.vector_top = Point3D{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
	return result_vector;
}

#endif