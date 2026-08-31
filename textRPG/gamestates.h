#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <concepts>
#include <string>
#include <memory>
#include "raylib.h"
#include "struct.h"
#include "character.h"


class gamestate
{
public:
	virtual int update_state() = 0;
	virtual ~gamestate() {}
	virtual void draw() = 0;

	static std::vector<std::string> gameLogs;
	std::vector<floating_text> active_texts;
	void spawn_floating_text(Vector3 pos, const std::string& text, bool is_crit = false, bool is_bleed = false);
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

	std::unique_ptr<object> create_world_object(const ObjectSpawnInfo& info);
	void spawn_object(const ObjectSpawnInfo& info);

	void apply_collision(Vector3 stara_pos);
	void apply_pathfinding(enemy* e);
	std::vector<GridPosition> find_path(int startX, int startZ, int targetX, int targetZ);
	bool is_walkable(int x, int y);
	bool is_walkable_subgrid(int gridX, int gridZ);
	bool is_near_wall(int gridX, int gridZ);

	void update_enemies();

	void draw() override;
	int update_state() override;

	void player_stats_init(player* c);
	void enemies_init();
	void world_map_init();
	void loot_init();
	std::vector<std::unique_ptr<item>> rand_loot(enemy* target_enemy, int slots_number);

	Node* get_node(int room_id);

	Vector3 set_enemy_pos(int enemy_id, int room_id, int floor_id);
	Vector3 set_player_pos( int room_id, int floor_id);

	enemy* get_current_enemy();
	enemy* get_enemy_by_id(int id);
	Node* get_room_by_id(int room_id, int floor_id);
	int get_enemy_dif();

	

	void delete_enemy(enemy* target);

	void event_check();

	

	void game_over() {};


	exploration(player& p);
	~exploration();
	
	void save_game() override;
	bool load_game() override;

};


class battle : public gamestate
{

	
public:
	player& p_ref;
	enemy& e_ref;

	battle(player& p, enemy& e);

	exploration* exp;

	bool showMap = false;
	bool showInventory = false;
	bool waiting_for_enemy = false;
	bool click_cooldown = false;
	bool attack_clicked = false;
	bool guard_clicked = false;



	float enemy_cooldown = 0.0;
	float player_cooldown = 0.0;


	void draw() override;

	void initiate_fight_view();
	int player_turn();
	int enemy_turn();
	

	


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
	gamestate* previous_state;

public:
	exploration* exp;
	battle* fight;
	bool equipment_tab = false;
	bool food_tab = false;
	bool books_tab = false;
	bool items_tab = false;

	inventory_state(player& p, gamestate* back_to);

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