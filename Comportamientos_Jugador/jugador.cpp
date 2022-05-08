#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action sigAccion = actIDLE;

	//actualizo la variable actual
	actual.fila = sensores.posF;
	actual.columna = sensores.posC;
	actual.orientacion = sensores.sentido;

	cout << "Fila: " << actual.fila << endl;
	cout << "Col : " << actual.columna << endl;
	cout << "Ori : " << actual.orientacion << endl;

	// Capturo los destinos
	cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
	objetivos.clear();
	for (int i = 0; i < sensores.num_destinos; i++)
	{
		estado aux;
		aux.fila = sensores.destino[2 * i];
		aux.columna = sensores.destino[2 * i + 1];
		objetivos.push_back(aux);
	}


	//Si no hay plan construyo uno
	if(!hay_plan){
		hay_plan = pathFinding(sensores.nivel, actual, objetivos, plan);
	}

	if(hay_plan and plan.size()>0){
		sigAccion = plan.front();
		plan.erase(plan.begin());
	}
	else{
		cout << "No se pudo encontrar un plan.\n";
	}

	return sigAccion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan)
{
	switch (level)
	{
	case 0:
		cout << "Demo\n";
		estado un_objetivo;
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;

	case 1:
		cout << "Optimo numero de acciones\n";
		// Incluir aqui la llamada al busqueda en anchura
		cout << "Nivel 1\n";
		estado objetivo_nivel_1;
		objetivo_nivel_1 = objetivos.front();
		cout << "fila: " << objetivo_nivel_1.fila << " col:" << objetivo_nivel_1.columna << endl;
		return pathFinding_Anchura(origen, objetivo_nivel_1, plan);
		break;
	case 2:
		cout << "Optimo en coste\n";
		// Incluir aqui la llamada al busqueda de costo uniforme/A*
		cout << "Nivel 2\n";
		estado objetivo_nivel_2;
		objetivo_nivel_2 = objetivos.front();
		cout << "fila: " << objetivo_nivel_2.fila << " col:" << objetivo_nivel_2.columna << endl;
		return pathFinding_Aestrella(origen, objetivo_nivel_2, plan);
		break;
	case 3:
		cout << "Reto 1: Descubrir el mapa\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 1
		cout << "No implementado aun\n";
		break;
	case 4:
		cout << "Reto 2: Maximizar objetivos\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 2
		cout << "No implementado aun\n";
		break;
	}
	return false;
}

//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		fil--;
		col++;
		break;
	case 2:
		col++;
		break;
	case 3:
		fil++;
		col++;
		break;
	case 4:
		fil++;
		break;
	case 5:
		fil++;
		col--;
		break;
	case 6:
		col--;
		break;
	case 7:
		fil--;
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}

struct nodo
{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

struct ComparaNodos{
	bool operator()(const nodoA &a, const nodoA &n) const{
		if ((a.st.fila > n.st.fila) or (a.st.fila == n.st.fila and a.st.columna > n.st.columna) or
			(a.st.fila == n.st.fila and a.st.columna == n.st.columna and a.st.orientacion > n.st.orientacion) or
			(a.st.fila == n.st.fila and a.st.columna == n.st.columna and a.st.orientacion == n.st.orientacion and a.tiene_bikini != n.tiene_bikini) or 
			(a.st.fila == n.st.fila and a.st.columna == n.st.columna and a.st.orientacion == n.st.orientacion and a.tiene_bikini == n.tiene_bikini and a.tiene_zapatillas != n.tiene_zapatillas))
			return true;
		else
			return false;
	}
};

//ComparaNodos
//meter nodos en la lista de cerrados en vez de estados
//////////
////////////////////////////////////////////////////////////////////////////////////////

// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.clear();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{
		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

// Implementación de la busqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	queue<nodo> Abiertos;				  // Cola de Abiertos

	//nodo almacena ( estado st, list<Action> secuencia )
	// estado st almacena (int fila, int col, int orientacion)
	nodo current;
	current.st = origen;
	current.secuencia.clear();

	//En un primer momento metemos current que almacena la posición del nodo inicial.
	Abiertos.push(current);

	//Seguimos buscando solución mientras queden nodos en la lista de abiertos y mientras la fila y columna sean distintos
	//de las del nodo objetivo
	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{
		//Para no repetir nodos en abiertos que ya estén en cerrados, se busca el nodo que esté al frente de abuertos en
		//la lista de cerrados, si el find devuelve algo que no es end quiere decir que se encuentra en la lista de cerrados
		//por lo que lo saca.
		while(Cerrados.find(Abiertos.front().st) != Cerrados.end()){
			Abiertos.pop();
		}

		//Cuando encuentra un nodo que no estuviera previamente en cerrados lo introduce en la lista de cerrados.
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		//Copia al padre en el hijo y le aplica la transformación
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;

		//Busca al hijo en cerrados para no meterlo en caso de que esté
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			//Si no está introduce en el nodo hijo la acción ya que forma parte de su secuencia
			hijoTurnR.secuencia.push_back(actTURN_R);

			//Introduce el nodo en la lista de abiertos
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			//Actualiza current al front de la lista de abiertos.
			current = Abiertos.front();
		}
	}

	cout << "Terminada la busqueda\n";

	//Cuando se sale del while puede ser por dos motivos, o bien ha encontrado el nodo objetivo, en cuyo caso
	//coinciden la fila y columnas con las de destino, por lo que llama a PintaPlan, o bien se ha salido porque se 
	//ha termiando la lista de abiertos y no ha encontrado nada, en cuyo caso devuelve eso.
	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

