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
#ifndef COSTUME_H
#define COSTUME_H

#include "role.h"

#include <QMap>

namespace ctbrpgsca
{
    class Ability;

    class Costume : public Role
    {
    #define FLAG_AUTOMATE 4
    #define FLAG_CONFUSE 8
    #define FLAG_COUNTER 16
    #define FLAG_REFLECT 32
    public:
        int getAttack();
        int getDefense();
        int getSpirit();
        int getWisdom();
        int getAgility();

        QMap<int, int>* getElementResistance();
        QMap<State*, int>* getStateResistance();

        Ability** getAddedSkills();

        Costume(int const id, const QString& name, const QString* const sprite, int const hp, int const mp, int const sp,
                int const atk, int const def, int const spi, int const wis, int const agi, int const mInit, bool const range,
                bool const automate, bool const confuse, bool const reflect, bool const counter, bool const revive,
                QMap<int, int>* const res, Ability** const skills, State** const states,  QMap<State*, int>* const stRes);

        ~Costume();
    private:
        int atk, def, spi, wis, agi;
        QMap<State*, int>* stRes;
        QMap<int, int>* res;
        Ability** aSkills;

        friend class Actor;
        friend class Ability;
        friend class State;
        friend class Scene;
    };
}

#endif // COSTUME_H
