#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <concepts>
#include <string>
#include <memory>
#include "raylib.h"
#include "struct.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "inventory/Item.h"
#include "world/Pathfinding.h"


class gamestate
{
public:
	virtual int update_state() = 0;
	virtual ~gamestate() {}
	virtual void draw() = 0;

	static std::vector<std::string> gameLogs;
	std::vector<floating_text> active_texts;
	void spawn_floating_text(Vector3 pos, const std::string& text, bool is_crit = false, bool is_bleed = false, Color custom_color = BLANK);
	void update_and_draw_floating_texts(Camera3D current_camera);

	bool showMenu = false;

	bool can_save_game = false;
	
	void draw_menu();

	virtual void save_game() {}
	virtual bool load_game() { return false; }
};

class exploration : public gamestate
{
private:
	

	void add_Node(const NodeConfig& config);
	

	
public:
	Camera3D camera;

	bool is_hovering_interactive = false;
	Vector3 hovered_point = { 0.0f, 0.0f, 0.0f };

	int dlugosc;
	int szerokosc;


	std::vector<dungeon_floor> floors;

	int current_floor_id = 0;

	std::vector<item*>item_pool;
	std::vector<enemy*> enemy_pool;

	Event* active_ui_event = nullptr;

	player& bohater;
	int current_node_id;

	bool showInventory = false;
	bool showMap = false;

	
	void generate_floor(int floor_id);
	void change_floor(int floor_id);
	int find_deepest_room(int floor_id, int start_room_id = 1);
	void ensure_trapdoor_in_deepest_room(int floor_id);

	std::unique_ptr<object> create_world_object(const ObjectSpawnInfo& info);
	void spawn_object(ObjectSpawnInfo info);

	void apply_collision(Vector3 stara_pos);
	void apply_pathfinding(enemy* e);
	std::vector<GridPosition> find_path(int startX, int startZ, int targetX, int targetZ, enemy* current_enemy = nullptr, bool ignore_other_enemies = false);
	bool is_walkable(int x, int y);
	bool is_walkable_subgrid(int gridX, int gridZ);
	bool is_near_wall(int gridX, int gridZ);
	bool has_line_of_sight(Vector3 start, Vector3 end);

	void update_enemies();

	void draw() override;
	int update_state() override;

	void player_stats_init(player* c);
	void enemies_init();
	void world_map_init();
	void loot_init();
	std::vector<std::unique_ptr<item>> rand_loot(enemy* target_enemy, int slots_number);

	Node* get_node(int room_id);

	Vector3 set_enemy_pos(int enemy_id, int room_id, int floor_id, float wall_margin = 2.0f, float corridor_depth = 6.0f);
	Vector3 set_player_pos( int room_id, int floor_id);

	enemy* get_current_enemy();
	enemy* get_enemy_by_id(int id);
	Node* get_room_by_id(int room_id, int floor_id);
	int get_enemy_dif();

	

	void delete_enemy(enemy* target);

	void event_check();

	void spawn_world_item(item* it, Vector3 pos);
	void spawn_loot_bag(Vector3 pos, const std::vector<item*>& items);

	void game_over() {};


	exploration(player& p);
	~exploration();
	
	void save_game() override;
	bool load_game() override;

};


struct ReinforcementEnemy
{
	enemy* enemy_ptr = nullptr;
	int rounds_to_arrive = 0;
	std::vector<GridPosition> path;
	int current_path_step = 0;
	int steps_per_round = 4;
	bool is_approaching_from_left = false;
};

class battle : public gamestate
{
public:
	player& p_ref;
	enemy& e_ref;

	battle(player& p, enemy& e);
	virtual ~battle();

	exploration* exp = nullptr;

	std::vector<enemy*> active_enemies;
	std::vector<enemy*> queued_enemies;
	std::vector<ReinforcementEnemy> incoming_reinforcements;
	std::vector<enemy*> pending_enemy_actions;
	std::vector<enemy*> unconfirmed_reinforcements;
	bool turn_start_effects_processed = false;

