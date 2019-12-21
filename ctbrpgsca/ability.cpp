/*
Copyright (C) AD 2013-2019 Claudiu-Stefan Costea

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "ability.h"
#include "actor.h"
#include "state.h"

#include <QStringBuilder>

using namespace tbrpgsca;

QString Ability::MissesTxt = ", but misses";
QString Ability::SuffersTxt = ", %s suffers";
QString Ability::ReflectTxt = ", reflected by %s";
QString Ability::ResistTxt = ", resisted by %s";
QString Ability::StolenTxt = ", obtaining %s from %s";

inline int Ability::getUsesRegen() const
{
    return this->rQty;
}

inline int Ability::getMaximumUses() const
{
    return this->mQty;
}

inline int Ability::getAttributeIncrement() const
{
    return this->attrInc;
}

inline int Ability::getRequiredLevel() const
{
    return this->lvRq;
}

inline int Ability::getDamageType() const
{
    return this->dmgType;
}

inline State& Ability::getAddedState(int n) const
{
    return *(this->aStates->at(n));
}

inline bool Ability::hasAddedState(State& state) const
{
    QVector<State*>* aStates = this->aStates;
    return aStates != nullptr && aStates->contains(&state);
}

inline int Ability::getAddedStatesSize() const
{
    QVector<State*>* aStates = this->aStates;
    return aStates == nullptr ? 0 : aStates->size();
}

inline State& Ability::getRemovedState(int const n) const
{
    return *(this->rStates->at(n));
}

inline bool Ability::hasRemovedState(State& state) const
{
    QVector<State*>* rStates = this->rStates;
    return rStates != nullptr && rStates->contains(&state);
}

inline int Ability::getRemovedStatesSize() const
{
    QVector<State*>* rStates = this->rStates;
    return rStates == nullptr ? 0 : rStates->size();
}

inline bool Ability::isStealing() const
{
    return (this->flags & FLAG_STEAL) == FLAG_STEAL;
}

inline bool Ability::isAbsorbing() const
{
    return (this->flags & FLAG_ABSORB) == FLAG_ABSORB;
}

inline bool Ability::isOnlyMelee() const
{
    return (this->flags & FLAG_MELEE) == FLAG_MELEE;
}

inline bool Ability::targetsSide() const
{
    return (this->flags & FLAG_TRG_SIDE) == FLAG_TRG_SIDE;
}

inline bool Ability::targetsSelf() const
{
    return (this->flags & FLAG_TRG_SELF) == FLAG_TRG_SELF;
}

inline bool Ability::targetsAll() const
{
    return (this->flags & FLAG_TRG_ALL) == FLAG_TRG_ALL;
}

Ability& Ability::replenish(Actor& user)
{
    Ability& ability = *this;
    int const mQty = ability.mQty;
    if (mQty > 0)
    {
        QMap<Ability*, int>* usrSkills = user.skillsCrQty;
        if (usrSkills == nullptr)
        {
            usrSkills = new QMap<Ability*, int>();
            user.skillsCrQty = usrSkills;
        }
        usrSkills->operator[](this) = mQty;
    }
    return ability;
}

inline bool Ability::canPerform(Actor& actor)
{
    QMap<Ability*, int>* skillsQty = actor.skillsCrQty;
    return this->mp <= actor.mp && this->hp < actor.hp && this->sp <= actor.sp && actor.lv >= this->lvRq
                    && (skillsQty == nullptr || skillsQty->value(this, 1) > 0);
}

inline Ability& Ability::execute(QString& ret, Actor& user, Actor& target, bool applyCosts)
{
    return this->execute(ret, nullptr, user, &target, applyCosts);
}

Ability& Ability::execute(QString& ret, Scene* scene, Actor& user, Actor* target, bool const applyCosts)
{
    Ability& ability = *this;
    int const dmgType = ability.dmgType;
    if (target->isReflecting() && dmgType == DMG_TYPE_WIS)
    {
        ret = ret % Ability::ReflectTxt.arg(target->name);
        target = &user;
    }
    QMap<int, int>* trgResMap = target->res;
    int res = trgResMap == nullptr ? 3 : trgResMap->value(ability.elm, 3);
    if (res > 6)
    {
        if (res == 7)
        {
            ret = ret % Ability::ResistTxt.arg(target->name);
            goto costs;
        }
        res = -7 + (res - 7);
        if (res > -1)
        {
            res = -1;
        }
    }
    else if (res < 1)
    {
        res = 1;
    }
    {
        bool ko = target->hp < 1;
        int dmg = std::rand() % 4;
        int canMiss = 0, def = 0, i = 0;
        if ((dmgType & DMG_TYPE_ATK) == DMG_TYPE_ATK)
        {
            dmg += user.atk;
            def += target->def;
            canMiss = 2;
            i++;
        }
        if ((dmgType & DMG_TYPE_DEF) == DMG_TYPE_DEF)
        {
            dmg += user.def;
            def += target->def;
            i++;
        }
        if ((dmgType & DMG_TYPE_SPI) == DMG_TYPE_SPI)
        {
            dmg += user.spi;
            def += target->wis;
            i++;
        }
        if ((dmgType & DMG_TYPE_WIS) == DMG_TYPE_WIS)
        {
            dmg += user.wis;
            def += target->spi;
            i++;
        }
        if ((dmgType & DMG_TYPE_AGI) == DMG_TYPE_AGI)
        {
            dmg += user.agi;
            def += target->agi;
            canMiss = 4;
            i++;
        }
        dmg = i == 0 ? 0 : (((ability.attrInc + (dmg / i)) / (def / i)) / res);
        if (canMiss == 0 || target == &user || ((std::rand() % 13) + user.agi / canMiss) > 2 + target->agi / 4)
        {
            {
                int dmgHp = ability.mHp, dmgMp = ability.mMp, dmgSp = ability.mSp;
                dmgHp = dmgHp == 0 ? 0 : ((dmgHp < 0 ? -1 : 1) * dmg + dmgHp);
                dmgMp = dmgMp == 0 ? 0 : ((dmgMp < 0 ? -1 : 1) * dmg + dmgMp);
                dmgSp = dmgSp == 0 ? 0 : ((dmgSp < 0 ? -1 : 1) * dmg + dmgSp);
                target->hp -= dmgHp;
                target->mp -= dmgMp;
                target->sp -= dmgSp;
                if (ability.isAbsorbing())
                {
                    user.hp += dmgHp / 2;
                    user.mp += dmgMp / 2;
                    user.sp += dmgSp / 2;
                }
                if (dmgHp != 0 || dmgMp != 0 || dmgSp != 0)
                {
                    ret = ret % Ability::SuffersTxt.arg(target->name) % Role::GetDmgText(dmgHp, dmgMp, dmgSp);
                }
            }
            {
                QVector<State*>* aStates = ability.aStates;
                if (aStates != nullptr)
                {
                    for (State* state : *aStates)
                    {
                        state->inflict(ret, scene, *target, false, false);
                    }
                }
            }
            {
                QMap<State*, int>* stateDur = target->stateDur;
                if (stateDur != nullptr)
                {
                    QVector<State*>* rStates = ability.rStates;
                    if (rStates != nullptr)
                    {
                        for (State* rState : *rStates)
                        {
                            for (State* aState : stateDur->keys())
                            {
                                if (aState == rState)
                                {
                                    rState->disable(scene, *target, false, false);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (ability.isStealing())
            {
                QMap<Ability*, int>* usrItems = user.items;
                if (usrItems != nullptr)
                {
                    int trgItemsSize;
                    QMap<Ability*, int>* trgItems = target->items;
                    if (trgItems != nullptr && trgItems != usrItems && (trgItemsSize = trgItems->size()) > 0
                            && (((std::rand() % 12) + user.agi / 4) > 4 + target->agi / 3))
                    {
                        int const itemId = std::rand() % trgItemsSize;
                        if (itemId < trgItemsSize)
                        {
                            Ability* stolen = trgItems->keys().at(itemId);
                            int trgItemQty = trgItems->values().at(itemId);
                            if (trgItemQty > 0)
                            {
                                usrItems->operator[](stolen) = usrItems->value(stolen, 0) + 1;
                                if ((--trgItemQty) == 0)
                                {
                                    trgItems->remove(stolen);
                                }
                                else
                                {
                                    trgItems->operator[](stolen) = trgItemQty;
                                }
                                ret = ret % Ability::StolenTxt.arg(stolen->name, target->name);
                            }
                        }
                    }
                }
            }
        }
        if (ko && target->hp > 0)
        {
            target->applyRoles(ret, scene);
        }
    }
    costs:
    {
        if (applyCosts)
        {
            user.hp -= ability.hp;
            user.mp -= ability.mp;
            user.sp -= ability.sp;
            int mQty = ability.mQty;
            if (mQty > 0)
            {
                QMap<Ability*, int>* usrSkillsQty = user.skillsCrQty;
                if (usrSkillsQty == nullptr)
                {
                    usrSkillsQty = new QMap<Ability*, int>();
                    user.skillsCrQty = usrSkillsQty;
                }
                usrSkillsQty->operator[](this) = (usrSkillsQty->value(this, mQty) - 1);
            }
        }
        user.checkStatus(ret);
    }
    return ability;
}

Ability::Ability(int const id, QString& name, QString& sprite, bool const steal, bool const range, bool const melee, int const lvRq, int const hpC,
                 int const mpC, int const spC, int const dmgType, int const attrInc, int const hpDmg, int const mpDmg, int const spDmg, int const trg,
                 int const elm,int const mQty, int const rQty, bool const absorb, bool const revive, QVector<State*>* const aStates, QVector<State*>* const rStates)
    : Role(id, name, sprite, hpC, mpC, spC, hpDmg, mpDmg, spDmg, elm, range, revive)
{
    this->elm = elm;
    this->lvRq = lvRq;
    this->mQty = mQty;
    this->rQty = rQty;
    this->dmgType = dmgType;
    this->attrInc = attrInc;
    this->aStates = aStates;
    this->rStates = rStates;
    int flags = this->flags;
    if (melee)
    {
        flags |= FLAG_MELEE;
    }
    if (steal)
    {
        flags |= FLAG_STEAL;
    }
    if (absorb)
    {
        flags |= FLAG_ABSORB;
    }
    flags |= trg;
    this->flags = flags;
}
