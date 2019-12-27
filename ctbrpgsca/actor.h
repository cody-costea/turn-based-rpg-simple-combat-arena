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
#ifndef ACTOR_H
#define ACTOR_H

#include "costume.h"

namespace tbrpgsca
{

    class Scene;

    class Actor : public Costume
    {
        #define FLAG_AI_PLAYER 128
        #define CHAR_NONE 0
        #define CHAR_RACE 1
        #define CHAR_JOB 2
    public:
        static QString KoTxt;
        static QString RiseTxt;

        inline Costume& getJob() const;
        inline Costume& getRace() const;
        inline bool isAiPlayer() const;
        inline int getLevel() const;
        inline int getMaximumLevel() const;
        inline int getMaximumExperience() const;
        inline int getCurrentActions() const;
        inline int getExperience() const;
        inline int getInitiative() const;
        //inline int getPartySide() const;
        inline void* getExtra() const;

        int getRemainingSkillUses(Ability& skill) const;
        int getRegeneratingSkillTurn(Ability& skill) const;

        Costume* unequipPos(char const pos);
        Costume* equipItem(char const pos, Costume* const item);
        char unequipItem(Costume& item);

        inline Actor& recover();
        //Actor& applyRoles(QString& ret);
        Actor& applyStates(QString& ret, bool const consume);
        Actor& setElementResistance(int const element, int const res);
        Actor& setStateResistance(State* const state, int const res);
        Actor& setExtra(void* const extra);
        Actor& setSprite(QString& value);
        Actor& setName(QString& value);
        Actor& setJob(Costume& job);
        Actor& setRace(Costume& race);
        Actor& setLevel(int const level);
        Actor& setExperience(int const xp);
        Actor& setMaximumLevel(int const maxLv);
        Actor& setMaximumActions(int const mActions);
        Actor& setMaximumHp(int const mHp);
        Actor& setMaximumMp(int const mMp);
        Actor& setMaximumRp(int const mSp);
        Actor& setCurrentHp(int const hp);
        Actor& setCurrentMp(int const mp);
        Actor& setCurrentRp(int const sp);
        Actor& setAttack(int const atk);
        Actor& setDefense(int const def);
        Actor& setSpirit(int const spi);
        Actor& setWisdom(int const wis);
        Actor& setAgility(int const agi);
        Actor& setInitiative(int const init);
        Actor& setRanged(bool const range);
        Actor& setStunned(bool const stun);
        Actor& setReflecting(bool const reflects);
        Actor& setShapeShifted(bool const shapeshift);
        Actor& setAutomated(bool const automate);
        Actor& setConfused(bool const confuse);
        Actor& setAiPlayer(bool const aiPlayer);
        Actor& setReviving(bool const revive);

        Actor(int const id, QString& name, Costume& race, Costume& job, int const level, int const maxLv, int const mHp, int const mMp, int const mSp, int const atk,
              int const def, int const spi, int const wis, int const agi, QMap<int, int>* const res, QMap<State*, int>* const stRes, QMap<Ability*, int>* const items);

        Actor(Actor& actor);

        ~Actor();
    protected:
        int lv, maxLv, xp, maxp, init, side, actions;
        QMap<Ability*, int>* skillsCrQty,* skillsRgTurn,* items;
        QMap<char, Costume*> equipment;
        QMap<State*, int>* stateDur;
        QVector<Costume*>* dmgRoles;
        void* extra;

        Actor& recover(Scene* const scene);
        Actor& levelUp(Scene* const scene);
        Actor& checkRegSkill(Ability& skill);
        Actor& checkStatus(Scene* const scene, QString& ret);
        Actor& applyDmgRoles(QString& ret, Scene* const scene);
        Actor& applyStates(QString& ret, Scene* const scene, bool const consume);
        Actor& updateAttributes(bool const remove, Scene* const scene, Costume& costume);
        Actor& updateResistance(bool const remove, QMap<int, int>* const elmRes, QMap<State*, int>* const stRes);
        Actor& updateSkills(bool const remove, bool const counters, QVector<Ability*>& skills);
        Actor& switchCostume(Scene* const scene, Costume* const oldCostume, Costume* const newCostume);
        //Actor& updateStates(bool const remove, QVector<State*>& states);
        Actor& setCurrentHp(int const hp, QString& ret, Scene& scene);
        Actor& setAgility(int const agi, Scene& scene);
        inline Actor& setJob(Scene* const scene, Costume& job);
        inline Actor& setRace(Scene* const scene, Costume& race);
        Actor& setLevel(Scene* const scene, int const level);
        Actor& setExperience(Scene* const scene, int const xp);
        Costume* equipItem(Scene* const scene, char const pos, Costume* const item);
        Costume* unequipPos(Scene* const scene, char const pos);
        char unequipItem(Scene* const scene, Costume& item);
        Actor& refreshCostume(Costume& costume);
        Actor& refreshCostumes();

        friend class Scene;
        friend class Ability;
        friend class State;
        friend class Costume;
    };

}

#endif // ACTOR_H
