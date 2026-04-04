#ifndef __IMAGE__H__
#define __IMAGE__H__
#include <vector>
#include <memory>
#include <iostream>
#include "assert.h"
#include <string>
#include <omp.h> // Añadido para asegurar la integración de OpenMP

template <typename T> class Block;

template <typename T> class Image{
public:
  int width, height, channels;
  std::shared_ptr<T[]> matrix;
  void release();
  Image();
  Image(int width, int height, int channels);
  Image(const Image<T> &a);
  ~Image();
  Image<T> operator=(const Image<T>& other);
  Image<T> operator*(const Image<T>& other) const;
  Image<T> operator*(float scalar) const;
  Image<T> operator+(const Image<T>& other) const;
  Image<T> operator+(float scalar) const;
  T get(int row, int col, int channel) const;
  void set(int row, int col, int channel, T value);
  template <typename S> Image<S> convert() const;
  Image<T> to_grayscale() const;
  Image<T> abs() const;
  Image<float> normalized() const;
  Image<T> convolution(const Image<float> &kernel) const;
  std::vector<Block<T>> get_blocks(int block_size=8);
};

Image<unsigned char> load_from_file(const std::string &filename);
void save_to_file(const std::string &filename, const Image<unsigned char> &image, int quality=100);

template <typename T> class Block{
public:
	int i, j, size, depth, rowsize;
	Image<T> *matrix;
	T get_pixel(int row, int col, int channel) const;
	void set_pixel(int row, int col, int channel, T value);
};

template <class T> T Block<T>::get_pixel(int row, int col, int channel) const {
    assert(row>=0 && row<size && col>=0 && col<size);
    return matrix->get(row+j, col+i, channel);
}
template <class T> void Block<T>::set_pixel(int row, int col, int channel, T value) {
    assert(row>=0 && row<size && col>=0 && col<size);
    return matrix->set(row+j, col+i, channel, value);
}

template <class T> Image<T>::Image() {
    matrix = NULL;
}
template <class T> Image<T>::Image(int width, int height, int channels) {
    this->width = width;
    this->height = height;
    this->channels = channels;
    matrix = std::shared_ptr<T[]>(new T[height*width*channels]);
}
template <class T> Image<T>::Image(const Image<T> &a) {
    width = a.width;
    height = a.height;
    channels = a.channels;
    if (a.matrix != NULL) {
        matrix = a.matrix;
    }
    else{
        matrix = NULL;
    }
}
template <class T> Image<T>::~Image() {
    release();
}

template <class T> Image<T> Image<T>::operator=(const Image<T> &a) {
    if (this == &a) return *this;
    release();
    width = a.width;
    height = a.height;
    channels = a.channels;
    if (a.matrix != NULL) {
        matrix = a.matrix;
    }
    else{
        matrix = NULL;
    }
    return *this;
}

template <class T> void Image<T>::release() {
    matrix = NULL;  
}

template <class T> T Image<T>::get(int row, int col, int channel) const{
    return matrix[row*width*channels + col*channels + channel];
}
template <class T> void Image<T>::set(int row, int col, int channel, T value) {
    matrix[row*width*channels + col*channels + channel] = value;
}

template <class T> Image<T> Image<T>::operator*(const Image<T>& other) const {
    assert(width == other.width && height == other.height && channels == other.channels);
    Image<T> new_image(width, height, channels);
    
    // OPTIMIZACIÓN MICRO: 'collapse(2)' aplana los bucles J e I en uno solo más grande.
    // Usamos 'schedule(static)' porque la multiplicación cuesta lo mismo para cada píxel,
    // eliminando el overhead del planificador de OpenMP en tiempo de ejecución.
    #pragma omp parallel for collapse(2) schedule(static)
    for(int j=0;j<height;j++)
    {
        for(int i=0;i<width;i++){
            for(int c=0;c<channels;c++){
                new_image.set(j, i, c, this->get(j, i, c) * other.get(j, i, c));
            }
        }    
    }
    return new_image;
}

template <class T> Image<T> Image<T>::operator*(float scalar) const {
    Image<T> new_image(width, height, channels);
    
    // OPTIMIZACIÓN MICRO: Mismo principio estático. Las escrituras en 'new_image' 
    // son en posiciones disjuntas, lo que garantiza thread-safety puro sin cerrojos.
    #pragma omp parallel for collapse(2) schedule(static)
    for(int j=0;j<height;j++)
    {
        for(int i=0;i<width;i++){
            for(int c=0;c<channels;c++){
                new_image.set(j, i, c, (T)(this->get(j, i, c)*scalar));
            }
        }    
    }
    return new_image;
}

template <class T> Image<T> Image<T>::operator+(const Image<T>& other) const {
    assert(width == other.width && height == other.height && channels == other.channels);
    Image<T> new_image(width, height, channels);
    
    // OPTIMIZACIÓN MICRO: Paralelización espacial 2D aplanada.
    // Maximiza el uso del ancho de banda de la memoria caché al procesar píxeles contiguos.
    #pragma omp parallel for collapse(2) schedule(static)
    for(int j=0;j<height;j++)
    {
        for(int i=0;i<width;i++){
            for(int c=0;c<channels;c++){
                new_image.set(j,i,c, this->get(j, i,c)+other.get(j, i, c));
            }
        }    
    }
    return new_image;
}

