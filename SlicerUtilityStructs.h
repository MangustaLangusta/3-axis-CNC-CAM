#ifndef SLICER_UTILITY_STRUCTS_H
#define SLICER_UTILITY_STRUCTS_H

#define DEFAULT_PRECISION 0.01
#define PRECISE_COORDINATES_PRECISION 0.001

enum ContourType { field, raw, equidistant, sweep };
enum AngleType { obtuse, sharp, developed };
enum PathType { travel, feed };
typedef long long id;
typedef float Offset;
typedef float Speed;
typedef float Precision;

class Coordinate;

struct Point2D;
struct PrecisePoint2D;
struct Line2D;
struct PreciseLine2D;
struct Matrix2D;

bool Lines2DIntercept(std::tuple<double, double, double> line_a, std::tuple<double, double, double> line_b, PrecisePoint2D &result);
bool Lines2DIntercept(std::tuple<double, double, double> line_a, std::tuple<double, double, double> line_b, Point2D &result);
bool Lines2DIntercept(PreciseLine2D line_a, PreciseLine2D line_b, PrecisePoint2D &intercept);
bool Lines2DIntercept(Line2D line_a, Line2D line_b, Point2D &intercept);

class Coordinate{
	private:
		static inline Precision coordinates_precision;
		void Truncate() { value = coordinates_precision * (long long)(value / coordinates_precision);	}
		friend Coordinate operator/ (Coordinate a, Coordinate b){
			Coordinate result;
			result.value = a.value / b.value;
			result.Truncate();
			return result;
		}
		friend Coordinate operator* (Coordinate a, Coordinate b) {
			Coordinate result;
			result.value = a.value * b.value;
			result.Truncate();
			return result;
		}
		friend Coordinate operator+ (Coordinate a, Coordinate b){
			Coordinate result;
			result.value = a.value + b.value;
			return result;
		}
		friend Coordinate operator- (Coordinate a, Coordinate b){
			Coordinate result; 
			result.value = a.value - b.value;
			return result;
		}
		friend Coordinate operator- (Coordinate a) { 
			a.value = -a.value;
			return a;
		}
		friend std::ostream& operator<< (std::ostream &os, const Coordinate &a){
			return os << a.value;
		}
		friend bool operator< (Coordinate a, Coordinate b) { return a.value < b.value; }
		friend bool operator> (Coordinate a, Coordinate b) { return a.value > b.value; }
		friend bool operator<= (Coordinate a, Coordinate b) { return a.value <= b.value; }
		friend bool operator>= (Coordinate a, Coordinate b) { return a.value >= b.value; }
		friend bool operator== (Coordinate a, Coordinate b) { return  a.value == b.value; }
		friend bool operator== (Coordinate a, int b) { return a.value == b; }
	public:
		double value;
		static void SetPrecision(Precision new_precision) { coordinates_precision = new_precision;	}
		Coordinate operator= (double a){
			this->value = a;
			this->Truncate();
			return *this;
		}
		Coordinate(){}
		Coordinate(double val) { value = val; }
	
	
};

double sqrt(Coordinate x) { return sqrt(x.value); }

struct PreciseDekartCoords{
	double x = 0;
	double y = 0;
	double z = 0;
	void ReducePrecision(){
		const double precision = PRECISE_COORDINATES_PRECISION;
		x = precision * (long long)(x / precision);
		y = precision * (long long)(y / precision);
		z = precision * (long long)(z / precision);
	}
	void MakeCoords(double f[3]){
		x = f[0];
		y = f[1];
		z = f[2];
		ReducePrecision();
	}
	void MakeCoords(float f[3]){
		x = f[0];
		y = f[1];
		z = f[2];
		ReducePrecision();
	}
	void PrintCoord(){ std::cout<<x<<" "<<y<<" "<<z<<std::endl;	}
};

struct DekartCoords{
	Coordinate x;
	Coordinate y;
	Coordinate z;
	void SetZero(){
		x = 0;
		y = 0;
		z = 0;
	}
	void MakeCoords(double f[3]){
		x = f[0];
		y = f[1];
		z = f[2];
	}
	void PrintCoord(){ std::cout<<x<<" "<<y<<" "<<z<<std::endl;	}
};



struct PrecisePoint2D{
	double x;
	double y;
	friend bool operator< (const PrecisePoint2D &p1, const PrecisePoint2D &p2){
		return ( (p1.x < p2.x) || ( (p1.x == p2.x) && (p1.y < p2.y) ) );
	}
	friend bool operator== (const PrecisePoint2D &p1, const PrecisePoint2D &p2){
		return ( (p1.x == p2.x) && (p1.y == p2.y) );
	}
	/* utility */
	void print_point(){
		std::cout<<"x = "<<x<<"  y = "<<y<<std::endl;
	}
	
};

