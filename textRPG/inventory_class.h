#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "raylib.h"

class enemy;
class player;
struct inventory;

enum class item_type { ARMOR, ITEM, FOOD, BOOK};
class item
{
protected:
	void add_player_defense(player* c, int val);
	void add_player_health(player* c, int val);
	void add_player_damage(player* c, int val);
	void add_player_block_chance(player* c, int val);
	void add_player_crit_chance(player* c, int val);
	void add_player_dodge_chance(player* c, int val);
	void add_player_head_damage_reduction(player* c, int val);

	void reduce_player_defense(player* c, int val);
	void reduce_player_health(player* c, int val);
	void reduce_player_damage(player* c, int val);
	void reduce_player_block_chance(player* c, int val);
	void reduce_player_crit_chance(player* c, int val);
	void reduce_player_dodge_chance(player* c, int val);
	//void reduce_player_mana(player* c, int val);
	void reduce_player_head_damage_reduction(player* c, int val);

	void restore_player_health(player* c, int val);
	//void restore_player_mana(player* c, int val);


private:
	std::string name;
	std::string rarity;
	int cost = 0;


public:
	Texture2D* icon_texture;
	item(std::string n, std::string r, int c, Texture2D* icon) :name(n), rarity(r), cost(c), icon_texture(icon){};
	virtual item* clone() const = 0;
	virtual ~item() {};
	virtual item_type get_type() = 0;
	virtual void use(player* c) {};
	std::string get_name();
	virtual bool is_equipped() { return false; };
	
};
class armor:public item
{
protected:
	int defense_stat;
	bool item_equipped=false;
	virtual item* clone() const = 0;

public:
	armor(std::string n, std::string r, int c, Texture2D* icon, int def) :item(n, r, c, icon), defense_stat(def) {};
	item_type get_type() override;
	void use(player* player) override;
	virtual bool is_equipped();
	
};
class helm :public armor
{
private:
	int head_damage_reduced;

public:
	helm(std::string n, std::string r, int c, Texture2D* icon, int def, int h_dmg_red) :armor(n, r, c, icon, def), head_damage_reduced(h_dmg_red) {};
	item* clone() const override;	
	void use(player* player) override;
	bool is_equipped() override;
};
class vest :public armor
{
private:
	int add_health;
	bool vest_slot = false;

public:
	vest(std::string n, std::string r, int c, Texture2D* icon, int def, int a_h) :armor(n, r, c, icon, def), add_health(a_h) {};
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
};
class gauntlets :public armor
{
private:
	int crit_chance_mod;
	bool gauntlets_slot = false;

public:
	gauntlets(std::string n, std::string r, int c, Texture2D* icon, int def, int c_ch_m) :armor(n, r, c, icon, def), crit_chance_mod(c_ch_m) {};
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
};
class boots :public armor
{
private:
	int dodge_chance_mod;
	bool boots_slot = false;

public:
	boots(std::string n, std::string r, int c,Texture2D* icon, int def, int d_ch_m) :armor(n, r, c, icon, def), dodge_chance_mod(d_ch_m) {};
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
};

class weapon:public armor
{
private:
	int damage_stat;
	bool weapon_slot=false;

public:
	weapon(std::string n, std::string r, int c,Texture2D* icon, int def, int dmg) :armor(n, r, c, icon, def), damage_stat(dmg) {};
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
};

class shield :public armor
{
private:
	int block_chance;
	bool shield_slot = false;
public:
	shield(std::string n, std::string r, int c, Texture2D* icon, int def, int block_ch) :armor(n, r, c, icon, def), block_chance(block_ch) {};
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
};

class usable:public item
{
public:
	usable(std::string n, std::string r,int c, Texture2D* icon) :item(n, r, c, icon) {};
	virtual item* clone() const = 0;
	virtual item_type get_type() override = 0;
	virtual void use(player* player) override;
};
class other_item :public usable
{
public:
	other_item(std::string n, std::string r, int c, Texture2D* icon) :usable(n, r, c, icon) {};
	item* clone() const override;
	virtual item_type get_type() override;
	virtual void use(player* player) override;
};
class scroll:public usable
{
protected:
	bool scroll_used = false;

public:
	scroll(std::string n, std::string r, int c, Texture2D* icon) :usable(n, r, c, icon){};
	item_type get_type() override;
	virtual item* clone() const = 0;
	virtual void use(player* player) override = 0;
};
class combat_scroll : public scroll
{
private: 
	float damage;
	Texture2D* animation_texture;
	int frame_count;
	float frame_time;
public:
	combat_scroll(std::string n, std::string r, int c, Texture2D* icon, float dmg, Texture2D* tex, int frames, float speed) :scroll(n, r, c, icon) , damage(dmg), animation_texture(tex), frame_count(frames), frame_time(speed) {};
	void use(player* player) override;
	item* clone() const override;
};
class food:public usable
{
public:
	food(std::string n, std::string r, int c, Texture2D* icon) : usable(n, r, c,icon) {};
	item_type get_type() override;
	virtual item* clone() const override;
	void use(player* player) override;
};
class health_potion:public food
{
private:
	int restore_health;
public:
	health_potion(std::string n, std::string r, int c, Texture2D* icon, int r_h) :food(n, r, c, icon), restore_health(r_h) {};
	item* clone() const override;
	void use(player* player) override;
};
/*class mana_potion:public food
{
private:
	int restore_mana;
public:
	mana_potion(std::string n, std::string r, int c, Texture2D* icon, int r_m) :food(n, r, c, icon), restore_mana(r_m) {};
	item* clone() const override;
	void use(player* player) override;
};*/




