#ifndef DTSW_HPP
#define DTSW_HPP
#include "sw_dist.hpp"

namespace dtsw{
  typedef byte* Buffer;
  /*------------------------------------------*/
  void init(int argc, char *argv[]);
  void finalize();
  void run(int , char *[]);
  /*----------------------------------------*/
  typedef struct Partition{    
    union {
      int rows_per_block;
      int M;
      int chunk_size;
    };
    union {
      int cols_per_block;
      int N;
    };
    union{
      int blocks_per_row;
      int Mb;
    };
    union{
      int blocks_per_col;
      int Nb;
    };
  }Partition_t;
  /*----------------------------------------*/
  typedef struct parameters{
    int         p,q,P;
    double      dt;
    Partition_t partition_level[3];
    char       *data_path;
    double      gh0;
    int         atm_block_size_L1,atm_block_size_L2,
      N,IterNo,atm_length;
    bool pure_mpi;
  }Parameters_t;

  extern Parameters_t Parameters;
  /*----------------------------------------*/
}
#endif // DTSW_HPP
