#include	<stdio.h>
#include	<stdlib.h>				/*standard C functionality*/
#include	"sndfile.h"			    /*libsndfile functionality*/

#define		BUFFER_LEN	1024        /*length of the audio buffer*/
#define		CHANNELS	2			/*number of channels in the file*/

int			TRUNC_LENGTH;
int			Q;
int			rpdf_layers;

static void add_GPDF4 (short *data, int count);/*function to process audio*/

int main (int argc, char *argv[])   /*main function*/
{   
	static short    data [BUFFER_LEN];      /*audio buffer*/
	SNDFILE         *infile, *outfile;      /*pointers to file names*/
	SF_INFO		    sfinfo;                 /*structure to hold soundfile info*/
	int			    readcount;              /*counter variable*/
	const char	    *infilename = argv[1];  /*command line arguments - file to be processed*/
	const char	    *outfilename = argv[2]; /*command line arguments - output file*/

   
    if(argc!=3) /*check for correct arguments*/
	{
		printf("Usage:template infile outfile\n");
		return 1;
	}	

	infile = sf_open (infilename, SFM_READ, &sfinfo); /*open infile and check for errors*/ 
	
    if (infile <= 0)
	{
		printf ("Not able to open input file %s.\n", infilename);
		return  1;
	}


	outfile = sf_open (outfilename, SFM_WRITE, &sfinfo); /*open infile and check for errors*/
	
	if (outfile <=0)
    {   
		printf ("Not able to open output file %s.\n", outfilename);
        return  1;
    }

	/*Process Loop*/
		
	printf("\nEnter Truncated Bits Number: ");				/*Accept values from the user for the amplitude of dithering and truncated bit number*/
	scanf("%d", &TRUNC_LENGTH);
	printf("\nEnter Noise Amplitude (in bits): ");
	scanf("%d", &Q);
	printf("\nEnter Gaussian Dithering Layers Number: ");	/*Accept value that sets the number of rectangular functions summed in the gaussian formula*/
	scanf("%d", &rpdf_layers);
            
	do
    {   
    	readcount = sf_read_short (infile, data, BUFFER_LEN);/*read 1024 shorts from the infile to the buffer*/
    	add_GPDF4 (data, readcount);                      /*call the process function and pass the data and amount of data*/
    	sf_write_short (outfile, data, readcount);           /*write the modified data to the outfile*/
    } while (readcount > 0);                              /*check that we have data to work with before repeating*/

	sf_close (infile); /*close files*/
	sf_close (outfile);/*close files*/
	printf("\nFile Processed!\n");
	return 0;
}


static void add_GPDF4 (short *data, int count)
{	
int k, chan, sum, i; /*some counters and a variable (sum) which will carry the rpdf functions summation*/

    for (chan = 0; chan < CHANNELS ; chan ++)       /*for the number of channels*/
	{ 
		for (k = chan; k < count ; k+= CHANNELS)    /*modify every sample in the buffer for that channel*/
		{		
			sum=0;
			for(i=0;i<rpdf_layers;i++) sum+=rand()%Q;
			data[k] = data[k] + (sum/rpdf_layers);
			data[k] = data[k]>>TRUNC_LENGTH;
			data[k] = data[k]<<TRUNC_LENGTH;
		}
	}
return;
}