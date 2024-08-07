#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "planejador.h"

using namespace std;


/// Distancia entre 2 pontos (formula de haversine)
double haversine(const Ponto& P1, const Ponto& P2)
{
  static const double MY_PI = 3.14159265358979323846;
  static const double R_EARTH = 6371.0;
  // Conversao para radianos
  double lat1 = MY_PI*P1.latitude/180.0;
  double lat2 = MY_PI*P2.latitude/180.0;
  double lon1 = MY_PI*P1.longitude/180.0;
  double lon2 = MY_PI*P2.longitude/180.0;

  return R_EARTH*acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon1-lon2));
}

/// Torna o mapa vazio
void Planejador::clear()
{
  pontos.clear();
  rotas.clear();
}

/// Retorna um Ponto do mapa, passando a id como parametro.
/// Se a id for inexistente, retorna um Ponto vazio.
Ponto Planejador::getPonto(const IDPonto& Id) const
{
    auto find_ponto = find(pontos.begin(),pontos.end(), Id);
    if(find_ponto != pontos.end()){ return *find_ponto;}

    return Ponto();
}

/// Retorna um Rota do mapa, passando a id como parametro.
/// Se a id for inexistente, retorna um Rota vazio.
Rota Planejador::getRota(const IDRota& Id) const
{
  auto find_rota = find(rotas.begin(),rotas.end(), Id);
  if(find_rota != rotas.end()){ return *find_rota;}

  return Rota();
}

/// Imprime os pontos do mapa no console
void Planejador::imprimirPontos() const
{
  for (const auto& P : pontos)
  {
    cout << P.id << '\t' << P.nome
         << " (" <<P.latitude << ',' << P.longitude << ")\n";
  }
}

/// Imprime as rotas do mapa no console
void Planejador::imprimirRotas() const
{
  for (const auto& R : rotas)
  {
    cout << R.id << '\t' << R.nome << '\t' << R.comprimento << "km"
         << " [" << R.extremidade[0] << ',' << R.extremidade[1] << "]\n";
  }
}

/// Leh um mapa dos arquivos arq_pontos e arq_rotas.
/// Caso nao consiga ler dos arquivos, deixa o mapa inalterado e retorna false.
/// Retorna true em caso de leitura bem sucedida
bool Planejador::ler(const std::string& arq_pontos,
                     const std::string& arq_rotas)
{
  // Listas temporarias para armazenamento dos dados lidos
  list<Ponto> listP;
  list<Rota> listR;
  // Variaveis auxiliares para buscas nas listas
  list<Ponto>::iterator itr_ponto;
  list<Rota>::iterator itr_rota;
  // Variaveis auxiliares para leitura de dados
  Ponto P;
  Rota R;
  string prov;

  // Leh os pontos do arquivo
  try
  {
    // Abre o arquivo de pontos
    ifstream arq(arq_pontos);
    if (!arq.is_open()) throw 1;

    // Leh o cabecalho
    getline(arq,prov);
    if (arq.fail() || prov != "ID;Nome;Latitude;Longitude") throw 2;

    // Leh os pontos
    do
    {
      // Leh a ID
      getline(arq,prov,';');
      if (arq.fail() || prov.size()<2 || prov[0]!='#') throw 3;
      P.id = move(prov);

      // Leh o nome
      getline(arq,prov,';');
      if (arq.fail() || prov.size()<2) throw 4;
      P.nome = move(prov);

      // Leh a latitude
      arq >> P.latitude;
      if (arq.fail()) throw 5;
      arq.ignore(1,';');

      // Leh a longitude
      arq >> P.longitude;
      if (arq.fail()) throw 6;
      arq >> ws;

      // Verifica se existe ponto com a mesma ID
      itr_ponto = find(listP.begin(),listP.end(), P.id);
      if(itr_ponto != listP.end()){ throw 7; }


      // Inclui o ponto na lista de pontos
      listP.push_back(move(P));
    }
    while (!arq.eof());

    // Fecha o arquivo de pontos
    arq.close();
  }
  catch (int i)
  {
    cerr << "Erro " << i << " na leitura do arquivo de pontos " << arq_pontos << endl;
    return false;
  }

  // Leh as rotas do arquivo
  try
  {
    // Abre o arquivo de rotas
    ifstream arq(arq_rotas);
    if (!arq.is_open()) throw 1;

    // Leh o cabecalho
    getline(arq,prov);
    if (arq.fail() || prov != "ID;Nome;Extremidade 1;Extremidade 2;Comprimento") throw 2;

    // Leh as rotas
    do
    {
      // Leh a ID
      getline(arq,prov,';');
      if (arq.fail() || prov.size()<2 || prov[0]!='&') throw 3;
      R.id = move(prov);

      // Leh o nome
      getline(arq,prov,';');
      if (arq.fail() || prov.size()<2) throw 4;
      R.nome = move(prov);

      // Leh a id da extremidade 1
      getline(arq,prov,';');
      if (arq.fail() || prov.size()<2 || prov[0]!='#') throw 5;

      // Verifica se a Id corresponde a um ponto existente
      itr_ponto = find(listP.begin(),listP.end(), prov);
      if(itr_ponto == listP.end()){ throw 6; }

      // Armazena a id da extremidade 1
      R.extremidade[0] = move(prov);

      // Leh a id da extremidade 2
      getline(arq,prov,';');
      if (arq.fail() || prov.size()<2 || prov[0]!='#') throw 7;

      // Verifica se a Id corresponde a um ponto existente
      itr_ponto = find(listP.begin(),listP.end(), prov);
      if(itr_ponto == listP.end()){ throw 8; }

      // Armazena a id da extremidade 2
      R.extremidade[1] = move(prov);

      // Leh o comprimento
      arq >> R.comprimento;
      if (arq.fail()) throw 9;
      arq >> ws;

      // Verifica se existe rota com a mesma ID
      itr_rota = find(listR.begin(),listR.end(), prov);
      if(itr_rota != listR.end()){ throw 10; }

      // Inclui a rota na lista de rotas
      listR.push_back(move(R));
    }
    while (!arq.eof());

    // Fecha o arquivo de rotas
    arq.close();
  }
  catch (int i)
  {
    cerr << "Erro " << i << " na leitura do arquivo de rotas " << arq_rotas << endl;
    return false;
  }

  // Soh chega aqui se nao entrou no catch, jah que ele termina com return.
  // Move as listas de pontos e rotas para o planejador.
  pontos = move(listP);
  rotas = move(listR);

  return true;
}

