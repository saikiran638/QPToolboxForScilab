/*
 * Quadratic Programming Toolbox for Scilab using IPOPT library
 * Authors :
	Sai Kiran
	Keyur Joshi
	Iswarya
 */

#include "QuadNLP.hpp"

extern "C"{ 
#include <sciprint.h>
}

using namespace Ipopt;

QuadNLP::~QuadNLP()
{}

//get NLP info such as number of variables,constraints,no.of elements in jacobian and hessian to allocate memory
bool QuadNLP::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g, Index& nnz_h_lag, IndexStyleEnum& index_style){
	n=numVars; // Number of variables
	m=numConstr; // Number of constraints
	nnz_jac_g = n*m; // No. of elements in Jacobian of constraints 
	nnz_h_lag = n*(n+1)/2; // No. of elements in lower traingle of Hessian of the Lagrangian.
	index_style=C_STYLE; // Index style of matrices
	return true;
	}

//get variable and constraint bound info
bool QuadNLP::get_bounds_info(Index n, Number* x_l, Number* x_u, Index m, Number* g_l, Number* g_u){
	
	unsigned int i;
	for(i=0;i<n;i++){
		x_l[i]=varLB[i];
		x_u[i]=varUB[i];
		}

	for(i=0;i<m;i++){
		g_l[i]=conLB[i];
		g_u[i]=conUB[i];
		}
	return true;
	}

//get value of objective function at vector x
bool QuadNLP::eval_f(Index n, const Number* x, bool new_x, Number& obj_value){
	unsigned int i,j;
	Number temp;
	obj_value=0;

	for (i=0;i<n;++i){
		for (j=0;j<n;++j){
			obj_value+=x[i]*x[j]*qMatrix[n*i+j];			
			}
		obj_value+=x[i]*lMatrix[i];
		}
	return true;
	}

//get value of gradient of objective function at vector x.
bool QuadNLP::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f){
	unsigned int i,j;
	for(i=0;i<n;i++){
		grad_f[i]=lMatrix[i];
		for(j=0;j<n;j++)
			grad_f[i]+=(qMatrix[n*i+j]+qMatrix[n*j+i])*x[j];
		}
	return true;
	}

//Get the values of constraints at vector x.
bool QuadNLP::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g){
	unsigned int i,j;
	for(i=0;i<m;i++){
		g[i]=0;
		for(j=0;j<n;j++)
			g[i]+=x[j]*conMatrix[n*i+j];
		}
	return true;
	}

// This method sets initial values for required vectors . For now we are assuming 0 to all values. 
bool QuadNLP::get_starting_point(Index n, bool init_x, Number* x,
										bool init_z, Number* z_L, Number* z_U,
										Index m, bool init_lambda,
										Number* lambda){
	if (init_x == true){ //we need to set initial values for vector x
		for (Index var=0;var<n;++var)
			x[var]=0.0;//initialize with 0 or we can change.
		}

	if (init_z == true){ //we need to provide initial values for vector bound multipliers
		for (Index var=0;var<n;++var){
			z_L[var]=0.0; //initialize with 0 or we can change.
			z_U[var]=0.0;//initialize with 0 or we can change.
			}
		}
	
	if (init_lambda == true){ //we need to provide initial values for lambda values.
		for (Index var=0;var<m;++var){
			lambda[var]=0.0; //initialize with 0 or we can change.
			}
		}

	return true;
	}
/* Return either the sparsity structure of the Jacobian of the constraints, or the values for the Jacobian of the constraints at the point x.

*/ 
bool QuadNLP::eval_jac_g(Index n, const Number* x, bool new_x,
								Index m, Index nele_jac, Index* iRow, Index *jCol,
								Number* values){
	
	//It asked for structure of jacobian.
	if (values==NULL){ //Structure of jacobian (full structure)
		int index=0;
		for (int var=0;var<m;++var)//no. of constraints
			for (int flag=0;flag<n;++flag){//no. of variables
				iRow[index]=var;
				jCol[index]=flag;
				index++;
				}
		}
	//It asked for values
	else { 
		int index=0;
		for (int var=0;var<m;++var)
			for (int flag=0;flag<n;++flag)
				values[index++]=conMatrix[n*var+flag];
		}
	return true;
	}

/*
 * Return either the sparsity structure of the Hessian of the Lagrangian, 
 * or the values of the Hessian of the Lagrangian  for the given values for
 * x,lambda,obj_factor.
*/
bool QuadNLP::eval_h(Index n, const Number* x, bool new_x,
							Number obj_factor, Index m, const Number* lambda,
							bool new_lambda, Index nele_hess, Index* iRow,
							Index* jCol, Number* values){
	if (values==NULL){
		Index idx=0;
		for (Index row = 0; row < n; row++) {
			for (Index col = 0; col <= row; col++) {
				iRow[idx] = row;
				jCol[idx] = col;
				idx++;
		  		}
			}
		}
	else {
		Index index=0;
		for (Index row=0;row < n;++row){
			for (Index col=0; col <= row; ++col){
				values[index++]=obj_factor*(qMatrix[n*row+col]+qMatrix[n*col+row]);
				}
			}
		}
	return true;
	}

/*
 * This method will print result of the problem to user.
 */

void QuadNLP::finalize_solution(SolverReturn status,
									   Index n, const Number* x, const Number* z_L, const Number* z_U,
									   Index m, const Number* g, const Number* lambda, Number obj_value,
									   const IpoptData* ip_data,
									   IpoptCalculatedQuantities* ip_cq){

	// Display result of the problem to user.
	sciprint("\nProblem solved::\n");
	
	sciprint("Solution of the primal variables::\n");
 	 for (Index i=0; i<n; i++) {
    		 sciprint("x[%d] == %lf\n",i+1,x[i]);
 		 }


	sciprint("Solution of the bound multipliers, z_L and z_U::\n");
  	for (Index i=0; i<n; i++) {
   		sciprint("z_L[%d] = %lf\n",i+1, z_L[i]);
 		 }
	 for (Index i=0; i<n; i++) {
    		sciprint("z_U[%d] = %lf\n",i+1, z_U[i]);
  		}

	sciprint("\nObjective value::\n");

	sciprint("f(x*) = %lf\n",obj_value);

	sciprint("\nThe multipliers for constraints ::\n");
	for (Index i= 0; i<m;++i){
		sciprint("lambda[%d] = %lf\n",i+1,lambda[i]);
		}

 	sciprint("Final value of the constraints::\n");
	for (Index i=0; i<m ;i++) {
  		sciprint( "g(%d) = %lf\n",i+1, g[i]);
  		}

	}

