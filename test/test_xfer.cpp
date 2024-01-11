#include <gtest/gtest.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iterator>
#include <memory>
#include <unordered_map>

#include "core.h"
#include "inf3173/config.h"
#include "testutils.h"
#include "utils.h"
#include "xfer.h"

static const char *fname_src = BINARY_DIR "/test_client_file.dat";
static const char *fname_dst = BINARY_DIR "/test_server_file.dat";
struct xfer_file srv_data = {
    .fname = fname_dst, .block_size = 64, .verbose = 0};
struct xfer_file clt_data = {
    .fname = fname_src, .block_size = 32, .verbose = 0};

class Server {
public:
  Server(struct xfer_file *data, xfer_function func) {
    xfer_server_init(&state, "127.0.0.1", 8888, func, data);
    xfer_server_start(state);
  }
  ~Server() { xfer_server_stop(state); }
  struct xfer_server_state *state;
};

/*
 * Vérifier que tous les descripteurs sont bien fermés.
 */
TEST(TransferTest, FileLeak) {
  generate_file(fname_src, 128);

  int f1 = get_active_fd();
  {
    Server s(&srv_data, xfer_recv_file);
    xfer_client("127.0.0.1", 8888, xfer_send_file, &clt_data);
  }
  int f2 = get_active_fd();
  EXPECT_EQ(f1, f2) << "Fuite de fichiers";
}

struct TransferParam {
  int file_size;
  int bs_recv;
  int bs_send;
  std::string name;
};

class TransferTest : public testing::TestWithParam<struct TransferParam> {};

int progress_count = 0;
void test_progress(int progress, void *arg) {
  struct timespec ts = {
      .tv_sec = 0, .tv_nsec = 10000000 // 10ms
  };
  nanosleep(&ts, NULL);
  progress_count++;
}

TEST_P(TransferTest, CheckProgress1) {
  struct TransferParam p = GetParam();
  generate_file(fname_src, p.file_size);
  if (is_regular_file(fname_dst)) {
    unlink(fname_dst);
  }

  progress_count = 0;
  clt_data.progress = test_progress;
  clt_data.block_size = p.bs_send;
  srv_data.block_size = p.bs_recv;

  {
    Server s(&srv_data, xfer_recv_file);
    xfer_client("127.0.0.1", 8888, xfer_send_file, &clt_data);
  }

  int exp = p.file_size / p.bs_send;
  int reste = p.file_size % p.bs_send;
  if (reste) {
    exp += 1;
  }

  EXPECT_EQ(progress_count, exp);
}

TEST_P(TransferTest, CheckFile) {
  struct TransferParam p = GetParam();
  generate_file(fname_src, p.file_size);
  if (is_regular_file(fname_dst)) {
    unlink(fname_dst);
  }

  clt_data.progress = test_progress;
  clt_data.block_size = p.bs_send;
  srv_data.block_size = p.bs_recv;

  {
    Server s(&srv_data, xfer_recv_file);
    xfer_client("127.0.0.1", 8888, xfer_send_file, &clt_data);
  }
  EXPECT_TRUE(are_files_identical(fname_src, fname_dst));
}

INSTANTIATE_TEST_SUITE_P(
    TransferFileSizeTest, TransferTest,
    testing::Values(TransferParam{128, 32, 32, "WholeBlocks"},
                    TransferParam{134, 32, 32, "WithRemainder"},
                    TransferParam{128, 16, 32, "SmallBlockClient"},
                    TransferParam{128, 32, 16, "SmallBlockServer"}),
    [](const testing::TestParamInfo<TransferTest::ParamType> &info) {
      return info.param.name;
    });
