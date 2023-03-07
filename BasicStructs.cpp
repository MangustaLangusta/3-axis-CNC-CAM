#include "BasicStructs.h"

bool operator== (const Point3D &a, const Point3D &b){	return ( (a.x == b.x) && (a.y == b.y) && (a.z == b.z) ); }

bool operator< (const Point3D &a, const Point3D &b){
	return ( 
	(a.x < b.x) || 
	((a.x == b.x) && (a.y < b.y)) ||
	((a.x == b.x) && (a.y == b.y) && (a.z < b.z))	
	);
}

bool operator!= (const Point3D &a, const Point3D &b) {
	return ( !(a == b) );
}

Line3D::Line3D(){}

Line3D::Line3D (const Point3D &new_a, const Point3D &new_b){
	a = new_a;
	b = new_b;
}

MathVector3D Line3D::GetDirectingVector() const{
	return MathVector3D(a, b);
}

double Line3D::Length() const {
	return GetDirectingVector().Module();
}

Line3DCanonicalEquasionMembers Line3D::GetCanonicalEquasionMembers() const{
	Line3DCanonicalEquasionMembers result;
	MathVector3D dir_vec = GetDirectingVector();
	
	result.a_x = dir_vec.GetX();
	result.a_y = dir_vec.GetY();
	result.a_z = dir_vec.GetZ();
	result.x1 = -a.x;
	result.y1 = -a.y;
	result.z1 = -a.z;
		
		//directing vector should not be zero
	assert( !((result.x1 == 0) && (result.a_y == 0) && (result.a_z == 0)) );	
	
	std::cout<<"Canonical equasion:"<<std::endl;
	std::cout<<"(x+"<<result.x1<<") / "<<result.a_x<<" = "<<"(y+"<<result.y1<<") / "<<result.a_y<<" = "<<"(z+"<<result.z1<<") / "<<result.a_z<<std::endl;
	return result;
}

