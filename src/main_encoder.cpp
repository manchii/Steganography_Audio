#include "info.h"


/* Flag set by ‘--verbose’. */
static int verbose_flag;
static size_t t_one=DEFAULT_T_ONE;
static size_t t_zero=DEFAULT_T_ZERO;
static int a_one=DEFAULT_A_ONE;
static int a_zero=DEFAULT_A_ZERO;
static size_t n_window=DEFAULT_WINDOW_SIZE;
static char metadata_dir[20] = DEFAULT_METADATA;

int main (int argc, char **argv)
{
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


	Array<char> data_buffer;
	Array<char> bitArray;

	data_buffer=load_from_file_to_buffer(metadata_dir);
	//printf("%s\n", data_buffer.payload);
	bitArray=get_bit_array(data_buffer);

	AudioFile<double> audioFile;
	audioFile.load ("input2.wav");

	size_t n_delay_one = (size_t) round((int) t_one/round(1/(audioFile.getSampleRate()*1e-6)));
	//size_t n_delay_one = 100;
	size_t n_delay_zero = (size_t) round((int) t_zero/round(1/(audioFile.getSampleRate()*1e-6)));
	//size_t n_delay_zero = 200;
	if(n_delay_one<=0){
		n_delay_one=1;
		printf("Warning: time delay of one was setted up to 23us\n\tDelay was too small");
	}
	if(n_delay_zero<=0){
		n_delay_zero=1;
		printf("Warning: time delay of zero was setted up to 23us\n\tDelay was too small");
	}
	if(n_delay_one==n_delay_zero){
		n_delay_zero++;
		printf("Warning: time delay of zero was setted up 23us\n\tDelay between one and zero is too small");
	}

	Array<double> kernel_one = create_kernel<double>(a_one,n_delay_one);
	Array<double> kernel_zero = create_kernel<double>(a_one,n_delay_zero);

	//audioFile.setNumSamplesPerChannel(n_window);

	Array<double> encoded_song = encode_data<double>(audioFile, bitArray,kernel_one,kernel_zero,n_window);

	free (data_buffer.payload);
	free(bitArray.payload);
	free(kernel_one.payload);
	free(kernel_zero.payload);

	AudioFile<double>::AudioBuffer new_audioFile_buffer;


	new_audioFile_buffer.resize (audioFile.getNumChannels());
	for(int i=0;i<audioFile.getNumChannels();i++){
		new_audioFile_buffer[i].resize (encoded_song.length);
	}
	printf("SAVING\n");
	for(int i=0;i<encoded_song.length;i++){
//		printf("newdata %f\n", new_song.payload[i]);
		new_audioFile_buffer[0][i]=encoded_song.payload[i];
		new_audioFile_buffer[1][i]=encoded_song.payload[i];
	}
	AudioFile<double> audioFileOut;
	audioFileOut.setAudioBuffer(new_audioFile_buffer);
	audioFileOut.setBitDepth (16);
	audioFileOut.setSampleRate (44100);
	audioFileOut.save ("audioFile.wav");
	printf("Ended\n");
	free(encoded_song.payload);

	exit (0);
}



/* Para decodificar bitArray del decoder
data_buffer.payload = (char*) malloc (sizeof(char)*data_buffer.length);
for(int i=0;i<data_buffer.length;i++){
	for(int j=0;j<8;j++){
		if(bitArray.payload[i*8+j]){
			data_buffer.payload[i]=setbit(data_buffer.payload[i],j);
		}else{
			data_buffer.payload[i]=unsetbit(data_buffer.payload[i],j);
		}
	}
	printf("%c",data_buffer.payload[i]);
}

pFile = fopen ( "output.txt" , "w" );
fputs(data_buffer.payload,pFile);
fclose(pFile);
*/

