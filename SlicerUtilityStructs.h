#ifndef SLICER_UTILITY_STRUCTS_H
#define SLICER_UTILITY_STRUCTS_H

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
	std::tuple<float, float, float> GetEquasionMembers() {
		float Dx = GetDx();
		float Dy = GetDy();
		float member_a, member_b, member_c;
		if (GetDx() == 0)
			return (std::tuple <float, float, float> {1, 0, a.x});
		if (GetDy() == 0)
			return (std::tuple <float, float, float> {0, 1, a.y});
		member_c = a.y - Dy / Dx * a.x;
		
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
};

Line2D DrawEquidistantLine(Line2D origin_line, float dist, bool side){
	Point2D new_a, new_b;
	int side_sign = side ? 1 : -1;
	float dx = origin_line.b.x - origin_line.a.x;
	float dy = origin_line.b.y - origin_line.a.y;
	float ratio = dist / sqrt(dx*dx + dy*dy);
	new_a.x = dy * ratio * side_sign;		//cos90 dx + sin90 dy
	new_a.y = -dx * ratio * side_sign;	//cos90 dy - sin90 dx
	new_b.x = new_a.x + dx;
	new_b.y = new_a.y + dy;
	return (Line2D) {new_a, new_b};
}

Point2D SolveLines2DIntercept(Line2D line_a, Line2D line_b){
	float dx = 
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