Line3DCrossingPlanesEquasionMembers Line3D::GetCrossingPlanesEquasionMembers() const{
	Line3DCrossingPlanesEquasionMembers result;	//members already pre-initialized to 0 in constructor
	Line3DCanonicalEquasionMembers canonical;
	double c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12;
		
		//find canonical equasion. it consists of 3 parts (x... = y... = z...)
		//each 2 parts will form an equasion (just equalize two parts together), so totally 3 equasions:
		//	 																								 / 	(x+x1)/a_x = (y+y1)/a_y 			 /	c1*x + c2*y + c4 = 0		(c3=0 since no z member)  
		//	(x+x1) / a_x = (y+y1) / a_y = (z+z1) / a_z  <>  < 	(x+x1)/a_x = (z+z1)/a_z		<>	<		c5*x + c7*z + c8 = 0 		(c6=0 since no y member)
		//																									 \ 	(y+y1)/a_y = (z+z1)/a_z				 \	c10*y + c11*z + c12 = 0	(c9=0 since no x member)
		//We are interested in coefficients (c1 .. c12):
		// c1 = a_y; c2 = -a_x; c4 = a_y*x1 - a_x*y1;
		// c5 = a_z; c7 = -a_x; c8 = a_z*x1 - a_x*z1;
		// c10 = a_z; c11 = -a_y; c12 = a_z*y1 - a_y*z1;
		//
		//c1..c3, c5..c7, c9..c11 coefficients form a main matrix M, which determinant should be equal 0
		//	matrix M:
		//	c1	c2	c3
		//	c5	c6	c7
		//	c9	c10	c11
		//find minors of this 3x3 matrix M. if one of these minors is <> 0, we can remove equasion not engaged in forming of that minor
		//for example, if minor {{c1 c2} {c5 c6}} is not equal 0, we can remove third equasion (c9 c10 c11 c12 from system shown above) bacause it is not envolved in basis minor
		
		canonical = GetCanonicalEquasionMembers();
		c1 = canonical.a_y;
		c2 = -canonical.a_x; 	
		c3 = 0;	//see explanation above
		c4 = canonical.a_y * canonical.x1 - canonical.a_x * canonical.y1;
		c5 = canonical.a_z;
		c6 = 0;	//see explanation above
		c7 = -canonical.a_x; 				
		c8 = canonical.a_z * canonical.x1 - canonical.a_x * canonical.z1;
		c9 = 0; //see explanation above
		c10 = canonical.a_z;
		c11 = -canonical.a_y;			
		c12 = canonical.a_z * canonical.y1 - canonical.a_y * canonical.z1;
						
		Matrix coeff_matrix( {{c1, c2, c3}, {c5, c6, c7}, {c9, c10, c11}} );
		
		int row_to_remove = -1;	//initial state
		
		assert(coeff_matrix.Determinant() == 0);
		
		std::set<int> row_set, cols_set;
	
		for(int row = 0; row < 3; row++){
			row_set = {0, 1, 2};
			row_set.erase(row_set.find(row));
			for(int col = 0; col < 3; col++){
				cols_set = {0, 1, 2};
				cols_set.erase(cols_set.find(col));
				double minor = coeff_matrix.Minor(row_set, cols_set);
				if(minor != 0){
					row_to_remove = row;
					break;
				}
			}
			if(row_to_remove > -1)	//if state changed from initial to anything else, row already found
				break;
		}
		
		assert(row_to_remove > -1);	
		
		switch(row_to_remove){
			case 0:
				result.A1 = c5;
				result.B1 = c6;
				result.C1 = c7;
				result.D1 = c8;
				result.A2 = c9;
				result.B2 = c10;	
				result.C2 = c11;	
				result.D2 = c12;	
				break;
			case 1:
				result.A1 = c1;
				result.B1 = c2;
				result.C1 = c3;
				result.D1 = c4;
				result.A2 = c9;
				result.B2 = c10;	
				result.C2 = c11;	
				result.D2 = c12;
				break;
			case 2:
				result.A1 = c1;
				result.B1 = c2;
				result.C1 = c3;
				result.D1 = c4;
				result.A2 = c5;
				result.B2 = c6;	
				result.C2 = c7;	
				result.D2 = c8;
				break;
			default:
				assert(false);
		}
			//put free members (D1, D2) in the right side of equasion: change sign
		result.D1 = -result.D1;
		result.D2 = -result.D2;
		return result;
}

Line3DCrossingPlanesEquasionMembers::Line3DCrossingPlanesEquasionMembers(){
	A1 = 0;
	A2 = 0;
	B1 = 0;
	B2 = 0;
	C1 = 0;
	C2 = 0;
	D1 = 0;
	D2 = 0;
}

Line3DCrossingPlanesEquasionMembers::~Line3DCrossingPlanesEquasionMembers(){}

std::vector<std::vector<double>> Line3DCrossingPlanesEquasionMembers::VectorForm() const {
	return std::vector<std::vector<double>>	({	{A1, B1, C1, D1}, 
																							{A2, B2, C2, D2}	}	);
}

void Line3DCrossingPlanesEquasionMembers::Print(){
	std::cout<<A1<<"x + "<<B1<<"y + "<<C1<<"z + "<<D1<<" = 0"<<std::endl;
	std::cout<<A2<<"x + "<<B2<<"y + "<<C2<<"z + "<<D2<<" = 0"<<std::endl;
}

MathVector3D::MathVector3D(const Point3D &root, const Point3D &top){
	vector_top = Point3D{ 
		top.x - root.x, 
		top.y - root.y, 
		top.z - root.z 
	};
}

void MathVector3D::Normalize(){
	double module = Module();
	double x = vector_top.x;
	double y = vector_top.y;
	double z = vector_top.z;
	assert(module != 0);
	vector_top.x = x / module;
	vector_top.y = y / module;
	vector_top.z = z / module;
}

