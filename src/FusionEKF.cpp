#include "FusionEKF.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>
#include <math.h>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/*
 * Constructor.
 */
FusionEKF::FusionEKF() {
    
  cout<< "in FusionEKF constructor" << endl;
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  Hj_ = MatrixXd(3, 4);

  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
        	  0, 0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0, 0,
        	0, 0.0009, 0,
        	0, 0, 0.09;
   
   // used to call CalculateJacobian method 
   Tools tools; 
  /**
  TODO:
    * Finish initializing the FusionEKF.
    * Set the process and measurement noises
  */
   
    
  //create a 4D state vector
  VectorXd x_in = VectorXd(4);
  x_in << 0, 0, 0, 0;
  //state covariance matrix P
  MatrixXd P_in = MatrixXd(4, 4);
  P_in << 1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 500, 0,
          0, 0, 0, 500;


  //measurement matrix
   
  H_laser_ << 1, 0, 0, 0,
              0, 1, 0, 0;

  //the initial transition matrix F_
  MatrixXd F_in = MatrixXd(4, 4);
  F_in << 1, 0, 1, 0,
          0, 1, 0, 1,
          0, 0, 1, 0,
          0, 0, 0, 1;

  MatrixXd R_in = MatrixXd(2, 2);
  MatrixXd Q_in = MatrixXd(4, 4);

  // initlaize all kalman matrices
  //Init(Eigen::VectorXd &x_in, Eigen::MatrixXd &P_in, Eigen::MatrixXd &F_in, 
  //Eigen::MatrixXd &H_in, Eigen::MatrixXd &R_in, Eigen::MatrixXd &Q_in);
  ekf_.Init(x_in,P_in,F_in,H_laser_,R_in,Q_in);


  //set the acceleration noise components
  float noise_ax = 9.0;
  float noise_ay = 9.0;
	
}

/**
* Destructor.
*/
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {


  /*****************************************************************************
   *  Initialization
   ****************************************************************************/
  if (!is_initialized_) {
    /**
    TODO:
      * Initialize the state ekf_.x_ with the first measurement.
      * Create the covariance matrix.
      * Remember: you'll need to convert radar from polar to cartesian coordinates.
    */
    // first measurement
    cout << "EKF: " + measurement_pack.sensor_type_ << endl;
    //ekf_.x_ = VectorXd(4);
    ekf_.x_ << 1, 1, 1, 1;


    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      /**
      Convert radar from polar to cartesian coordinates and initialize state.
      */
      cout<< "in ProcessMeasurement for first reading of RADAR" << endl;
      ekf_.x_ = tools.Convert_polar_to_cartesian(measurement_pack.raw_measurements_);
      
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      /**
      Initialize state.      
      */
      cout<< "in ProcessMeasurement for first reading of LASER" << endl;
	  ekf_.x_ << measurement_pack.raw_measurements_[0], measurement_pack.raw_measurements_[1], 0, 0;
      
    }

    // done initializing, no need to predict or update
    previous_timestamp_ = measurement_pack.timestamp_;
	is_initialized_ = true;
	return;
  }

  /*****************************************************************************
   *  Prediction
   ****************************************************************************/

  /**
   TODO:
     * Update the state transition matrix F according to the new elapsed time.
      - Time is measured in seconds.
     * Update the process noise covariance matrix.
     * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
  
  //compute the time elapsed between the current and previous measurements
  float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;	//dt - expressed in seconds
  previous_timestamp_ = measurement_pack.timestamp_;
  
  //Modify the F matrix so that the time is integrated
  ekf_.F_(0,2)=dt;
  ekf_.F_(1,3)=dt;
  
  // Set the process covariance matrix Q
  float dt_4 = pow(dt,4);
  float dt_2 = pow(dt,2);
  float dt_3 = pow(dt,3);
	
  ekf_.Q_ << dt_4/4*noise_ax,  0,  dt_3/2*noise_ax,    0,
	          0,  dt_4/4*noise_ay,  0,  dt_3/2*noise_ay,
	          dt_3/2*noise_ax,  0,  dt_2*noise_ax, 0,
	          0, dt_3/2*noise_ay,    0,  dt_2*noise_ay;
   
  ekf_.Predict();

  /*****************************************************************************
   *  Update
   ****************************************************************************/

  /**
     * Use the sensor type to perform the update step.
     * Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    // do not process radar
    //return ;
    // Radar updates
    ekf_.R_=R_radar_;
    ekf_.H_=tools.CalculateJacobian(ekf_.x_);
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);
    
  } else {
    // Laser updates
    // do not process lidar
    //return ;
    ekf_.R_=R_laser_;
    ekf_.H_=H_laser_;
    ekf_.Update(measurement_pack.raw_measurements_);
  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}
