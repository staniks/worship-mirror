#pragma once

#include "mau/audio/audio_clip.hpp"
#include "mau/base/module.hpp"
#include "mau/base/timer.hpp"
#include "mau/math/quaternion.hpp"
#include "mau/math/vector.hpp"

#include <array>
#include <vector>
#include <map>

namespace mau {

// SDL mixer channels per our audio frame.
inline static constexpr int32_t channels_per_audio_frame_k = 256;

// Used to determine whether the channel represents a 3D sound, and where it is.
struct channel_info_t
{
    bool      positional;
    glm::vec3 position;
};

// Audio frames are used to allow pausing and resuming game state sound channels.
struct audio_frame_t
{
    std::array<channel_info_t, channels_per_audio_frame_k> channel_info;
    std::map<audio_clip_handle_t, uint64_t>                timestamps;
};

class audio_t : public module_t
{
public:
    audio_t(engine_context_t& engine);

    // We're caching listener position and orientation in order to simulate 3D sound. Use this to update this pair.
    void update_listener(glm::vec3 position, glm::quat orientation);

    // Play audio clip at normal volume.
    void play_clip(audio_clip_handle_t audio_clip);

    // Play audio clip at given position.
    void play_clip(audio_clip_handle_t audio_clip, glm::vec3 position);

    // Clear the audio frame stack.
    void clear();

    // Push the audio frame to the stack.
    void push();

    // Pop the audio frame from the stack.
    void pop();

private:
    // Convert between absolute and relative channel indices. Relative channel is [0, channels_per_audio_frame>, while the
    // absolute channel is [0, channels_per_audio_frame * audio_frame_count>. Absolute channels are convenience to allow
    // for easier referencing of SDL mixer channels.
    int32_t relative_channel(int32_t absolute_channel_index);
    int32_t absolute_channel(int32_t relative_channel_index);

    // Returns the absolute index of the first free channel in current audio frame.
    int32_t find_free_channel();

    // Returns true if sound was not played recently. Used to avoid sound-stacking earrape.
    bool check_timestamp(audio_clip_handle_t audio_clip);

    // Returns a reference to channel_info_t at specified absolute index.
    channel_info_t& absolute_channel_info(int32_t absolute_channel_index);

    // Resets channel_info_t at specified absolute index.
    void reset_absolute_channel_info(int32_t absolute_channel_index);

    // SDL mixer needs distance and angle to simulate 3D sound. This function calculates these in relation to specified position
    // and cached listener info.
    void calculate_3d_parameters(glm::vec3 position, int32_t& angle_out, uint8_t& distance_out);

    // Audio frame stack. Each game state gets one.
    std::vector<audio_frame_t> audio_frame_stack_m;

    // Cached listener info.
    glm::vec3 listener_position_m;
    glm::quat listener_orientation_m;

    // Timer for timestamp caching and checks.
    timer_t timer_m;
};

} // namespace mau
