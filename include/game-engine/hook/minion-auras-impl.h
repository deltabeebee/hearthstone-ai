#pragma once

#include "minion-auras.h"
#include "minion-aura.h"

namespace GameEngine
{
	inline MinionAuras::MinionAuras(Minion & minion)
		: minion(minion)
	{
	}

	inline MinionAuras::~MinionAuras()
	{
#ifdef DEBUG
		if (!this->auras.empty()) {
			throw std::runtime_error("auras should be cleared first");
		}
#endif
	}

	inline bool MinionAuras::operator==(MinionAuras const & rhs) const
	{
		if (this->auras.size() != rhs.auras.size()) return false;

		auto it_lhs = this->auras.begin();
		auto it_rhs = rhs.auras.begin();

		while (true)
		{
			if (it_lhs == this->auras.end()) break;
			if (it_rhs == rhs.auras.end()) break;

			if (**it_lhs != **it_rhs) return false;

			++it_lhs;
			++it_rhs;
		}
		// both iterators should reach end here, since the size is equal

		return true;
	}

	inline bool MinionAuras::operator!=(MinionAuras const & rhs) const
	{
		return !(*this == rhs);
	}

	inline bool MinionAuras::Empty() const
	{
		return this->auras.empty();
	}

	inline void MinionAuras::Clear()
	{
		for (auto aura : this->auras) {
			aura->BeforeRemoved(this->minion);
			delete aura;
		}
		this->auras.clear();
	}

	inline void MinionAuras::Destroy()
	{
		for (auto aura : this->auras) delete aura;
		this->auras.clear();
	}

	inline void MinionAuras::HookAfterMinionAdded(Minion & added_minion)
	{
		for (auto & aura : this->auras) aura->HookAfterMinionAdded(added_minion);
	}

	inline void MinionAuras::HookAfterOwnerEnraged()
	{
		for (auto & aura : this->auras) aura->HookAfterOwnerEnraged();
	}

	inline void MinionAuras::HookAfterOwnerUnEnraged()
	{
		for (auto & aura : this->auras) aura->HookAfterOwnerUnEnraged();
	}

	inline void MinionAuras::HookAfterMinionDamaged(Minion & minion_, int damage)
	{
		for (auto & aura : this->auras) aura->HookAfterMinionDamaged(minion_, damage);
	}

	inline void MinionAuras::HookBeforeMinionTransform(Minion & minion_, int new_card_id)
	{
		for (auto & aura : this->auras) aura->HookBeforeMinionTransform(minion_, new_card_id);
	}

	inline void MinionAuras::HookAfterMinionTransformed(Minion & minion_)
	{
		for (auto & aura : this->auras) aura->HookAfterMinionTransformed(minion_);
	}

	template<typename Aura, typename ...Params>
	inline void MinionAuras::Add(Params && ...params)
	{
		Aura * aura = new Aura(this->minion, params...);
		this->auras.push_back(aura);
		aura->AfterAdded(this->minion);
	}

} // namespace GameEngine

inline std::size_t std::hash<GameEngine::MinionAuras>::operator()(const GameEngine::MinionAuras &s) const
{
	result_type result = 0;

	for (auto const& aura : s.auras) {
		GameEngine::hash_combine(result, *aura);
	}

	return result;
}