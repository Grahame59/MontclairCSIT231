//  Kyler Grahame
//  CSIT 231
//  7/5/24
//  Multi_Thread_Sphere_Area_Estimation_Project

/* The radius for the sphere is 1.  You need to model a three-dimensional space (x, y, and z coordinates). The range for each value should be 
-1 <= x,y,z <= 1. Picture a sphere. The coordinates of it’s center are (0,0,0). It has a radius of 1. If we take the z-axis as the up and down axis, 
your sphere has a North Pole at (0,0,1) and a South Pole at (0,0,-1). The sphere fits exactly inside a 3D cube with height, width, and length all equal 
to two which is twice of the radius. The cube’s corners will be at (1,1,1), (-1,1,1), ...... , (-1,-1,-1). North Pole of sphere touches top of cube, 
South Pole touches the bottom of the cube. The cube’s volume is easy to calculate which is 2^3= 8. The sphere only occupies some, but not all of the 
cube’s volume. We will randomly generate lots of 3D points (x, y, z) within the cube, using a pseudorandom and each coordinate of x, y, z falling 
within the range -1 to 1 inclusive. Then we need to check if each point you generate falls within the sphere. Centre of sphere is (0,0,0). 
A point within the sphere is at a distance no greater than one from the centre. so if the distance computed by sqrt (x^2 + y^2 + z^2) <= 1, 
then the point is within the sphere. Doing this lots of times and write down the total number of randomly generated points that lie inside the sphere. 
Then we can get an estimation of sphere volume 2^3 / volume of sphere = total random points / points within sphere To check your result is sensible, 
compare it to the actual volume of the sphere which will be 4 * pi* r^3 / 3; here r is 1. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Platform-specific includes and definitions b/c it wouldn't run on VS on my Windows PC
#ifdef _WIN32
#include <windows.h> // For Windows API
#define THREAD_TYPE HANDLE
#else
#include <pthread.h> // For POSIX threads (Linux, macOS)
#define THREAD_TYPE pthread_t
#endif

#define numOfPoints 5000000
#define numOfThreads 4
#define realPi 3.14159265358979323846


int sphereCount = 0; //num of hits INSIDE the drawn sphere

// Platform-specific mutex declaration
#ifdef _WIN32
HANDLE mutex; //Mutex protection for the sphereCount to prevent thread "entanglement"
#else
pthread_mutex_t mutex;
#endif

// Platform-specific thread function declaration
#ifdef _WIN32
DWORD WINAPI countPointsSphere(LPVOID arg) //SO I don't fully understand this fucntion I found it on StackOverflow but to my understanding it is a windows API that creates a pointer to LPVOID which allows the thread to take any datatype as an argument

#else
void *countPointsSphere(void *arg)
#endif
{
    int pointsPerThread = numOfPoints / numOfThreads;
    int localCount = 0;
    unsigned int seed = (unsigned int)time(NULL) + (int)arg; // the arguments data type being cast is for an int type used as a random number generator

    //Point Generation math... loops pointsPerThread times
    for (int i = 0; i < pointsPerThread; ++i)
    {
        double x = (double)rand() / RAND_MAX * 2.0 - 1.0; // generates a value between (0 & RAND_MAX /RAND_MAX) = 0 < 1 rand
        double y = (double)rand() / RAND_MAX * 2.0 - 1.0; // multiplies by 2 so range is = 0 < 2
        double z = (double)rand() / RAND_MAX * 2.0 - 1.0; // subtracts by 1 so 0-1 = -1 and 2 - 1 = 1 so range is -1 < 1 for all 3 planes x, y & z

        if (x * x + y * y + z * z <= 1.0) //distance check formula inside sphere
        {
            localCount++; //increment

        }
}
    //mutex lock and unlock ('Gate' that gives a thread ownership) AKA single file line for threads
    #ifdef _WIN32
    WaitForSingleObject(mutex, INFINITE);
    #else
    pthread_mutex_lock(&mutex);
    #endif

    sphereCount += localCount; //carries points that were inside the sphere into the global var
    
    // Unlock mutex after updating shared variable
    #ifdef _WIN32
    ReleaseMutex(mutex);
    #else
    pthread_mutex_unlock(&mutex);
    #endif

    // Return thread exit status
    #ifdef _WIN32
    return 0;
    #else
    return NULL;
    #endif

}//end of countPointsSphere

int main() {
    HANDLE threads[numOfThreads];

    // Platform-specific mutex initialization
    #ifdef _WIN32
    mutex = CreateMutex(NULL, FALSE, NULL);
    #else
    pthread_mutex_init(&mutex, NULL);
    #endif

    clock_t start, end;
    double cpuTimeUsed;
    start = clock();

    // Create threads
    for (int i = 0; i < numOfThreads; ++i) {
        #ifdef _WIN32
        threads[i] = CreateThread(NULL, 0, countPointsSphere, (LPVOID)(size_t)i, 0, NULL);
        #else
        pthread_create(&threads[i], NULL, countPointsSphere, (void *)(size_t)i);
        #endif
    }

    // Wait for threads to finish
    for (int i = 0; i < numOfThreads; ++i) {
        #ifdef _WIN32
        WaitForSingleObject(threads[i], INFINITE);
        #else
        pthread_join(threads[i], NULL);
        #endif
    }
    end = clock();
    cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Calculate the estimated volume of the sphere 
    double cubeVol = 8.0;
    double sphereVol = (double)sphereCount / numOfPoints * cubeVol;

    // Actual volume of the sphere 
    double actualSphereVol = (4.0 / 3.0) * realPi;

    printf("Estimated volume of the sphere: %f\n", sphereVol);
    printf("Actual volume of the sphere: %f\n", actualSphereVol);
    printf("CPU time used: %f seconds\n", cpuTimeUsed);

    // Clean up platform-specific resources
    #ifdef _WIN32
    CloseHandle(mutex);
    #else
    pthread_mutex_destroy(&mutex);
    #endif

    return 0;
}


