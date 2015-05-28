#include <fstream>
#include <sstream>
#include <string.h>
#include <iostream>
#include <vector>

using namespace std;

typedef struct{
	string nTerminal;
	vector <string> producao;
}TPRegras;

typedef struct{
	string nTerminal;
	vector <string> first;
	vector <string> follow;
}TPFF;

vector< vector < string > > entrada;
vector< TPRegras > gramatica;
vector< TPFF > conjuntoFF;

//faz a leitura da entrada
void leituraEntrada(){ 
	ifstream infile("gramatica.txt");
	string line;

	while (getline(infile, line)){
		istringstream iss(line);
		string s;
		vector<string> li;

		while (iss >> s){
			li.push_back(s);
		}

		entrada.push_back(li);
	}

	infile.close();
}

//processa a entrada separando as produções
void processaEntrada(){
	for(int i=0; i < entrada.size(); i++){
		
		for(int j=0; j < entrada[i].size(); j++){
			if(j > 1){
				TPRegras rule;
				rule.nTerminal = entrada[i][0];
				
				while(j < entrada[i].size() ){
					if(entrada[i][j] == "|" )break;
					rule.producao.push_back(entrada[i][j]);
					j++;
				}
				gramatica.push_back(rule);
			}
		}
	}
}

//imprime a entrada lida
void imprimeEntrada(){
	for(int i=0; i< entrada.size(); i++){
		cout << endl;
		for(int j=0; j<entrada[i].size(); j++){
			cout << entrada[i][j] << " ";
		}
	}
	cout <<"\n\n";
}

//imprima a entrada processada
void imprimeGramatica(){
	cout << "\n\t Gramatica estendida\n\n\t  ";
	for(int i=0; i< gramatica.size(); i++){
		cout << gramatica[i].nTerminal <<" -> ";
		int j=0;
		while(j < gramatica[i].producao.size()){
			cout << gramatica[i].producao[j];
			j++;
		}
		cout << "\n\t  ";
	}
	cout << "\n";
}

//imprime o conjunto FIRST e FOLLOW
void imprimeConjuntoFF(){
	for(int i=0; i<conjuntoFF.size();i++){
		cout <<"FIRST( "<< conjuntoFF[i].nTerminal << " )  == { ";
		for(int j=0;j<conjuntoFF[i].first.size();j++){
			cout << conjuntoFF[i].first[j];
			if(j != conjuntoFF[i].first.size()-1)cout << ", " ;
			else cout << " ";
		}
		cout << "}\n";
		cout <<"FOLLOW( "<< conjuntoFF[i].nTerminal << " ) == { ";
		for(int j=0;j<conjuntoFF[i].follow.size();j++){
			cout << conjuntoFF[i].follow[j];
			if(j != conjuntoFF[i].follow.size()-1)cout << ", " ;
			else cout << " ";
		}
		cout << "}\n\n";
	}
}

//verifica se não é letra Maiúscula que seriam terminais
int lowerCase(char carac){
	if(carac >= 'A' && carac <= 'Z')return 0;
	//cout <<"lower";
	return 1;
}

//verifica se uma string inteira não 
int onlyLower(string str){
	int sl =1;
	for(int i =0; i < str.size();i++){
		if(lowerCase(str[i]) == 0) sl =0;
	}
	return sl;
}

//procura se te existe um não terminal com o nome passado em nter
int nTerminalPeloNome(string nter){
	for(int i=0; i< gramatica.size();i++){
		if(gramatica[i].nTerminal == nter)return i;
	}
	return -1;
}

//inicializa o vetor que guardará FIRST e FOLLOW
void inicializaFF(vector<string> ntdf){
	for(int i =0; i < ntdf.size(); i++){
		TPFF  ax;
		ax.nTerminal = ntdf[i];
		conjuntoFF.push_back(ax);
	}
	return;
}

// verifica repetições no conjunto FIRST
int jaTemFR(int p, string ef){
	for(int i =0; i< conjuntoFF[p].first.size();i++){
		if(conjuntoFF[p].first[i] == ef) return 1;
	}
	return 0;
}

// verifica repetições no conjunto FOLLOW
int jaTemFL(int p, string ef){
	for(int i =0; i< conjuntoFF[p].follow.size();i++){
		if(conjuntoFF[p].follow[i] == ef) return 1;
	}
	return 0;
}

//insera um não terminal no conjunto FIRST
void guardaFirst(int p, string ef){
	for(int i =0; i < conjuntoFF.size(); i++){
		if(conjuntoFF[i].nTerminal == gramatica[p].nTerminal && jaTemFR(i,ef) == 0){
			conjuntoFF[i].first.push_back(ef);
		}
	}
}

