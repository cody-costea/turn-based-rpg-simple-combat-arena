/*
Copyright (C) AD 2013-2020 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/

import Foundation

public let MIN_ROUND = Int.min

public let PerformsTxt = "%@ performs %@"
public let VictoryTxt = "The party has won!"
public let FallenTxt = "The party has fallen!"
public let EscapeTxt = "The party has escaped!"
public let FailTxt = "The party attempted to escape, but failed."

public enum SceneEvent {
    case begin, beforeAct, afterAct, newTurn, end
}

public enum SceneStatus: Int {
    case defeat = -2, retreat = -1, ongoing = 0, victory = 1
}

public protocol Scene : class {
    
    typealias SceneRun = (Scene, inout String) -> Bool
    
    typealias SpriteRun = (Scene, Actor?, Ability?, Bool, Actor?, Ability?) -> Bool
    
    var status: SceneStatus {
        get
        set
    }
    
    var current: Int {
        get
        set
    }
    
    var previous: Int { //oldCurrent
        get
        set
    }
    
    var players: [Actor]? {
        get
        set
    }
    
    var targets: [Actor]? {
        get
        set
    }
    
    var parties: [[Actor]] {
        get
        set
    }
    
    var lastAbility: Ability? {
        get
        set
    }
    
    var crActor: Actor? {
        get
        set
    }
    
    var surprise: Int {
        get
        set
    }
    
    var fTarget: Int {
        get
        set
    }
    
    var lTartet: Int {
        get
        set
    }
    
    var mInit: Int {
        get
        set
    }
    
    var spriteRun: SpriteRun? {
        get
        set
    }
    
    var events: [SceneEvent : [SceneRun]]? {
        get
        set
    }
    
    func playAi(ret: inout String, player: Actor)
    
    func endTurn(ret: inout String, actor: Actor)
    
    func getGuardian(user: Actor, target: Actor, skill: Ability) -> Actor
    
    func getAiSkill(user: Actor, skills: [Ability], index: Int, nRestore: Bool) -> Int
    
    func execute(ret: inout String, user: Actor, target: Actor?, ability: Ability, applyCosts: Bool)
    
    func perform(ret: inout String, user: Actor, target: Actor, ability: Ability, item: Bool)
    
    func initialize(ret: inout String, parties: [[Actor]], spriteRun: SpriteRun?,
                    events: [SceneEvent : [SceneRun]]?, surprise: Int, mInit: Int)
    
    func canTarget(user: Actor, ability: Ability, target: Actor) -> Bool
    
    func checkStatus(ret: inout String)
    
    func escape(ret: inout String)
    
    func resetTurn(actor: Actor)
    
    func agiCalc()
    
}

public extension Scene where Self: AnyObject {
    
    func playAi(ret: inout String, player: Actor) {
        
    }
    
    func endTurn(ret: inout String, actor: Actor) {
        
    }
    
    func getGuardian(user: Actor, target: Actor, skill: Ability) -> Actor {
        let side = target._oldSide
        if user._oldSide != side && ((!skill.ranged) && ((!user.ranged) || skill.melee)) {
            var pos = -1
            var first = true
            var fGuard: Actor! = nil, lGuard: Actor! = nil
            let party = self.parties[side], pSize = party.count
            for i in 0..<pSize {
                let guardian = party[i]
                if guardian == target {
                    if fGuard == nil || i == pSize - 1 {
                        return target
                    } else {
                        pos = i
                        first = false
                        continue
                    }
                } else if (fGuard == nil || pos != -1) && guardian.hp > 0 && !(guardian.stunned || guardian.confused) {
                    if first {
                        fGuard = guardian
                    } else {
                        lGuard = guardian
                    }
                }
            }
            if fGuard != nil && lGuard != nil {
                return pos < (pSize / 2) ? fGuard : lGuard
            }
        }
        return target
    }
    
    func getAiSkill(user: Actor, skills: [Ability], index: Int, nRestore: Bool) -> Int {
        return 0
    }
    
    func execute(ret: inout String, user: Actor, target: Actor, ability: Ability, applyCosts: Bool) {
        let healing = ability.hp < 0
        let ko = target.hp < 1
        if (healing && ability.revives) || !ko {
            var cntSkill: Ability! = nil
            ability.execute(ret: &ret, user: user, target: target, applyCosts: applyCosts)
            if !healing {
                if let counters = target.counters, (counters.count > 0 && (!target.stunned)
                    && (target.side != user.side || target.confused)) {
                    let usrDmgType = ability.dmgType
                    for counter in counters {
                        let cntDmgType = counter.dmgType
                        if (usrDmgType & cntDmgType) == cntDmgType && (cntSkill == nil || counter.hp > cntSkill.hp) {
                            cntSkill = counter
                        }
                    }
                    if cntSkill != nil {
                        cntSkill.execute(ret: &ret, user: target, target: user, applyCosts: false)
                    }
                }
            }
            let actorEvent: SpriteRun! = self.spriteRun
            if actorEvent == nil || actorEvent(self, applyCosts ? user : nil, ability, ko && target.hp > 0,
                                               target, target == user ? ability : cntSkill) {
                var targets: [Actor]! = self.targets
                if targets == nil {
                    targets = [Actor]()
                    self.targets = targets
                }
                targets.append(target)
            }
        }
    }
    
    func perform(ret: inout String, user: Actor, target: Actor, ability: Ability, item: Bool) {
        
    }
    
    func checkStatus(ret: inout String) {
        if self.status == SceneStatus.ongoing {
            let parties = self.parties
            var party = parties[0]
            for actor in party {
                if !actor.knockedOut {
                    let pSize = parties.count
                    for i in 1..<pSize {
                        party = parties[i]
                        for player in party {
                            if !player.knockedOut {
                                return
                            }
                        }
                    }
                    ret.append(VictoryTxt)
                    self.status = SceneStatus.victory
                    return
                }
            }
            ret.append(FallenTxt)
            self.status = SceneStatus.defeat
        }
    }
    
    func escape(ret: inout String) {
        
    }
    
    func resetTurn(actor: Actor) {
        let mInit = self.mInit + 1
        if mInit < 2 {
            if actor.cInit > mInit {
                actor.cInit = mInit
            } else if mInit == 1 && actor.cInit < -1 {
                actor.cInit = 0
            }
            self.previous = -1
        }
    }
    
    func agiCalc() {
        if self.mInit < 1 {
            self.players?.sort(by: { $0.agi > $1.agi })
            self.previous = -1
        }
    }
    
    func canTarget(user: Actor, ability: Ability, target: Actor) -> Bool {
        return (ability.canPerform(user: user) && (ability.targetsSelf || ((user.drawnBy == nil
            || user.drawnBy == target) && (target.hp > 0 || ability.revives))))
    }
    
    func initialize(ret: inout String, parties: [[Actor]], spriteRun: SpriteRun?,
                    events: [SceneEvent : [SceneRun]]?, surprise: Int, mInit: Int) {
        assert(parties.count > 1)
        var players: [Actor]?
        let useInit: Bool
        if mInit > 0 {
            self.mInit = mInit
            useInit = true
            players = nil
        } else {
            players = [Actor]()
            useInit = false
            self.mInit = 0
        }
        var current = 0
        var crActor: Actor! = nil
        self.events = events
        self.parties = parties
        self.players = players
        self.spriteRun = spriteRun
        self.targets = nil
        self.surprise = 0
        for (i, party) in parties.enumerated() {
            let aiPlayer = i > 0
            let surprised = surprise == i
            if crActor == nil {
                crActor = party[0]
            }
            for (j, player) in party.enumerated() {
                player.actions = 0
                if surprised {
                    player.cInit = useInit ? -(mInit + 1) : -1
                } else {
                    player.cInit = 0
                    if player.agi > crActor.agi || crActor.cInit < 0 {
                        crActor = player
                        if useInit {
                            current = j
                        }
                    }
                }
                if aiPlayer {
                    player.aiControl = true
                    //player.randomAi = true
                }
                player._oldSide = i
                player.side = i
            }
            players?.append(contentsOf: party)
        }
        self.crActor = crActor
        self.agiCalc()
        if useInit {
            crActor.cInit = mInit
        } else {
            current = players!.firstIndex(of: crActor)!
        }
        self.current = current
        self.previous = current
        if let events = events?[SceneEvent.begin] {
            var defAction = false
            for event in events {
                if event(self, &ret) {
                    defAction = true
                }
            }
            if defAction {
                self.endTurn(ret: &ret, actor: crActor)
            }
        }
    }
    
}