double MathVector3D::Module() const{
	double x = vector_top.x;
	double y = vector_top.y;
	double z = vector_top.z;
	return sqrt(x*x + y*y + z*z);
}

double MathVector3D::GetX() const{
	return vector_top.x;
}

double MathVector3D::GetY() const{
	return vector_top.y;
}

double MathVector3D::GetZ() const{
	return vector_top.z;
}

MathVector3D Triangle3D::GetNormal() const{
	MathVector3D vec_a(vertex[0], vertex[1]);
	MathVector3D vec_b(vertex[0], vertex[2]);
	MathVector3D result = MathOperations::VectorMultiplication(vec_a, vec_b);	
	result.Normalize();
	return result;
}

Matrix::Matrix(const std::vector<std::vector<double>> &new_elements){
	elements = new_elements;
	size = std::make_pair(elements.size(), elements.begin()->size());	//LATER TO MAKE BETTER CHECK AND PROBABLY VALIDATION FUNC
}	

Matrix::~Matrix() {}

bool Matrix::FindNonZeroWrappingMinor(std::set<int> *row_set, std::set<int> *col_set) const {
		//return false if no non-zero wrapping minors found
	
		//only square matrix
	assert(row_set->size() == col_set->size());
	
	int wrapping_matrix_order = row_set->size() + 1;	
	
	if( (ColsAmount() < wrapping_matrix_order) || (RowsAmount() < wrapping_matrix_order) )
		return false;
	
	double last_minor;
	
	std::set<int> new_row_set, new_col_set;
	
	for(int row = 0; row < RowsAmount(); row++){
		
		new_row_set = *row_set;
		if(new_row_set.find(row) == new_row_set.end())
			new_row_set.insert(row);
		else 
			continue;
		
		for(int col = 0; col < ColsAmount(); col++){
		
			new_col_set = *col_set;
			if(new_col_set.find(col) == new_col_set.end())
				new_col_set.insert(col);
			else 
				continue;
			
			last_minor = Minor(new_row_set, new_col_set);
		
			if(last_minor != 0){
				*row_set = new_row_set;
				*col_set = new_col_set;
				return true;
			}
		}
	}
	return false;
}

std::vector<std::vector<double>> Matrix::GetElements() const {
	return elements;
}

int Matrix::Rank() const {
		//find rank of matrix using wrapping minors
	
	int rank = 0;
	std::set<int> row_set, col_set;
		
		//find non-zero element
	for(int row = 0; row < RowsAmount(); row++){
		for(int col = 0; col < ColsAmount(); col++){
			if(elements[row][col] != 0){
				row_set = {row};
				col_set = {col};
				rank = 1;
				break;
			}
		}
		if(rank == 1)
			break;
	}
	
	if(rank == 0)
		return rank;
	
	while(1){
		if( FindNonZeroWrappingMinor(&row_set, &col_set) )
			rank++;
		else 
			break;
	}
	
	return rank;
	
}

double Matrix::Minor(const std::set<int> &rows, const std::set<int> &cols) const {
	std::vector<std::vector<double>> minor_matrix;
	std::vector<double> current_row;
	
		//minor should be a square matrix
	assert(cols.size() <= ColsAmount());
	assert(rows.size() <= RowsAmount());
	assert(cols.size() == rows.size());
	
	for(int row = 0; row < RowsAmount(); row++){
		if(rows.find(row) == rows.end())
			continue;
		current_row.clear();
		for(int col = 0; col < ColsAmount(); col++){
			if(cols.find(col) == cols.end())
				continue;
			current_row.push_back(elements[row][col]);
		}
		minor_matrix.push_back(current_row);
	}
	return Matrix(minor_matrix).Determinant();
}

