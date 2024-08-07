#include <iostream>
#include "planejador.h"

using namespace std;

int main()
{
  // O planejador de caminhos
  Planejador G;
  // O caminho a ser calculado:
  // Os pontos do caminho
  Caminho C;
  // O numero de nohs gerados no calculo do caminho
  int NA(-1),NF(-1);
  // O comprimento do caminho calculado
  double compr(-1.0);

  if (!G.ler("pontos.txt", "rotas.txt"))
  {
    cerr << "Erro na leitura dos arquivos do mapa\n";
    return -1;
  }

  // Variaveis auxiliares
  IDPonto id_origem, id_destino;
  Rota R;
  Ponto P;

  int opcao;
  do
  {
    cout << endl;
    cout << "1 - Imprimir pontos\n";
    cout << "2 - Imprimir rotas\n";
    cout << "3 - Calcular caminho\n";
    cout << "4 - Imprimir caminho\n";
    cout << "0 - Sair\n";
    do
    {
      cout << "OPCAO: ";
      cin >> opcao;
    }
    while (opcao<0 || opcao>4);
    switch(opcao)
    {
    case 1:
      cout << "PONTOS:\n";
      G.imprimirPontos();
      break;
    case 2:
      cout << "ROTAS\n";
      G.imprimirRotas();
      break;
    case 3:
      cout << "ID do ponto de origem: ";
      cin >> id_origem;
      cout << "ID do ponto de destino: ";
      cin >> id_destino;
      compr = G.calculaCaminho(id_origem,id_destino,C,NA,NF);
      if (NA<0 || NF<0)
      {
        cout << "Erro no calculo do caminho\n";
      }
      else if (compr<0.0)
      {
        cout << "Algoritmo concluido. Nenhum caminho foi encontrado\n";
      }
      else
      {
        cout << "Algoritmo concluido. Caminho mais curto foi encontrado\n";
      }
      break;
    case 4:
      for (auto par : C)
      {
        if (par.first == "")
        {
          cout << "\nDe ";
        }
        else
        {
          R = G.getRota(par.first);
          cout << "Por " << R.nome << " ateh ";
        }
        P = G.getPonto(par.second);
        cout << P.nome;
        if (par.first != "") cout << " (" << R.comprimento << "km)";
        cout << endl;
      }
      cout << "TOTAL: " << compr << "km\n";
      cout << "Nohs em aberto: " << NA << " fechados: " << NF << endl;
      break;
    case 0:
    default:
      break;
    }
  }
  while (opcao!=0);

  return 0;
}
