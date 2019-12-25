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
#ifndef SCENE_H
#define SCENE_H

#include "actor.h"

#include <QVector>
#include <QStringBuilder>

namespace tbrpgsca
{

    class Scene
    {
        #define STATUS_DEFEAT -2
        #define STATUS_RETREAT -1
        #define STATUS_ONGOING 0
        #define STATUS_VICTORY 1
        #define EVENT_BEGIN_SCENE 0
        #define EVENT_BEFORE_ACT 1
        #define EVENT_AFTER_ACT 2
        #define EVENT_NEW_TURN 3
        #define EVENT_END_SCENE 4
    public:
        typedef bool (* SceneAct)(Scene& scene, QString& ret);

        static QString EscapeTxt;
        static QString VictoryTxt;
        static QString PerformsTxt;
        static QString FallenTxt;
        static QString FailTxt;

        static bool actorAgiComp(const Actor& a, const Actor& b);

        Scene& endTurn(QString& ret);
        Scene& playAi(QString& ret, Actor& player);
        Scene& perform(QString& ret, Actor& user, Actor& target, Ability& ability, bool const item);
        Scene& checkStatus(QString& ret);
        Scene& escape(QString& ret);

        Ability& getAiSkill(Actor& user, QVector<Ability*>& skills, int const index, bool const nRestore) const;
        Actor& getGuardian(Actor& user, Actor& target, Ability& skill) const;

        inline Actor& getPartyPlayer(int const party, int const player) const;
        inline bool hasPartyPlayer(int const party, Actor& player) const;
        inline int getPartyPlayersSize(int const party) const;
        inline int getPartiesSize() const;

        inline Actor& getOrderedPlayer(int const n) const;
        inline bool hasOrderedPlayer(Actor& player) const;
        inline int getOrderedPlayersSize() const;

        inline Actor& getTargetedPlayer(int const n) const;
        inline bool hasTargetedPlayer(Actor& player) const;
        inline int getTargetedPlayersSize() const;

        inline Ability& getLastAbility() const;
        inline int getCurrent() const;
        inline int getStatus() const;

        Scene(QString& ret, QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit);

        ~Scene();
    protected:
        Ability* lastAbility;
        QVector<SceneAct*>* events;
        int current, surprise, fTarget, lTarget, status, mInit;
        QVector<Actor*>* players,* targets;
        QVector<QVector<Actor*>*> parties;

        Scene& execute(QString& ret, Actor& user, Actor* target, Ability& ability, bool const applyCosts);
        inline void agiCalc();

        friend class Actor;
        friend class Ability;
        friend class Costume;
        friend class State;
        friend class Role;
    };

}

#endif // SCENE_H
