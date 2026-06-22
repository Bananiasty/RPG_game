#pragma once
#include <iostream>
#include <vector>
#include <map>

class enemy;
class player;
struct inventory;

enum class item_type { ARMOR, USABLE };
class item
{
protected:
	void add_player_defense(player* c, int val);
	void add_player_health(player* c, int val);
	void add_player_damage(player* c, int val);
	void add_player_block_chance(player* c, int val);
	void add_player_crit_chance(player* c, int val);
	void add_player_dodge_chance(player* c, int val);

	void reduce_player_defense(player* c, int val);
	void reduce_player_health(player* c, int val);
	void reduce_player_damage(player* c, int val);
	void reduce_player_block_chance(player* c, int val);
	void reduce_player_crit_chance(player* c, int val);
	void reduce_player_dodge_chance(player* c, int val);
	void reduce_player_mana(player* c, int val);

	void restore_player_health(player* c, int val);
	void restore_player_mana(player* c, int val);

	//int get_cost();

private:
	std::string name;
	std::string rarity;
	int cost = 0;

public:
	item(std::string n, std::string r, int c) :name(n), rarity(r), cost(c) {};
	virtual item* clone() const = 0;
	virtual ~item() {};
	virtual item_type get_type() = 0;
	virtual void use(player* c, int index) {};
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
	armor(std::string n, std::string r, int c, int def) :item(n, r, c), defense_stat(def) {};
	item_type get_type() override;
	void use(player* player, int index) override;
	virtual bool is_equipped();
	
};
class helm :public armor
{
private:
	int block_chance;

public:
	helm(std::string n, std::string r, int c, int def, int b_ch) :armor(n, r, c, def), block_chance(b_ch) {};
	item* clone() const override;	
	void use(player* player, int index) override;
	bool is_equipped() override;
};
class vest :public armor
{
private:
	int add_health;
	bool vest_slot = false;

public:
	vest(std::string n, std::string r, int c, int def, int a_h) :armor(n, r, c, def), add_health(a_h) {};
	item* clone() const override;
	void use(player* player, int index) override;
	bool is_equipped() override;
};
class gauntlets :public armor
{
private:
	int crit_chance_mod;
	bool gauntlets_slot = false;

public:
	gauntlets(std::string n, std::string r, int c, int def, int c_ch_m) :armor(n, r, c, def), crit_chance_mod(c_ch_m) {};
	item* clone() const override;
	void use(player* player, int index) override;
	bool is_equipped() override;
};
class boots :public armor
{
private:
	int dodge_chance_mod;
	bool boots_slot = false;

public:
	boots(std::string n, std::string r, int c, int def, int d_ch_m) :armor(n, r, c, def), dodge_chance_mod(d_ch_m) {};
	item* clone() const override;
	void use(player* player, int index) override;
	bool is_equipped() override;
};

class weapon:public armor
{
private:
	int damage_stat;
	bool weapon_slot=false;

public:
	weapon(std::string n, std::string r, int c, int def, int dmg) :armor(n, r, c, def), damage_stat(dmg) {};
	item* clone() const override;
	void use(player* player, int index) override;
	bool is_equipped() override;
};

class usable:public item
{
public:
	usable(std::string n, std::string r, int c) :item(n, r, c) {};
	virtual item* clone() const = 0;
	item_type get_type() override;
	virtual void use(player* player, int index) override;
};
class scroll:public usable
{
private:
	int mana_cost;
	int damage;
public:
	scroll(std::string n, std::string r, int c, int m_c, int d) :usable(n, r, c), mana_cost(m_c), damage(d) {};
	item* clone() const override;
	void use(player* player, int index) override;
};
class potion:public usable
{
public:
	potion(std::string n, std::string r, int c) : usable(n, r, c) {};
	virtual item* clone() const = 0; 
	void use(player* player, int index) override=0;
};
class health_potion:public potion
{
private:
	int restore_health;
public:
	health_potion(std::string n, std::string r, int c, int r_h) :potion(n, r, c), restore_health(r_h) {};
	item* clone() const override;
	void use(player* player, int index) override;
};
class mana_potion:public potion
{
private:
	int restore_mana;
public:
	mana_potion(std::string n, std::string r, int c, int r_m) :potion(n, r, c), restore_mana(r_m) {};
	item* clone() const override;
	void use(player* player, int index) override;
};




