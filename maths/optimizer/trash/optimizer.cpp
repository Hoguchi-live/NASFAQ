#include "optimizer.h"

namespace operations_research {
void BasicExample() {
	// Create the linear solver with the GLOP backend.
  	std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
	if (!solver) {
  		LOG(WARNING) << "SCIP solver unavailable.";
  		return;
	}

	// Create the variables x and y.
	MPVariable* const x = solver->MakeIntVar(0.0, 1, "x");
	MPVariable* const y = solver->MakeIntVar(0.0, 2, "y");

	LOG(INFO) << "Number of variables = " << solver->NumVariables();

	// Create a linear constraint, 0 <= x + y <= 2.
	MPConstraint* const ct = solver->MakeRowConstraint(0.0, 2.0, "ct");
	ct->SetCoefficient(x, 1);
	ct->SetCoefficient(y, 1);

	LOG(INFO) << "Number of constraints = " << solver->NumConstraints();

	// Create the objective function, 3 * x + y.
	MPObjective* const objective = solver->MutableObjective();
	objective->SetCoefficient(x, 3);
	objective->SetCoefficient(y, 1);
	objective->SetMaximization();

	solver->Solve();

	LOG(INFO) << "Solution:" << std::endl;
	LOG(INFO) << "Objective value = " << objective->Value();
	LOG(INFO) << "x = " << x->solution_value();
	LOG(INFO) << "y = " << y->solution_value();
}
}  // namespace operations_research
