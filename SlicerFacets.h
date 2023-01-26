#ifndef SLICER_FACETS_H
#define SLICER_FACETS_H

struct Facet{
		long int id;
		std::vector<PreciseDekartCoords> vertex_vector;
		PreciseDekartCoords normal;
		double max_z;
		double min_z;
		Facet(){
			vertex_vector.resize(3);
		}
		void PrintFacet() {
			normal.PrintCoord();
			for(int i = 0; i < 3; i++){
				std::cout<<vertex_vector[i].x<<" "<<vertex_vector[i].y<<" "<<vertex_vector[i].z<<std::endl;
			}
			std::cout<<std::endl;
		}
};

class FacetsSet{
	private:
		std::vector<Facet> facets_vector;
		double max_z;
		bool InterceptionOfFacetAndZPlane(double z_plane, Facet facet, std::vector<PreciseLine2D> *interception_lines){
			std::vector <PrecisePoint2D> interception_points;
			PrecisePoint2D point;
			PreciseDekartCoords dekart_point;
			if((facet.max_z >= z_plane)&&(facet.min_z <= z_plane)){
				double delta_z, z1, z2;
				for(int i = 0; i < 3; i++){
					z1 = facet.vertex_vector[i].z;
					if (z1 == z_plane){
						point.x = facet.vertex_vector[i].x;
						point.y = facet.vertex_vector[i].y;
						interception_points.push_back(point);
						continue;
					}
					z2 = facet.vertex_vector[(i+1) % 3].z;
					delta_z = z1 - z2;
					if(delta_z == 0) 
						delta_z = 1;
					if( ( (z1 > z_plane) && (z2 < z_plane) ) || ( (z1 < z_plane) && (z2 > z_plane) ) ) {
						point.x = (facet.vertex_vector[(i+1) % 3].x - facet.vertex_vector[i].x) * (facet.vertex_vector[i].z - z_plane) / delta_z + facet.vertex_vector[i].x;
						point.y = (facet.vertex_vector[(i+1) % 3].y - facet.vertex_vector[i].y) * (facet.vertex_vector[i].z - z_plane) / delta_z + facet.vertex_vector[i].y;
						interception_points.push_back(point);	
					}
				}
			}
			if(interception_points.size() == 2){
				dekart_point.x = interception_points[1].x - interception_points[0].x;
				dekart_point.y = interception_points[1].y - interception_points[0].y;
				if (VectorMultZSign(facet.normal, dekart_point))
					interception_lines->push_back( {interception_points[0], interception_points[1]} );
				else
					interception_lines->push_back( {interception_points[1], interception_points[0]} );
				return true;
			}
			return false;
		}
	public:
		void AddFacet(Facet f) { 
			f.id = facets_vector.size();
			facets_vector.push_back(f); 
		}
		void SortTopToBottom(){
			std::sort(facets_vector.begin(),facets_vector.end(), [](Facet a, Facet b) {
				return a.max_z > b.max_z;
			} );
			if (!facets_vector.empty()){
				max_z = facets_vector[0].max_z;
			}
		}
		void PrintIdOrder(){
			for(auto it = facets_vector.begin(); it != facets_vector.end(); it++)
				std::cout<<it->id<<" ";
			std::cout<<std::endl;
		}
		void PrintZOrder(){
			for(auto it = facets_vector.begin(); it != facets_vector.end(); it++)
				std::cout<<it->max_z<<" ";
			std::cout<<std::endl;
		}
		double GetMaxZ(){return max_z;}
		
		bool InterceptionLinesOnZPlane(double z_plane, std::vector<PreciseLine2D> *interception_lines) {
			interception_lines->clear();
			for(auto it = facets_vector.begin(); it != facets_vector.end(); it++){
				if(it->max_z >= z_plane){
					if(it->min_z <= z_plane){
						InterceptionOfFacetAndZPlane(z_plane, *it, interception_lines);
					}
				}
				else 
					break;
			}
			//print
			/*
			std::cout<<"prining lines for z = "<<z_plane<<std::endl;
			for(auto it = interception_lines->begin(); it != interception_lines->end(); it++){
				it->print_line();
			}*/
			if(interception_lines->empty())
				return false;
			return true;
		}
};

#endif