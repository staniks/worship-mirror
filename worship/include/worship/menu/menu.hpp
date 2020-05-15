#pragma once

#include <mau/base/engine_context.hpp>
#include <mau/base/types.hpp>
#include <mau/rendering/font.hpp>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace mau {

class menu_t : non_copyable_t, non_movable_t
{
public:
    struct entry_t
    {
        virtual ~entry_t() = default;

        std::string_view text;

        virtual void                          execute() = 0;
        virtual std::vector<std::string_view> values();
        virtual size_t                        length();
    };

    struct function_entry_t : public entry_t
    {
        std::function<void()> callback;

        void execute() override;
    };

    struct option_entry_t : public entry_t
    {
        uint32_t                      previous_option{0}; // Value to reset to if canceled.
        uint32_t                      selected_option{0};
        std::vector<std::string_view> options;

        void                          execute() override;
        std::vector<std::string_view> values() override;
        size_t                        length() override;
    };
    menu_t(engine_context_t& engine, font_handle_t font, texture_handle_t indicator, std::string_view title);

    void add_function_entry(std::string_view text, std::function<void()> callback);
    void add_option_entry(std::string_view text, std::initializer_list<std::string_view> options, uint32_t selected_option);

    option_entry_t* get_option_entry(std::string_view text);

    void reset_options();

    void select_next();
    void select_previous();
    void execute_entry();

    void render(float delta_time);

private:
    engine_context_t&                     engine_m;
    font_handle_t                         font_m;
    texture_handle_t                      indicator_m;
    std::string                           title_m;
    std::vector<std::unique_ptr<entry_t>> entries_m;
    int32_t                               current_entry_m{0};
};

} // namespace mau