double Matrix::Determinant() const {
	if(!IsSquare())
		return 0;
	
	if(size.first == 1)
		return elements[0][0];
	
	if(size.first == 2)
		return (elements[0][0] * elements[1][1] - elements[0][1] * elements[1][0]);
	
	const int FIRST_ROW = 0;
	double result = 0;
	double minor;
	int sign;
	std::set<int> minor_rows;
	std::set<int> minor_cols;
	
		//prepare rows for minor in advance
	for(int i = 1; i < RowsAmount(); i++)
		minor_rows.insert(i);
	
		//main loop
	for(int col = 0; col < size.first; col++){
		sign = col % 2 ? -1 : 1;
			//prepare cols for monor
		minor_cols.clear();
		for(int i = 0; i < ColsAmount(); i++){
			if(i == col)
				continue;
			minor_cols.insert(i);
		}
		minor = this->Minor(minor_rows, minor_cols);
		result += sign * elements[FIRST_ROW][col] * minor; 
	}
	return result;
}

bool Matrix::IsSquare() const {
	return ((size.first == size.second) && (size.first != 0));
}

std::pair<int, int> Matrix::Size() const {
	return size;
}

int Matrix::ColsAmount() const {
	return size.second;
}

int Matrix::RowsAmount() const {
	return size.first;
}

