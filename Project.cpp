#include "Project.h"

Project::Project(){
	composite_facet_body = NULL;
}

Project::Project(std::string new_project_name){
	project_name = new_project_name;
}

Project::~Project(){}

void Project::AssignCompositeFacetBody(CompositeFacetBody* new_composite_facet_body){
	composite_facet_body = new_composite_facet_body;
}
		
CompositeFacetBody* Project::GetAssignedCompositeFacetBody(){
	return composite_facet_body;
}

std::string Project::GetProjectName(){
	return project_name;
}