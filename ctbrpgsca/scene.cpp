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
#include "scene.h"

using namespace tbrpgsca;

bool Scene::actorAgiComp(const Actor& a, const Actor& b)
{
    return (a.agi > b.agi);
}

Actor& Scene::getGuardian(Actor& user, Actor& target, Ability& skill) const
{
    if (!user.isRanged() || (skill.isOnlyMelee() && !skill.isRanged()))
    {
        int pos = -1;
        int side = target.side;
        Actor* fGuard = nullptr,* lGuard = nullptr;
        Actor** guardPos = &fGuard;
        QVector<Actor*>& party = *(this->parties[side]);
        int pSize = party.size();
        for (int i = 0; i < pSize; i++)
        {
            Actor* const guardian = party[i];
            if (guardian == &target)
            {
                if (fGuard == nullptr || i == pSize - 1)
                {
                    return target;//break;
                }
                else
                {
                    pos = i;
                    guardPos = &lGuard;
                    continue;
                }
            }
            else if ((fGuard == nullptr || pos != -1) && guardian->isGuarding())
            {
                (*guardPos) = guardian;
            }
        }
        if (fGuard != nullptr && lGuard != nullptr)
        {
            return *((pos < (pSize / 2)) ? fGuard : lGuard);
        }
    }
    return target;
}

Scene& Scene::checkStatus(QString& ret)
{
    Scene& scene = *this;
    if (scene.status == 0)
    {
        auto parties = scene.parties;
        auto party = parties[0];
        int partySize = party->size();
        {
            for (int k = 0; k < partySize; k++)
            {
                if (party->at(k)->hp > 0)
                {
                    goto enemyCheck;
                }
            }
            scene.status = -2;
            ret = ret % Scene::FallenTxt;
            return scene;
        }
        enemyCheck:
        {
            int const partiesSize = parties.size();
            for (int i = 1; i < partiesSize; i++)
            {
                party = parties[i];
                partySize = party->size();
                for (int j = 0; j < partySize; j++)
                {
                    if (party->at(j)->hp > 0)
                    {
                        return scene;
                    }
                }
            }
            ret = ret % Scene::VictoryTxt;
            scene.status = 1;
        }
    }
    return scene;
}

Scene& Scene::execute(QString& ret, Actor& user, Actor* target, Ability& ability, bool const applyCosts)
{
    Scene& scene = *this;
    bool healing = ability.mHp < 0;
    if ((healing && ability.isReviving()) || target->hp > 0)
    {
        int cntSize;
        QVector<Ability*>* counters;
        ability.execute(ret, this, user, *target, applyCosts);
        if ((!healing) && target != &user && target->hp > 0 && target->isGuarding()
                && (counters = target->counters) != nullptr && (cntSize = counters->size()) > 0)
        {
            Ability* counter = nullptr;
            int usrDmgType = ability.dmgType;
            for (int i = 0; i < cntSize; i++)
            {
                Ability* cntSkill = counters->at(i);
                int cntDmgType = cntSkill->dmgType;
                if (((usrDmgType & cntDmgType) == cntDmgType) && cntSkill->mHp > counter->mHp)
                {
                    counter = cntSkill;
                }
            }
            if (counter != nullptr)
            {
                counter->execute(ret, *target, user, false);
            }
        }
    }
    return scene;
}

