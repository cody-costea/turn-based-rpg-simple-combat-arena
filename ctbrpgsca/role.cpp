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
#include "role.h"

using namespace tbrpgsca;

QString Role::HpTxt = "HP";
QString Role::MpTxt = "MP";
QString Role::SpTxt = "RP";
QString Role::CausesTxt = " %s causes %s";

inline int Role::getMaximumHp() const
{
    return this->mHp;
}

inline int Role::getMaximumMp() const
{
    return this->mMp;
}

inline int Role::getMaximumRp() const
{
    return this->mSp;
}

inline int Role::getCurrentHp() const
{
    return this->hp;
}

inline int Role::getCurrentMp() const
{
    return this->mp;
}

inline int Role::getCurrentRp() const
{
    return this->sp;
}

State& Role::getAddedState(int n) const
{
    QVector<State*>* aStates = this->aStates;
    return *(aStates == nullptr || n < 0 || n >= aStates->size() ? nullptr : aStates->at(n));
}

int Role::getAddedStatesSize() const
{
    QVector<State*>* aStates = this->aStates;
    return aStates == nullptr ? 0 : aStates->size();
}

inline QString Role::getName() const
{
    return this->name;
}

inline QString Role::getSprite() const
{
    return this->sprite;
}

inline int Role::getId() const
{
    return this->id;
}

inline bool Role::isRanged() const
{
    return (this->flags & FLAG_RANGE) == FLAG_RANGE;
}

inline bool Role::isReviving() const
{
    return (this->flags & FLAG_REVIVE) == FLAG_REVIVE;
}

Role::Role(int const id, const QString& name, const QString& sprite, int const hpDmg, int const mpDmg, int const spDmg,
           int const mHp, int const mMp, int const mSp, bool const range, bool const revive, QVector<State*>* const states)
{
    this->id = id;
    this->name = name;
    this->aStates = states;
    this->sprite = sprite;
    this->mHp = mHp;
    this->mHp = mMp;
    this->mSp = mSp;
    this->hp = hpDmg;
    this->mp = mpDmg;
    this->sp = spDmg;
    int flags = revive ? FLAG_REVIVE : 0;
    if (range)
    {
        flags |= FLAG_RANGE;
    }
    this->flags = flags;
}

Role::~Role()
{
    //delete this->name;
}
