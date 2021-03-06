/*
 * Quadratic Programming Toolbox for Scilab using IPOPT library
 * Authors :
	Sai Kiran
	Keyur Joshi
	Iswarya


 * Optimizing (minimizing) the quadratic objective function having any number of variables and linear constraints.
 *
*/

#ifndef __QuadNLP_HPP__
#define __QuadNLP_HPP__

#include "IpTNLP.hpp"
extern "C"{
#include <sciprint.h>
}
using namespace Ipopt;

class QuadNLP : public TNLP
{
	private:
		Index numVars,numConstr; // Number of variables and constraints
		/*
		 * qMatrix is a pointer to matrix of size numVars X numVars with coefficents of quadratic terms in objective function.
		 * lMatrix is a pointer to matrix of size 1*numVars with coefficents of linear terms in objective function.
		 * conMatrix is a pointer to matrix of size numConstrXnumVars with coefficients of terms in a each objective in each row.
		 * conUB is a pointer to a matrix of size of 1*numConstr with upper bounds of all constraints.
		 * con
		*/
		Number *qMatrix,*lMatrix,*conMatrix,*conUB,*conLB,*varUB,*varLB; 
		QuadNLP(const QuadNLP&);
		QuadNLP& operator=(const QuadNLP&);
	public:
		/*
		 * Constructor 
		*/
		QuadNLP(Index nV, Index nC, Number *qM, Number *lM, Number *cM, Number *cUB, Number *cLB, Number *vUB, Number *vLB):
			numVars(nV),numConstr(nC),qMatrix(qM),lMatrix(lM),conMatrix(cM),conUB(cUB),conLB(cLB),varUB(vUB),varLB(vLB){

			sciprint("\nProblem Loaded\n");
			
			}
		/* Go to :

	http://www.coin-or.org/Ipopt/documentation/node23.html#SECTION00053130000000000000
		For details about these below methods.
		*/
		virtual ~QuadNLP();
		virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
								  Index& nnz_h_lag, IndexStyleEnum& index_style);
		virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
									 Index m, Number* g_l, Number* g_u);
		virtual bool get_starting_point(Index n, bool init_x, Number* x,
										bool init_z, Number* z_L, Number* z_U,
										Index m, bool init_lambda,
										Number* lambda);
		virtual bool eval_f(Index n, const Number* x, bool new_x, Number& obj_value);
		virtual bool eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f);
		virtual bool eval_g(Index n, const Number* x, bool new_x, Index m, Number* g);
		virtual bool eval_jac_g(Index n, const Number* x, bool new_x,
								Index m, Index nele_jac, Index* iRow, Index *jCol,
								Number* values);
		virtual bool eval_h(Index n, const Number* x, bool new_x,
							Number obj_factor, Index m, const Number* lambda,
							bool new_lambda, Index nele_hess, Index* iRow,
							Index* jCol, Number* values);
		virtual void finalize_solution(SolverReturn status,
									   Index n, const Number* x, const Number* z_L, const Number* z_U,
									   Index m, const Number* g, const Number* lambda, Number obj_value,
									   const IpoptData* ip_data,
									   IpoptCalculatedQuantities* ip_cq);

};

#endif __QuadNLP_HPP__
