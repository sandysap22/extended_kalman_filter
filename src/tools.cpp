#include <iostream>
#include "tools.h"
#include <math.h>

using Eigen::VectorXd;
using Eigen::MatrixXd;
using std::vector;

Tools::Tools() {}

Tools::~Tools() {}

VectorXd Tools::CalculateRMSE(const vector<VectorXd> &estimations,
                              const vector<VectorXd> &ground_truth) {
  /**
  TODO:
    * Calculate the RMSE here.
  */

   //int n_features = estimations[0].size();
   //VectorXd rmse(n_features);
   
   //cout << "in Tools::CalculateRMSE" << endl;
   
   VectorXd rmse(4);
   rmse << 0,0,0,0;

   //accumulate squared residuals
   for(int i=0; i < estimations.size(); ++i){
		
		VectorXd estimation = estimations[i];
        VectorXd ground_th = ground_truth[i];
		 if (estimation.size() == 0 ){
		    cout << "estimations vector should not be empty " << endl;
		    continue;
		 }
		
    	 if (estimation.size() != ground_th.size() ){
		    cout << "estimation and ground thruth vector size should match " << endl;
		    continue;
		 }
		
		VectorXd deviation= estimation - ground_th; // deviation in each feature
		VectorXd square_deviation = deviation.array() * deviation.array(); // sqare of deviation
		rmse += square_deviation;  // accumulating square of deviations
		
		//cout<<"rmse"<< endl << rmse << endl;
	} // end of for

    //calculate the mean values of each feature
    rmse = rmse/estimations.size();
	//calculate the squared root of mean
	rmse=rmse.array().sqrt();
	
    //return the result
	return rmse;
	
}

MatrixXd Tools::CalculateJacobian(const VectorXd& x_state) {
  /**
    * Calculate a Jacobian here.
  */
    //cout << "in Tools::CalculateJacobian" << endl;
	MatrixXd Hj(3,4);
	//recover state parameters
	float px = x_state(0);
	float py = x_state(1);
	float vx = x_state(2);
	float vy = x_state(3);

	//check division by zero
	if (px==0.0 && py==0.0){
	    cout << "px and py are zero" << endl;
	}
	
	//compute the Jacobian matrix
	float px_2=px * px;
	float py_2=py * py;
	
	float sqr_sum = px_2 + py_2;
	float sum_sqrt = sqrt(sqr_sum);
	float sum_3_by_2 = pow(sqr_sum,3.0/2.0);

    if(fabs(sqr_sum) < 0.0001) {
		cout << "Error : Devision by zero, px and py squre sum is zero." << endl;
		return Hj;
	}
	
	Hj(0,0)=px/sum_sqrt;
	Hj(0,1)=py/sum_sqrt;
	Hj(0,2)=0.0;
	Hj(0,3)=0.0;
	
    Hj(1,0)=-(py/sqr_sum);
	Hj(1,1)=px/sqr_sum;
	Hj(1,2)=0.0;
	Hj(1,3)=0.0;
	
	Hj(2,0)=py*(vx*py - vy*px)/sum_3_by_2;
	Hj(2,1)=px*(vy*px - vx*py)/sum_3_by_2;
	Hj(2,2)=px/sum_sqrt;
	Hj(2,3)=py/sum_sqrt;

	return Hj;
}

VectorXd Tools::Convert_cartesian_to_polar(const VectorXd& x_state){
  
  const double THRESH = 0.0001;
  VectorXd polar_vector(3);

  const double px = x_state(0);
  const double py = x_state(1);
  const double vx = x_state(2);
  const double vy = x_state(3);

  const double rho = sqrt( px * px + py * py);


  const bool degenerate = (py == 0.0 && px == 0.0);
  //const double phi = degenerate ? 0.0 : atan2(py, px);
  const double phi = atan2(py, px);
  // Avoid dividing by zero
  const double rho_dot = (rho > THRESH) ? ( px * vx + py * vy ) / rho : 0.0;

   

  polar_vector << rho, phi, rho_dot;
  return polar_vector;
}

VectorXd Tools::Convert_polar_to_cartesian(const VectorXd& x_meaurements){
    
  VectorXd cartesian_vector(4);
  float rho = x_meaurements[0];
  float phi = x_meaurements[1];
  float rho_dot = x_meaurements[2];
   
  float x = rho * cos(phi);
  float y = rho * sin(phi);
  float vx = rho_dot * cos(phi);
  float vy = rho_dot * sin(phi);
    
 
  cartesian_vector << x,y,vx,vy;
  
  return cartesian_vector;
  
}
