#include <filesystem>
#include <fstream>
#include "gamestates.h"



void exploration::save_game()
{
    std::filesystem::path folder_path = "saves";
    std::filesystem::path file_path = folder_path / "save.txt";

    std::filesystem::create_directory(folder_path);

    std::ofstream plik(file_path);

    if (plik.is_open())
    {
        plik << current_floor_id << " " << current_node_id << "\n\n";

        for (size_t floor_idx = 0; floor_idx < floors.size(); ++floor_idx)
        {
            for (const auto& [id, node] : floors[floor_idx].world_map)
            {
                plik << floor_idx << " " << id << " " << node.discovered << "\n";
            }
        }

        plik.close();
    }
}

bool exploration::load_game()
{
    std::filesystem::path file_path = "saves/save.txt";

    if (!std::filesystem::exists(file_path))
    {
        return false;
    }

    std::ifstream plik(file_path);

    if (plik.is_open())
    {
        plik >> current_floor_id >> current_node_id;

        int floor_idx;
        int id;
        bool status_discovered;

        while (plik >> floor_idx >> id >> status_discovered)
        {
            if (floor_idx >= 0 && floor_idx < static_cast<int>(floors.size()))
            {
                auto& floor_map = floors[floor_idx].world_map;
                if (floor_map.find(id) != floor_map.end())
                {
                    floor_map[id].discovered = status_discovered;
                }
            }
        }

        plik.close();
        return true;
    }

    return false;
}

