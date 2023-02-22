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
		
CompositeFacetBody* Project::GetAssignedCompositeFacetBody() const{
	return composite_facet_body;
}

void Project::AssignContoursAggregator (ContoursAggregator* new_contours_aggregator){
	assert(new_contours_aggregator != NULL);
	contours_aggregator = new_contours_aggregator;
}

ContoursAggregator* Project::GetAssignedContoursAggregator() const{
	return contours_aggregator;
}

void Project::SetProjectName(std::string new_project_name){
	project_name = new_project_name;
}

std::string Project::GetProjectName(){
	return project_name;
}

void Project::SetFileName(std::string new_filename){
	filename = new_filename;
}

std::string Project::GetFilename(){
	return filename;
}