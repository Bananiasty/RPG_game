#pragma once
#include "inventory/Consumable.h"

class scroll : public usable
{
protected:
	bool scroll_used = false;

public:
	scroll(std::string n, std::string r, int c, Texture2D* icon) : usable(n, r, c, icon) {}
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
	combat_scroll(std::string n, std::string r, int c, Texture2D* icon, float dmg, Texture2D* tex, int frames, float speed)
		: scroll(n, r, c, icon), damage(dmg), animation_texture(tex), frame_count(frames), frame_time(speed) {}
	void use(player* player) override;
	item* clone() const override;
	float get_damage() const { return damage; }
};