bool ComportamientoJugador::pathFinding_Aestrella(const estado &origen, const estado &destino, list<Action> &plan)
{
	//Para este algoritmo he creado un nuevo tipo de nodo llamado nodoA que además de la información que almacenaba
	//el nodo normal almacena también los valores de A* f, g y h, además de si tiene o no tiene zapatillas en un determinado
	//momento del plan.

	cout << "Calculando plan\n";
	plan.clear();

	//En este caso voy a tener una lista de nodos cerrados y una priority_queue de nodos abiertos ordenados en función del menor
	//valor de f para que siempre tengamos el nodo óptimo.
	set<nodoA, ComparaNodos> Cerrados; // Lista de nodosA en vez de estados y un nuevo struct comparaNodos
	priority_queue<nodoA> Abiertos;// Cola de Abiertos

	nodoA current;
	current.st = origen;
	
	//inicializo a 0 el costo del nodo origen ya que es lo que cuesta llegar desde el origen a la misma casilla e inicializo el resto de
	//variables.
	current.g = 0;
	current.h = FuncionHeuristica(origen, destino);
	current.f = current.g + current.h;
	
	//cambiar para el nivel 3 y 4
	current.tiene_bikini = false;
	current.tiene_zapatillas = false;

	current.secuencia.clear();

	Abiertos.push(current);

	while(!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna)){
		Abiertos.pop();

		//Mirar si la casilla current me da zapatillas o bikini para cambiar el estado.
		Cerrados.insert(current);
		
		// Generar descendiente de girar a la derecha 90 grados
		nodoA hijoTurnR = current;
		Action ac = actTURN_R;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;

		//Comprobar que el nuevo nodo no esté ya en abiertos o en cerrados
		// La función yaEnAbiertos modifica la cola de Abiertos y en caso de que el nodo ya se encuentre
		// comprueba el nodo que tenga menor valor de f.
		bool estaEnAbiertos = yaEnAbiertos(Abiertos, hijoTurnR, ac, destino);
		if (!estaEnAbiertos and Cerrados.find(hijoTurnR) == Cerrados.end())
		{
			ActualizarValorHeuristico(hijoTurnR,ac,destino);
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}
		
		//Comprobar si el nodo se encuentra ya en cerrados buscar si es mejor.
		
		// Generar descendiente de girar a la derecha 45 grados
		nodoA hijoSEMITurnR = current;
		ac = actSEMITURN_R;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;

		estaEnAbiertos = yaEnAbiertos(Abiertos, hijoSEMITurnR, ac, destino);
		if (!estaEnAbiertos and Cerrados.find(hijoSEMITurnR) == Cerrados.end())
		{
			ActualizarValorHeuristico(hijoSEMITurnR,ac,destino);
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}
		//Comprobar si el nodo se encuentra ya en cerrados buscar si es mejor.

		// Generar descendiente de girar a la izquierda 90 grados
		nodoA hijoTurnL = current;
		ac = actTURN_L;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;

		estaEnAbiertos = yaEnAbiertos(Abiertos, hijoTurnL, ac, destino);
		if (!estaEnAbiertos and Cerrados.find(hijoTurnL) == Cerrados.end())
		{
			ActualizarValorHeuristico(hijoTurnL,ac,destino);
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}
		//Comprobar si el nodo se encuentra ya en cerrados buscar si es mejor.

		// Generar descendiente de girar a la izquierda 45 grados
		nodoA hijoSEMITurnL = current;
		ac = actSEMITURN_L;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;

		estaEnAbiertos = yaEnAbiertos(Abiertos, hijoSEMITurnL, ac, destino);
		if (!estaEnAbiertos and Cerrados.find(hijoSEMITurnL) == Cerrados.end())
		{
			ActualizarValorHeuristico(hijoSEMITurnL,ac,destino);
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}
		//Comprobar si el nodo se encuentra ya en cerrados buscar si es mejor.


		// Generar descendiente de avanzar
		nodoA hijoForward = current;
		ac = actFORWARD;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			estaEnAbiertos = yaEnAbiertos(Abiertos, hijoForward, ac, destino);

			if (!estaEnAbiertos and Cerrados.find(hijoForward) == Cerrados.end())
			{
				ActualizarValorHeuristico(hijoForward,ac,destino);
				hijoForward.secuencia.push_back(actFORWARD);
				/////////////////////////////////////////////
				if(mapaResultado[hijoForward.st.fila][hijoForward.st.columna] == 'K'){
					hijoForward.tiene_bikini = true;
					hijoForward.tiene_zapatillas = false;
				}
				else if(mapaResultado[hijoForward.st.fila][hijoForward.st.columna] == 'D'){
					hijoForward.tiene_zapatillas = true;
					hijoForward.tiene_bikini = false;
				}
				//////////////////////////////////////////////
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
		return false;
	}
}

