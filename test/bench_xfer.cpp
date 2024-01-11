#include <benchmark/benchmark.h>

#include "core.h"
#include "inf3173/config.h"
#include "testutils.h"
#include "utils.h"
#include "xfer.h"

static const char* fname_src = BINARY_DIR "/bench_client_file.dat";
static const char* fname_dst = BINARY_DIR "/bench_server_file.dat";
static const int fsize = (1 << 20);

static void BM_Transfer(benchmark::State& state) {
  if (!is_regular_file(fname_src)) {
    if (generate_file(fname_src, fsize) < 0) {
      state.SkipWithError("Echec creation fichier");
    }
  }

  int bs = state.range(0);
  struct xfer_file srv_data = {
      .fname = fname_dst, .block_size = bs, .verbose = 0};
  struct xfer_file clt_data = {
      .fname = fname_src, .block_size = bs, .verbose = 0};
  struct xfer_server_state* sstate;
  xfer_server_init(&sstate, "127.0.0.1", 8888, xfer_recv_file, &srv_data);
  if (xfer_server_start(sstate) < 0) {
    state.SkipWithError("Echec demarrage server");
  }

  // Boucle de mesure
  for (auto _ : state) {
    if (xfer_client("127.0.0.1", 8888, xfer_send_file, &clt_data) < 0) {
      state.SkipWithError("Echec transfert");
    }
  }

  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * fsize);
  xfer_server_stop(sstate);
}
// Register the function as a benchmark
BENCHMARK(BM_Transfer)->RangeMultiplier(2)->Range(1, 32768);
// Run the benchmark
BENCHMARK_MAIN();
