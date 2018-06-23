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

import android.content.Context
import android.graphics.Color
import android.graphics.drawable.AnimationDrawable
import android.graphics.drawable.Drawable
import android.media.MediaPlayer
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.*
import java.util.*
import kotlin.collections.LinkedHashMap
import android.graphics.Bitmap
import android.graphics.drawable.BitmapDrawable

class AdCostume(id : Int, name : String, var sprName : String, mHp : Int = 30, mMp : Int = 10, mSp : Int = 10, atk : Int = 7, def: Int = 7,
                spi: Int = 7, wis : Int = 7, agi : Int = 7, mActions: Int = 1, range: Boolean = false, res : MutableMap<Int, Int>? = null,
                skills : Array<Ability>? = null, states : Array<State>? = null, stRes : MutableMap<State, Int>? = null)
    : Costume(id, name, mHp, mMp, mSp, atk, def, spi, wis, agi, mActions, range, res, skills, states, stRes) {

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is AdCostume && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }

}

class AdActor(id : Int, private val context : Context, name: String, sprites : Array<Array<AnimationDrawable>>? = null, race: Costume, job: AdCostume,
              level : Int, maxLv: Int, mActions : Int = 1, mHp: Int, mMp: Int, mSp: Int, mAtk: Int, mDef: Int, mSpi: Int, mWis: Int, mAgi: Int,
              range : Boolean, mRes: MutableMap<Int, Int>? = null, skills: Array<Ability>? = null, states: Array<State>?, mStRes: MutableMap<State, Int>?)
    : Actor(id, name, race, job, level, maxLv, mActions, mHp, mMp, mSp, mAtk, mDef, mSpi, mWis, mAgi, range, mRes, skills, states, mStRes) {

    override var job : Costume = job
        set(value) {
            super.job = value
            this.sprites = arrayOf(arrayOfNulls(7), arrayOfNulls(7))
            this.spritesDur = arrayOf(intArrayOf(0, 0, 0, 0, 0, 0, 0), intArrayOf(0, 0, 0, 0, 0, 0, 0))
        }

    internal var spritesDur = arrayOf(intArrayOf(0, 0, 0, 0, 0, 0, 0), intArrayOf(0, 0, 0, 0, 0, 0, 0))
    private var sprites : Array<Array<AnimationDrawable?>> = arrayOf(arrayOfNulls(7), arrayOfNulls(7))
    //private var sprites : Array<Array<AnimationDrawable?>> = this.setAllBtSprites()

    fun getBtSprite(side : Int, spr : Int) : AnimationDrawable? {
        var sprAnim = this.sprites[side][spr]
        if (sprAnim === null) {
            val job = this.job
            var sprName = "spr_bt_" + ((job as? AdCostume)?.sprName?.toLowerCase() ?: job.name.toLowerCase()) + if (side == 0) "_l_" else "_r_"
            sprName += when (spr) {
                0 -> "idle"
                1 -> "ko"
                2 -> "hit"
                3 -> "fallen"
                4 -> "restored"
                5 -> "act"
                6 -> "cast"
                else -> return null
            }
            val drawable = this.context.resources.getDrawable(
                    this.context.resources.getIdentifier(sprName, "drawable", this.context.packageName))
            when (drawable) {
                is AnimationDrawable -> sprAnim = drawable
                is BitmapDrawable -> {
                    sprAnim = if (spr == 4) this.getBtSprite(side, 3)?.getInvertedSprite(true)
                            else (drawable.getSprite(this.context,
                            if (spr in 0..1) null
                            else this.getBtSprite(side, 0)?.getFrame(0), spr in 2..3,
                            if (spr < 2) null else (if (spr == 3) this.getBtSprite(side, 1)?.getFrame(0)
                            else this.getBtSprite(side, 0)?.getFrame(0)), true))
                }
            }
            this.sprites[side][spr] = sprAnim
            this.spritesDur[side][spr] = sprAnim?.fullDur ?: 0
        }
        return sprAnim
    }

    private fun setAllBtSprites(spriteName: String? = null) : Array<Array<AnimationDrawable?>> {
        val sprName : String
        sprName = if (spriteName === null) {
            val job = this.job
            (job as? AdCostume)?.sprName?.toLowerCase() ?: job.name.toLowerCase()
        }
        else {
            spriteName.toLowerCase()
        }
        val sprites = arrayOf(
                arrayOf(this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_idle", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_ko", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_hit", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_fallen", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_restored", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_act", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_l_cast", "drawable", this.context.packageName))
                                as AnimationDrawable?),
                arrayOf(this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_idle", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_ko", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_hit", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_fallen", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_restored", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_act", "drawable", this.context.packageName))
                                as AnimationDrawable?,
                        this.context.resources.getDrawable(
                                this.context.resources.getIdentifier("spr_bt_" + sprName + "_r_cast", "drawable", this.context.packageName))
                                as AnimationDrawable?)
        )
        this.sprites = sprites
        this.spritesDur = arrayOf(
                intArrayOf(sprites[0][0]?.fullDur ?: 0, sprites[0][1]?.fullDur ?: 0, sprites[0][2]?.fullDur ?: 0, sprites[0][3]?.fullDur ?: 0,
                        sprites[0][4]?.fullDur ?: 0, sprites[0][4]?.fullDur ?: 0, sprites[0][5]?.fullDur ?: 0, sprites[0][6]?.fullDur ?: 0),
                intArrayOf(sprites[1][0]?.fullDur ?: 0, sprites[1][1]?.fullDur ?: 0, sprites[1][2]?.fullDur ?: 0, sprites[1][3]?.fullDur ?: 0,
                        sprites[1][4]?.fullDur ?: 0, sprites[1][4]?.fullDur ?: 0, sprites[1][5]?.fullDur ?: 0, sprites[1][6]?.fullDur ?: 0)
        )
        return sprites
    }

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is AdActor && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }
}

