#include "info.h"

/* Flag set by ‘--verbose’. */
static int verbose_flag;
static size_t t_one=DEFAULT_T_ONE;
static size_t t_zero=DEFAULT_T_ZERO;
static int a_one=DEFAULT_A_ONE;
static int a_zero=DEFAULT_A_ZERO;
static size_t n_window=DEFAULT_WINDOW_SIZE;
static char metadata_dir[30] = DEFAULT_METADATA_OUT;

int main(int argc, char **argv) {
  int c;

	while (1){
		static struct option long_options[] =
		{
          /* These options set a flag. */
			{"verbose", no_argument,       &verbose_flag, 1},
			{"brief",   no_argument,       &verbose_flag, 0},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
			{"set-t-one",	required_argument, 0, 'a'},
			{"set-t-zero",	required_argument, 0, 'b'},
			{"set-a-one",	required_argument, 0, 'd'},
			{"set-a-zero",  required_argument, 0, 'c'},
			{"set-window-size",  required_argument, 0, 'w'},
			{"mdt-dir",		required_argument, 0, 'f'},
			{0, 0, 0, 0}
		};
      /* getopt_long stores the option index here. */
	int option_index = 0;

	c = getopt_long (argc, argv, "a:b:c:d:f:w:",
                       long_options, &option_index);

		/* Detect the end of the options. */
	if (c == -1)
		break;

	switch (c)
		{
		case 0:
          /* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			printf ("option %s", long_options[option_index].name);
			if (optarg)
				printf (" with arg %s", optarg);
			printf ("\n");
			break;

		case 'a':
			if(atoi(optarg)!=0){
				t_one=(size_t) atoi(optarg);
			}else{
				printf("Value --set-t-one not a number\n");
				}
			break;

		case 'b':
			if(atoi(optarg)!=0){
				t_zero=(size_t) atoi(optarg);
			}else{
				printf("Value --set-t-zero not a number\n");
				}
			break;

		case 'c':
			if(atoi(optarg)!=0){

				a_one=atoi(optarg);
			}else{
				printf("Value --set-a-one not a number\n");
				}
			break;

		case 'd':
			if(atoi(optarg)!=0){
				a_zero=atoi(optarg);
			}else{
				printf("Value --set-a-zero not a number\n");
				}

			break;
		case 'f':
				strcpy(metadata_dir,optarg);
			break;
		case 'w':
			if(atoi(optarg)!=0){
				n_window=(size_t) atoi(optarg);
			}else{
				printf("Value --set-window-size not a number\n");
			}
			break;

		case '?':
          /* getopt_long already printed an error message. */
			break;
	default:
		abort ();
		}
	}

  /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
	if (verbose_flag)
		puts ("verbose flag is set");
	/* Print any remaining command line arguments (not options). */
	if (optind < argc){
		printf ("non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		putchar ('\n');
	}

	printf("Time delay of zero with value of %d\n us", (int) t_one);
	printf("Time delay of one with value of %d\n us", (int) t_zero);
	printf("Amplitude of delay of one with value of %d\n", a_one);
	printf("Size of the window with value of %d\n", (int) n_window);
	printf("Amplitude of delay of zero with value of %d\n", a_zero);
	printf("File to encode: %s\n", metadata_dir);

  AudioFile<double> audioFile;
	audioFile.load (OUTPUT_SONG);
  Array<char> data_buffer;
  size_t n_delay_one = (size_t) round((int) t_one/round(1/(audioFile.getSampleRate()*1e-6)));
	//size_t n_delay_one = 100;
	size_t n_delay_zero = (size_t) round((int) t_zero/round(1/(audioFile.getSampleRate()*1e-6)));
  Array<char> bitArray = decode_song(audioFile, n_delay_one, n_delay_zero, n_window);

  data_buffer.length = bitArray.length/8;
  data_buffer.payload = (char*) malloc (sizeof(char)*data_buffer.length);
  for(int i=0;i<data_buffer.length;i++){
  	for(int j=0;j<8;j++){
      //printf("%d\n", (int) bitArray.payload[i*8+j]);
  		if(bitArray.payload[i*8+j]){
  			data_buffer.payload[i]=setbit(data_buffer.payload[i],j);
  		}else{
  			data_buffer.payload[i]=unsetbit(data_buffer.payload[i],j);
  		}
  	}
  	//printf("%c",data_buffer.payload[i]);
  }
  FILE *pFile;

  pFile = fopen (metadata_dir, "w" );
  fputs(data_buffer.payload,pFile);
  fclose(pFile);

  free(data_buffer.payload);
  free(bitArray.payload);

  //double input[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
  //fft_test_ceptrm(input,32);

  return 0;
}
