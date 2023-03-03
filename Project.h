#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <assert.h>
#include "Facets.h"
#include "Contours.h"
#include "Errors.h"

using ProjectName = std::string;

struct ProjectSettings{
	ProjectName new_project_name;
	WorkField* new_workfield;
	
	ProjectSettings();
	~ProjectSettings();
};

class Project{
	private:
		ProjectName project_name;
		std::string filename;
		WorkField* workfield;	
		CompositeFacetBody* composite_facet_body;
		ContoursAggregator* contours_aggregator;
	public:
		Project();
		Project(const ProjectSettings &settings);
		~Project();
		void Update(const ProjectSettings &settings);
		bool AssignWorkField(WorkField *new_workfield);
		WorkField* GetAssignedWorkField();
		void AssignCompositeFacetBody(CompositeFacetBody* new_composite_facet_body);
		CompositeFacetBody* GetAssignedCompositeFacetBody() const;
		void AssignContoursAggregator (ContoursAggregator* new_contours_aggregator);
		ContoursAggregator* GetAssignedContoursAggregator() const;
		void SetProjectName(std::string new_project_name);
		std::string GetProjectName();
		void SetFileName(std::string new_filename);
		std::string GetFilename();
};




#endif