#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <chrono>
#include <CL/sycl.hpp>
#include "cxxopts.hpp"
using namespace std;
class MyKernel;

int main(int argc, char **argv){
    //numero de casos para serem resolvidos
    int casos = 10;
    int32_t tamanho = 10000000;
    //Para calculo da media dos tempos de execuçoes
    long long soma = 0;
    for(int c = 0; c < casos; c++ ){
        
    //Declaracao de variaveis array<int32_t, tamanho>
    array<int32_t, 10000000> V;
    array<int32_t, 10000000> Auxiliar;
    
    //inserindo elementos iniciais
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<mt19937::result_type> dist6(1,tamanho);
    //cout << "Dados :" << endl;
    for(int32_t i = 0; i < tamanho; i++){
        V[i] = dist6(rng);
        //cout << V[i] << endl ;
    }
    //cout << "Fim dos dados" << endl;
    
    
    
    
    cl::sycl::device device = cl::sycl::gpu_selector{}.select_device();
    cl::sycl::queue queue(device, [] (cl::sycl::exception_list el) {
       for (auto ex : el) { rethrow_exception(ex); }
    } );
    
    //Loop de Reducao
    auto inicio1 = chrono::high_resolution_clock::now();
    {
        cl::sycl::buffer<int32_t,1> buf(V.data(), cl::sycl::range<1>(V.size()));
        cl::sycl::buffer<int32_t,1> aux(Auxiliar.data(), cl::sycl::range<1>(Auxiliar.size()));

        int32_t m = 1;
        for(int i = 0; i < ceil(log2(tamanho));i++){
            m *= 2;
            queue.submit([&] (cl::sycl::handler& cgh){
                auto global_mem = buf.get_access<cl::sycl::access::mode::read_write>(cgh);
                //Em vez de juntar um par de vetor por vez, a paralização permite a união de todos de uma vez
                auto VAux = aux.get_access<cl::sycl::access::mode::read_write>(cgh);
                cgh.parallel_for<class MyKernel>(cl::sycl::range<1>(tamanho),[=](cl::sycl::item<1> item){

                    int32_t id = item.get_id();
                    //Funcao merge
                    if(id < (tamanho + m - 1)/m ){
                    
                        int32_t inicio = id*m,
                               meio = m*id +m/2,
                               fim = ((inicio+m) > tamanho) ? tamanho : inicio + m;

                        int32_t a = inicio,
                               b = meio;

                        for(int32_t j = inicio; j < fim; j++){
                            if((a < meio) && (!(b < fim) || (global_mem[a] < global_mem[b]))){


                                VAux[j] = global_mem[a];

                                a++;

                            } else{


                                VAux[j] = global_mem[b];

                                b++;

                            }
                        }
                        for(int32_t k = inicio; k < fim; k++){
                            global_mem[k] = VAux[k];
                        }

                    }
                    //Fim da //Funcao merge
                });

            });
            queue.wait_and_throw();

        }
    }
    auto resultado1 = chrono::high_resolution_clock::now() - inicio1;
    long long t = chrono::duration_cast<chrono::microseconds>(resultado1).count();
    
    bool ordenado = true;
    cout << "Resultados: " << tamanho << endl;
    
    //verificacao
    for(int32_t contador = 1; contador < tamanho; contador++){
        if(V[contador] < V[contador - 1]){
            ordenado = false;
        }
    }
    
    cout << "Vetor ordenado?: " << ordenado;
    cout << " Tempo de Execucao: " << t << endl;
    soma += t;
    }
    cout << "Casos: " << casos << " tamanho = " << tamanho << " Media =" << (double) soma/casos << endl;
}