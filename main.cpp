#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <iterator>
#include <math.h>
#include <cfloat>
#include <xmmintrin.h>

#define NUMBER_OF_SAMPLES 1000000
#define NUMBER_OF_REPETITIONS 10

using namespace std;
using namespace std::chrono;

inline float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;
 
	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    //y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
 
	return y;
}

inline void ssesqrt_recip( float * pout, float * pin )
{
   __m128 in = _mm_load_ss( pin );
   _mm_store_ss( pout, _mm_rsqrt_ss( in ) );
}

int main( int argc, const char **argv )
{
	float *input = new float[NUMBER_OF_SAMPLES];
	float *slowSqrt = new float[NUMBER_OF_SAMPLES];
	float *fastSqrt = new float[NUMBER_OF_SAMPLES];
	float *sseSqrt = new float[NUMBER_OF_SAMPLES];

	for (int repetition = 0; repetition < NUMBER_OF_REPETITIONS; repetition++) {
		for (long long sample=0; sample < NUMBER_OF_SAMPLES; sample++) { 
			input[sample] = rand() % 100 + 0.01; //FLT_MAX/static_cast<float>(NUMBER_OF_SAMPLES);
			//input[sample] *= static_cast<float>(sample);
		}

    	high_resolution_clock::time_point t1_1 = high_resolution_clock::now();
		for (long long sample = 0; sample < NUMBER_OF_SAMPLES; sample++){    
			slowSqrt[sample] = 1.0 / sqrt(input[sample]); 
		}
		high_resolution_clock::time_point t2_1 = high_resolution_clock::now();
		nanoseconds slowExecTime = duration_cast<nanoseconds>(t2_1 - t1_1);

    	high_resolution_clock::time_point t1_2 = high_resolution_clock::now();
		for (long long sample = 0; sample < NUMBER_OF_SAMPLES; sample++) {    
			fastSqrt[sample] = Q_rsqrt(input[sample]);
		}
		high_resolution_clock::time_point t2_2 = high_resolution_clock::now();
		nanoseconds fastExecTime = duration_cast<nanoseconds>(t2_2 - t1_2);

		high_resolution_clock::time_point t1_3 = high_resolution_clock::now();
		for (long long sample = 0; sample < NUMBER_OF_SAMPLES; sample++) {
			float f;    
			ssesqrt_recip(&f, &(input[sample]));
			sseSqrt[sample] = f;
		}
		high_resolution_clock::time_point t2_3 = high_resolution_clock::now();
		nanoseconds sseExecTime = duration_cast<nanoseconds>(t2_3 - t1_3);

		float avgError = 0;
		for (long long sample = 0; sample < NUMBER_OF_SAMPLES; sample++) {    
			avgError += fabs(fastSqrt[sample] - slowSqrt[sample]) / slowSqrt[sample];
		}
		avgError = avgError / static_cast<float>(NUMBER_OF_SAMPLES);

		float stDev = 0;
		for (long long sample = 0; sample < NUMBER_OF_SAMPLES; sample++) {    
			float aux = (fastSqrt[sample] - slowSqrt[sample]) / slowSqrt[sample];
			stDev += (aux - avgError) * (aux - avgError);
		}
		stDev = stDev / static_cast<float>(NUMBER_OF_SAMPLES);
		stDev = sqrt(stDev);

		std::cout << "SQRT: " << slowExecTime.count() << 
			" ns; RQSRT: " << fastExecTime.count() << 
			" ns; SSE: " << sseExecTime.count() << 
			" ns; Error medio: " << avgError << 
			" ; Desviacion estandar del error: " << stDev <<std::endl;
	}
	return 0;
}