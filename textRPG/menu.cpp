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
        plik << current_node_id << "\n\n";

        for (auto const& [id, node] : world_map)
        {
            plik << id << " " << node.discovered << "\n";
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

        plik >> current_node_id;

        int id;
        bool status_discovered;

        while (plik >> id >> status_discovered)
        {
            if (world_map.find(id) != world_map.end())
            {
                world_map[id].discovered = status_discovered;
            }
        }

        plik.close();
        return true;
    }

    return false;
}

