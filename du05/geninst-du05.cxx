#include <iostream>
#include <cstdlib>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/nondet_random.hpp>
#include <unistd.h>


const int MAX_NEEDLE_ROWS = 10000;
const int MAX_NEEDLE_COLUMNS = 1000;

const int MAX_HAYSTACK_ROWS = 10000;
const int MAX_HAYSTACK_COLUMNS = 1000;


//typedef boost::random_device gen_type;
typedef boost::mt19937 gen_type;
gen_type gen;


unsigned
xrandom (unsigned mod)
{
  //return unsigned(double(std::rand ()) / (RAND_MAX) * mod);
  boost::uniform_int<unsigned> dist (0u, mod-1);
  boost::variate_generator<gen_type&, boost::uniform_int<unsigned> > die (gen, dist);
  return die ();
}


void
gen_matrix (std::size_t rows, std::size_t cols)
{
  for (std::size_t i = 0; i != rows; ++i)
    {
      for (std::size_t j = 0; j != cols; ++j)
        std::cout << static_cast<char>(xrandom (9) + '0');
      
      std::cout << "\n";
    }
  std::cout << std::flush;
}


int
main ()
{
  std::ios_base::sync_with_stdio (false);

  unsigned seed = static_cast<unsigned>(getpid () ^ time (0));
  std::srand (seed);
  gen.seed (seed);

  std::size_t needle_rows = xrandom (MAX_NEEDLE_ROWS) + 1;
  std::size_t needle_columns = xrandom (MAX_NEEDLE_COLUMNS) + 1;

  std::size_t haystack_rows = xrandom (MAX_HAYSTACK_ROWS) + 1;
  std::size_t haystack_columns = xrandom (MAX_HAYSTACK_COLUMNS) + 1;

  std::clog << "seed: " << seed << "\n"
            << "needle_rows: " << needle_rows << "\n"
            << "needle_columns: " << needle_columns << "\n"
            << "haystack_rows: " << haystack_rows << "\n"
            << "haystack_columns: " << haystack_columns << std::endl;

  std::cout << needle_rows << std::endl;
  gen_matrix (needle_rows, needle_columns);
  //gen_matrix (haystack_rows, haystack_columns);
  gen_matrix (1000000, haystack_columns);
}
