#include "worship/menu/menu.hpp"

namespace mau {

menu_t::menu_t(engine_context_t& engine, font_handle_t font, texture_handle_t indicator, std::string_view title) :
    engine_m(engine), font_m(font), indicator_m(indicator), title_m(title)
{
}
void menu_t::add_function_entry(std::string_view text, std::function<void()> callback)
{
    auto entry      = std::make_unique<function_entry_t>();
    entry->text     = text;
    entry->callback = callback;
    entries_m.push_back(std::move(entry));
}
void menu_t::add_option_entry(std::string_view text, std::initializer_list<std::string_view> options, uint32_t selected_option)
{
    auto entry             = std::make_unique<option_entry_t>();
    entry->text            = text;
    entry->selected_option = selected_option;
    entry->previous_option = selected_option;
    entry->options         = options;
    entries_m.push_back(std::move(entry));
}

menu_t::option_entry_t * menu_t::get_option_entry(std::string_view text)
{
    for (auto& entry : entries_m)
    {
        option_entry_t* option = dynamic_cast<option_entry_t*>(entry.get());
        if (option && option->text == text)
            return option;
    }
    return nullptr;
}

void menu_t::reset_options()
{
    for (auto& entry : entries_m)
    {
        option_entry_t* option = dynamic_cast<option_entry_t*>(entry.get());
        if (option)
            option->selected_option = option->previous_option;
    }
}

void menu_t::select_next()
{
    ++current_entry_m;
    if((size_t)current_entry_m >= entries_m.size())
        current_entry_m = 0;
}

void menu_t::select_previous()
{
    --current_entry_m;
    if(current_entry_m < 0)
        current_entry_m = (int32_t)entries_m.size() - 1;
}

void menu_t::execute_entry()
{
    entries_m[current_entry_m]->execute();
}

void menu_t::render(float delta_time)
{
    auto& renderer = engine_m.renderer();

    float longest_entry_size = 0;
    for(auto& entry: entries_m)
    {
        float size = entry->length() * font_m->character_size.x;

        if(size > longest_entry_size)
            longest_entry_size = size;
    }

    glm::vec2 menu_position{engine_m.viewport_size().x / 2.0f - longest_entry_size / 2.0f,
                            engine_m.viewport_size().y / 2.0f -
                                ((int32_t)entries_m.size() - 2) * font_m->character_size.y / 2.0f};
    glm::vec2 title_position{engine_m.viewport_size().x / 2.0f - title_m.length() * font_m->character_size.x / 2.0f,
                             menu_position.y - font_m->character_size.y * 2};

    // Scope vertex batch.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};

        renderer.batch_string(batch, font_m, title_position, title_m, glm::vec4{0.65, 0, 0, 1});

        for(size_t entry_index = 0; entry_index < entries_m.size(); ++entry_index)
        {
            const auto& entry  = entries_m[entry_index];
            const auto& values = entry->values();

            renderer.batch_string(batch,
                                  font_m,
                                  menu_position + glm::vec2{0, font_m->character_size.y * entry_index},
                                  values.at(0),
                                  glm::vec4{0.65, 0, 0, 1});

            if(values.size() > 1)
            {
                std::string_view value  = values.at(1);
                float            offset = longest_entry_size - value.length() * font_m->character_size.x;

                renderer.batch_string(batch,
                                      font_m,
                                      menu_position + glm::vec2{offset, font_m->character_size.y * entry_index},
                                      value,
                                      glm::vec4{0.65, 0, 0, 1});
            }
        }
    }

    renderer.bind_texture(indicator_m, 0);
    // Scope vertex batch.
    {
        vertex_batch_t batch{renderer, vertex_primitive_t::triangle_k};
        renderer.batch_sprite(batch,
                              menu_position +
                                  glm::vec2{-font_m->character_size.x * 1.2f, font_m->character_size.y * current_entry_m},
                              font_m->character_size,
                              glm::vec4(1, 1, 1, 1));
    }
}

std::vector<std::string_view> menu_t::entry_t::values()
{
    return {text};
}

size_t menu_t::entry_t::length()
{
    return text.length();
}

void menu_t::option_entry_t::execute()
{
    selected_option = (selected_option + 1) % options.size();
}

std::vector<std::string_view> menu_t::option_entry_t::values()
{
    return {text, options.at(selected_option)};
}

size_t menu_t::option_entry_t::length()
{
    auto it = std::max_element(
        options.begin(), options.end(), [](std::string_view a, std::string_view b) { return a.size() < b.size(); });

    static constexpr int space_between_keyword_option_k = 4;

    return text.length() + space_between_keyword_option_k + it->length();
}

void menu_t::function_entry_t::execute()
{
    callback();
}

} // namespace mau
