#include "Project.h"

ProjectSettings::ProjectSettings() {
	new_workfield = NULL;
	new_project_name = "";
}

ProjectSettings::~ProjectSettings() {}

Project::Project(){
	composite_facet_body = NULL;
	contours_aggregator = NULL;
	workfield = NULL;
}

Project::Project(const ProjectSettings &settings){
	if(settings.new_project_name == "")
		project_name = "noname";
	else 
		project_name = settings.new_project_name;
	
	workfield = NULL;
	AssignWorkField(settings.new_workfield);
		
	composite_facet_body = NULL;
	contours_aggregator = NULL;
}

Project::~Project(){}

void Project::Update(const ProjectSettings &settings){
	if(settings.new_project_name != "")
		project_name = settings.new_project_name;
	
	AssignWorkField(settings.new_workfield);
}

bool Project::AssignWorkField(WorkField *new_workfield){
	if(new_workfield == NULL)
		return false;
	if(!new_workfield->IsValid())
		return false;
	delete workfield;
	workfield = new_workfield;
	return true;
}

WorkField* Project::GetAssignedWorkField(){
	return workfield;
}

void Project::AssignCompositeFacetBody(CompositeFacetBody* new_composite_facet_body){
	composite_facet_body = new_composite_facet_body;
}
		
CompositeFacetBody* Project::GetAssignedCompositeFacetBody() const{
	return composite_facet_body;
}

void Project::AssignContoursAggregator (ContoursAggregator* new_contours_aggregator){
	assert(new_contours_aggregator != NULL);
	delete contours_aggregator;
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