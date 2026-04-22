/**
 * @file aprs_receiver_test.cpp
 * @brief Integration tests for signal_easel::aprs::Receiver.
 *
 * The aprs receiver implementation is known to
 * be hot trash rn, however the decoder seems to work well. Basically, wav files
 * good, pulse/live audio bad (10% success rate with decoding real packets)
 *
 * ## How these tests differ from the non-pulse tests in aprs_test.cpp
 *
 * The tests in aprs_test.cpp use aprs::Modulator and aprs::Demodulator
 * directly: encode a packet to a WAV file, load the entire file into the
 * demodulator at once with loadAudioFromFile(), call processAudioBuffer()
 * once, then assert on the result. There is no signal-detection layer, no
 * chunking, no SNR gating.
 *
 * Because aprs::Receiver owns its own PulseAudioReader, a
 * TestableAprsReceiver subclass is used here. It overrides process() to
 * feed fake chunked data from a WAV file instead of a live audio device,
 * while the inherited detectSignal() and decode() path runs unmodified.
 *
 * ## Build and run
 *
 * # Must run from the build directory to find the test WAV files
 *
 *   cmake -S . -B build
 *   cmake --build build --target signal_easel_unit_tests
 *   cd build/tests && ./signal_easel_unit_tests --gtest_filter="AprsReceiver*"
 *
 *
 */

#include "gtest/gtest.h"

#include <SignalEasel/aprs.hpp>
#include <SignalEasel/ax25.hpp>
#include <wav_gen.hpp>

#include <memory>
#include <vector>

namespace signal_easel {
namespace aprs {

/**
 * @brief Fake PulseAudioReader that reads from a WAV file in chunks.
 * @details This simulates the behavior of the real PulseAudioReader but
 * reads from a pre-recorded WAV file instead of a live audio stream.
 */
class FakePulseAudioReader {
public:
  explicit FakePulseAudioReader(const std::string &wav_file_path)
      : all_samples_(), current_position_(0), audio_buffer_() {
    wavgen::Reader reader(wav_file_path);
    reader.getAllSamples(all_samples_);
    audio_buffer_.fill(0);
  }

  /**
   * @brief Simulates reading a chunk of audio data.
   * @return true if there was data to read, false if we've reached the end.
   */
  bool process() {
    if (current_position_ >= all_samples_.size()) {
      return false;
    }

    size_t samples_to_copy = std::min(PULSE_AUDIO_BUFFER_SIZE,
                                      all_samples_.size() - current_position_);
    audio_buffer_.fill(0);
    std::copy(all_samples_.begin() + current_position_,
              all_samples_.begin() + current_position_ + samples_to_copy,
              audio_buffer_.begin());
    current_position_ += samples_to_copy;

    return samples_to_copy > 0;
  }

  const PulseAudioBuffer &getAudioBuffer() const { return audio_buffer_; }

private:
  std::vector<int16_t> all_samples_;
  size_t current_position_;
  PulseAudioBuffer audio_buffer_;
};

/**
 * @brief Testable version of aprs::Receiver that injects a fake pulse audio
 * reader, exercising the full aprs::Receiver decode path.
 */
class TestableAprsReceiver : public Receiver {
public:
  TestableAprsReceiver(std::shared_ptr<FakePulseAudioReader> fake_reader,
                       aprs::Settings settings = aprs::Settings())
      : Receiver(settings), fake_reader_(fake_reader) {}

  bool process() override {
    if (!fake_reader_->process()) {
      return false;
    }
    return detectSignal(fake_reader_->getAudioBuffer());
  }

private:
  std::shared_ptr<FakePulseAudioReader> fake_reader_;
};

} // namespace aprs
} // namespace signal_easel

/**
 * @brief Test that the receiver properly decodes a real APRS packet from
 * aprs_real.wav using the full aprs::Receiver pipeline.
 */
TEST(AprsReceiver, DecodeRealAprsPacketChunked) {
  const std::string kInputFile = "aprs_real.wav";

  auto fake_reader =
      std::make_shared<signal_easel::aprs::FakePulseAudioReader>(kInputFile);
  signal_easel::aprs::TestableAprsReceiver receiver(fake_reader);

  const int kMaxChunks = 1000;
  int chunk_count = 0;
  while (chunk_count < kMaxChunks && receiver.process()) {
    chunk_count++;
  }

  auto stats = receiver.getStats();
  uint32_t total_packets =
      stats.total_message_packets + stats.total_position_packets +
      stats.total_experimental_packets + stats.total_telemetry_packets +
      stats.total_other_packets;

  EXPECT_GT(total_packets, 0u)
      << "Failed to decode any APRS packet after " << chunk_count << " chunks";
}

/**
 * @brief Test that the receiver decodes multiple APRS packets from
 * multi_packet_aprs.wav. All packets should be experimental packets with
 * source KD9GDC-1, destination KD9GDC-11, type char 'b', and data
 * "0000000acmd/dat/cae/".
 */
TEST(AprsReceiver, DecodeMultipleAprsPacketsChunked) {
  const std::string kInputFile = "multi_packet_aprs.wav";

  auto fake_reader =
      std::make_shared<signal_easel::aprs::FakePulseAudioReader>(kInputFile);
  signal_easel::aprs::TestableAprsReceiver receiver(fake_reader);

  const int kMaxChunks = 10000;
  int chunk_count = 0;
  while (chunk_count < kMaxChunks && receiver.process()) {
    chunk_count++;
  }

  auto stats = receiver.getStats();
  EXPECT_GT(stats.total_experimental_packets, 1u)
      << "Should have decoded multiple experimental packets";

  // Drain and verify each experimental packet
  signal_easel::aprs::ExperimentalPacket packet;
  signal_easel::ax25::Frame frame;
  int verified_count = 0;
  while (receiver.getAprsExperimental(packet, frame)) {
    EXPECT_EQ(packet.source_address, "KD9GDC");
    EXPECT_EQ(packet.source_ssid, 1u);
    EXPECT_EQ(packet.packet_type_char, 'b');
    EXPECT_EQ(packet.getStringData(), "0000000acmd/dat/cae/");
    verified_count++;
  }

  EXPECT_GT(verified_count, 0) << "No experimental packets found in queue";
}
