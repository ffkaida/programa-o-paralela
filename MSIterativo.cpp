#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h> 
#include <math.h> 
#include <chrono>
using namespace std;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

//--------------------------------------------------------------------------------------
template<class T>
void validar(T v, int32_t n){
	for(int32_t i = 1; i < n; i++){
		if(v[i-1] > v[i]){
			cout << "Vetor nao ordenado" << endl;
			return;
		}
	}
	cout << "Vetor ordenado" << endl;
}

void merge(int32_t vetor[], int32_t ini, int32_t meio, int32_t fim) {
    int32_t a = ini,
		b = meio;
	vector<int32_t> vetAux(fim-ini);
		
    for(int32_t i = ini; i < fim; i++){
        if((a < meio) && (!(b < fim) || (vetor[a] < vetor[b]))){
            vetAux.push_back(vetor[a]);
            a++;
        }
        else{
            vetAux.push_back(vetor[b]);
            b++;
        }
    }
    
    for(int32_t i = 0; i < (fim - ini); i++){
        vetor[i + ini] = vetAux[i];
    }
}

void MSI(int32_t vetor[], int32_t n){
	int32_t a = 1;
	for(int32_t i = 0; i < ceil(log2(n));i++){
		int meio = a;
		a*=2;
		int32_t j;
		for(j = 0; j < (n+a-1)/a; j++){
			int32_t inicio = j*a;
			int32_t fim = ((inicio+a) > n) ? n : inicio + a;
			merge(vetor,inicio,meio,fim);
			meio += a;
		}		
	}
}


int main(int argc, char** argv) {

    
	

        int32_t V[1000000];
        int32_t A[1000000];
        int32_t tamanho = 1000000,
            casos = 10;
        long long soma = 0;
        
        
        srand (time(NULL));
        for(int32_t a = 1; a <= casos; a++){

            for(int32_t i = 0; i < tamanho; i++){
                V[i] = rand() % tamanho;
            }

            auto inicio = chrono::high_resolution_clock::now();
            MSI(V,tamanho);
            auto resultado = chrono::high_resolution_clock::now() - inicio;
            long long tempo = chrono::duration_cast<chrono::microseconds>(resultado).count();
            cout << "Caso: " << a << " Tempo: " << tempo << endl;
            soma+= tempo;
        }
        cout << "Para tamanho = " << tamanho << " Tempo medio = " << (long double) soma/casos << endl;
    
	return 0;
}