//Calcular el coste de trasladarse por una casilla
int ComportamientoJugador::CosteCasilla(estado &st, Action &ac, const bool &tiene_bikini, const bool &tiene_zapatillas){
	char casilla = mapaResultado[st.fila][st.columna];

	switch(ac){
		case actFORWARD:
			if(casilla == 'B'){
				if (!tiene_zapatillas)
					return 100;
				else
					return 15;
			}
			else if(casilla == 'A'){
				if(!tiene_bikini)
					return 200;
				else
					return 10;
			}
			else if(casilla == 'T')
				return 2;
			else
				return 1;
			break;
		case actTURN_L:
		case actTURN_R:
			if(casilla == 'B'){
				if (!tiene_zapatillas)
					return 3;
				else
					return 1;
			}
			else if(casilla == 'A'){
				if(!tiene_bikini)
					return 500;
				else
					return 5;
			}
			else if(casilla == 'T')
				return 2;
			else
				return 1;
			break;
		case actSEMITURN_L:
		case actSEMITURN_R:
			if(casilla == 'B'){
				if (!tiene_zapatillas)
					return 2;
				else
					return 1;
			}
			else if(casilla == 'A'){
				if(!tiene_bikini)
					return 300;
				else
					return 2;
			}
			else if(casilla == 'T')
				return 1;
			else
				return 1;
			break;
	}
}


//Prueba de heurística en que la distancia es, en caso de que una de las componentes sea igual (es decir que se encuentran en línea), devuelve
// la resta de la componente diferente en valor absoluto.
// En el caso de que ambas componentes sean diferentes calcula la suma de valores absolutos de las restas de las componentes dos a dos y divide el valor entre 2.
int ComportamientoJugador::FuncionHeuristica(const estado &actual, const estado &meta){
	if( actual.fila == meta.fila && actual.columna != meta.columna)
		return abs(meta.columna - actual.columna);
	else if( actual.columna == meta.columna && actual.fila != meta.fila)
		return abs(meta.fila - actual.fila);
	else
		return (abs(meta.fila - actual.fila) + abs(meta.columna - actual.columna)/2);
}

// Funcion auxiliar que actualiza el valor heurístico a partir de la g y la h de un nodo
void ComportamientoJugador::ActualizarValorHeuristico(nodoA &actualizar, Action &ac, const estado &destino){
	actualizar.g += ComportamientoJugador::CosteCasilla(actualizar.st, ac, actualizar.tiene_bikini, actualizar.tiene_zapatillas);
	actualizar.h = ComportamientoJugador::FuncionHeuristica(actualizar.st, destino);
	actualizar.f = actualizar.g + actualizar.h;
}

//Funcion que comprueba si un nodo descendiente se encuentra ya en la cola de Abiertos, en caso de que esté comprueba si es mejor
//que el existente y en caso de serlo lo introduce y elimina el otro.
bool ComportamientoJugador::yaEnAbiertos(priority_queue<nodoA> &Abiertos,nodoA &descendiente, Action &ac, const estado &destino){
	priority_queue<nodoA> Auxiliar;
	bool estaEnAbiertos = false;

	while(!Abiertos.empty()){
		nodoA comparar = Abiertos.top();
		Abiertos.pop();

		if( descendiente.st == comparar.st and descendiente.tiene_bikini==comparar.tiene_bikini and 
			descendiente.tiene_zapatillas == comparar.tiene_zapatillas){
			estaEnAbiertos = true;
			ActualizarValorHeuristico(descendiente,ac,destino);
			if(descendiente.f <= comparar.f){
				descendiente.secuencia.push_back(ac);
				Auxiliar.push(descendiente);
			}
			else{
				Auxiliar.push(comparar);
			}
		}
		else{
			Auxiliar.push(comparar);
		}
	}

	Abiertos = Auxiliar;

	return estaEnAbiertos;
}




// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actSEMITURN_R)
		{
			cout << "d ";
		}
		else if (*it == actTURN_L)
		{
			cout << "I ";
		}
		else if (*it == actSEMITURN_L)
		{
			cout << "I ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.fila--;
				cst.columna++;
				break;
			case 2:
				cst.columna++;
				break;
			case 3:
				cst.fila++;
				cst.columna++;
				break;
			case 4:
				cst.fila++;
				break;
			case 5:
				cst.fila++;
				cst.columna--;
				break;
			case 6:
				cst.columna--;
				break;
			case 7:
				cst.fila--;
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actTURN_R)
		{
			cst.orientacion = (cst.orientacion + 2) % 8;
		}
		else if (*it == actSEMITURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 8;
		}
		else if (*it == actTURN_L)
		{
			cst.orientacion = (cst.orientacion + 6) % 8;
		}
		else if (*it == actSEMITURN_L)
		{
			cst.orientacion = (cst.orientacion + 7) % 8;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