Scene& Scene::perform(QString& ret, Actor& user, Actor& target, Ability& ability, bool const item)
{
    Scene& scene = *this;
    QVector<SceneAct*>* events = scene.events;
    if (events != nullptr && events->size() > 1)
    {
        auto event = events->at(1);
        if (event != nullptr && !event(scene, ret))
        {
            return scene;
        }
    }
    if (ability.isRanged() && ability.targetsAll())
    {
        bool applyCosts = true;
        int usrSide = user.side;
        bool sideTarget = ability.targetsSide();
        bool noSelfTarget = !ability.targetsSelf();
        int pSize = scene.players->size();
        for (int i = 0; i < pSize; i++)
        {
            Actor* trg = players->at(i);
            if (sideTarget && noSelfTarget && trg->side == usrSide)
            {
                continue;
            }
            else if (trg == &user)
            {
                if (noSelfTarget)
                {
                    continue;
                }
                else
                {
                    ability.execute(ret, this, user, user, applyCosts);
                }
            }
            else
            {
                scene.execute(ret, user, trg, ability, applyCosts);
            }
            applyCosts = false;
        }
    }
    else if (ability.targetsSide())
    {
        int side = ability.targetsSelf() ? user.side : target.side;
        QVector<Actor*>& party = *(scene.parties[side]);
        int pSize = party.size();
        for (int i = 0; i < pSize; i++)
        {
            scene.execute(ret, user, party[i], ability, i == 0);
        }
    }
    else
    {
        if (&user == &target || ability.targetsSelf())
        {
            ability.execute(ret, this, user, user, true);
        }
        else
        {
            scene.execute(ret, user, &(scene.getGuardian(user, target, ability)), ability, true);
        }
    }
    if (item)
    {
        QMap<Ability*, int>* items = user.items;
        if (items != nullptr)
        {
            items->operator[](&ability) = items->value(&ability, 1) - 1;
        }
    }
    this->lastAbility = &ability;
    if (events->size() > 2)
    {
        auto event = events->at(2);
        if (event != nullptr && !event(scene, ret))
        {
            return scene;
        }
    }
    user.setExperience(user.xp + 1);
    return scene;
}

Ability& Scene::getAiSkill(Actor& user, QVector<Ability*>& skills, int const defSkill, bool const restore) const
{
    int sSize = skills.size();
    Ability* s = skills[defSkill];
    for (int i = 0; i < sSize; i++)
    {
        Ability* a = skills[i];
        if (a->canPerform(user) && ((defSkill > 0 && (a->mHp < s->mHp)
            && (a->isReviving() || !restore)) || (a->mHp > s->mHp)))
        {
            s = a;
        }
    }
    return *s;
}

Scene& Scene::playAi(QString& ret, Actor& player)
{
    //TODO: implement cases for "automatic", "confused" or both
    Scene& scene = *this;
    int side = player.side, skillIndex = 0, heal = -1;
    QVector<Actor*>& party = *(scene.parties[side]);
    int partySize = party.size();
    for (int i = 0; i < partySize; i++)
    {
        Actor& iPlayer = *(party.at(i));
        int iHp = iPlayer.hp;
        if (iHp < 1)
        {
            heal = 1;
        }
        else if (iHp < (iPlayer.mHp / 3))
        {
            heal = 0;
        }
    }
    QVector<Ability*>& skills = *(player.aSkills);
    if (heal > -1)
    {
        int skillsSize = skills.size();
        for (int i = 0; i < skillsSize; i++)
        {
            Ability& s = *(skills[i]);
            if (s.canPerform(player) && (s.mHp < 0 && ((heal == 0) || s.isReviving())))
            {
                skillIndex = 0;
                break;
            }
        }
    }
    Actor* target;
    Ability& ability = scene.getAiSkill(player, skills, skillIndex, heal == 1);
    if (ability.mHp > -1)
    {
        int trg = 0;
        QVector<Actor*>& players = *(scene.players);
        int playerSize = players.size();
        do
        {
            target = players.at(trg);
        } while (((++trg) < playerSize) && (target->hp < 1 || target->side == side));
        for (int i = trg; i < playerSize; i++)
        {
            int iHp;
            Actor* iPlayer = players.at(i);
            if (iPlayer->side != side && (iHp = iPlayer->hp) > 0 && iHp < target->hp)
            {
                target = iPlayer;
            }
        }
    }
    else
    {
        target = party.at(0);
        bool restore = ability.isReviving();
        for (int i = 1; i < partySize; i++)
        {
            Actor* iPlayer = party.at(i);
            int iHp = iPlayer->hp;
            if (iHp < target->hp && (restore || iHp > 0))
            {
                target = iPlayer;
            }
        }
    }
    scene.perform(ret, player, *target, ability, false);
    return scene;
}

