#include "inventory/Scroll.h"
#include "character.h"
#include "gamestates.h"
#include "struct.h"

item_type scroll::get_type()
{
	return item_type::BOOK;
}

void combat_scroll::use(player* c)
{
	if (c->current_enemy != nullptr && c->spell_queued == false)
	{
		c->spell_queued = true;
		c->queued_damage = this->damage;

		c->queued_animation_texture = this->animation_texture;
		c->queued_frame_count = this->frame_count;
		c->queued_frame_time = this->frame_time;

		usable::use(c);
	}
}

item* combat_scroll::clone() const { return new combat_scroll(*this); }
