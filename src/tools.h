#ifndef TOOLS_H_
#define TOOLS_H_
#include <vector>
#include "Eigen/Dense"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using namespace std;

class Tools {
public:
  /**
  * Constructor.
  */
  Tools();

  /**
  * Destructor.
  */
  virtual ~Tools();

  /**
  * A helper method to calculate RMSE.
  */
  VectorXd CalculateRMSE(const vector<VectorXd> &estimations, const vector<VectorXd> &ground_truth);

  /**
  * A helper method to calculate Jacobians.
  */
  MatrixXd CalculateJacobian(const VectorXd& x_state);
  
  /**
  * A helper method convert cartesian_to_polar
  */
  VectorXd Convert_cartesian_to_polar(const VectorXd& x_state);
  
  /**
  * A helper method convert polar to cartecian
  */
  VectorXd Convert_polar_to_cartesian(const VectorXd& x_first);

};

#endif /* TOOLS_H_ */