struct Point2D{
	Coordinate x;
	Coordinate y;
	
	Point2D(){}
	Point2D(PrecisePoint2D a) { 
		x.value = a.x;
		y.value = a.y;
	}
	Point2D(Coordinate _x, Coordinate _y){
		x = _x;
		y = _y;
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
	Coordinate GetDx(){ return (b.x - a.x); }
	Coordinate GetDy(){ return (b.y - a.y); }
	double GetLength() { return sqrt( GetDx() * GetDx() + GetDy() * GetDy()); }
	bool GetNormal(Point2D &normal){
		Coordinate Dx = GetDx();
		Coordinate Dy = GetDy();
		double len = GetLength();
		if((Dx == 0) && (Dy == 0)){
			std::cout<<"Cannot find normal due to Dx = Dy"<<std::endl;
			return false;
		}
		normal.x = Dy.value / len;		//cos90 dx + sin90 dy
		normal.y = -Dx.value / len;		//cos90 dy - sin90 dx
		return true;
	}
	bool GetEquasionMembers(std::tuple<double, double, double> &equasion_members) {
		Coordinate Dx = GetDx();
		Coordinate Dy = GetDy();
		Coordinate member_c;
		if ((Dx == 0) && (Dy == 0)){
			std::cout<<"Zero line!"<<std::endl;
			return false;
		}
		if (Dx == 0){
			equasion_members = std::make_tuple(1.0, 0.0, a.x.value);
			return true;
		}
		if (Dy == 0){
			equasion_members = std::make_tuple(0.0, 1.0, a.y.value);
			return true;
		}
		member_c = Dy / Dx * a.x - a.y;
		Point2D ab_members;			
		
		if(Lines2DIntercept(std::make_tuple(a.x.value, a.y.value, member_c.value), std::make_tuple(b.x.value, b.y.value, member_c.value), ab_members)){
			equasion_members = std::make_tuple(ab_members.x.value, ab_members.y.value, member_c.value);
			return true; 
		}
		else
			std::cout<<"Equasion members not solved, fix this!"<<std::endl;
		return false;
	}
	void ReverseLine() { std::swap(a,b); }
	void PrintLine(){
		std::cout<<a.x<<" "<<a.y<<" / "<<b.x<<" "<<b.y<<std::endl;
	}
};

struct PreciseLine2D{
	PrecisePoint2D a;
	PrecisePoint2D b;
	double GetDx(){ return (b.x - a.x); }
	double GetDy(){ return (b.y - a.y); }
	double GetLength() { return sqrt( GetDx() * GetDx() + GetDy() * GetDy()); }
	bool GetNormal(PrecisePoint2D &normal){
		double Dx = GetDx();
		double Dy = GetDy();
		double len = GetLength();
		if((Dx == 0) && (Dy == 0)){
			std::cout<<"Cannot find normal due to Dx = Dy"<<std::endl;
			return false;
		}
		normal.x = Dy / len;		//cos90 dx + sin90 dy
		normal.y = -Dx / len;		//cos90 dy - sin90 dx
		return true;
	}
	bool GetEquasionMembers(std::tuple<double, double, double> &equasion_members) {
		double Dx = GetDx();
		double Dy = GetDy();
		double member_c;
		if ((Dx == 0) && (Dy == 0)){
			std::cout<<"Zero line!"<<std::endl;
			return false;
		}
		if (Dx == 0){
			equasion_members = std::make_tuple(1.0, 0.0, a.x);
			return true;
		}
		if (Dy == 0){
			equasion_members = std::make_tuple(0.0, 1.0, a.y);
			return true;
		}
		member_c = Dy / Dx * a.x - a.y;
		PrecisePoint2D ab_members;			
		
		if(Lines2DIntercept(std::make_tuple(a.x, a.y, member_c), std::make_tuple(b.x, b.y, member_c), ab_members)){
			equasion_members = std::make_tuple(ab_members.x, ab_members.y, member_c);
			return true; 
		}
		else
			std::cout<<"Equasion members not solved, fix this!"<<std::endl;
		return false;
	}
	void ReverseLine() { std::swap(a,b); }
	void PrintLine(){
		std::cout<<a.x<<" "<<a.y<<" / "<<b.x<<" "<<b.y<<std::endl;
	}
};



struct Matrix2D{
	double a1;
	double a2;
	double b1;
	double b2;
	double Det(){ return (a1 * b2 - a2 * b1); }
	Matrix2D(Line2D line){	
		a1 = line.a.x.value;
		a2 = line.a.y.value;
		b1 = line.b.x.value;
		b2 = line.b.y.value;
	}
	Matrix2D(PreciseLine2D line){	
		a1 = line.a.x;
		a2 = line.a.y;
		b1 = line.b.x;
		b2 = line.b.y;
	}
	Matrix2D(double new_a1, double new_a2, double new_b1, double new_b2){
		a1 = new_a1;
		a2 = new_a2; 
		b1 = new_b1;
		b2 = new_b2;
	}
};



Line2D DrawEquidistantLine(Line2D origin_line, Offset dist){
	Point2D new_a, new_b;
	int side_sign = (dist >= 0) ? 1 : -1;
	Coordinate dx = origin_line.b.x - origin_line.a.x;
	Coordinate dy = origin_line.b.y - origin_line.a.y;
	double ratio = dist / sqrt(dx*dx + dy*dy);
	new_a.x = origin_line.a.x.value + dy.value * ratio * side_sign;		//cos90 dx + sin90 dy
	new_a.y = origin_line.a.y.value + -dx.value * ratio * side_sign;	//cos90 dy - sin90 dx
	new_b.x = new_a.x + dx;
	new_b.y = new_a.y + dy;
	return (Line2D) {new_a, new_b};
}

bool Lines2DIntercept(std::tuple<double, double, double> line_a, std::tuple<double, double, double> line_b, Point2D &result){	//Cramer's method
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
	result = Point2D(delta_x.Det() / delta.Det(), delta_y.Det() / delta.Det());
	return true;
}

bool Lines2DIntercept(std::tuple<double, double, double> line_a, std::tuple<double, double, double> line_b, PrecisePoint2D &result){	//Cramer's method
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
	std::tuple<double, double, double> equasion_a, equasion_b;
	if (line_a.GetEquasionMembers(equasion_a) && line_b.GetEquasionMembers(equasion_b)){
		if(Lines2DIntercept(equasion_a, equasion_b, intercept))
			return true;
	}
	return false;
}

bool Lines2DIntercept(Line2D line_a, Line2D line_b, PrecisePoint2D &intercept){
	std::tuple<double, double, double> equasion_a, equasion_b;
	if (line_a.GetEquasionMembers(equasion_a) && line_b.GetEquasionMembers(equasion_b)){
		if(Lines2DIntercept(equasion_a, equasion_b, intercept))
			return true;
	}
	return false;
}

DekartCoords VectorMult(DekartCoords v1, DekartCoords v2){
	DekartCoords result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = -( v1.x * v2.z - v1.z * v2.x );
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

PreciseDekartCoords VectorMult(PreciseDekartCoords v1, PreciseDekartCoords v2){
	PreciseDekartCoords result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = -( v1.x * v2.z - v1.z * v2.x );
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}


bool VectorMultZSign(DekartCoords v1, DekartCoords v2) {
	if( (v1.x.value * v2.y.value - v1.y.value * v2.x.value) >= 0 )
		return true;
	else 
		return false;
}

bool VectorMultZSign(PreciseDekartCoords v1, PreciseDekartCoords v2) {
	if( (v1.x * v2.y - v1.y * v2.x) >= 0 )
		return true;
	else 
		return false;
}

AngleType CheckAngleType(Line2D &line_a, Line2D &line_b){
	DekartCoords v1 = {-line_a.GetDx(), -line_a.GetDy(), 0};
	DekartCoords v2 = {line_b.GetDx(), line_b.GetDy(), 0};
	double mult = VectorMult(v1, v2).z.value;
	if(mult > 0) return sharp;
	if(mult == 0) return developed;
	return obtuse;
}

Point2D LinearInterpolation(Point2D a, Point2D b, Coordinate x){
	if(a.x == b.x) 
		return b;
	double ratio = (x.value-a.x.value)/(b.x.value-a.x.value);
	double dy = b.y.value-a.y.value;
	return Point2D{x, a.y.value + dy * ratio};
}

PrecisePoint2D LinearInterpolation(PrecisePoint2D a, PrecisePoint2D b, double x){
	if(a.x == b.x) 
		return b;
	double ratio = (x-a.x)/(b.x-a.x);
	double dy = b.y-a.y;
	return PrecisePoint2D{x, a.y + dy * ratio};
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
