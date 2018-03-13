//#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include <complex>
#include <fftw3.h>

#include <math.h>
#include "AudioFile.h"






#define DEFAULT_METADATA "metadatos"
#define DEFAULT_T_ONE 2300
#define DEFAULT_T_ZERO 4600
#define DEFAULT_A_ONE 40
#define DEFAULT_A_ZERO 40
#define DEFAULT_WINDOW_SIZE 10000

#define getbit(IN,N) (IN >> N) & 1;
#define setbit(IN,N) IN |= 1 << N;
#define unsetbit(IN,N) IN &= ~(1 << N);
#define togglebit(IN,N) IN ^= 1 << N;




template <typename T>
struct Array {
  size_t length;
  T *payload;
};

Array<char> load_from_file_to_buffer(const char*);
Array<char> get_bit_array(const Array<char>);


Array<char> load_from_file_to_buffer(const char *file_name){
	Array<char> data_buffer;
	FILE * pFile;
	size_t result;
	pFile = fopen ( file_name , "r" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
	printf("Success: File found\nData to encode:\n");
	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	data_buffer.length = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	data_buffer.payload = (char*) malloc (sizeof(char)*data_buffer.length);
	if (data_buffer.payload == NULL) {fputs ("Memory error",stderr); exit (2);}

	// copy the file into the buffer:
	result = fread (data_buffer.payload,1,data_buffer.length,pFile);
	if (result != data_buffer.length) {fputs ("Reading error",stderr); exit (3);}

	/* the whole file is now loaded in the memory buffer. */
	// terminate
	fclose (pFile);
	return data_buffer;
}

Array<char> get_bit_array(const Array<char> data_buffer){
	Array<char> bitArray;
	bitArray.length=data_buffer.length*8;
	bitArray.payload =(char *) malloc(sizeof(char)*bitArray.length);
	for(int i = 0; i<data_buffer.length;i++){
//		printf("%c ", data_buffer.payload[i]);
		for(int j=0;j<8;j++){
			bitArray.payload[i*8+j]=getbit(data_buffer.payload[i],j);
//			printf("%o",bitArray.payload[i*8+j]);
		}
//		printf("\n");
	}
	return bitArray;
}


template <typename T>
Array<T> create_kernel(int pcent_amp, size_t n_delay){
  Array<T> kernel;
  kernel.length=n_delay+1;
	kernel.payload = (T *) malloc(sizeof(T)*kernel.length);
	double percent = ((double) pcent_amp)/100;
	for(int i = 0; i<kernel.length; i++){
		if(i==0){
			kernel.payload[i]=1;
		}else{
			if(i<n_delay){
				kernel.payload[i]=0;
			}else{
				kernel.payload[i]=percent;
			}
		}
	}
  return kernel;
}

template <typename T>
Array<T> convolution(const Array<T> signal_in, const Array<T> kernel){
  Array <T> result;
  result.length = ((size_t)(int) signal_in.length + (int) kernel.length - 1);
  result.payload = (T *) malloc(sizeof(T)*result.length);
  for (int n = 0; n < result.length; n++){
    size_t kmin, kmax, k;

    result.payload[n] = 0;

    kmin = (n >= kernel.length - 1) ? n - (kernel.length - 1) : 0;
    kmax = (n < signal_in.length - 1) ? n : signal_in.length - 1;

    for (k = kmin; k <= kmax; k++)
    {
      result.payload[n] += signal_in.payload[k] * kernel.payload[n - k];
    }
  }
  return result;
}

template<typename T>
void add_tail_conv(Array<T> conv, Array<T> conv_before, size_t window_size){
  int diff = (int) (conv_before.length-window_size);
  for(int i=0;i<diff;i++){
    conv.payload[i]+=conv_before.payload[window_size+i];
  }
}

template<typename T>
void blackman_window(Array<T> window){
  double w1=2*M_PI/(((double) window.length)-1);
//  printf("blk\n");
  for (int i = 0; i < window.length; i++) {
    if(i%100==0)
//      printf("%d\n", i);
    window.payload[i] = window.payload[i]*(0.42-0.5*cos(w1*i)+0.08*cos(2*w1*i));
  }
}

template <typename T>
Array<T> encode_data(AudioFile<T> audioFile, Array<char> bitArray, Array<T> kernel_one, Array<T> kernel_zero, size_t window_size){
  Array<T> encoded_song;
  Array<T> conv, conv_before;
  Array<T> window;
  window.length = window_size;
//  FILE *fp = fopen("golden","w");
  int N_windows=((int)ceil((double)audioFile.getNumSamplesPerChannel()/(double) window.length));
  encoded_song.length = window.length*((size_t) N_windows);
  encoded_song.payload=(T*) malloc(sizeof(T)*encoded_song.length);
  conv_before.length = kernel_zero.length+window.length-1;
  conv_before.payload = (T*) malloc(sizeof(T)*conv_before.length);
  for(int i=0;i<conv_before.length;i++){
    conv_before.payload[i]=0;
  }
  for(int i=0;i<N_windows-1;i++){
    window.payload = &audioFile.samples[0][i*window.length];
    blackman_window<T>(window);
    printf("%d\t%d\n", N_windows, i);
    if(i<bitArray.length){
      if(bitArray.payload[i]){
//        printf("len %d %d\n",window.length,kernel_one.length);
        conv= convolution<T>(window,kernel_one);
      }else{
//      printf("0\n");
        conv= convolution<T>(window,kernel_zero);
      }
    }else{
        conv= convolution<T>(window,kernel_zero);
    }
    add_tail_conv<T>(conv,conv_before,window.length);
    for(int j=0;j<window.length;j++){
//      fprintf(fp,"%df\t%df\n", window.payload[j], conv.payload[j]);
      encoded_song.payload[i*window.length+j]=conv.payload[j];
    }
//    printf("%d %d\n", N_windows, i);
    free(conv_before.payload);
    conv_before.length=conv.length;
    conv_before.payload=conv.payload;
  }

  free(conv.payload);
//  fclose(fp);
  printf("Returning Encoded song\n");
//  free(window.payload);
  return encoded_song;
}
template<typename T>
bool corr_ceptrm(T* buffer, int t_delay_one, int t_delay_zero, size_t window_size){

  std::complex<T> *input;
  std::complex<T> *fft;
  std::complex<T> *ifft;

  bool bit;


  input = (std::complex<T> *) malloc(sizeof(std::complex<T>)*window_size);
  fft = (std::complex<T> *) malloc(sizeof(std::complex<T>)*window_size);
  ifft = (std::complex<T> *) malloc(sizeof(std::complex<T>)*window_size);

  for (int i = 0; i < window_size; i++){
    input[i] = std::complex<T>(buffer[i],0);
    //std::cout << i << ' '<< input[i].real() << '\n';
  }



  fftw_plan plan = fftw_plan_dft_1d(window_size,
                                    reinterpret_cast<fftw_complex*>(input),
                                    reinterpret_cast<fftw_complex*>(fft),
                                    FFTW_FORWARD,
                                    FFTW_ESTIMATE);
  fftw_plan plan2 = fftw_plan_dft_1d(window_size,
                                    reinterpret_cast<fftw_complex*>(fft),
                                    reinterpret_cast<fftw_complex*>(ifft),
                                    FFTW_BACKWARD,
                                    FFTW_ESTIMATE);

  fftw_execute(plan);
  for (int i = 0;  i < window_size; i++) {
    fft[i]= std::pow<T>(std::log<T>(fft[i]),2);
  }
  fftw_execute(plan2);

  printf("%f\t%f\t%f\n", std::abs<T>(ifft[0])/window_size, std::abs<T>(ifft[t_delay_one-1])/window_size, std::abs<double>(ifft[t_delay_zero-1])/window_size);
  if (std::abs<T>(ifft[t_delay_one-1])>std::abs<double>(ifft[t_delay_zero-1])) {
    bit = true;
  }else{
    bit = false;
  }
  FILE *pFile = fopen("corcor","w");

  for(int i=0;i<300;i++){
    fprintf(pFile, "%f\n", (std::abs<T>(ifft[i])/window_size));
  }
  fclose(pFile);
  int L;
  std::string kek;
  getline(std::cin,kek);

  fftw_destroy_plan(plan);
  fftw_destroy_plan(plan2);
  free(input);
  free(fft);
  free(ifft);
  return bit;
}

template<typename T>
void fft_test_ceptrm(T* buffer, size_t window_size){

  std::complex<T> *input;
  std::complex<T> *fft;
  std::complex<T> *ifft;



  input = (std::complex<T> *) malloc(sizeof(std::complex<T>)*window_size);
  fft = (std::complex<T> *) malloc(sizeof(std::complex<T>)*window_size);
  ifft = (std::complex<T> *) malloc(sizeof(std::complex<T>)*window_size);
  Array<double> win;
  win.length =window_size;
  win.payload = buffer;
  blackman_window<double>(win);
  for (int i = 0; i < window_size; i++){
    input[i] = std::complex<T>(buffer[i],0);
    //std::cout << i << ' '<< input[i].real() << '\n';
  }



  fftw_plan plan = fftw_plan_dft_1d(window_size,
                                    reinterpret_cast<fftw_complex*>(input),
                                    reinterpret_cast<fftw_complex*>(fft),
                                    FFTW_FORWARD,
                                    FFTW_ESTIMATE);
  fftw_plan plan2 = fftw_plan_dft_1d(window_size,
                                    reinterpret_cast<fftw_complex*>(fft),
                                    reinterpret_cast<fftw_complex*>(ifft),
                                    FFTW_BACKWARD,
                                    FFTW_ESTIMATE);

  fftw_execute(plan);
  for (int i = 0;  i < window_size; i++) {
    fft[i]= std::pow<T>(std::log<T>(fft[i]),2);
  }
  fftw_execute(plan2);

for (int i = 0;  i < window_size; i++) {
    std::cout << i << '\t' << ifft[i]/(double)window_size << '\n';
}

  fftw_destroy_plan(plan);
  fftw_destroy_plan(plan2);
  free(input);
  free(fft);
  free(ifft);
}


template <typename T>
Array<char> decode_song(AudioFile<T> audioFile, int t_delay_one, int t_delay_zero, size_t window_size){
  Array<char> bitArray;
  int n_window = ((int)ceil((double)audioFile.getNumSamplesPerChannel()/(double) window_size));
  if((n_window%8)!=0){
    bitArray.length = n_window + 8 - n_window%8;
  }else{
    bitArray.length = n_window;
  }
  bitArray.payload=(char*) malloc(sizeof(char)*bitArray.length);
  for (int i = 0; i < bitArray.length; i++) {
    bitArray.payload[i]=0;
  }
  for (int i = 0; i < n_window; i++) {
    bitArray.payload[i]=(char) corr_ceptrm<double>(&audioFile.samples[0][window_size*i], t_delay_one, t_delay_zero, window_size);
  }
  return bitArray;
}
