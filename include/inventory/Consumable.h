#pragma once
#include "inventory/Item.h"

class usable : public item
{
public:
	usable(std::string n, std::string r, int c, Texture2D* icon) : item(n, r, c, icon) {}
	virtual item* clone() const = 0;
	virtual item_type get_type() override = 0;
	virtual void use(player* player) override;
};

class other_item : public usable
{
public:
	other_item(std::string n, std::string r, int c, Texture2D* icon) : usable(n, r, c, icon) {}
	item* clone() const override;
	virtual item_type get_type() override;
	virtual void use(player* player) override;
};

class food : public usable
{
public:
	food(std::string n, std::string r, int c, Texture2D* icon) : usable(n, r, c, icon) {}
	item_type get_type() override;
	virtual item* clone() const override;
	void use(player* player) override;
	//int get_restore_hunger() const { return 0; }
};

class health_potion : public food
{
private:
	int restore_health;
public:
	health_potion(std::string n, std::string r, int c, Texture2D* icon, int r_h) : food(n, r, c, icon), restore_health(r_h) {}
	item* clone() const override;
	void use(player* player) override;
	int get_restore_health() const override { return restore_health; }
};
