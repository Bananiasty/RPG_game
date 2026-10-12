#pragma once
#include "inventory/Item.h"

class armor : public item
{
protected:
	int defense_stat;
	bool item_equipped = false;
	virtual item* clone() const = 0;

public:
	int get_defense_stat() const override { return defense_stat; }
	armor(std::string n, std::string r, int c, Texture2D* icon, int def) : item(n, r, c, icon), defense_stat(def) {}
	item_type get_type() override;
	void use(player* player) override;
	virtual bool is_equipped();
};

class helm : public armor
{
private:
	int head_damage_reduced;

public:
	helm(std::string n, std::string r, int c, Texture2D* icon, int def, int h_dmg_red) : armor(n, r, c, icon, def), head_damage_reduced(h_dmg_red) {}
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
	int get_head_damage_reduced() const { return head_damage_reduced; }
};

class vest : public armor
{
private:
	int add_health;
	bool vest_slot = false;

public:
	vest(std::string n, std::string r, int c, Texture2D* icon, int def, int a_h) : armor(n, r, c, icon, def), add_health(a_h) {}
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
	int get_add_health() const { return add_health; }
};

class gauntlets : public armor
{
private:
	int crit_chance_mod;
	bool gauntlets_slot = false;

public:
	gauntlets(std::string n, std::string r, int c, Texture2D* icon, int def, int c_ch_m) : armor(n, r, c, icon, def), crit_chance_mod(c_ch_m) {}
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
	int get_crit_chance_mod() const { return crit_chance_mod; }
};

class boots : public armor
{
private:
	int dodge_chance_mod;
	bool boots_slot = false;

public:
	boots(std::string n, std::string r, int c, Texture2D* icon, int def, int d_ch_m) : armor(n, r, c, icon, def), dodge_chance_mod(d_ch_m) {}
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
	int get_dodge_chance_mod() const { return dodge_chance_mod; }
};

class weapon : public armor
{
private:
	BodyPart equip_slot = BodyPart::RIGHT_ARM;
	int damage_stat = 0;
	bool weapon_slot = false;

public:
	weapon(std::string n, std::string r, int c, Texture2D* icon, int def, int dmg) : armor(n, r, c, icon, def), damage_stat(dmg) {}
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
	int get_attack_stat() const override { return damage_stat; }
};

class shield : public armor
{
private:
	int block_chance;
	bool shield_slot = false;

public:
	shield(std::string n, std::string r, int c, Texture2D* icon, int def, int block_ch) : armor(n, r, c, icon, def), block_chance(block_ch) {}
	item* clone() const override;
	void use(player* player) override;
	bool is_equipped() override;
	int get_block_chance() const { return block_chance; }
};
