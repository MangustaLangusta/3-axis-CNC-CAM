#ifndef BASIC_STRUCTS_H
#define BASIC_STRUCTS_H

struct Point3D;
struct MathVector3D;
struct Triangle3D;
struct Plane3D;
struct Matrix3D;

namespace MathOperations{
	MathVector3D VectorMultiplication(const MathVector3D &vec_a, const MathVector3D &vec_b);
	std::vector<Plane3D> CreateZPlanesArray(double interval, std::pair<double, double> z_extremums);
	
	
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
	void Normalize();
};

struct Line3D{
	Point3D a;
	Point3D b;
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

bool operator== (const Point3D &a, const Point3D &b){	return ( (a.x == b.x) && (a.y == b.y) && (a.z == b.z) ); }

bool operator< (const Point3D &a, const Point3D &b){
	return ( 
	(a.x < b.x) || 
	((a.x == b.x) && (a.y < b.y)) ||
	((a.x == b.x) && (a.y == b.y) && (a.z < b.z))	
	);
}

Line3D::Line3D (const Point3D &new_a, const Point3D &new_b){
	a = new_a;
	b = new_b;
}


MathVector3D::MathVector3D(const Point3D &root, const Point3D &top){
	vector_top = Point3D{ 
		top.x - root.x, 
		top.y - root.y, 
		top.z - root.z 
	};
}

void MathVector3D::Normalize(){
	double module;
	double x = vector_top.x;
	double y = vector_top.y;
	double z = vector_top.z;
	module = sqrt(x*x + y*y + z*z);
	vector_top.x = x / module;
	vector_top.y = y / module;
	vector_top.z = z / module;
}

MathVector3D Triangle3D::GetNormal() const{
	MathVector3D vec_a(vertex[0], vertex[1]);
	MathVector3D vec_b(vertex[0], vertex[2]);
	MathVector3D result = MathOperations::VectorMultiplication(vec_a, vec_b);	
	result.Normalize();
	return result;
}

Plane3D::Plane3D(MathVector3D new_normal, Point3D new_point){
	normal = new_normal;
	plane_point = new_point;
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

std::vector<Plane3D> MathOperations::CreateZPlanesArray(double interval, std::pair<double, double> z_extremums){
	const MathVector3D new_normal(Point3D{0,0,0}, Point3D{0,0,1});
	Point3D new_plane_point;
	std::vector<Plane3D> result_vector;
	double diff;
	double delta_z;
	int planes_amount;
	
	new_plane_point = {0,0,z_extremums.first};
	result_vector = {Plane3D(new_normal, new_plane_point)};
	if(interval == 0){
		std::cout<<"CreateZPlanesArray error: interval == 0"<<std::endl;
		return result_vector;
	}
	diff = z_extremums.first - z_extremums.second;
	planes_amount = floor(diff / interval);
	delta_z = diff / planes_amount;
	for(int i = 0; i < planes_amount; i++){
		new_plane_point.z = new_plane_point.z + delta_z * i;
		result_vector.emplace_back(Plane3D(new_normal, new_plane_point));
	}	
	return result_vector;
}

#endif