#ifndef _CLANGD_MOCK_OMP_H
#define _CLANGD_MOCK_OMP_H

/* 
 * This is a mock omp.h specifically used to silence clangd/IntelliSense errors.
 * GCC 14's omp.h contains syntax that Clang 19 fails to parse (e.g. __malloc__ attributes).
 * This mock provides the necessary definitions so clangd doesn't show false positives.
 * It is NOT used during actual compilation (which uses g++).
 */

#ifdef __cplusplus
extern "C" {
#endif

void omp_set_num_threads(int num_threads);
double omp_get_wtime(void);

#ifdef __cplusplus
}
#endif

#endif