template <class T> Image<T> Image<T>::operator+(float scalar) const {
    Image<T> new_image(width, height, channels);
    
    #pragma omp parallel for collapse(2) schedule(static)
    for(int j=0;j<height;j++)
    {
        for(int i=0;i<width;i++){
            for(int c=0;c<channels;c++){
                new_image.set(j, i, c, ((T)this->get(j, i, c)+scalar));
            }
        }    
    }
    return new_image;
}

template <class T> Image<T> Image<T>::abs() const {
    Image<T> new_image(width, height, channels);
    
    #pragma omp parallel for collapse(2) schedule(static)
    for(int j=0;j<height;j++)
    {
        for(int i=0;i<width;i++){
            for(int c=0;c<channels;c++){
                new_image.set(j, i, c, (T)std::abs(this->get(j,i,c)));
            }
        }    
    }
    return new_image;
}

template <class T> Image<T> Image<T>::convolution(const Image<float> &kernel) const {
    assert(kernel.width%2 != 0 && kernel.height%2 != 0 && kernel.width == kernel.height && kernel.channels==1);
    int kernel_size = kernel.width;
    Image<T> convolved(width, height, channels);
    
    // OPTIMIZACIÓN MICRO: Usamos 'schedule(guided)' porque el cálculo en los bordes de la 
    // imagen requiere menos iteraciones válidas ("continue"). Guided balancea esta carga asimétrica.
    #pragma omp parallel for collapse(2) schedule(guided)
    for(int j=0;j<height;j++){
        for(int i=0;i<width; i++){
            for(int c=0;c<channels;c++){
                float sum = 0.0;
                for(int u=0;u<kernel_size;u++){
                    for(int v=0;v<kernel_size;v++){
                        int s = (j + u - kernel_size/2)%height;
                        int t = (i + v - kernel_size/2)%width;
                        if (s < 0 || s >= height || t < 0 || t >= width)
                            continue;
                        sum += (this->get(s, t, c) * kernel.get(u,v, 0));
                    }
                }
                convolved.set(j, i, 0, (T)sum/(kernel_size*kernel_size));
            }
        }
    }
    return convolved;
}

template <class T> template <typename S> Image<S> Image<T>::convert() const {
    Image<S> new_image(width, height, channels);
    
    #pragma omp parallel for collapse(2) schedule(static)
    for(int j=0;j<height;j++)
    {
        for(int i=0;i<width;i++){
            for(int c=0;c<channels;c++){
                new_image.set(j, i, c, (T)this->get(j, i, c));
            }
        }    
    }
    return new_image;
}

template <class T> Image<T> Image<T>::to_grayscale() const {
    if (channels == 1) return convert<T>();
    Image<T> image(width, height, 1);
    
    #pragma omp parallel for collapse(2) schedule(static)
    for(int j=0;j<height;j++){
        for(int i=0;i<width;i++){
            image.set(j, i, 0, (T)((0.299 * this->get(j, i, 0) + (0.587 * this->get(j, i, 1)) + (0.114 * this->get(j,i,2)))));
        }
    }
    return image;
}

template <class T> Image<float> Image<T>::normalized() const {
    Image<float> new_image(width, height, channels);
    float max_value = -999999999;
    float min_value = 999999999;
    
    // OPTIMIZACIÓN MICRO (CRÍTICA): La búsqueda de mínimos y máximos globales genera dependencias.
    // La cláusula 'reduction' ordena a OpenMP crear variables privadas por hilo y fusionarlas 
    // algorítmicamente al final, evitando colisiones letales (race conditions).
    #pragma omp parallel for collapse(2) reduction(max:max_value) reduction(min:min_value)
    for(int j=0;j<height;j++)
    {
        for(int i=0;i<width;i++){
            for(int c=0;c<channels;c++){
                if (this->get(j,i,c) > max_value) max_value = this->get(j,i,c);
                if (this->get(j,i,c) < min_value) min_value = this->get(j,i,c);
            }
        }    
    }

    #pragma omp parallel for collapse(2) schedule(static)
    for(int j=0;j<height;j++)
    {
        for(int i=0;i<width;i++){
            for(int c=0;c<channels;c++){
                new_image.set(j,i,c, (this->get(j, i, c)-min_value) / (max_value - min_value));
            }
        }    
    }
    return new_image;
}

template <class T> std::vector<Block<T>> Image<T>::get_blocks(int block_size) {
  	int depth = channels;
  	assert(width % block_size == 0 || height % block_size == 0);
  	std::vector<Block<T>> blocks;
  	
  	// NOTA ARQUITECTÓNICA: Este bucle NO se paraleliza intencionadamente.
  	// El método std::vector::push_back() muta la estructura del contenedor (memoria dinámica).
  	// Paralelizarlo sin cerrojos generaría corrupción de memoria (condición de carrera pura).
  	for (int row=0;row<height;row+=block_size)
  		for(int col=0;col<width;col+=block_size){
  			Block<T> b;
  			b.i=col;
  			b.j=row;
  			b.size=block_size;
  			b.rowsize=width*channels;
  			b.matrix=this;
  			b.depth=depth;
  			blocks.push_back(b);
  		}
  	return blocks;
}

#endif