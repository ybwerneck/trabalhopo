Tutorial para integrar bibliotecas CPLEX à IDE Visual Studio:

Primeiramente faça o download da IDE CPLEX em https://www.ibm.com/br-pt/analytics/cplex-optimizer e instale o software.

Em seguida, inicie o Visual Studio e abra o projeto TSP4000. Acesse a janela lateral "Gerenciador de Soluções" e dê dois cliques no arquivo TSP4000.sln para abrir completamente o projeto. Logo em seguida, na nova janela que irá aparecer, clique com botão direito em TSP4000 e acesse as propriedades.
No topo da janela, em "Plataforma", escolha Todas as Plataformas.
No menu Vinculador/Entrada, clique em "editar" em Dependências Adicionais e adicione os seguintes arquivos, localizados na pasta raiz do programa CPLEX Studio:

D:\Arquivos de Programas\IBM\ILOG\CPLEX_Studio_Community201\cplex\lib\x64_windows_msvc14\stat_mda\cplex2010.lib
D:\Arquivos de Programas\IBM\ILOG\CPLEX_Studio_Community201\cplex\lib\x64_windows_msvc14\stat_mda\ilocplex.lib
D:\Arquivos de Programas\IBM\ILOG\CPLEX_Studio_Community201\concert\lib\x64_windows_msvc14\stat_mda\concert.lib

Logo em seguida, acesse C/C++ -> Geral e em Diretórios de Inclusão Adicionais, clique em editar e adicione os seguintes caminhos, um a um:

D:\Arquivos de Programas\IBM\ILOG\CPLEX_Studio_Community201\concert\include
D:\Arquivos de Programas\IBM\ILOG\CPLEX_Studio_Community201\cplex\include

Em C/C++ -> Pré-processador, adicione a seguinte instrução em Definições do Pré-processador:

NDEBUG
_CONSOLE
IL_STD

Por último acesse C/C++ -> Geração de código a Biblioteca  em Tempo de Execução e escolha a opção DLL Multithread (/MD).
Aplique as alterações e em seguida clique em OK.
Ao compilar o código, escolha a opção ao lado do depurador do programa que se adeque ao seu sistema (x64 ou x86).