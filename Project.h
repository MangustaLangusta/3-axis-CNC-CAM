#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include "Facets.h"
#include "Errors.h"

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
		void SetProjectName(std::string new_project_name);
		std::string GetProjectName();
		void SetFileName(std::string new_filename);
		std::string GetFilename();
};




#endif