/// *******************************************************************************
/// Calcula o caminho entre a origem e o destino do planejador usando o algoritmo A*
/// *******************************************************************************

/// Calcula o caminho entre a origem e o destino do planejador usando o algoritmo A*
/// Retorna o comprimento do caminho encontrado.
/// (<0 se  parametros invalidos ou nao existe caminho).
/// O parametro C retorna o caminho encontrado
/// (vazio se  parametros invalidos ou nao existe caminho).
/// O parametro NA retorna o numero de nos em aberto ao termino do algoritmo A*
/// (<0 se parametros invalidos, retorna >0 mesmo quando nao existe caminho).
/// O parametro NF retorna o numero de nos em fechado ao termino do algoritmo A*
/// (<0 se parametros invalidos, retorna >0 mesmo quando nao existe caminho).
double Planejador::calculaCaminho(const IDPonto& Id_origem,
                                  const IDPonto& Id_destino,
                                  Caminho& C, int& NA, int& NF)
{
  // Zera o caminho resultado
  C.clear();

  try
  {
    // Mapa vazio
    if (empty()) throw 1;

    // Testa se id_origem eh valida
    if (Id_origem.size()<2 || Id_origem[0]!='#') throw 2;

    // Testa se id_destino eh valida
    if (Id_destino.size()<2 || Id_destino[0]!='#') throw 3;

    // Testa se id_origem eh uma Id de um ponto existente
    if(this->getPonto(Id_origem)== "") throw 4;

    // Testa se id_destino eh uma Id de um ponto existente
    if(this->getPonto(Id_destino)== "") throw 5;


    /* *****************************  /
    /  IMPLEMENTACAO DO ALGORITMO A*  /
    /  ***************************** */
    auto pt_orig = find(pontos.begin(),pontos.end(),Id_origem);
    auto pt_dest = find(pontos.begin(),pontos.end(),Id_destino);
        // atual ← Noh inicial
        Noh atual;
        atual.id_ponto = Id_origem;
        atual.id_rota = "";
        atual.g = 0.0;
        atual.h = haversine(*pt_orig,*pt_dest);

        // Inicializa o conjunto Aberto
        list<Noh> aberto;
        list<Noh> fechado;

        // apenas com Noh inicial
        aberto.push_front(atual);
        // Repita enquanto houver Noh’s em Aberto e não encontrou solução
        do{
            // Lê e exclui o 1º Noh (o de menor custo) de Aberto
            atual = aberto.front();
            aberto.pop_front();

            // Acrescenta "atual" em Fechado
            fechado.push_front(atual);
            // Expande se não é solução
            if(atual != Id_destino){
                for(auto &rota: this->rotas){
                    if(rota.extremidade[0] == atual.id_ponto || rota.extremidade[1] == atual.id_ponto){
                        // Gera novo sucessor "suc"
                        Noh suc;
                        // Id do ponto
                        suc.id_ponto = (rota.extremidade[0] != atual.id_ponto)? rota.extremidade[0]:rota.extremidade[1];
                        // Id da rota
                        suc.id_rota = rota.id;
                        // Custo passado
                        suc.g = atual.g + rota.comprimento;
                        // Procura o ponto de "suc"
                        auto pt_suc = find(pontos.begin(),pontos.end(),suc.id_ponto);
                        // Custo futuro
                        suc.h = haversine(*pt_suc,*pt_dest);
                        // No início, assume que não existe Noh igual a "suc"
                        bool eh_inedito = true;
                        // Procura em Aberto Noh igual a "suc"
                        auto old = find(aberto.begin(),aberto.end(),suc.id_ponto);
                        if(old != aberto.end()){
                            // Testa qual tem menor custo total f=g+h
                            if(suc.f() < (*old).f()){
                                // Exclui anterior
                                aberto.erase(old);
                            }else{
                                // Noh já existe
                                eh_inedito = false;
                            }
                        }else{
                            // Procura em Fechado Noh igual a "suc"
                            auto old = find(fechado.begin(),fechado.end(),suc.id_ponto);
                            if(old != fechado.end()){
                                // Testa qual tem menor custo total f=g+h
                                if(suc.f() < (*old).f()){
                                    // Exclui anterior
                                    fechado.erase(old);
                                }else{
                                    // Noh já existe
                                    eh_inedito = false;
                                }
                            }
                        }
                        // Insere suc em Aberto se não existe nem em Aberto nem em Fechado
                        if(eh_inedito){
                            // Acha big, o 1º Noh de Aberto com custo total f() maior que o custo  total f() de "suc"
                            auto big = find_if(aberto.begin(),aberto.end(),[&](Noh &noh){return noh.f()>suc.f();});
                            // Insere suc em Aberto antes de big
                            aberto.insert(big,suc);
                        }
                    }
                }
            }
        }while(atual!=Id_destino && !(aberto.empty()));

        // Disponibiliza estado final da busca, quer encontre ou não o caminho
        NF = fechado.size();
        NA = aberto.size();
        double compr;
        //Caminho encontrado inicialmente vazio
        // Pode ter terminado porque encontrou a solução ou porque não há mais Noh’s a testar
        if (atual != Id_destino){
            // Não existe caminho:"caminho" permanece vazio e comprimento inválido
            compr = -1.0;
        }else{
            // Calcula comprimento e caminho
            compr = atual.g;
            while(atual.id_rota != ""){
                // Acrescenta ao caminho
                C.push_front({atual.id_rota,atual.id_ponto});
                // Procura a rota que levou até "atual"
                auto rota = *(find(rotas.begin(),rotas.end(),atual.id_rota));
                // Calcula id do Noh antecessor
                auto id_ant = (rota.extremidade[0] != atual.id_ponto)? rota.extremidade[0]:rota.extremidade[1];
                // Procura em Fechado Noh com id_ponto igual a id_ant
                atual = *(find(fechado.begin(),fechado.end(),id_ant));
            }
        // Insere origem em "caminho"
        C.push_front({"",atual.id_ponto});
        }
    // O try tem que terminar retornando o comprimento calculado
    return compr;
  }
  catch(int i)
  {
    cerr << "Erro " << i << " no calculo do caminho\n";
  }

  // Soh chega aqui se executou o catch, jah que o try termina sempre com return.
  // Caminho C permanece vazio.
  NA = NF = -1;
  return -1.0;
}
