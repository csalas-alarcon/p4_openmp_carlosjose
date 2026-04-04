// Task1/src/main.cc

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "png.h"
#include <vector>
#include <assert.h>
#include <iostream>
#include <memory>
#include "utils/image.h"
#include "utils/dct.h"
#include <string>
#include <chrono>
#include <omp.h>

// 3x3 Statistical Region Merging Kernel
Image<float> get_srm_3x3() {
    Image<float> kernel(3, 3, 1);
    kernel.set(0, 0, 0, -1); kernel.set(0, 1, 0, 2); kernel.set(0, 2, 0, -1);
    kernel.set(1, 0, 0, 2); kernel.set(1, 1, 0, -4); kernel.set(1, 2, 0, 2);
    kernel.set(2, 0, 0, -1); kernel.set(2, 1, 0, 2); kernel.set(2, 2, 0, -1);
    return kernel;
}
// 5x5 Statistical Region Merging Kernel
// Why not const? X2
Image<float> get_srm_5x5() {
    Image<float> kernel(5, 5, 1);
    kernel.set(0, 0, 0, -1); kernel.set(0, 1, 0, 2); kernel.set(0, 2, 0, -2); kernel.set(0, 3, 0, 2); kernel.set(0, 4, 0, -1);
    kernel.set(1, 0, 0, 2); kernel.set(1, 1, 0, -6); kernel.set(1, 2, 0, 8); kernel.set(1, 3, 0, -6); kernel.set(1, 4, 0, 2);
    kernel.set(2, 0, 0, -2); kernel.set(2, 1, 0, 8); kernel.set(2, 2, 0, -12); kernel.set(2, 3, 0, 8); kernel.set(2, 4, 0, -2);
    kernel.set(3, 0, 0, 2); kernel.set(3, 1, 0, -6); kernel.set(3, 2, 0, 8); kernel.set(3, 3, 0, -6); kernel.set(3, 4, 0, 2);
    kernel.set(4, 0, 0, -1); kernel.set(4, 1, 0, 2); kernel.set(4, 2, 0, -2); kernel.set(4, 3, 0, 2); kernel.set(4, 4, 0, -1);
    return kernel;
}

// Useless if const kernels
// ¿? We paralelize this ¿?
// Auxiliar Function for SRM
Image<float> get_srm_kernel(int size) {
    assert(size == 3 || size == 5);
    switch(size){
        case 3:
            return get_srm_3x3();
        case 5:
            return get_srm_5x5();
    }
    return get_srm_3x3();
}

Image<unsigned char> compute_srm(const Image<unsigned char> &image, int kernel_size) {
    auto begin = std::chrono::steady_clock::now();
    std::cout<<"Computing SRM "<<kernel_size<<"x"<<kernel_size<<"..."<<std::endl;          
    Image<float> srm = image.to_grayscale().convert<float>();
    srm = srm.convolution(get_srm_kernel(kernel_size));
    srm = srm.abs().normalized();
    srm = srm * 255;
    Image<unsigned char> result = srm.convert<unsigned char>();
    
    auto end = std::chrono::steady_clock::now();
    std::cout<<"SRM elapsed time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()<<"ms"<<std::endl;
    return result;
}

Image<unsigned char> compute_dct(const Image<unsigned char> &image, int block_size, bool invert) {
    auto begin = std::chrono::steady_clock::now();
    std::cout<<"Computing"; 
    if (invert) std::cout<<" inverse";
    else std::cout<<" direct";
    std::cout<<" DCT "<<block_size<<"x"<<block_size<<"..."<<std::endl;
    Image<float> grayscale = image.convert<float>().to_grayscale();
    std::vector<Block<float>> blocks = grayscale.get_blocks(block_size);

    #pragma omp parallel for schedule(guided)
    for(int i=0;i<blocks.size();i++){
        float **dctBlock = dct::create_matrix(block_size, block_size);
        dct::direct(dctBlock, blocks[i], 0);
        if (invert) {
          for(int k=0;k<blocks[i].size/2;k++)
            for(int l=0;l<blocks[i].size/2;l++)
              dctBlock[k][l] = 0.0;
          dct::inverse(blocks[i], dctBlock, 0, 0.0, 255.);
        }else dct::assign(dctBlock, blocks[i], 0);
        dct::delete_matrix(dctBlock);
    }
    Image<unsigned char> result = grayscale.convert<unsigned char>();
    auto end = std::chrono::steady_clock::now();
    std::cout<<"DCT elapsed time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()<<"ms"<<std::endl;
    return result;
}

Image<unsigned char> compute_ela(const Image<unsigned char> &image, int quality){
    std::cout<<"Computing ELA..."<<std::endl;
    auto begin = std::chrono::steady_clock::now();
    Image<unsigned char> grayscale = image.to_grayscale();
    save_to_file("_temp.jpg", grayscale, quality);
    Image<float> compressed = load_from_file("_temp.jpg").convert<float>();
    compressed = compressed + (grayscale.convert<float>()*(-1));
    compressed = compressed.abs().normalized() * 255;
    Image<unsigned char> result = compressed.convert<unsigned char>();
    auto end = std::chrono::steady_clock::now();
    std::cout<<"ELA elapsed time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()<<"ms"<<std::endl;
    return result;
}

int main(int argc, char **argv) {
    if(argc == 1) {
        std::cerr<<"Image filename missing from arguments. Usage ./dct <filename>"<<std::endl;
        exit(1);
    }
    
    // Activamos el paralelismo anidado.
    // Como el main va a crear una región paralela, si no activamos esto, los pragmas 
    // dentro de image.h no harían nada y se ejecutarían con un solo hilo secuencial.
    omp_set_max_active_levels(2);

    int block_size=8;
    Image<unsigned char> image = load_from_file(argv[1]);

    Image<unsigned char> srm3x3, srm5x5, ela, dct_inv, dct_dir;

    // Paralelismo de Tareas Puro de OpenMP
    // 1. Abrimos el equipo de hilos general
    #pragma omp parallel
    {
        // 2. Obligamos a que un solo hilo (single) sea el encargado de generar el grafo de tareas
        #pragma omp single
        {
            // 3. Empaquetamos cada llamada en una tarea independiente
            #pragma omp task shared(srm3x3, image)
            srm3x3 = compute_srm(image, 3);

            #pragma omp task shared(srm5x5, image)
            srm5x5 = compute_srm(image, 5);

            #pragma omp task shared(ela, image)
            ela = compute_ela(image, 90);

            #pragma omp task shared(dct_inv, image)
            dct_inv = compute_dct(image, block_size, true);

            #pragma omp task shared(dct_dir, image)
            dct_dir = compute_dct(image, block_size, false);

            // 4. Barrera de sincronización: Esperamos a que todas las tareas terminen
            #pragma omp taskwait
        }
    }

    save_to_file("srm_kernel_3x3.png", srm3x3);
    save_to_file("srm_kernel_5x5.png", srm5x5);
    save_to_file("ela.png", ela);
    save_to_file("dct_invert.png", dct_inv);
    save_to_file("dct_direct.png", dct_dir);

    return 0;
}
