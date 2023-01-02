#ifndef SLICER_UTILITY_STRUCTS_H
#define SLICER_UTILITY_STRUCTS_H

enum ContourType { field, raw, equidistant, sweep };
enum AngleType { obtuse, sharp, developed };
typedef long long id;
typedef float Offset;

struct Point2D;
struct Line2D;
struct Circle2D;
struct Matrix2D;

bool Lines2DIntercept(std::tuple<float, float, float> line_a, std::tuple<float, float, float> line_b, Point2D &result);
bool Lines2DIntercept(Line2D line_a, Line2D line_b, Point2D &intercept);


struct DekartCoords{
	float x;
	float y;
	float z;
	void SetZero(){
		x = 0;
		y = 0;
		z = 0;
	}
	void MakeCoords(float f[3]){
		x = f[0];
		y = f[1];
		z = f[2];
	}
	void PrintCoord(){ std::cout<<x<<" "<<y<<" "<<z<<std::endl;	}
};

struct Point2D{
	float x;
	float y;
	void ReducePrecision(float precision){
		x = floor(x / precision) * precision;
		y = floor(y / precision) * precision;
	}
	friend bool operator< (const Point2D &p1, const Point2D &p2){
		return ( (p1.x < p2.x) || ( (p1.x == p2.x) && (p1.y < p2.y) ) );
	}
	friend bool operator== (const Point2D &p1, const Point2D &p2){
		return ( (p1.x == p2.x) && (p1.y == p2.y) );
	}
	/* utility */
	void print_point(){
		std::cout<<"x = "<<x<<"  y = "<<y<<std::endl;
	}
};

struct Line2D{
	Point2D a;
	Point2D b;
	float GetDx(){ return (b.x - a.x); }
	float GetDy(){ return (b.y - a.y); }
	float GetLength() { return sqrt( GetDx() * GetDx() + GetDy() * GetDy()); }
	bool GetNormal(Point2D &normal){
		float Dx = GetDx();
		float Dy = GetDy();
		float len = GetLength();
		if((Dx == 0) && (Dy == 0)){
			std::cout<<"Cannot find normal due to Dx = Dy"<<std::endl;
			return false;
		}
		normal.x = Dy / len;		//cos90 dx + sin90 dy
		normal.y = -Dx / len;		//cos90 dy - sin90 dx
		/*
		std::tuple<float,float,float> eq;
		if(GetEquasionMembers(eq))
			normal = {std::get<0>(eq), std::get<1>(eq)};
		else 
			return false;
		*/
		return true;
	}
	bool GetEquasionMembers(std::tuple<float, float, float> &equasion_members) {
		float Dx = GetDx();
		float Dy = GetDy();
		float member_c;
		if ((Dx == 0) && (Dy == 0)){
			std::cout<<"Zero line!"<<std::endl;
			return false;
		}
		if (Dx == 0){
			equasion_members = std::make_tuple(1, 0, a.x);
			return true;
		}
		if (Dy == 0){
			equasion_members = std::make_tuple(0, 1, a.y);
			return true;
		}
		member_c = Dy / Dx * a.x - a.y;
		Point2D ab_members;			
		
		if(Lines2DIntercept(std::make_tuple(a.x, a.y, member_c), std::make_tuple(b.x, b.y, member_c), ab_members)){
			equasion_members = std::make_tuple(ab_members.x, ab_members.y, member_c);
			return true; 
		}
		else
			std::cout<<"Equasion members not solved, fix this!"<<std::endl;
		return false;
	}
	void print_line(){
		std::cout<<a.x<<" "<<a.y<<" / "<<b.x<<" "<<b.y<<std::endl;
	}
};

struct Circle2D{
	Point2D center;
	float radius;
	void print_circle(){
		std::cout<<"center x = "<<center.x<<" y = "<<center.y<<" radius "<<radius<<std::endl;
	}
};

struct Matrix2D{
	float a1;
	float a2;
	float b1;
	float b2;
	float Det(){ return (a1 * b2 - a2 * b1); }
	Matrix2D(Line2D line){	
		a1 = line.a.x;
		a2 = line.a.y;
		b1 = line.b.x;
		b2 = line.b.y;
	}
	Matrix2D(float new_a1, float new_a2, float new_b1, float new_b2){
		a1 = new_a1;
		a2 = new_a2; 
		b1 = new_b1;
		b2 = new_b2;
	}
};

