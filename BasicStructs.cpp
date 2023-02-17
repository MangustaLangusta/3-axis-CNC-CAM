#include "BasicStructs.h"

bool operator== (const Point3D &a, const Point3D &b){	return ( (a.x == b.x) && (a.y == b.y) && (a.z == b.z) ); }

bool operator< (const Point3D &a, const Point3D &b){
	return ( 
	(a.x < b.x) || 
	((a.x == b.x) && (a.y < b.y)) ||
	((a.x == b.x) && (a.y == b.y) && (a.z < b.z))	
	);
}

Line3D::Line3D(){}

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

std::vector<Plane3D> MathOperations::CreateZPlanesArray(std::pair<double, double> z_extremums, double interval){
	const MathVector3D new_normal(Point3D{0,0,0}, Point3D{0,0,1});
	Point3D new_plane_point;
	std::vector<Plane3D> result_vector;
	double diff;
	double delta_z;
	int planes_amount;
	new_plane_point = {0,0,z_extremums.first};
	result_vector = {Plane3D(new_normal, new_plane_point)};
	if(interval == 0){
		return result_vector;
	}
	diff = z_extremums.first - z_extremums.second;
	std::cout<<"diff = "<<diff<<std::endl;
	std::cout<<"interval = "<<interval<<std::endl;
	planes_amount = floor(diff / interval);
	std::cout<<"planes_amount = "<<planes_amount<<std::endl;
	delta_z = diff / planes_amount;
	std::cout<<"delta_z = "<<delta_z<<std::endl;
	for(int i = 0; i < planes_amount; i++){
		new_plane_point.z = new_plane_point.z + delta_z * i;
		result_vector.emplace_back(Plane3D(new_normal, new_plane_point));
	}	
	return result_vector;
}

double MathOperations::Interpolate(const double x1, const double x2, const double ratio){
	return ( x1 + (x2 - x1) * ratio );
}

bool MathOperations::IntersectionOfLineAndZPlane(const Line3D line, const double z_plane, Point3D* intersection_point){
	Point3D point_a = line.a;
	Point3D point_b = line.b;
	intersection_point = NULL;
	if( ( (point_a.z > z_plane) && (point_b.z > z_plane) ) || ( (point_a.z < z_plane) && (point_b.z < z_plane) ) )
		return false;
	double delta_z = point_b.z - point_a.z;
	if(delta_z == 0)
		return false;
	double ratio = (z_plane - point_a.z) / delta_z;
	intersection_point->x = MathOperations::Interpolate(point_a.x, point_b.x, ratio);
	intersection_point->y = MathOperations::Interpolate(point_a.y, point_b.y, ratio);
	intersection_point->z = z_plane;
	return true;
}
