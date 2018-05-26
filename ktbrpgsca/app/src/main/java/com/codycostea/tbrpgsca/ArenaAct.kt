/*
Copyright (C) AD 2018 Claudiu-Stefan Costea

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
package com.codycostea.tbrpgsca

import android.app.Activity
import android.content.Context
import android.graphics.Color
import android.graphics.drawable.AnimationDrawable
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.animation.Animation
import android.widget.*
import java.util.*
import kotlin.collections.LinkedHashMap

class AdActor(id : Int, private val context : Context, name: String, sprites : Array<Array<AnimationDrawable>>? = null, race: Costume, job: Costume,
              level : Int, maxLv: Int, mActions : Int = 1, mHp: Int, mMp: Int, mSp: Int, mAtk: Int, mDef: Int, mSpi: Int, mWis: Int, mAgi: Int,
              mRes: MutableMap<Int, Int>? = null, skills: Array<Ability>? = null, states: Array<State>?, mStRes: MutableMap<State, Int>?)
    : Actor(id, name, race, job, level, maxLv, mActions, mHp, mMp, mSp, mAtk, mDef, mSpi,
            mWis, mAgi, mRes, skills, states, mStRes) {

    override var job : Costume = job
        set(value) {
            super.job = value
            this.sprites = this.getSprites(value.name.toLowerCase())
        }

    internal var sprites : Array<Array<AnimationDrawable>> = sprites ?: this.getSprites(job.name.toLowerCase())

    internal var spritesDur = arrayOf(
            arrayOf(this.sprites[0][0].fullDur, this.sprites[0][1].fullDur, this.sprites[0][2].fullDur, this.sprites[0][3].fullDur,
                    this.sprites[0][4].fullDur, this.sprites[0][4].fullDur, this.sprites[0][5].fullDur, this.sprites[0][6].fullDur),
            arrayOf(this.sprites[1][0].fullDur, this.sprites[1][1].fullDur, this.sprites[1][2].fullDur, this.sprites[1][3].fullDur,
                    this.sprites[1][4].fullDur, this.sprites[1][4].fullDur, this.sprites[1][5].fullDur, this.sprites[1][6].fullDur)
    )

    private fun getSprites(sprName: String) : Array<Array<AnimationDrawable>> {
        return arrayOf(
                arrayOf(this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_idle", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_ko", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_hit", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_fallen", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_restored", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_act", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_cast", "drawable", this.context.packageName))
                                as AnimationDrawable),
                arrayOf(this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_idle", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_ko", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_hit", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_fallen", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_restored", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_act", "drawable", this.context.packageName))
                                as AnimationDrawable,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_cast", "drawable", this.context.packageName))
                                as AnimationDrawable)
        )
    }

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is Actor && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }
}

class AdAbility(id: Int, name: String, val sprId : Int, val sndId : Int, range: Boolean = false,
                steal: Boolean = false, lvRq: Int, hpC: Int, mpC: Int, spC: Int, hpDmg: Int, mpDmg: Int,
                spDmg: Int, dmgType: Int, atkI: Int, trg : Int, elm: Int, mQty: Int, rQty: Int, absorb: Boolean,
                restoreKO: Boolean, aStates: Array<State>? = null, rStates: Array<State>? = null)
    : Ability(id, name, range, steal, lvRq, hpC, mpC, spC, hpDmg, mpDmg, spDmg, dmgType, atkI, trg, elm, mQty,
              rQty, absorb, restoreKO, aStates, rStates) {

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is Ability && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }
}

class AdState(id : Int, name : String, inactivate : Boolean, automate : Boolean, confuse : Boolean, reflect : Boolean,
              dur : Int = 3, sRes : Int = 0, mHp : Int, mMp : Int, mSp : Int, mAtk : Int, mDef: Int, mSpi: Int, mWis : Int,
              mAgi : Int, mRes : MutableMap<Int, Int>? = null, skills : Array<Ability>? = null, rSkills : Array<Ability>? = null,
              rStates : Array<State>? = null,mStRes : MutableMap<State, Int>? = null)
    : State(id, name, inactivate, automate, confuse, reflect, dur, sRes, mHp, mMp, mSp, mAtk, mDef, mSpi, mWis, mAgi, mRes,
            skills, rSkills, rStates, mStRes) {

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is State && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }
}

val AnimationDrawable.fullDur : Int
get() {
    var s = 0
    for (i in 0 until this.numberOfFrames) {
        s += getDuration(i)
    }
    return s
}

class ArenaAct : Activity() {

    lateinit var scenePlay : Scene

    lateinit var imgActor : Array<ImageView>
    lateinit var autoBtn : Button
    lateinit var runBtn : Button
    lateinit var skillActBtn : Button
    lateinit var itemUseBtn : Button
    lateinit var skillsSpn : Spinner
    lateinit var itemsSpn : Spinner
    lateinit var targetSpn : Spinner
    lateinit var actionsTxt : TextView
    lateinit var infoTxt : TextView

    lateinit var koActors : Array<Boolean>

    private var partySide = 0
    private var otherSide = 1
    private var automatic = false

    private class ViewHolder(var nameText : TextView) {
        var usable : Boolean = true
    }

    private class AbilityArrayAdater(context: ArenaAct, val layoutRes: Int, skills: List<Ability>, val asItems : Boolean)
        : ArrayAdapter<Ability>(context, layoutRes) {

        var arenaAct = context

        var skills = skills
        set(value) {
            field = value
            this.notifyDataSetChanged()
        }

        override fun getItem(position: Int): Ability {
            return this.skills[position]
        }

        override fun getCount(): Int {
            return this.skills.size
        }

        override fun getDropDownView(position: Int, convertView: View?, parent: ViewGroup?): View {
            return this.getView(position, convertView, parent)
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view : View
            val vHolder : ViewHolder
            if (convertView === null || convertView.tag === null) {
                view = (this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) as LayoutInflater)
                        .inflate(this.layoutRes, parent, false)
                val txt = view.findViewById<TextView>(android.R.id.text1)
                vHolder = ViewHolder(txt)
                view.tag = vHolder
            }
            else {
                view = convertView
                vHolder = convertView.tag as ViewHolder
            }

            vHolder.nameText.text = this.skills[position].name
            vHolder.usable = this.asItems || this.skills[position].canPerform(this.arenaAct.crActor)
            vHolder.nameText.setTextColor(if (vHolder.usable) Color.WHITE else Color.GRAY)
            return view
        }
    }

    private class ActorArrayAdater(context: ArenaAct, val layoutRes: Int, actors: Array<Actor>)
        : ArrayAdapter<Actor>(context, layoutRes) {

        //var arenaAct = context

        var actors = actors
        set(value) {
            field = value
            this.notifyDataSetChanged()
        }

        override fun getItem(position: Int): Actor {
            return this.actors[position]
        }

        override fun getCount(): Int {
            return this.actors.size
        }

        override fun getDropDownView(position: Int, convertView: View?, parent: ViewGroup?): View {
            return this.getView(position, convertView, parent)
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view : View
            val vHolder : ViewHolder
            if (convertView === null || convertView.tag === null) {
                view = (this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) as LayoutInflater)
                        .inflate(this.layoutRes, parent, false)
                val txt = view.findViewById<TextView>(android.R.id.text1)
                vHolder = ViewHolder(txt)
                view.tag = vHolder
            }
            else {
                view = convertView
                vHolder = convertView.tag as ViewHolder
            }

            vHolder.nameText.text = this.actors[position].name
            /*val crSkill =  this.arenaAct.skillsSpn.selectedItem as Ability
            vHolder.usable = this.arenaAct.scenePlay.getGuardian(position, crSkill) == position
                && (this.actors[position].hp > 0 || crSkill.restoreKO)
            vHolder.nameText.setTextColor(if (vHolder.usable) Color.WHITE else Color.GRAY)*/
            return view
        }
    }

    private var skillsAdapter : AbilityArrayAdater? = null
    private lateinit var playersAdapter : ActorArrayAdater
    private var itemsAdapter : AbilityArrayAdater? = null

    private val crActor : AdActor
        get() {
            return this.scenePlay.players[this.scenePlay.current] as AdActor
        }

    private fun canTarget(target : Int, ability : Ability) : Boolean {
        return this.scenePlay.getGuardian(target, ability) == target
                && (this.scenePlay.players[target].hp > 0 || ability.restoreKO)
    }

    private fun enableControls(enable : Boolean) {
        if (!enable) {
            this.skillActBtn.isEnabled = false
            this.itemUseBtn.isEnabled = false
            this.itemsSpn.isEnabled = false
        }
        this.skillsSpn.isEnabled = enable
        this.runBtn.isEnabled = enable
    }

    private fun setCrAutoSkill() {
        val autoSkill = this.scenePlay.getAIskill(
                if (this.targetSpn.selectedItemPosition < this.scenePlay.enIdx) 1 else 0)
        if (this.skillsSpn.selectedItemPosition == autoSkill) {
            this.skillActBtn.isEnabled = (this.crActor.availableSkills[autoSkill]).canPerform(this.crActor)
                    && this.canTarget(this.targetSpn.selectedItemPosition, this.crActor.availableSkills[autoSkill])
        }
        else {
            this.skillsSpn.setSelection(autoSkill)
        }
    }

    private fun afterAct() {
        if (this.automatic || this.crActor.automatic != 0) {
            this.actionsTxt.append(this.scenePlay.executeAI(""))
            this.playSpr()
        }
        else {
            this.setCrSkills()
            this.setCrItems()
            this.enableControls(true)
            this.autoBtn.isEnabled = true
            this.setCrAutoSkill()
        }
    }

    private fun playSpr() {
        val lastAbility = this.scenePlay.lastAbility
        val sprType = if (lastAbility == null || lastAbility.trg < 0
                || lastAbility.dmgType == 2 || lastAbility.dmgType == 3) 6 else 5
        val usrSide = if (this.scenePlay.current < this.scenePlay.enIdx) this.partySide else this.otherSide
        val crActor = this.crActor
        val actAnim = crActor.sprites[usrSide][sprType]
        var dur = crActor.spritesDur[usrSide][sprType]
        actAnim.stop()
        this.imgActor[this.scenePlay.current].setBackgroundDrawable(actAnim)
        var htActor : AdActor
        for (trg in this.scenePlay.fTarget..this.scenePlay.lTarget) {
            if (trg != this.scenePlay.current && (lastAbility == null
                    || !(lastAbility.dmgType == 2 && this.scenePlay.players[trg].reflect))) {
                htActor = (this.scenePlay.players[trg] as AdActor)
                val trgAnim : Int
                if (htActor.hp > 0) {
                    if (koActors[trg]) {
                        trgAnim = 4
                        this.koActors[trg] = false
                    }
                    else {
                        trgAnim = 2
                    }
                }
                else {
                    if (this.koActors[trg]) continue
                    trgAnim = 3
                    this.koActors[trg] = true
                }
                val trgSide = if (trg < this.scenePlay.enIdx) this.partySide else this.otherSide
                val hitAnim = htActor.sprites[trgSide][trgAnim]
                hitAnim.stop()
                if (htActor.spritesDur[trgSide][trgAnim] > dur) {
                    dur = htActor.spritesDur[trgSide][trgAnim]
                }
                this.imgActor[trg].setBackgroundDrawable(hitAnim)
                hitAnim.start()
            }
        }
        actAnim.start()
        this.imgActor[this.scenePlay.current].postDelayed({
            if (crActor.hp < 0) {
                this.koActors[this.scenePlay.current] = true
                val fallAnim = crActor.sprites[usrSide][3]
                fallAnim.stop()
                this.imgActor[this.scenePlay.current].setBackgroundDrawable(fallAnim)
                fallAnim.start()
            }
            this.actionsTxt.append(this.scenePlay.endTurn(""))
            this.afterAct()
        }, dur.toLong())
    }

    private fun setCrItems() {
        val crItems = this.scenePlay.crItems!![this.scenePlay.current]
        if (crItems === null || crItems.isEmpty()) {
            if (this.itemsSpn.isEnabled) {
                this.itemsSpn.setSelection(Spinner.INVALID_POSITION)
                this.itemsSpn.isEnabled = false
            }
        }
        else {
            var itemsAdapter = this.itemsAdapter
            if (itemsAdapter === null) {
                itemsAdapter = AbilityArrayAdater(this, android.R.layout.simple_spinner_dropdown_item, crItems, true)
                this.itemsAdapter = itemsAdapter
                this.itemsSpn.adapter = itemsAdapter

                /*this.itemsSpn.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                    override fun onNothingSelected(parent: AdapterView<*>?) {
                        itemUseBtn.isEnabled = false
                    }

                    override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                        itemUseBtn.isEnabled = (view?.tag as ViewHolder).usable
                                && canTarget(targetSpn.selectedItemPosition, itemsSpn.selectedItem as Ability)
                    }

                }*/

                this.itemUseBtn.setOnClickListener {
                    this.enableControls(false)
                    this.actionsTxt.append(this.scenePlay.useItem(this.itemsSpn.selectedItemPosition,
                            this.targetSpn.selectedItemPosition, ""))
                    itemsAdapter.notifyDataSetChanged()
                    this.playSpr()
                }
            }
            else if (itemsAdapter.skills !== crItems) {
                itemsAdapter.skills = crItems

            }
            if (!this.itemsSpn.isEnabled) {
                this.itemsSpn.setSelection(0)
                this.itemsSpn.isEnabled = true
            }
            this.itemUseBtn.isEnabled = true
        }
    }

    private fun setCrSkills() {
        var skillsAdapter = this.skillsAdapter
        if (skillsAdapter === null) {
            skillsAdapter = AbilityArrayAdater(this, android.R.layout.simple_spinner_dropdown_item,
                    this.crActor.availableSkills, false)
            //this.skillsAdapter.setNotifyOnChange(true)
            this.skillsSpn.adapter = skillsAdapter
            this.skillsAdapter = skillsAdapter

            this.skillsSpn.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onNothingSelected(parent: AdapterView<*>?) {
                    this@ArenaAct.skillActBtn.isEnabled = false
                }

                override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    this@ArenaAct.skillActBtn.isEnabled = ((view !== null && (view.tag as ViewHolder).usable)
                            || (view === null && this@ArenaAct.crActor.availableSkills[position].canPerform(this@ArenaAct.crActor)))
                            && canTarget(this@ArenaAct.targetSpn.selectedItemPosition, this@ArenaAct.crActor.availableSkills[position])
                }

            }

            this.skillActBtn.setOnClickListener {
                this.enableControls(false)
                this.actionsTxt.append(this.scenePlay.performSkill(this.skillsSpn.selectedItemPosition,
                        this.targetSpn.selectedItemPosition, ""))
                this.playSpr()
            }
        }
        else {
            skillsAdapter.skills = this.crActor.availableSkills
        }
    }

    private fun ImageView.setTargetClickListener(targetPos : Int) {
        this.setOnClickListener {
            if (targetPos == this@ArenaAct.targetSpn.selectedItemPosition) {
                if (this@ArenaAct.crActor.automatic == 0 && this@ArenaAct.skillActBtn.isEnabled
                        && android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.ICE_CREAM_SANDWICH_MR1) {
                    this@ArenaAct.skillActBtn.callOnClick()
                }
            } else {
                this@ArenaAct.targetSpn.setSelection(targetPos)
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        this.setContentView(R.layout.activity_arena)

        val humanRace = Costume(1, "Human")
        val heroJob = Costume(1, "Hero")

        val skills : Array<Ability> = arrayOf(
                AdAbility(1, "Attack", 0, 0, false, false, 1, 0, 0, 1, 10, 0, 0,
                        0, 0, 0, 0, 0, 0, false, false, null, null),
                AdAbility(2, "Defend", 0, 0, false, false, 1, 0, 0, 0, 0, -2, -3,
                        1, 0, -1, 0, 0, 0, false, false, null, null),
                AdAbility(3, "Heal", 0, 0, true, false, 1, 0, 3, 0, -15, 0, 0,
                        3, 0, 0, 0, 0, 0, false, true, null, null)
        )

        val skills2 : Array<Ability> = arrayOf(
                AdAbility(1, "Act", 0, 0, false, false, 1, 0, 0, 0, 10, 0, 0,
                        0, 0, 0, 0, 0, 0, false, false, null, null),
                AdAbility(2, "Guard", 0, 0, false, false, 1, 0, 0, 0, 0, -2, -3,
                        1, 0, -1, 0, 0, 0, false, false, null, null),
                AdAbility(1, "Hit", 0, 0, false, false, 1, 0, 45, 1, 30, 0, 0,
                        0, 0, 0, 0, 0, 0, false, false, null, null))

        val party : Array<Actor> = arrayOf(
                AdActor(1, this, "Cody", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills2, null, null),
                AdActor(2, this, "Victoria", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(3, this, "Stephanie", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(4, this, "George", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills2, null, null)
        )
        party[0].items = LinkedHashMap()
        val potion = AdAbility(10, "Potion", 0, 0, true, false, 1, 0, 3, 0, -15, 0, 0,
                3, 0, 0, 0, 0, 0, false, true, null, null)
        party[0].items!![potion] = 3
        party[1].items = party[0].items

        val enemy : Array<Actor> = arrayOf(
                AdActor(8, this, "Goblin", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(7, this, "Troll", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(6, this, "Lizard", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null),
                AdActor(5, this, "Ogre", null, humanRace, heroJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, null, skills, null, null)
        )

        val surprised = -1
        if (surprised < 0) {
            this.partySide = 1
            this.otherSide = 0
        }
        else {
            this.partySide = 0
            this.otherSide = 1
        }

        this.scenePlay = Scene(party, enemy, surprised)

        this.runBtn = this.findViewById(R.id.RunBt)
        this.autoBtn = this.findViewById(R.id.AutoBt)
        this.skillActBtn = this.findViewById(R.id.ActBt)
        this.itemUseBtn = this.findViewById(R.id.UseBt)
        this.skillsSpn = this.findViewById(R.id.SkillBox)
        this.itemsSpn = this.findViewById(R.id.ItemBox)
        this.targetSpn = this.findViewById(R.id.TargetBox)
        this.actionsTxt = this.findViewById(R.id.ItemCost)
        this.infoTxt = this.findViewById(R.id.SkillCost)

        this.koActors = this.scenePlay.players.map {
            it.hp < 1
        }.toTypedArray()

        val imgViews = ArrayList<ImageView>(party.size + enemy.size)

        var pos = 0
        var imgView : ImageView?
        if (surprised < 0) {
            if (party.isNotEmpty()) {
                imgView = this.findViewById(R.id.ImgEnemy1)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 1) {
                imgView = this.findViewById(R.id.ImgEnemy2)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 2) {
                imgView = this.findViewById(R.id.ImgEnemy3)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 3) {
                imgView = this.findViewById(R.id.ImgEnemy4)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.isNotEmpty()) {
                imgView = this.findViewById(R.id.ImgPlayer1)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 1) {
                imgView = this.findViewById(R.id.ImgPlayer2)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 2) {
                imgView = this.findViewById(R.id.ImgPlayer3)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 3) {
                imgView = this.findViewById(R.id.ImgPlayer4)
                imgView.setTargetClickListener(pos)
                imgViews.add(imgView)
            }
        }
        else {
            if (party.isNotEmpty()) {
                imgView = this.findViewById(R.id.ImgPlayer1)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 1) {
                imgView = this.findViewById(R.id.ImgPlayer2)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 2) {
                imgView = this.findViewById(R.id.ImgPlayer3)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (party.size > 3) {
                imgView = this.findViewById(R.id.ImgPlayer4)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.isNotEmpty()) {
                imgView = this.findViewById(R.id.ImgEnemy1)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 1) {
                imgView = this.findViewById(R.id.ImgEnemy2)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 2) {
                imgView = this.findViewById(R.id.ImgEnemy3)
                imgView.setTargetClickListener(pos++)
                imgViews.add(imgView)
            }
            if (enemy.size > 3) {
                imgView = this.findViewById(R.id.ImgEnemy4)
                imgView.setTargetClickListener(pos)
                imgViews.add(imgView)
            }
            this.setCrSkills()
            this.setCrItems()
        }

        this.imgActor = imgViews.toTypedArray()

        for (i in 0 until this.scenePlay.enIdx) {
            this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).sprites[this.partySide]
                    [if (this.koActors[i]) 1 else 0])
        }

        for (i in this.scenePlay.enIdx until this.scenePlay.players.size) {
            this.scenePlay.players[i].automatic = 2
            this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).sprites[this.otherSide]
                    [if (this.koActors[i]) 1 else 0])
        }

        this.playersAdapter = ActorArrayAdater(this, android.R.layout.simple_spinner_dropdown_item,
                this.scenePlay.players)

        this.targetSpn.adapter = this.playersAdapter

        this.targetSpn.setSelection(this.scenePlay.enIdx)

        this.targetSpn.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {

            override fun onNothingSelected(parent: AdapterView<*>?) {
                this@ArenaAct.skillActBtn.isEnabled = false
                this@ArenaAct.itemUseBtn.isEnabled = false
            }

            override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                //skillActBtn.isEnabled = crCanPerform && canTarget(position, skillsSpn.selectedItem as Ability)
                //itemUseBtn.isEnabled = crCanUse && canTarget(position, itemsSpn.selectedItem as Ability)
                if (this@ArenaAct.crActor.automatic == 0 && this@ArenaAct.skillsSpn.isEnabled) {
                    this@ArenaAct.setCrAutoSkill()
                }
            }

        }

        if (this.itemsAdapter === null) {
            this.itemsSpn.isEnabled = false
            this.itemUseBtn.isEnabled = false
        }

        this.autoBtn.setOnClickListener {
            this.automatic = !this.automatic
            if (this.automatic) {
                this.afterAct()
            }
            else {
                this.autoBtn.isEnabled = false
            }
        }

        if (this.crActor.automatic != 0) {
            this.afterAct()
        }
        else {
            this.enableControls(true)
            this.setCrAutoSkill()
        }
    }
}