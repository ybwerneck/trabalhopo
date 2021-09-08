#include<iostream> 
#include"ilcplex/ilocplex.h";

using namespace std;
typedef IloArray<IloNumVarArray> NumVar2D; // enables us to defien a 2-D decision varialbe
typedef IloArray<NumVar2D> NumVar3D;


int main()
{
	
#pragma region Dados
	int nS = 4;
	int nD = 3;

	int* S = new int[nS] {10, 30, 40, 20};
	int* D = new int[nD] {20, 50, 30};

	int** C = new int* [nS];
	C[0] = new int[nD] {2, 3, 4};
	C[1] = new int[nD] {3, 2, 1};
	C[2] = new int[nD] {1, 4, 3};
	C[3] = new int[nD] {4, 5, 2};
#pragma endregion

	IloEnv env;
	IloModel Model(env);

#pragma region Define decision variable
	NumVar2D X(env, nS);

	for (int i = 0; i < nS; i++) {
		X[i] = IloNumVarArray(env, nD, 0, IloInfinity, ILOINT);
	}
#pragma endregion

#pragma region Objective Function

	IloExpr exp0(env);

	for (int i = 0; i < nS; i++) {
		for (int j = 0; j < nD; j++) {
			exp0 += C[i][j] * X[i][j];
		}
	}

	Model.add(IloMinimize(env, exp0));

#pragma endregion

#pragma region Constraints
	
	//restrição 1
	for (int i = 0; i < nS; i++){
		IloExpr exp1(env);
		for (int j = 0; j < nD; j++) {
			exp1 += X[i][j];
		}

		Model.add(exp1 <= S[i]);
	}

	//restrição 2
	for (int j = 0; j < nD; j++){
		IloExpr exp2(env);
		for (int i = 0; i < nS; i++) {
			exp2 += X[i][j];
		}

		Model.add(exp2 >= D[j]);
	}
#pragma endregion

	IloCplex cplex(Model);
	cplex.setOut(env.getNullStream());
	if (!cplex.solve()) {
		env.error() << "Failed to optimize the Master Problem!!!" << endl;
		throw(-1);
	}

	double obj = cplex.getObjValue();

	cout << "\n\n\t Caminho minimo: " << obj << endl;

	for (int i = 0; i < nS; i++)
	{
		for (int j = 0; j < nD; j++)
		{
			double Xval = cplex.getValue(X[i][j]);
			if (Xval > 0){
				cout << "\t\t\t X[" << i << "][" << j << "] = " << Xval << endl;
			}

		}
	}
}