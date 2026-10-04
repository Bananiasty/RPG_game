#include "gamestates.h"
#include "character.h"



exploration::exploration(player& p) : bohater(p)
{
    active_ui_event = nullptr;
    current_node_id = 1;
    current_floor_id = 0;
    const int total_floors = 2;
    floors.clear();
    floors.reserve(total_floors);
    for (int i = 0; i < total_floors; ++i)
    {
        floors.emplace_back();
    }
    dlugosc = 200;
    szerokosc = 200;

    for (auto& floor : floors)
    {
        floor.dungeon.assign(szerokosc, std::vector<int>(dlugosc, 2));
    }

    world_map_init();
    generate_floor(current_floor_id);

    player_stats_init(&bohater);


    this->camera = { 0 };
    this->camera.position = Vector3{ bohater.position.x, 1.5f, bohater.position.z };
    this->camera.target = Vector3{ bohater.position.x, 1.5f, bohater.position.z + 5.0f };
    this->camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    this->camera.fovy = 80.0f;
    this->camera.projection = CAMERA_PERSPECTIVE;


}
exploration::~exploration()
{
    if (active_ui_event != nullptr)
    {
        delete active_ui_event;
        active_ui_event = nullptr;
    }

    for (auto& floor : floors)
    {
        floor.world_objects.clear();

        for (enemy* e : floor.active_enemies)
        {
            delete e;
        }
        floor.active_enemies.clear();
    }
}
void exploration::event_check()
{
    // 1. Sprawdzenie aktywnego UI eventu (np. otwarte okno łupu)
    if (active_ui_event != nullptr)
    {
        loot_event* current_loot = dynamic_cast<loot_event*>(active_ui_event);
        if (current_loot != nullptr && !current_loot->is_active)
        {
            delete active_ui_event;
            active_ui_event = nullptr;
        }
        else
        {
            is_hovering_interactive = false;
            return;
        }
    }

    is_hovering_interactive = false;

    Vector3 player_pos = camera.position;
    auto& current_objects_list = floors[current_floor_id].world_objects;

    Vector2 ray_origin_pos = IsCursorHidden()
        ? Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f }: GetMousePosition();

    Ray ray = GetScreenToWorldRay(ray_origin_pos, camera);

    object* targeted_obj = nullptr;
    float closest_hit_distance = 999999.0f;

    for (const auto& obj_ptr : current_objects_list)
    {
        if (!obj_ptr || obj_ptr->is_destroyed)
        {
            continue;
        }

        if (auto* drop_ptr = dynamic_cast<loot_object*>(obj_ptr.get()))
        {
            if (drop_ptr->drop_loot.empty())
            {
                continue;
            }
        }

        float max_dist_sqr = (dynamic_cast<trapdoor*>(obj_ptr.get()) != nullptr) ? 14.0f : 7.25f;
        if (Vector3DistanceSqr(player_pos, obj_ptr->position) > max_dist_sqr)
        {
            continue;
        }

        BoundingBox box = obj_ptr->get_interaction_box();

        RayCollision collision = GetRayCollisionBox(ray, box);

        if (collision.hit && collision.distance < closest_hit_distance)
        {
            closest_hit_distance = collision.distance;
            targeted_obj = obj_ptr.get();
            hovered_point = collision.point;
            is_hovering_interactive = true;
        }
    }

    if (targeted_obj != nullptr && IsKeyPressed(KEY_E))
    {
        targeted_obj->interact(*this, bohater);
    }

    std::erase_if(current_objects_list, [](const auto& ptr)
        {
            return ptr && ptr->is_destroyed;
        });
}


Vector3 exploration::set_enemy_pos(int enemy_id, int room_id, int floor_id)
{
    if (enemy_id == -1)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    enemy* enemy_ptr = this->get_enemy_by_id(enemy_id);
    if (enemy_ptr == nullptr)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    Node* room_ptr = this->get_room_by_id(room_id, floor_id);
    if (room_ptr == nullptr)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    float enemy_y = enemy_ptr->get_position().y;
    float centerX = room_ptr->room_x + (room_ptr->room_width / 2.0f);
    float centerZ = room_ptr->room_y + (room_ptr->room_length / 2.0f);

    return Vector3{ centerX * 2.0f, enemy_y, centerZ * 2.0f };
}

Vector3 exploration::set_player_pos(int room_id, int floor_id)
{
    Node* room_ptr = this->get_room_by_id(room_id, floor_id);
    if (room_ptr == nullptr)
    {
        return { 0.0f, 1.5f, 0.0f };
    }

    float player_y = 1.5f;
    float centerX = room_ptr->room_x + (room_ptr->room_width / 2.0f);
    float centerZ = room_ptr->room_y + (room_ptr->room_length / 2.0f);

    return Vector3{ centerX * 2.0f, player_y, centerZ * 2.0f };
}

void exploration::spawn_world_item(item* it, Vector3 pos)
{
    if (it == nullptr)
    {
        return;
    }
    const Model* model = nullptr;

    std::unique_ptr<item> item_ptr(it);

    auto new_world_item = std::make_unique<world_item>(
        pos,
        model,
        std::move(item_ptr),
        1.0f,
        0.0f
    );
    floors[current_floor_id].world_objects.push_back(std::move(new_world_item));
}

void exploration::spawn_loot_bag(Vector3 pos, const std::vector<item*>& items)
{
    if (items.empty()) return;

    std::vector<std::unique_ptr<item>> container;
    container.reserve(items.size());

    for (item* it : items)
    {
        if (it != nullptr)
        {
            container.push_back(std::unique_ptr<item>(it));
        }
    }

    if (container.empty()) return;

    int slots = static_cast<int>(container.size());
    auto bag = std::make_unique<p_drop>(pos, nullptr, slots, std::move(container));

    floors[current_floor_id].world_objects.push_back(std::move(bag));
}