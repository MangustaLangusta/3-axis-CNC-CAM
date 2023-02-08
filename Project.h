#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include "Facets.h"
#include "ErrorCodes.h"

class Project{
	private:
		std::string project_name;
		std::string filename;
		CompositeFacetBody* composite_facet_body;
		
	public:
		Project();
		Project(std::string new_project_name);
		~Project();
		void AssignCompositeFacetBody(CompositeFacetBody* new_composite_facet_body);
		CompositeFacetBody* GetAssignedCompositeFacetBody();
		std::string GetProjectName();
};




#endif