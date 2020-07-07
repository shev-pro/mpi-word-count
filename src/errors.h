//
// Created by Sergio Shevchenko on 26/05/2020.
//

#ifndef MPI_WORD_COUNT_ERRORS_H
#define MPI_WORD_COUNT_ERRORS_H
enum wc_error {
    NO_ERROR = 0,
    IO_ERROR = 1,
    OOM_ERROR = 2,
    USELESS_ERROR = 3,
    GENERIC_ERROR = 10,

};
#endif //MPI_WORD_COUNT_ERRORS_H
