/*This file has been written by Claudiu-Stefan Costea,
 * and is released under the terms of the LGPL-2.1
 */
package com.tbrpgsca.library;

import java.util.Locale;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.BitmapDrawable;
public class Job extends Race {
	public String jname,bsprite;
    public int hpp,mpp,spp,atkp,defp,wisp,spip,agip;
    
    public Job(){
    	this("Hero", 0, 0, 0, 0, 0, 0, 0, 0);
    }
    
    public Job(String jname, int hpp, int mpp, int spp, int atkp, int defp,
            int wisp, int spip, int agip){
    	this.jname=jname;
    	this.jobStats(hpp, mpp, spp, atkp, defp, wisp, spip, agip);
    }
    
    public Job(String jname, int hpp, int mpp, int spp, int atkp, int defp,
            int wisp, int spip, int agip, int[] newSkill){
    	this(jname,hpp, mpp, spp, atkp, defp, wisp, spip, agip);
    	this.addSkills(newSkill);
    }
    
    protected void jobStats(int hpp, int mpp, int spp, int atkp, int defp,
            int wisp, int spip, int agip) {        
        this.bsprite=(this.jname!="")?"hero":jname.toLowerCase(Locale.US);
        this.hpp = hpp;
        this.mpp = mpp;
        this.spp = spp;
        this.atkp = atkp;
        this.defp = defp;
        this.wisp = wisp;
        this.spip = spip;
        this.agip = agip;
    }
    
    private Bitmap getBmpSpr(int i, String pos, int[] c, Activity context){
    	/*BitmapFactory.Options opt = new BitmapFactory.Options();
    	opt.inPurgeable=true;opt.inDither=false;opt.inSampleSize=1;*/
    	Bitmap bmp=BitmapFactory.decodeResource(context.getResources(), context.getResources().getIdentifier("bt_"+bsprite+"_"+i+"_"+pos, "drawable", context.getPackageName())/*,opt*/);
    	c[0]=bmp.getHeight();c[2]=bmp.getWidth()/c[0];c[1]=bmp.getWidth()/c[2];
    	return bmp;
    }
        
    public AnimationDrawable getBtSprite(int i, String pos, boolean bkw, int[] time, Activity context){
    	Bitmap bmp=null; int p[]={128,128,7}; int s=i>2?1:2; time[s]=0;//System.gc();
    	AnimationDrawable sprite=new AnimationDrawable();
    	if (i>0) {
    		if (i==2){
    			bmp = getBmpSpr(1,pos,p,context);
    			sprite.addFrame(new BitmapDrawable(context.getResources(),Bitmap.createBitmap(bmp, 0, 0, p[1], p[0])),261);
    			time[s]+=261;
    		}
    		bmp = getBmpSpr(i,pos,p,context);
    		for (int j=0;j<p[2];j++){
    			int t=(i!=1||j!=0)?87:261;
    			sprite.addFrame(new BitmapDrawable(context.getResources(),Bitmap.createBitmap(bmp, j*p[1], 0, p[1], p[0])),t);
    			time[s]+=t;
    		}
    		for (int j=p[2]-2;bkw&&j>0;j--){
    			sprite.addFrame(new BitmapDrawable(context.getResources(),Bitmap.createBitmap(bmp, j*p[1], 0, p[1], p[0])),87);
    			time[s]+=87;
    		}
    	}
    	if (i!=2&&i>=0) {
    		if (i!=1) bmp = getBmpSpr(1,pos,p,context);
    		sprite.addFrame(new BitmapDrawable(context.getResources(),Bitmap.createBitmap(bmp, 0, 0, p[1], p[0])),0);
    	}
    	if (i<0){
    		bmp = getBmpSpr(2,pos,p,context);
			sprite.addFrame(new BitmapDrawable(context.getResources(),Bitmap.createBitmap(bmp, (p[2]-1)*p[1], 0, p[1], p[0])),0);
    	}
    	bmp.recycle(); return sprite;
    }
}