class AdAbility(id: Int, name: String, private val sprId : Int, private val sndId : Int, range: Boolean? = null,
                steal: Boolean = false, lvRq: Int, hpC: Int, mpC: Int, spC: Int, hpDmg: Int, mpDmg: Int,
                spDmg: Int, dmgType: Int, atkI: Int, trg : Int, elm: Int, mQty: Int, rQty: Int, absorb: Boolean,
                restoreKO: Boolean, aStates: Array<State>? = null, rStates: Array<State>? = null)
    : Ability(id, name, range, steal, lvRq, hpC, mpC, spC, hpDmg, mpDmg, spDmg, dmgType, atkI, trg, elm, mQty,
              rQty, absorb, restoreKO, aStates, rStates) {

    private var _context : Context? = null
    private var _sndPlayer : MediaPlayer? = null
    private var _sprAnim : AnimationDrawable? = null
    var spriteDur : Int = 0

    fun getSprite(context : Context) : AnimationDrawable? {
        if (this.sprId < 1) return null
        var sprAnim = this._sprAnim
        if (this._context !== context) {
            sprAnim = null
            val sndPlayer = this._sndPlayer
            if (sndPlayer !== null) {
                sndPlayer.release()
                this._sndPlayer = null
            }
        }
        if (sprAnim === null) {
            val drawable = context.resources.getDrawable(this.sprId)
            when (drawable) {
                is AnimationDrawable -> sprAnim = drawable
                is BitmapDrawable -> sprAnim = drawable.getSprite(context, null,
                        false, null, false)
            }
            if (sprAnim === null) {
                this.spriteDur = 0
            }
            else {
                this.spriteDur = sprAnim.fullDur
            }
            this._sprAnim = sprAnim
        }
        return sprAnim
    }

    fun playSound(context : Context) : Int {
        if (this.sndId < 1) return 0
        var sndPlayer = this._sndPlayer
        if (this._context !== context) {
            this._sprAnim = null
            this.spriteDur = 0
            if (sndPlayer !== null) {
                sndPlayer.release()
                sndPlayer = null
            }
        }
        if (sndPlayer === null) {
            sndPlayer = MediaPlayer.create(context, this.sndId)
            if (sndPlayer === null) return 0
            this._context = context
            sndPlayer.isLooping = false
            this._sndPlayer = sndPlayer
        }
        sndPlayer.start()
        return sndPlayer.duration
    }

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is AdAbility && other.id == this.id)
    }

    override fun hashCode(): Int {
        return javaClass.hashCode()
    }
}