	Vector3 combat_facing_dir = { 0.0f, 0.0f, 1.0f };
	Vector3 combat_enemy_center = { 0.0f, 0.0f, 0.0f };
	Vector3 combat_player_pos = { 0.0f, 0.0f, 0.0f };

	bool showMap = false;
	bool showInventory = false;
	bool waiting_for_enemy = false;
	bool click_cooldown = false;
	bool attack_clicked = false;
	bool guard_clicked = false;

	float enemy_cooldown = 0.0;
	float player_cooldown = 0.0;

	enum class EnemyAttackPhase {
		IDLE,
		INIT_NEXT_ENEMY,
		FLASH_LIMB,
		RECOVERY_PAUSE
	};

	EnemyAttackPhase enemy_phase = EnemyAttackPhase::INIT_NEXT_ENEMY;
	enemy* current_turn_enemy = nullptr;
	std::vector<BodyPart> current_enemy_attacks;
	size_t current_attack_index = 0;
	float attack_phase_timer = 0.0f;

	void draw() override;

	void initiate_fight_view();
	int player_turn();
	int enemy_turn();

	void scan_reinforcements();
	void tick_reinforcements();
	void join_battle(enemy* e, bool comes_from_left = false);
	int get_max_combat_capacity() const;
	bool is_tile_walkable_at(Vector3 world_pos) const;
	bool is_player_in_room() const;
	bool is_player_in_narrow_corridor() const;
	bool is_narrow_corridor_at_fight() const;
	void reposition_active_enemies();
	void handle_dead_enemies();
	bool is_fight_finished() const;

	void turn_camera_left();
	void turn_camera_right();
	void set_combat_facing(Vector3 new_dir);
	bool has_enemy_on_left() const;
	bool has_enemy_on_right() const;
	bool is_direction_valid(Vector3 dir) const;
	Vector3 get_next_turn_left_dir() const;
	Vector3 get_next_turn_right_dir() const;
	bool can_turn_left() const;
	bool can_turn_right() const;

	enemy* get_current_target();
	enemy& get_enemy();
	int get_player_hp();
	int get_enemy_hp();
	int get_enemy_max_hp();

	std::string get_enemy_name();
	player* get_player_ptr() { return &p_ref; }

	int update_state() override;

};

class inventory_state : public gamestate
{
private: 
	player& p_ref;
	exploration& world;
	gamestate* previous_state;
	

public:
	exploration* exp;
	battle* fight;

	bool equipment_tab = false;
	bool food_tab = false;
	bool books_tab = false;
	bool items_tab = false;

	std::vector<item*> pending_drops;

	
	inventory_state(player& p, exploration& w, gamestate* back_to);



	void draw() override;
	int update_state() override;

	gamestate* get_previous_state() { return previous_state; }
};

class map_state : public gamestate
{
private: 
	
	gamestate* previous_state;
	
	

public:
	exploration* exp;
	battle* fight;

	map_state(gamestate* back_to);
	void draw() override;
	int update_state() override;
	gamestate* get_previous_state() { return previous_state; }

};


class Event
{
protected:
	exploration* exp;

public:
	explicit Event(exploration* e) : exp(e) {}
	virtual ~Event() = default;

	virtual void update() = 0;
	virtual void draw_event(exploration* exp) = 0;
};

class loot_event : public Event
{
public:
	player& p_ref;
	object* target_container = nullptr;
	bool is_active = true;

	loot_event(exploration* e, player& p, object* container) : Event(e), p_ref(p), target_container(container) {}

	void collect_item(size_t index);
	void collect_all();
	void close_event();

	void update() override;
	void draw_event(exploration* exp) override;
};











//KONCEPT
template <typename T>
concept Describable = requires(T object) {
	{ object.get_intro_text() } -> std::same_as<std::string>;
};

template <Describable T>
void log_object_intro(T& object)
{
	gamestate::gameLogs.push_back(object.get_intro_text());
}