Scene& Scene::endTurn(QString& ret)
{
    Scene& scene = *this;
    int current = scene.current;
    QVector<Actor*>& players = *(scene.players);
    int playersSize = players.size();
    Actor* crActor = players[current];
    crActor->actions--;
    while (crActor->actions < 1)
    {
        crActor->applyStates(ret, true);
        int mInit = scene.mInit;
        if (mInit > 0)
        {
            //int next = current;
            //Actor* nxActor = crActor;
            int cInit = crActor->init - mInit;
            crActor->init = cInit;
            do
            {
                for (int i = 0; i < playersSize; i++)
                {
                    Actor* iPlayer = players[i];
                    if (iPlayer->hp > 0)
                    {
                        int iInit = iPlayer->init + iPlayer->agi;
                        iPlayer->init = iInit;
                        if (iInit > cInit)
                        {
                            cInit = iInit;
                            crActor = iPlayer;
                            current = i;
                        }
                    }
                }
            }
            while (crActor->init < mInit);
            //crActor = nxActor;
            //current = next;
        }
        else
        {
            do
            {
                if (++current == playersSize)
                {
                    current = 0;
                }
                crActor = players[current];
            }
            while (crActor->hp < 1);
        }
        crActor->actions = crActor->mActions;
        QMap<Ability*, int>* regSkills = crActor->skillsRgTurn;
        if (regSkills != nullptr)
        {
            QMap<Ability*, int>* skillsQty = crActor->skillsCrQty;
            if (skillsQty == nullptr)
            {
                skillsQty = new QMap<Ability*, int>();
                crActor->skillsCrQty = skillsQty;
            }
            for (Ability* skill : regSkills->keys())
            {
                int skillMaxQty = skill->mQty;
                int skillCrQty = skillsQty->value(skill, skillMaxQty);
                if (skillCrQty < skillMaxQty)
                {
                    int skillRgTurn = regSkills->value(skill, 0);
                    if (skillRgTurn == skill->rQty)
                    {
                        skillsQty->operator[](skill) = skillCrQty + 1;
                        regSkills->operator[](skill) = 0;
                    }
                    else
                    {
                        regSkills->operator[](skill) = skillRgTurn + 1;
                    }
                }
            }
        }
        crActor->applyStates(ret, false);
    }
    scene.current = current;
    QVector<SceneAct*>* events = scene.events;
    if (events != nullptr && events->size() > EVENT_NEW_TURN)
    {
        auto event = events->at(EVENT_NEW_TURN);
        if (event != nullptr && event(scene, ret) && (crActor->isAiPlayer() || crActor->isAutomated() || crActor->isConfused()))
        {
            scene.playAi(ret, (*crActor));
        }
    }
    return scene;
}

inline void Scene::agiCalc()
{
    if (this->mInit < 1)
    {
        QVector<Actor*>& players = *(this->players);
        std::sort(players.begin(), players.end(), Scene::actorAgiComp);
    }
}

Scene::Scene(QString& ret, const QVector<QVector<Actor*>*>& parties, QVector<SceneAct*>* const events, int const surprise, int const mInit)
{
    int partiesSize = parties.size();
    assert(partiesSize > 1);
    Scene& scene = *this;
    scene.mInit = mInit;
    scene.events = events;
    scene.parties = parties;
    QVector<Actor*>& players = *(scene.players);
    for (int i = 0; i < partiesSize; i++)
    {
        bool aiPlayer = i > 0;
        bool surprised = (surprise == i);
        QVector<Actor*>& party = *(parties[i]);
        int pSize = party.size();
        for (int j = 0; j < pSize; j++)
        {
            Actor& player = *(party[j]);
            player.actions = surprised ? 0 : player.mActions;
            if (aiPlayer)
            {
                player.setAiPlayer(true);
            }
            player.side = i;
        }
        players.append(party);
    }
    SceneAct* event;
    scene.agiCalc();
    if (events == nullptr || events->size() == 0 || ((event = events->at(0)) == nullptr) || event(scene, ret))
    {
        scene.endTurn(ret);
    }
}