class AdState(id : Int, name : String, inactivate : Boolean, automate : Boolean, confuse : Boolean, reflect : Boolean,
              dur : Int = 3, sRes : Int = 0, mHp : Int, mMp : Int, mSp : Int, mAtk : Int, mDef: Int, mSpi: Int, mWis : Int,
              mAgi : Int, mActions : Int, range: Boolean, mRes : MutableMap<Int, Int>? = null, skills : Array<Ability>? = null,
              rSkills : Array<Ability>? = null, rStates : Array<State>? = null,mStRes : MutableMap<State, Int>? = null)
    : State(id, name, inactivate, automate, confuse, reflect, dur, sRes, mHp, mMp, mSp, mAtk, mDef, mSpi, mWis, mAgi,
            mActions, range, mRes, skills, rSkills, rStates, mStRes) {

    override fun equals(other: Any?): Boolean {
        return super.equals(other) || (other is AdState && other.id == this.id)
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

fun AnimationDrawable.getInvertedSprite(firstFrameWait: Boolean) : AnimationDrawable {
    val animSpr = AnimationDrawable()
    animSpr.isOneShot = true
    for (i in this.numberOfFrames - 1 downTo 0) {
        animSpr.addFrame(this.getFrame(i), if (firstFrameWait && i == this.numberOfFrames - 1) 261 else 87)
    }
    return animSpr
}

fun BitmapDrawable.getSprite(context : Context, firstFrame : Drawable? = null, firstFrameWait : Boolean,
                             lastFrame : Drawable? = null, addPlayback : Boolean) : AnimationDrawable {
    val bmp: Bitmap = this.bitmap
    val animSpr = AnimationDrawable()
    animSpr.isOneShot = true
    if (firstFrameWait && firstFrame !== null) {
        animSpr.addFrame(firstFrame, 261)
    }
    val sprCount = bmp.width / bmp.height
    val sprWidth = bmp.width / sprCount
    val lastSprDur = if (addPlayback && sprCount < 7) 261 else 87
    for (i in 0 until sprCount) {
        animSpr.addFrame(BitmapDrawable(context.resources,
                Bitmap.createBitmap(bmp, i * sprWidth, 0, sprWidth, bmp.height)),
                if (i < sprCount - 1) 87 else lastSprDur)
    }
    if (lastSprDur == 261) {
        for (i in sprCount - 2 downTo 1) {
            animSpr.addFrame(animSpr.getFrame(i), 87)
        }
    }
    if (lastFrame !== null) {
        animSpr.addFrame(lastFrame, 1)
    }
    //bmp.recycle()
    return animSpr
}

class ArenaAct : AppCompatActivity() {

    private lateinit var scenePlay : Scene

    private lateinit var imgActor : Array<ImageView>
    private lateinit var autoBtn : Button
    private lateinit var runBtn : Button
    private lateinit var skillActBtn : Button
    private lateinit var itemUseBtn : Button
    private lateinit var skillsSpn : Spinner
    private lateinit var itemsSpn : Spinner
    private lateinit var targetSpn : Spinner
    private lateinit var actionsTxt : TextView
    private lateinit var infoTxt : TextView

    private lateinit var songPlayer : MediaPlayer

    private lateinit var koActors : Array<Boolean>

    private var partySide = 0
    private var otherSide = 1
    private var automatic = false

    private class ViewHolder(var nameText : TextView) {
        var usable : Boolean = true
    }

    private class AbilityArrayAdapter(context: ArenaAct, val layoutRes: Int, skills: List<Ability>, val asItems : Boolean)
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

        private fun prepareView(position: Int, convertView: View?, parent: ViewGroup?): View {
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

            vHolder.usable = this.skills[position].canPerform(this.arenaAct.crActor)
            vHolder.nameText.setTextColor(if (vHolder.usable) Color.WHITE else Color.GRAY)
            return view
        }

        override fun getDropDownView(position: Int, convertView: View?, parent: ViewGroup?): View {
            //return this.getView(position, convertView, parent)
            val view = this.prepareView(position, convertView, parent)
            val vHolder = view.tag as ViewHolder
            val skill = this.skills[position]
            vHolder.nameText.text = skill.name +
                    (if (this.asItems) " x ${this.arenaAct.crActor.items?.get(skill)}" else "") +
                    " (LvRq: ${skill.lvRq}, HPc: ${skill.hpC}, MPc: ${skill.mpC}, RPc: ${skill.spC}" +
                            ", Uses: ${(this.arenaAct.crActor.skillsQty?.get(skill) ?: "∞")}, Trg: " +
                            (if (skill.trg == 0) "One" else if (skill.trg == -1) "Self" else "All") +
                            ", Range: ${if (skill.range == true) "Yes" else "No"})"
            return view
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view = this.prepareView(position, convertView, parent)
            val vHolder = view.tag as ViewHolder
            vHolder.nameText.text = this.skills[position].name
            return view
        }
    }

    private class ActorArrayAdapter(context: ArenaAct, val layoutRes: Int, actors: Array<Actor>)
        : ArrayAdapter<Actor>(context, layoutRes) {

        var arenaAct = context

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

        private fun prepareView(convertView: View?, parent: ViewGroup?): View {
            val view : View
            if (convertView === null || convertView.tag === null) {
                view = (this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) as LayoutInflater)
                        .inflate(this.layoutRes, parent, false)
                val txt = view.findViewById<TextView>(android.R.id.text1)
                view.tag = ViewHolder(txt)
            }
            else {
                view = convertView
            }
            return view
        }

        override fun getDropDownView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view = this.prepareView(convertView, parent)
            val vHolder = view.tag as ViewHolder
            val actor = this.actors[position]
            vHolder.nameText.text = "${actor.name} (HP: " +
                    ((if (position < this.arenaAct.scenePlay.enIdx)
                        "${actor.hp}/${actor.mHp}, MP: ${actor.mp}/${actor.mMp}, RP: ${actor.sp}/${actor.mSp}"
                    else "%.2f".format((actor.hp.toFloat() / actor.mHp.toFloat()) * 100.0f) + "%") + ")")
            return view
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view = this.prepareView(convertView, parent)
            val vHolder = view.tag as ViewHolder
            vHolder.nameText.text = this.actors[position].name
            return view
        }
    }

    private var skillsAdapter : AbilityArrayAdapter? = null
    private lateinit var playersAdapter : ActorArrayAdapter
    private var itemsAdapter : AbilityArrayAdapter? = null

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
        val lastAbility = this.scenePlay.lastAbility as AdAbility?
        val sprType = if (lastAbility === null || lastAbility.trg < 0
                || lastAbility.dmgType == 2 || lastAbility.dmgType == 3) 6 else 5
        val usrSide = if (this.scenePlay.current < this.scenePlay.enIdx) this.partySide else this.otherSide
        val crActor = this.crActor
        val actAnim = crActor.getBtSprite(usrSide, sprType)//crActor.sprites[usrSide][sprType]
        var dur = crActor.spritesDur[usrSide][sprType] - 174
        if (actAnim !== null) {
            actAnim.stop()
            this.imgActor[this.scenePlay.current].setBackgroundDrawable(actAnim)
            actAnim.start()
        }
        this.imgActor[this.scenePlay.current].postDelayed({
            var htActor: AdActor
            for (trg in this.scenePlay.fTarget..this.scenePlay.lTarget) {
                if (trg != this.scenePlay.current && (lastAbility === null
                                || !(lastAbility.dmgType == 2 && this.scenePlay.players[trg].reflect))) {
                    htActor = (this.scenePlay.players[trg] as AdActor)
                    val trgAnim: Int
                    if (htActor.hp > 0) {
                        if (koActors[trg]) {
                            trgAnim = 4
                            this.koActors[trg] = false
                        } else {
                            trgAnim = 2
                        }
                    } else {
                        if (this.koActors[trg]) continue
                        trgAnim = 3
                        this.koActors[trg] = true
                    }
                    if (lastAbility !== null) {
                        val abilitySpr = lastAbility.getSprite(this)
                        if (abilitySpr !== null) {
                            if (lastAbility.spriteDur > dur) {
                                dur = lastAbility.spriteDur
                            }
                            abilitySpr.stop()
                            this.imgActor[trg].setImageDrawable(abilitySpr)
                            abilitySpr.start()
                        }
                        val soundDur = lastAbility.playSound(this)
                        if (soundDur > dur) {
                            dur = soundDur
                        }
                    }
                    val trgSide = if (trg < this.scenePlay.enIdx) this.partySide else this.otherSide
                    val hitAnim = htActor.getBtSprite(trgSide, trgAnim)//htActor.sprites[trgSide][trgAnim]
                    if (hitAnim !== null) {
                        hitAnim.stop()
                        if (htActor.spritesDur[trgSide][trgAnim] > dur) {
                            dur = htActor.spritesDur[trgSide][trgAnim]
                        }
                        this.imgActor[trg].setBackgroundDrawable(hitAnim)
                        hitAnim.start()
                    }
                }
            }
            this.imgActor[this.scenePlay.current].postDelayed({
                if (crActor.hp < 0) {
                    this.koActors[this.scenePlay.current] = true
                    val fallAnim = crActor.getBtSprite(usrSide, 3)//crActor.sprites[usrSide][3]
                    if (fallAnim !== null) {
                        fallAnim.stop()
                        this.imgActor[this.scenePlay.current].setBackgroundDrawable(fallAnim)
                        fallAnim.start()
                    }
                }
                this.actionsTxt.append(this.scenePlay.endTurn(""))
                this.afterAct()
            }, dur.toLong())
        }, 174)
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
                itemsAdapter = AbilityArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item, crItems, true)
                this.itemsAdapter = itemsAdapter
                this.itemsSpn.adapter = itemsAdapter

                this.itemsSpn.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                    override fun onNothingSelected(parent: AdapterView<*>?) {
                        this@ArenaAct.itemUseBtn.isEnabled = false
                    }

                    override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                        this@ArenaAct.itemUseBtn.isEnabled = (view?.tag as ViewHolder).usable
                                && this@ArenaAct.canTarget(this@ArenaAct.targetSpn.selectedItemPosition, this@ArenaAct.itemsSpn.selectedItem as Ability)
                    }

                }

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
            this.targetSpn.selectedItemPosition
            if (this.targetSpn.selectedItemPosition > -1) {
                val item = this.itemsSpn.selectedItem as Ability
                this.itemUseBtn.isEnabled = item.canPerform(this.crActor) && this.canTarget(this.targetSpn.selectedItemPosition, item)
            }
        }
    }

    private fun setCrSkills() {
        var skillsAdapter = this.skillsAdapter
        if (skillsAdapter === null) {
            skillsAdapter = AbilityArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item,
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
                            && this@ArenaAct.canTarget(this@ArenaAct.targetSpn.selectedItemPosition, this@ArenaAct.crActor.availableSkills[position])
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
                if (this@ArenaAct.crActor.automatic == 0 && this@ArenaAct.skillActBtn.isEnabled) {
                    this@ArenaAct.skillActBtn.callOnClick()
                }
            }
            else {
                this@ArenaAct.targetSpn.setSelection(targetPos)
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        this.setContentView(R.layout.activity_arena)

        val extra = this.intent.extras
        val surprised : Int
        if (extra !== null) {
            surprised = extra.getInt("surprise", 0)
            val songResId = extra.getInt("song", 0)
            if (songResId > 0) {
                this.songPlayer = MediaPlayer.create(this, songResId);
                this.songPlayer.isLooping = true;
                this.songPlayer.start();
            }
            val arenaResId = extra.getInt("arena", 0)
            if (arenaResId > 0) {
                this.findViewById<ImageView>(R.id.ImgArena).setBackgroundResource(arenaResId)
            }
        }
        else {
            surprised = 0
        }

        val humanRace = Costume(1, "Human")
        val heroJob = AdCostume(1, "Hero", "hero")
        val valkyrieJob = AdCostume(1, "Valkyrie", "valkyrie")
        val crusaderJob = AdCostume(1, "Crusader", "crusader")
        val sorceressJob = AdCostume(1, "Sorceress", "sorceress")
        val ninjaJob = AdCostume(1, "Ninja", "ninja")
        val dragoonJob = AdCostume(1, "Dragoon", "dragoon")
        val hesychastJob = AdCostume(1, "Hesychast", "hesychast")
        val shamanJob = AdCostume(1, "Shaman", "shaman")
        val alchemistJob = AdCostume(1, "Alchemist", "alchemist")
        val reaverJob = AdCostume(1, "Reaver", "reaver")
        val rangerJob = AdCostume(1, "Ranger", "ranger")
        val corsairJob = AdCostume(1, "Corsair", "corsair")
        val druidJob = AdCostume(1, "Druid", "druid")
        val knightJob = AdCostume(1, "Knight", "knight")
        val spyJob = AdCostume(1, "Spy", "spy")
        val wizardJob = AdCostume(1, "Wizard", "wizard")
        val berserkerJob = AdCostume(1, "Berserker", "berserker")

        val skills : Array<Ability> = arrayOf(
                AdAbility(1, "Attack", 0, 0, null, false, 1, 0, 0, 1, 10, 0, 0,
                        0, 0, 0, 0, 0, 0, false, false, null, null),
                AdAbility(2, "Defend", 0, 0, false, false, 1, 0, 0, 0, 0, -2, -3,
                        1, 0, -1, 0, 0, 0, false, false, null, null),
                AdAbility(3, "Heal", 0, 0, true, false, 1, 0, 3, 0, -15, 0, 0,
                        3, 0, 0, 0, 0, 0, false, true, null, null)
        )

        val skills2 : Array<Ability> = arrayOf(
                AdAbility(1, "Act", 0, 0, null, false, 1, 0, 0, 0, 10, 0, 0,
                        0, 0, 0, 0, 0, 0, false, false, null, null),
                AdAbility(2, "Guard", 0, 0, false, false, 1, 0, 0, 0, 0, -2, -3,
                        1, 0, -1, 0, 0, 0, false, false, null, null),
                AdAbility(1, "Hit", 0, 0, false, false, 1, 0, 45, 1, 30, 0, 0,
                        0, 0, 0, 0, 0, 0, false, false, null, null))

        val party : Array<Actor> = arrayOf(
                AdActor(1, this, "Cody", null, humanRace, knightJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, false, null, skills2, null, null),
                AdActor(2, this, "Victoria", null, humanRace, valkyrieJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, false, null, skills2, null, null),
                AdActor(3, this, "Stephanie", null, humanRace, sorceressJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, false, null, skills, null, null),
                AdActor(4, this, "George", null, humanRace, hesychastJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, false, null, skills, null, null)
        )
        party[0].items = LinkedHashMap()
        val potion = AdAbility(10, "Potion", 0, 0, false, false, 1, 0, 3, 0, -15, 0, 0,
                3, 0, 0, 0, 0, 0, false, false, null, null)
        party[0].items!![potion] = 3
        party[1].items = party[0].items

        val enemy : Array<Actor> = arrayOf(
                AdActor(8, this, "Goblin", null, humanRace, ninjaJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, false, null, skills, null, null),
                AdActor(7, this, "Troll", null, humanRace, druidJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, false, null, skills, null, null),
                AdActor(6, this, "Lizard", null, humanRace, alchemistJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, false, null, skills, null, null),
                AdActor(5, this, "Ogre", null, humanRace, dragoonJob, 1, 9, 1, 50, 25, 25, 7, 7,
                        7, 7, 7, false, null, skills, null, null)
        )

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
            /*this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).sprites[this.partySide]
                    [if (this.koActors[i]) 1 else 0])*/
            this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).getBtSprite(this.partySide,
                    if (this.koActors[i]) 1 else 0))
        }

        for (i in this.scenePlay.enIdx until this.scenePlay.players.size) {
            this.scenePlay.players[i].automatic = 2
            this.imgActor[i].setBackgroundDrawable((this.scenePlay.players[i] as AdActor).getBtSprite(this.otherSide,
                    if (this.koActors[i]) 1 else 0))
        }

        this.playersAdapter = ActorArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item,
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
                if (this@ArenaAct.crActor.automatic == 0) {
                    if (this@ArenaAct.itemsSpn.isEnabled) {
                        this@ArenaAct.itemUseBtn.isEnabled = (this@ArenaAct.itemsSpn.selectedView.tag as ViewHolder).usable
                                && this@ArenaAct.canTarget(position, this@ArenaAct.itemsSpn.selectedItem as Ability)
                    }
                    if (this@ArenaAct.skillsSpn.isEnabled) {
                        this@ArenaAct.setCrAutoSkill()
                    }
                }
            }

        }

        this.autoBtn.setOnClickListener {
            this.automatic = !this.automatic
            if (this.automatic) {
                this.enableControls(false)
                if (this.crActor.automatic == 0) {
                    this.afterAct()
                }
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