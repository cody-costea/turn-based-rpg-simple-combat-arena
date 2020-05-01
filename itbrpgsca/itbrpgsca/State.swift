/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

class State : Costume {
    
    public static let STATE_END_DUR = -3
    
    internal var _dur: Int, _sRes: Int, _rSkills: [Ability]?
    
    open var dur: Int {
        return self._dur
    }
    
    open var sRes: Int {
        return self._sRes
    }
    
    open var rSkills: [Ability]? {
        return self._rSkills
    }
    
    open func alter(ret: inout String, scene: Scene?, actor: Actor, consume: Bool) -> State {
        return self
    }
    
    open func disable(ret: inout String, scene: Scene?, actor: Actor, dur: Int, remove: Bool) -> Bool {
        return true
    }
    
    open func remove(ret: inout String, scene: Scene?, actor: Actor) -> State {
        return self
    }
    
    open func inflict(ret: inout String, scene: Scene?, user: Actor, target: Actor, dur: Int, always: Bool) -> State {
        return self
    }
    
    init(id: Int, name: String, sprite: String?, shapeShift: Bool, dur: Int, sRes: Int, mActions: Int,
         elm: Int, hpDmg: Int, mpDmg: Int, spDmg: Int, mHp: Int, mMp: Int, mSp: Int, atk: Int, def: Int,
         spi: Int,wis: Int, agi: Int, stun: Bool, range: Bool, enrage: Bool, confuse: Bool, convert: Bool,
         reflect: Bool, invincible: Bool, ko: Bool, revive: Bool, aSkills: [Ability]?, rSkills: [Ability]?,
         counters: [Ability]?, states: [State: Int]?, stRes: [State: Int]?, res: [Int: Int]?) {
        self._dur = dur
        self._sRes = sRes
        self._rSkills = rSkills
        super.init(id: id, name: name, sprite: sprite, shapeShift: shapeShift, mActions: mActions,
                   elm: elm, hpDmg: hpDmg, mpDmg: mpDmg, spDmg: spDmg, mHp: mHp, mMp: mMp, mSp: mSp,
                   atk: atk, def: def, spi: spi, wis: wis, agi: agi, stun: stun, range: range, enrage: enrage,
                   confuse: confuse, reflect: reflect, invincible: invincible, ko: ko, revive: revive,
                   skills: aSkills, counters: counters, states: states, stRes: stRes, res: res)
        if convert {
            self._flags |= State.FLAG_CONVERT
        }
    }
    
}
