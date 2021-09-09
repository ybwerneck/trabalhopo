#include<iostream> 
#include"ilcplex/ilocplex.h";
#include <cmath>
#include <limits>

using namespace std;

void printMatrix(int n, int** c) {

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			std::cout << c[i][j] << " ";
		std::cout << std::endl;

	}
}
int countLines(string s) {
	std::string line;
	std::ifstream file(s);
	if (!file.is_open())
		std::cout << ("Could not open file :") << s;
	int nc = 0;
	while (std::getline(file, line))
	{
		nc++; //conta colunas
	}
	file.close();
	return nc;
}
 int loadFile(string s,int*** c) {
	
	 std::string line;
	 int val;
	 int nc = countLines(s);
	 (*c) = new int* [nc];
	 int lineIdx = 0;
	 std::ifstream myFile(s);
	 while (std::getline(myFile, line))
	 {
		 std::stringstream ss(line);
		 int colIdx = 0;
	     int* aux = new int[nc];
		 while (ss >> val) {
			 aux[colIdx] = val;
			 if (ss.peek() == ',') ss.ignore();
			 colIdx++;
		 }
		 (*c)[lineIdx++] = aux;
	
	 }
	 myFile.close();
	 return nc;
}
int main()
{
	IloEnv env;
	IloModel modelo(env);
	
#pragma region Dados
	

	//Matriz de distancias
	int** C;
	int n=loadFile("a.csv", &C);
	int nd = n;
	printMatrix(n, C);

	/*
	C[0] = new int[nD] {0 , 2,29,22,13,24};
	C[1] = new int[nD] {12, 0,19, 3,25, 6};
	C[2] = new int[nD] {29,19,0 ,21,23,25};
	C[3] = new int[nD] {22, 3,21, 0, 4, 5};
	C[4] = new int[nD] {13,25,23, 4, 0,16};
	C[5] = new int[nD] {24, 6,28, 5,16, 0};
	*/
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
	// Restrição de i == 0
	mtz[0] = IloRangeArray(env);
	// Continua i > 0
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

	try {
		double result = cplex.getObjValue(); //retorna valor da função objetivo
		cout << "Valor do objetivo: " << result << "\n";
	}
	catch (IloException& ex) {
		cerr << ex << endl;
	}

	// solved = cplex.solve();	
}