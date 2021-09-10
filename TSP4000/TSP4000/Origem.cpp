#include<iostream> 
#include"ilcplex/ilocplex.h";
#include"ilconcert/ilolinear.h";
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
	// criacao do ambiente
	IloEnv env;

	// criacao do objeto modelo
	IloModel modelo(env);
	
#pragma region Variaveis de Decisao

	// variavel de custo	
	int** C; 
	int n; //tamanho dos vetores
	n = loadFile("a.csv", &C); //carrega dados

	// vetor de variaveis binarias
	IloArray<IloNumVarArray> X(env);
	IloNumVarArray t(env, n);
	
	// Cria variaveis x binárias	
	for (int i = 0; i < n; i++) {		
		for (int j = 0; j < n; j++) {
			X[i][j] = IloNumVar(env, 0, 1, ILOBOOL);
		}
	}
#pragma endregion

	// cria expressões para o modelo
	IloExpr expr(env);

#pragma region Restricoes

	// Restrições 1, 2 e 3 respectivamente
	IloRangeArray entrada(env, n);
	IloRangeArray saida(env, n);
	IloArray<IloRangeArray> mtz(env, n);

	// Restricao 1 (fluxo de entrada)
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				expr += X[j][i];
			}
		}
		entrada[i] = IloRange(env, 1, expr, 1);
	}

	// Adiciona restricao 1 ao modelo
	modelo.add(entrada);

	// Restricao 2 (fluxo de saida)
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				expr += X[i][j];
			}
		}
		saida[i] = IloRange(env, 1, expr, 1);
	}

	// Adiciona restricao 2 ao modelo
	modelo.add(saida);	

	// Create constraints 3)
   // The constraint is for i = 1,...,n and therefore we add empty constraints for i == 0
	mtz[0] = IloRangeArray(env);
	// We then continue normally for all other i > 0
	for (auto i = 1u; i < n; ++i) {
		mtz[i] = IloRangeArray(env, n);
		for (auto j = 1u; j < n; ++j) {
			expr = t[i] - t[j] + static_cast<int>(n) * X[i][j];

			
			mtz[i][j] = IloRange(env, -IloInfinity, expr, n - 1);
		}
		// Add constraints 3)[i] to the model
		modelo.add(mtz[i]);
	}

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
	IloObjective obj = IloMinimize(env, expr);

#pragma endregion

	// Adiciona funcao objetivo ao modelo
	modelo.add(obj);

	// Finaliza expressões, desalocando memória
	expr.end();

	// Cria o objeto conversor para a linguagem cplex
	IloCplex cplex(modelo);	

	//imprime matriz de distancias (custos)
	printMatrix(n, C);

	try {
		cplex.extract(modelo);
		cplex.solve();
		double result = cplex.getObjValue(); //retorna valor da função objetivo
		cout << "Valor da funcao objetivo: " << result;
		cout << "\n";
	}
	catch (IloException& ex) {
		cout << "Solucao nao encontrada" << endl;
	}
		
}