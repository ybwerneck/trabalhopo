#include<iostream> 
#include"ilcplex/ilocplex.h";
#include <cmath>
#include <limits>

using namespace std;

int main()
{
	IloEnv env;
	IloModel modelo(env);
	
#pragma region Dados
	int n = 4;
	int nD = 4;

	//Matriz de distancias
	int** C = new int* [n];
	C[0] = new int[nD] {2, 3, 4, 1};
	C[1] = new int[nD] {3, 2, 1, 4};
	C[2] = new int[nD] {1, 4, 3, 6};
	C[3] = new int[nD] {4, 5, 2, 4};
#pragma endregion


#pragma region Variaveis de Decisao

	// variaveis de decisao
	IloArray<IloNumVarArray> X(env, n); 
	IloNumVarArray t(env, n); 

	// Restrições 1, 2 e 3 respectivamente
	IloRangeArray inbound_arcs(env, n);  
	IloRangeArray outbound_arcs(env, n); 
	IloArray<IloRangeArray> mtz(env, n); 

	// variavel fixa em valor 1
	t[0] = IloNumVar(env, 1, 1, IloNumVar::Int); 

	// Criar variavel t[1],...., t[n]
	for (int i = 1; i < n; i++) {
		t[i] = IloNumVar(env, 2, n, IloNumVar::Int);		
	}

	// Cria variaveis x binárias	
	for (int i = 0; i < n; i++) {
		X[i] = IloNumVarArray(env, n);
		for (auto j = 0u; j < n; ++j) {
			X[i][j] = IloNumVar(env, 0, 1, IloNumVar::Bool);
		}
	}
#pragma endregion

	IloExpr expr(env);

#pragma region Restricoes

	// Restricao 1
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			expr += X[j][i];
		}
		inbound_arcs[i] = IloRange(env, 1, expr, 1);
	}

	// Adiciona restricao 1 ao modelo
	modelo.add(inbound_arcs);

	// Restricao 2
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			expr += X[i][j];
		}
		outbound_arcs[i] = IloRange(env, 1, expr, 1);
	}

	// Adiciona restricao 2 ao modelo
	modelo.add(outbound_arcs);

	// Restricao 3
	// The constraint is for i = 1,...,n and therefore we add empty constraints for i == 0
	mtz[0] = IloRangeArray(env);
	// We then continue normally for all other i > 0
	for (int i = 1; i < n; i++) {
		mtz[i] = IloRangeArray(env, n);
		for (int j = 1; j < n; j++) {
			expr = t[i] - t[j] + static_cast<int>(n) * X[i][j];			
			mtz[i][j] = IloRange(env, -IloInfinity, expr, n - 1);
		}
		// Adiciona restricao 3 ao modelo
		modelo.add(mtz[i]);
	}

#pragma endregion

#pragma region Funcao Objetivo

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			expr += C[i][j] * X[i][j];
		}
	}
	IloObjective obj(env, expr, IloObjective::Minimize);

	// Adiciona funcao objetivo ao modelo
	modelo.add(obj);

	// Finaliza expr
	expr.end();

#pragma endregion

	// Cria o objeto solver
	IloCplex cplex(modelo);

	double result = cplex.getObjValue();
	cout << "Valor do objetivo: " << result << "\n";
	// solved = cplex.solve();	
}