Line2D DrawEquidistantLine(Line2D origin_line, Offset dist){
	Point2D new_a, new_b;
	int side_sign = (dist >= 0) ? 1 : -1;
	float dx = origin_line.b.x - origin_line.a.x;
	float dy = origin_line.b.y - origin_line.a.y;
	float ratio = dist / sqrt(dx*dx + dy*dy);
	new_a.x = origin_line.a.x + dy * ratio * side_sign;		//cos90 dx + sin90 dy
	new_a.y = origin_line.a.y + -dx * ratio * side_sign;	//cos90 dy - sin90 dx
	new_b.x = new_a.x + dx;
	new_b.y = new_a.y + dy;
	return (Line2D) {new_a, new_b};
}

bool Lines2DIntercept(std::tuple<float, float, float> line_a, std::tuple<float, float, float> line_b, Point2D &result){	//Cramer's method
	Matrix2D delta(	std::get<0>(line_a), std::get<0>(line_b), 
									std::get<1>(line_a), std::get<1>(line_b));
	Matrix2D delta_x(	std::get<2>(line_a), std::get<2>(line_b), 
										std::get<1>(line_a), std::get<1>(line_b));
	Matrix2D delta_y(	std::get<0>(line_a), std::get<0>(line_b), 
										std::get<2>(line_a), std::get<2>(line_b));
	if(delta.Det()==0){
		std::cout<<"Determinant = 0, cannot solve system!"<<std::endl;
		std::cout<<"Line a: "<<std::get<0>(line_a)<<" "<<std::get<1>(line_a)<<" "<<std::get<2>(line_a)<<std::endl;
		std::cout<<"Line b: "<<std::get<0>(line_b)<<" "<<std::get<1>(line_b)<<" "<<std::get<2>(line_b)<<std::endl;
		return false;
	}	
	result = {delta_x.Det() / delta.Det(), delta_y.Det() / delta.Det()};
	return true;
}

bool Lines2DIntercept(Line2D line_a, Line2D line_b, Point2D &intercept){
	std::tuple<float, float, float> equasion_a, equasion_b;
	if (line_a.GetEquasionMembers(equasion_a) && line_b.GetEquasionMembers(equasion_b)){
		if(Lines2DIntercept(equasion_a, equasion_b, intercept))
			return true;
	}
	return false;
}

DekartCoords VectorMult(DekartCoords v1, DekartCoords v2){
	DekartCoords result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = -1 * ( v1.x * v2.z - v1.z * v2.x );
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}


bool VectorMultZSign(DekartCoords v1, DekartCoords v2) {
	if( (v1.x * v2.y - v1.y * v2.x) >= 0 )
		return true;
	else 
		return false;
}

AngleType CheckAngleType(Line2D &line_a, Line2D &line_b){
	DekartCoords v1 = {-line_a.GetDx(), -line_a.GetDy(), 0};
	DekartCoords v2 = {line_b.GetDx(), line_b.GetDy(), 0};
	float mult = VectorMult(v1, v2).z;
	if(mult > 0) return sharp;
	if(mult == 0) return developed;
	return obtuse;
}

Point2D LinearInterpolation(Point2D a, Point2D b, float x){
	float ratio = (x-a.x)/(b.x-a.x);
	float dy = b.y-a.y;
	return Point2D{x, a.y + dy * ratio};
}

void ConsoleHex(std::string s, int len){
	std::cout.unsetf(std::ios::dec);
	std::cout.setf(std::ios::hex);
	for(int i = 0; i < s.length(); i++){
		std::cout<<static_cast<short>(static_cast<unsigned char>(s[i]));
		if(static_cast<unsigned char>(s[i]) > 15)
			std::cout<<' ';
		else 
			std::cout<<' '<<' ';
		if(!((i+1)%len))
			std::cout<<std::endl;
	}
	std::cout<<std::endl;
	std::cout.unsetf(std::ios::hex);
	std::cout.setf(std::ios::dec);
}


#endif