//insera um não terminal no conjunto FOLLOW
void guardaFollow(int p, string ef){
	for(int i =0; i< i < conjuntoFF.size(); i++){
		if(conjuntoFF[i].nTerminal == gramatica[p].nTerminal && jaTemFL(i,ef) == 0){
			conjuntoFF[i].follow.push_back(ef);
		}
	}
}

//copia o first já existente para outro não terminal
int copiaFIRST(int i, string fr){
	for(int j =0; j<conjuntoFF.size();j++){
		if(conjuntoFF[j].nTerminal == fr){
			int tem =0;
			for(int x=0; x<conjuntoFF[j].first.size(); x++){
				guardaFirst(i,conjuntoFF[j].first[x]);
				if(conjuntoFF[j].first[x] == "$"){
					tem =1;
				}
			}
			return tem;
		}
	}
}

//copia o first do nao terminal sequinte para ser o follow do atual
int copiaFOLLOW(int i, string fr){
	for(int j =0; j<conjuntoFF.size();j++){
		if(conjuntoFF[j].nTerminal == fr){
			int tem =0;
			for(int x=0; x<conjuntoFF[j].first.size(); x++){
				guardaFollow(i,conjuntoFF[j].follow[x]);
				if(conjuntoFF[j].follow[x] == "$"){
					tem =1;
				}
			}
			return tem;
		}
	}
}

//encontra os componentes do conjunto FIRST
void encontraFirst(string nter){
	vector <int> jaFR;
	string prod = "";
	for(int i=gramatica.size()-1; i >=0 ; i--){ //produções com terminais de cara sozinhos ou seguidos de não terminais
		if(gramatica[i].nTerminal == nter){
			prod = "";
			int k=0;while(k<gramatica[i].producao.size()){
				prod += gramatica[i].producao[k];
				k++;
			}
			if(onlyLower(prod) == 1){ //somente terminal do lado direito
				guardaFirst(i,prod);
				jaFR.push_back(i);
			}else if(onlyLower(prod) == 0 && nTerminalPeloNome(prod) == -1){// misto de terminais e não terminais  abA  AaB
				if(onlyLower(gramatica[i].producao[0]) == 1){ // terminais somente
					guardaFirst(i,gramatica[i].producao[0]); 
					jaFR.push_back(i);
				}
			}
			
		}
	}
	
	for(int i=gramatica.size()-1; i >=0; i--){ // terminais indiretos EX:  S-> A   A-> a  se quer encontrar first de S
		int jafoi=0;
		for(int j=0; j<jaFR.size(); j++){
			if(jaFR[j] == i)jafoi=1;
		}
		prod = "";
		int k=0;while(k<gramatica[i].producao.size()){
				prod += gramatica[i].producao[k];
				k++;
			}
		if(jafoi == 0){
			if(nTerminalPeloNome(prod) == 1){ // somente um não terminal
				copiaFIRST(i,prod);
			}else{// C -> Ef
				int k=0;
				while(k<gramatica[i].producao.size()){
					if(onlyLower(gramatica[i].producao[k]) == 1){
						guardaFirst(i,gramatica[i].producao[k]);
						if(gramatica[i].producao[k] != "$")break;
					}else{
						int ret = copiaFIRST(i,gramatica[i].producao[k]);
						if(ret == 0)break;
					}
					k++;
				}
			}
		}
	}
	
}

//encontra os componentes do conjunto FOLLOW
void encontraFollow(string nter){
	
	for(int i=gramatica.size()-1; i >= 0 ; i--){
		
	}
}
//Cria uma lista dos não terminais com apenas uma cópia de cada e chama a função para encontrar o FIRST

void chamaFirstFollow(){
	vector< string > nTerminaisDiferentes;
	for(int i=0; i< gramatica.size(); i++){
		if(nTerminaisDiferentes.size() == 0){
			nTerminaisDiferentes.push_back(gramatica[i].nTerminal);
		}else{
			int jaTem =0;
			for(int j=0; j< nTerminaisDiferentes.size();j++){
				if(nTerminaisDiferentes[j] == gramatica[i].nTerminal){
					jaTem = 1;
					break;
				}
			}
			if(jaTem == 0){
				nTerminaisDiferentes.push_back(gramatica[i].nTerminal);
			}
		}
	}
	inicializaFF(nTerminaisDiferentes);
	
	conjuntoFF[0].follow.push_back("$");
	
	for(int j=nTerminaisDiferentes.size()-1; j>=0 ;j--){
		encontraFirst(nTerminaisDiferentes[j]);
		encontraFollow(nTerminaisDiferentes[j]);
	}
}

int main()
{
	leituraEntrada();
	imprimeEntrada();
	processaEntrada();
	imprimeGramatica();
	chamaFirstFollow();
	
	imprimeConjuntoFF();
	
	cout <<"\n\n\n";
	system("pause");
	return 0;
}
