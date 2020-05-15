#include "mau/audio/audio.hpp"

#include "mau/base/engine_context.hpp"
#include "mau/math/algorithms.hpp"

namespace mau {

inline static constexpr float LISTENER_RANGE = 32;

audio_t::audio_t(engine_context_t& engine) : module_t(engine, "audio")
{
    clear();
}
void audio_t::update_listener(glm::vec3 position, glm::quat orientation)
{
    listener_position_m    = position;
    listener_orientation_m = orientation;

    auto& audio_frame = audio_frame_stack_m.back();
    for(size_t i = 0; i < audio_frame.channel_info.size(); ++i)
    {
        auto& channel_info = audio_frame.channel_info[i];
        if(channel_info.positional)
        {
            int32_t angle;
            uint8_t distance;
            calculate_3d_parameters(channel_info.position, angle, distance);

            Mix_SetPosition(absolute_channel(i), angle, distance);
        }
    }
}
void audio_t::play_clip(audio_clip_handle_t audio_clip)
{
    if (!check_timestamp(audio_clip))
        return;

    int32_t free_channel = find_free_channel();
    if(free_channel == -1)
        return;

    reset_absolute_channel_info(free_channel);
    Mix_PlayChannel(free_channel, audio_clip->sdl_handle(), 0);

    audio_frame_stack_m.back().timestamps[audio_clip] = timer_m.milliseconds();
}
void audio_t::play_clip(audio_clip_handle_t audio_clip, glm::vec3 position)
{
    if (!check_timestamp(audio_clip))
        return;

    int32_t free_channel = find_free_channel();
    if(free_channel == -1)
        return;

    reset_absolute_channel_info(free_channel);

    channel_info_t& channel_info = absolute_channel_info(free_channel);
    channel_info.positional      = true;
    channel_info.position        = position;

    int32_t angle;
    uint8_t distance;
    calculate_3d_parameters(position, angle, distance);

    Mix_SetPosition(free_channel, angle, distance);
    Mix_PlayChannel(free_channel, audio_clip->sdl_handle(), 0);

    audio_frame_stack_m.back().timestamps[audio_clip] = timer_m.milliseconds();
}
void audio_t::clear()
{
    audio_frame_stack_m.clear();
    Mix_AllocateChannels(0);
}
void audio_t::push()
{
    if(audio_frame_stack_m.size() > 0)
    {
        const int32_t range_start = (int32_t)(audio_frame_stack_m.size() - 1) * channels_per_audio_frame_k;
        const int32_t range_end   = range_start + channels_per_audio_frame_k;
        for(int32_t i = range_start; i < range_end; ++i)
        {
            Mix_Pause(i);
        }
    }

    audio_frame_stack_m.push_back({});
    Mix_AllocateChannels((int32_t)audio_frame_stack_m.size() * channels_per_audio_frame_k);
}
void audio_t::pop()
{
    audio_frame_stack_m.pop_back();
    Mix_AllocateChannels((int32_t)audio_frame_stack_m.size() * channels_per_audio_frame_k);

    if(audio_frame_stack_m.size() == 0)
        return;

    const int32_t range_start = (int32_t)(audio_frame_stack_m.size() - 1) * channels_per_audio_frame_k;
    const int32_t range_end   = range_start + channels_per_audio_frame_k;
    for(int32_t i = range_start; i < range_end; ++i)
    {
        Mix_Resume(i);
    }
}

int32_t audio_t::relative_channel(int32_t pChannel)
{
    assert(audio_frame_stack_m.size() > 0);

    return pChannel - (int32_t)(audio_frame_stack_m.size() - 1) * channels_per_audio_frame_k;
}

int32_t audio_t::absolute_channel(int32_t pIndexInChannelFrame)
{
    assert(audio_frame_stack_m.size() > 0);

    return (int32_t)(audio_frame_stack_m.size() - 1) * channels_per_audio_frame_k + pIndexInChannelFrame;
}

int32_t audio_t::find_free_channel()
{
    assert(audio_frame_stack_m.size() > 0);

    const int32_t range_start = (int32_t)(audio_frame_stack_m.size() - 1) * channels_per_audio_frame_k;
    const int32_t range_end   = range_start + channels_per_audio_frame_k;
    for(int32_t i = range_start; i < range_end; ++i)
    {
        if(!Mix_Playing(i))
            return i;
    }
    return -1;
}

bool audio_t::check_timestamp(audio_clip_handle_t audio_clip)
{
    auto& timestamps = audio_frame_stack_m.back().timestamps;
    auto it = timestamps.find(audio_clip);
    if (it != timestamps.end())
    {
        uint64_t timestamp = it->second;
        if (timer_m.milliseconds() - timestamp < 100)
            return false;
    }
    return true;
}

channel_info_t& audio_t::absolute_channel_info(int32_t absolute_channel_index)
{
    assert(audio_frame_stack_m.size() > 0);

    return audio_frame_stack_m.back().channel_info[relative_channel(absolute_channel_index)];
}

void audio_t::reset_absolute_channel_info(int32_t absolute_channel_index)
{
    auto& audioFrame   = audio_frame_stack_m.back();
    auto& channel_info = audioFrame.channel_info[relative_channel(absolute_channel_index)];

    channel_info = channel_info_t{};
    Mix_UnregisterAllEffects(absolute_channel_index);
}

void audio_t::calculate_3d_parameters(glm::vec3 position, int32_t& angle_out, uint8_t& distance_out)
{
    glm::vec3 direction  = listener_orientation_m * glm::vec4(0, 0, -1, 0);
    glm::vec3 up         = listener_orientation_m * glm::vec4(0, 1, 0, 0);
    glm::vec3 difference = glm::normalize(position - listener_position_m);

    float angle    = glm::degrees(glm::orientedAngle(direction, difference, up));
    float distance = glm::length(position - listener_position_m);

    // Our rotation is actually opposite of what SDL mixer expects.
    angle_out    = 360 - math::signed_mod((int32_t)angle, 360);
    distance_out = (uint8_t)(math::clamp(distance / LISTENER_RANGE) * std::numeric_limits<uint8_t>::max());
}

} // namespace mau
