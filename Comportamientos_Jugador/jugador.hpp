#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <queue>

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;

  friend bool operator==(const estado &a, const estado &b){
    return(a.fila == b.fila and a.columna == b.columna and a.orientacion == b.orientacion);
  }
};

struct nodoA{
	//¿Puntero al nodo padre más eficiente?
	estado st;
	int h;
	int g; //Coste de llegar desde el nodo origen hasta el nodo actual.
	int f;
	bool tiene_bikini;
    bool tiene_zapatillas;
	list<Action>secuencia;

	friend bool operator<(const nodoA &a, const nodoA &b){
		return (a.f > b.f);
	}
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hay_plan = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    estado actual;
    list<estado> objetivos;
    list<Action> plan;
    bool hay_plan;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Aestrella(const estado &origen, const estado &destino, list<Action> &plan);

  //Métodos h y g para el algoritmo A*
    int CosteCasilla(estado &st, Action &ac, const bool &tiene_bikini, const bool &tiene_zapatillas);
    int FuncionHeuristica(const estado &actual, const estado &meta);
    void ActualizarValorHeuristico(nodoA &actualizar, Action &ac, const estado &destino);
    bool yaEnAbiertos(priority_queue<nodoA> &Abiertos,nodoA &descendiente, Action &ac, const estado &destino);
  ///////////////////////////////////////////////////////////
    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);

};

#endif