void Matrix::Print() const {
	for(int row = 0; row < size.first; row++){
		for(int col = 0; col < size.second; col++){
			std::cout<<elements[row][col]<<" ";
		}
		std::cout<<std::endl;
	}
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

double MathOperations::Dot(const MathVector3D &vec_a, const MathVector3D &vec_b){
	Point3D point_a = vec_a.vector_top;
	Point3D point_b = vec_b.vector_top;
	return point_a.x * point_b.x + point_a.y * point_b.y + point_a.z * point_b.z;
}

std::vector<Plane3D> MathOperations::CreateZPlanesArray(std::pair<double, double> z_extremums, double interval){
	const MathVector3D new_normal(Point3D{0,0,0}, Point3D{0,0,1});
	Point3D new_plane_point;
	std::vector<Plane3D> result_vector;
	double diff;
	double delta_z;
	double z_min, z_max;
	int planes_amount;
	
	z_min = std::min(z_extremums.first, z_extremums.second);
	z_max = std::max(z_extremums.first, z_extremums.second);
	if(interval == 0){
		return result_vector;
	}
	diff = z_max - z_min;
	assert(diff >= 0);
	planes_amount = floor(diff / abs(interval) );
	delta_z = diff / planes_amount;
	for(int i = 0; i <= planes_amount; i++){
		new_plane_point.z = z_min + delta_z * i;
		result_vector.emplace_back(Plane3D(new_normal, new_plane_point));
	}	
	return result_vector;
}

double MathOperations::Interpolate(const double x1, const double x2, const double ratio){
	return ( x1 + (x2 - x1) * ratio );
}

bool MathOperations::IntersectionOfLineAndZPlane(const Line3D line, const double z_plane, Point3D* intersection_point){
	assert(intersection_point != NULL);
	Point3D point_a = line.a;
	Point3D point_b = line.b;
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

	//true if two given lines intersect each other within their ends. false if no intersection, parallel or collinear
bool MathOperations::LimitedIntersectionOfTwoLines(const Line3D &line_a, const Line3D &line_b, Point3D* intersection_point){
	
	if(MathOperations::UnlimitedIntersectionOfTwoLines(line_a, line_b, intersection_point)){
		double dist;
		dist = line_a.Length();
		if( (Line3D(line_a.a, *intersection_point).Length() > dist) || (Line3D(line_a.b, *intersection_point).Length() > dist) )
			return false;
		dist = line_b.Length();
		if( (Line3D(line_b.a, *intersection_point).Length() > dist) || (Line3D(line_b.b, *intersection_point).Length() > dist) )
			return false;
		return true;
	}
	return false;
}

	//true if two given lines intersect each other, even beyond their limits. False if no intersection, parallel or collinear
bool MathOperations::UnlimitedIntersectionOfTwoLines(const Line3D &line_a, const Line3D &line_b, Point3D* intersection_point){
	Line3DCrossingPlanesEquasionMembers line_a_equasion, line_b_equasion;
	
	line_a_equasion = line_a.GetCrossingPlanesEquasionMembers();
	line_b_equasion = line_b.GetCrossingPlanesEquasionMembers();
	
	Matrix base_matrix({	{line_a_equasion.A1, line_a_equasion.B1, line_a_equasion.C1},
												{line_a_equasion.A2, line_a_equasion.B2, line_a_equasion.C2},
												{line_b_equasion.A1, line_b_equasion.B1, line_b_equasion.C1},
												{line_b_equasion.A2, line_b_equasion.B2, line_b_equasion.C2} });
	
	Matrix extended_matrix({	{line_a_equasion.A1, line_a_equasion.B1, line_a_equasion.C1, line_a_equasion.D1},
														{line_a_equasion.A2, line_a_equasion.B2, line_a_equasion.C2, line_a_equasion.D2},
														{line_b_equasion.A1, line_b_equasion.B1, line_b_equasion.C1, line_b_equasion.D1},
														{line_b_equasion.A2, line_b_equasion.B2, line_b_equasion.C2, line_b_equasion.D2} });
	
	std::cout<<"ranks: "<<std::endl;
	std::cout<<base_matrix.Rank()<<" "<<extended_matrix.Rank()<<std::endl;
	
	Point3D result;
	
	if(Gauss(extended_matrix, result)){
		*intersection_point = result;
		Print(result);
		return true;
	}
	return false;
}



bool MathOperations::Gauss(const Matrix input_matrix, Point3D &intersection){
		//
	input_matrix.Print();
	const double EPS = CALCULATIONS_PRECISION;	//precision	
	std::vector<std::vector<double>> a = input_matrix.GetElements();
	std::vector<double> ans;
	int n = a.size();
	int m = a[0].size() - 1;
 
	std::vector<int> where (m, -1);
	for (int col=0, row=0; col<m && row<n; ++col) {
		int sel = row;
		for (int i=row; i<n; ++i)
			if (abs (a[i][col]) > abs (a[sel][col]))
				sel = i;
		if (abs (a[sel][col]) < EPS)
			continue;
		for (int i=col; i<=m; ++i)
			std::swap (a[sel][i], a[row][i]);
		where[col] = row;
 
		for (int i=0; i<n; ++i)
			if (i != row) {
				double c = a[i][col] / a[row][col];
				for (int j=col; j<=m; ++j)
					a[i][j] -= a[row][j] * c;
			}
		++row;
	}
 
	ans.assign (m, 0);
	for (int i=0; i<m; ++i)
		if (where[i] != -1)
			ans[i] = a[where[i]][m] / a[where[i]][i];
	for (int i=0; i<n; ++i) {
		double sum = 0;
		for (int j=0; j<m; ++j)
			sum += ans[j] * a[i][j];
		if (abs (sum - a[i][m]) > EPS)
			return false;
	}
 
	for (int i=0; i<m; ++i)
		if (where[i] == -1)
			return false;
	
	intersection = {ans[0], ans[1], ans[2]};
	return true;

}


double MathOperations::AngleBetweenVectors(const MathVector3D &vec_a, const MathVector3D &vec_b){
	double cosine;
	double numerator;
	double denominator;
	numerator = MathOperations::Dot(vec_a, vec_b);
	denominator = vec_a.Module() * vec_b.Module();
	if(denominator == 0)
		return 0;
	cosine = numerator / denominator;
	return acos(cosine);	
}

double MathOperations::DistanceBetweenPoints(const Point3D &point_a, const Point3D &point_b){
	MathVector3D new_vector(point_a, point_b);
	return new_vector.Module();
}