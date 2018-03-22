#include "info.h"

int main(){


  Array<char> data_buffer;
  Array<char> bitArrayRef;
  Array<char> bitArray;
  int err_count=0;
  float err_rate=0;
  data_buffer = load_from_file_to_buffer(DEFAULT_METADATA);
  bitArrayRef = get_bit_array(data_buffer);
  free(data_buffer.payload);
  data_buffer = load_from_file_to_buffer(DEFAULT_METADATA_OUT);
  bitArray = get_bit_array(data_buffer);

  for(int i =0; i<bitArray.length; i++){
    if(i<bitArrayRef.length){
      if(bitArray.payload[i]!=bitArrayRef.payload[i]){
//        printf("error\n");
        err_count++;
      }
    }else{
      if(bitArray.payload[i]!=0){
        err_count++;
//        printf("error\n");
      }
    }
  }

  err_rate = ((float) err_count)/((float) bitArray.length);
  printf("Error rate: %f %%\n", err_rate*100);

  return 0;
}
