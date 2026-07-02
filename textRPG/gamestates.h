#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <concepts>
#include <string>
#include "raylib.h"
#include "struct.h"

class enemy;
class player;
struct chest;

class gamestate
{
public:
	virtual int update_state() = 0;
	virtual ~gamestate() {}
	virtual void draw() = 0;
	static std::vector<std::string> gameLogs;

	bool showMenu = false;

	bool can_save_game = false;

	
	void draw_menu();

	virtual void save_game() {}
	virtual bool load_game() { return false; }
};

class exploration : public gamestate
{
private:
	

	void add_Node(int id, int left = -1, int right = -1, int previous = -1, bool disc=false, int posX=600, int posY=500, enemy* e = nullptr, chest* s_chest = nullptr, int rx=0, int rh=0, int r_width=0, int r_length=0);
	

	
public:
	Camera3D camera;

	int dlugosc;
	int szerokosc;

	std::map<int, Node> world_map;
	std::vector<std::vector<int>> dungeon;


	std::vector<item*>item_pool;
	std::vector<enemy*> enemy_pool;

	player& bohater;
	int current_node_id;

	bool showInventory = false;
	bool showMap = false;
	
	void generate_map_from_graph();
	void apply_collision(Vector3 stara_pos);

	void draw() override;
	int update_state() override;

	void player_stats_init(player* c);
	void enemies_init();
	void world_map_init();
	void loot_init();

	Node* get_current_node();
	enemy* get_current_enemy();
	int get_enemy_dif();

	int rand_chest_slots(enemy* e);
	chest* rand_loot(enemy* e);

	void delete_dead_enemies();

	void event_check();

	void move_left();
	void move_right();
	void move_back();

	void map_graph();

	void game_over() {};


	exploration(player& p);

	
	void save_game() override;
	bool load_game() override;

};

class battle : public gamestate
{
private:
	player& p_ref;
	enemy& e_ref;

	
public:

	battle(player& p, enemy& e);

	exploration* exp;

	bool showMap = false;
	bool showInventory = false;
	bool waiting_for_enemy = false;
	bool click_cooldown = false;
	bool attack_clicked = false;

	float enemy_cooldown = 0.0;
	float player_cooldown = 0.0;


	void draw() override;

	int player_attack();
	int enemy_turn();
	
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
	bool back_requested;
	

public:
	exploration* exp;
	battle* fight;
	bool equipment_tab = false;
	bool usables_tab = false;
	bool scrolls_tab = false;
	bool other_tab = false;

	inventory_state(player& p, gamestate* back_to);

	void draw() override;
	int update_state() override;
	void request_back() { back_requested = true; }

	gamestate* get_previous_state() { return previous_state; }
};

class map_state : public gamestate
{
private:
	
	gamestate* previous_state;
	
	

public:
	exploration* exp;
	battle* fight;

	bool back_requested;

	map_state(gamestate* back_to);
	void draw() override;
	int update_state() override;
	gamestate* get_previous_state() { return previous_state; }

};


class Event
{
protected:
	exploration* exp;
	bool can_save_game = false;

public:
	virtual ~Event() {}
	virtual void draw_event(exploration* exp) = 0;
};

class chest_drop : public Event
{

public:
	chest* chest_ptr;
	bool is_chest_open=false;
	void discard_chest();
	void collect_loot();
	
	chest_drop(player& p, chest* chest);

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