/*

	const int channel=0;



	//audioFile.printSummary();

	double * kernel_one;
	double * kernel_zero;
	create_kernel(kernel_one, a_one,n_delay_one);
	create_kernel(kernel_zero, a_zero,n_delay_zero);


	struct buffer{
		double *payload;
		int length;
	}new_song,window_tmp,window_residue,conv_res;
	new_song.length = (DEFAULT_WINDOW_SIZE*((int)(ceil(numSamples/DEFAULT_WINDOW_SIZE))));
	window_tmp.payload = (double *)malloc(sizeof(double)*DEFAULT_WINDOW_SIZE);
	window_tmp.length = DEFAULT_WINDOW_SIZE;
	if(n_delay_zero>n_delay_one){
		window_residue.payload= (double *) malloc(sizeof(double)*n_delay_zero);
		window_residue.length=n_delay_zero;
		conv_res.payload= (double *) malloc(sizeof(double)*(DEFAULT_WINDOW_SIZE+n_delay_zero));
		conv_res.length=DEFAULT_WINDOW_SIZE+n_delay_zero;
		new_song.length+=n_delay_zero;
	}
	else{
		window_residue.payload= (double *) malloc(sizeof(double)*n_delay_one);
		window_residue.length=n_delay_one;
		conv_res.payload= (double *) malloc(sizeof(double)*(DEFAULT_WINDOW_SIZE+n_delay_one));
		conv_res.length=DEFAULT_WINDOW_SIZE+n_delay_one;
		new_song.length+=n_delay_one;
	}

	new_song.payload = (double *) malloc(sizeof(double)*new_song.length);
	for(int i;i<new_song.length;i++){
		new_song.payload[i]=0;
	}

	for(int i;i<window_residue.length;i++){
		window_residue.payload[i]=0;
	}

	for(int samples_read=0;samples_read<numSamples;samples_read+=DEFAULT_WINDOW_SIZE){
		printf("read: %d\n",(samples_read*100)/numSamples);
		memcpy(
			window_tmp.payload,
			&audioFile.samples[0][samples_read],
			sizeof(double)*DEFAULT_WINDOW_SIZE);
		//ZERO
		conv(window_tmp.payload,kernel_zero,(size_t) window_tmp.length,(size_t) n_delay_zero+1,conv_res.payload);
		conv_res.length=DEFAULT_WINDOW_SIZE+n_delay_zero;
		for(int j=0;j<window_tmp.length;j++){

			if(j<window_residue.length)
				new_song.payload[j+samples_read]=window_residue.payload[j]+window_tmp.payload[j];
			else
				new_song.payload[j+samples_read]=window_tmp.payload[j];
			//printf("newdata %f\n", new_song.payload[j+samples_read]);
		}
		memcpy(
			window_residue.payload,
			&conv_res.payload[conv_res.length-n_delay_zero-1],
			sizeof(double)*n_delay_zero);
		window_residue.length=n_delay_zero;
	}

	AudioFile<double>::AudioBuffer buffer;
	buffer.resize (2);
	buffer[0].resize (new_song.length);
	buffer[1].resize (new_song.length);
	for(int i=0;i<new_song.length;i++){
		printf("newdata %f\n", new_song.payload[i]);
		buffer[0][i]=new_song.payload[i];
		buffer[1][i]=new_song.payload[i];
	}
	//memcpy(&buffer[0][0],new_song.payload,sizeof(double)*new_song.length);
//memcpy(&buffer[1][0],new_song.payload,sizeof(double)*new_song.length);
	bool ok = audioFile.setAudioBuffer (buffer);
	// Set both the number of channels and number of samples per channel
	for(int i=0;i<new_song.length;i++){
		printf("buff %f\n", audioFile.samples[0][i]);
	}
	audioFile.setBitDepth (16);
	audioFile.setSampleRate (44100);
	audioFile.save ("audioFile.wav");

//	readwav();
//	encodewav();
//	writewav();
	printf("FIN\n");

	free(new_song.payload);
	free(conv_res.payload);
	free(window_residue.payload);
	free(window_tmp.payload);
	free(kernel_one);
	free(kernel_zero);
*/
